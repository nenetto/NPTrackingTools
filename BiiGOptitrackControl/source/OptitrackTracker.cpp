#include "OptitrackTracker.h"

// VNL
#include <vnl/vnl_sparse_matrix_linear_system.h>
#include <vnl/algo/vnl_lsqr.h>
#include <vnl/vnl_vector.h>
#include <vnl/vnl_sparse_matrix.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>


// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <math.h>
#include <time.h>

// NPTrackingTools library
#include <NPTrackingTools.h>


//tinyxml2 lib
#include <tinyxml2.h>
#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif

namespace Optitrack
{

    void OptitrackTracker::SetState(OptitrackTracker::OPTITRACK_TRACKER_STATE state_)
    {
        MutexLockHolder lock(*m_StateMutex);
        if (m_State == state_)
        {
            this->m_StateMutex->Unlock();
            return;
        }
        this->m_State = state_;
        this->Modified();
        this->m_StateMutex->Unlock();
        return;
    }

    OptitrackTracker::OPTITRACK_TRACKER_STATE OptitrackTracker::GetState( void )
    {
        return this->m_State;
    }

    OptitrackTracker::OptitrackTracker()
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::OptitrackTracker]\n");
        // Set the MultiThread and Mutex
        this->m_MultiThreader = itk::MultiThreader::New();
        this->m_ToolsMutex = itk::FastMutexLock::New();
        this->m_StopTrackingMutex = itk::FastMutexLock::New();
        this->m_StopTracking = false;
        this->m_TrackingFinishedMutex = itk::FastMutexLock::New();
        this->m_StateMutex = itk::FastMutexLock::New();
        this->m_TrackingFinishedMutex->Lock();  // execution rights are owned by the application thread at the beginning
		this->m_State = STATE_TRACKER_Idle;
        //Clear List of tools
        this->m_LoadedTools.clear();
    }

    OptitrackTracker::~OptitrackTracker()
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]\n");

        ResultType result;
        // If device is in Tracking mode, stop the Tracking first
        if (this->GetState() == STATE_TRACKER_Tracking)
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: in Tracking State -> Stopping Tracking \n");
            result = this->StopTracking();

            if(result == SUCCESS){
                fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: Device Stopped \n");
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::~OptitrackTracker]: Error Stopping the Device\n");
            }
        }

        // Call InternalClose first
        if (this->GetState() != STATE_TRACKER_Idle)
        {
            //fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: Calling InternalClose \n");
            result = this->Close();

            if(result == SUCCESS)
            {
                fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: InternalClose SUCCESS \n");
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::~OptitrackTracker]: Error Closing the Device\n");
            }
        }

        // Change State to Setup
        fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: OptitrackTracker deleted successfully \n");
    }

	/*! \brief Initialization of the system.
	* 
	* This function allows the system to be initialize by moving the rigid body to Communication Established state.
	* a normal member taking two arguments and returning an integer value.
	* 
	* @return Result of the system initialization: SUCCESS or FAILURE.
	*/
	ResultType OptitrackTracker::Open( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::InternalOpen]\n");

        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToEstablishCommunication);

        if(previous_state == STATE_TRACKER_CommunicationEstablished)
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalOpen]: System was initialized before\n");
            this->SetState(STATE_TRACKER_CommunicationEstablished);
            return SUCCESS;
        }
		else if (previous_state == STATE_TRACKER_Idle)
		{
			NPRESULT result = TT_Initialize();

			if (result == NPRESULT_SUCCESS)
			{
				this->SetState(STATE_TRACKER_CommunicationEstablished);
				fprintf(stdout, "<INFO> - [OptitrackTracker::InternalOpen]: System was Initialized\n");
				Sleep(30);
				return SUCCESS;
			}
			else
			{
				this->SetState(STATE_TRACKER_Idle);
				fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalOpen]: TT_Initialize failed\n");
				return FAILURE;
			}
		}
		else
		{
			this->SetState(previous_state);
			fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalOpen]: System cannot be initialized from that state!\n");
			return FAILURE;
		}
    }

	/*! \brief Load the calibration file.
	*
	* This function allows the system to load the calibration file of the OptiTrack system. For a correct loading of the file
	* the system must be initialized and the file must be optimum (.cal extension).
	*
	* @return Result of calibration file loading: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::LoadCalibration( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::LoadCalibration]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToLoadCalibration);

        if(previous_state != STATE_TRACKER_CommunicationEstablished)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::LoadCalibration]: System has not been Initialized/Open\n");
            this->SetState(previous_state);
            return FAILURE;
        }

        if(this->GetCalibrationFile().empty()){
            fprintf(stdout, "#ERROR# - [OptitrackTracker::LoadCalibration]: Calibration File is empty\n");
            this->SetState(STATE_TRACKER_CommunicationEstablished);
            return FAILURE;
        }

        for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
        {
            NPRESULT resultLoadCalibration = TT_LoadCalibration(this->GetCalibrationFile().c_str());

            if(resultLoadCalibration != NPRESULT_SUCCESS)
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::LoadCalibration]: TT_LoadCalibration failed\n");
                this->SetState(STATE_TRACKER_CommunicationEstablished);
                return FAILURE;
            }
            else
            {
                fprintf(stdout, "<INFO> - [OptitrackTracker::LoadCalibration]: Calibration was successfully loaded\n");
                this->SetState(STATE_TRACKER_CalibratedState);
				Sleep(30);
                return SUCCESS;
            }

        }
    }

	/*! \brief Shutdown of the system.
	*
	* This function closes the system. First, it stops the tracking Then, the system is shutdown. Finally, the function shuts 
	* down the camera device driver and ensures all the driver threads are terminated properly.
	* 
	* @return Result of the system shutdown: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::Close( void )
    {
		//fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToCloseCommunication);

		if ((previous_state == STATE_TRACKER_CommunicationEstablished) || (previous_state == STATE_TRACKER_CalibratedState))
		{

			//fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]: Stopping the Tracking\n");
			//ResultType resultStop = this->StopTracking();
			//if (resultStop == FAILURE)
			//{
			//	fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalClose]: Cannot Stop the Tracking\n");
			//	this->SetState(previous_state);
			//	return FAILURE;
			//}


			for (int i = OPTITRACK_ATTEMPTS; i > 0; i--)
			{
				TT_ClearTrackableList();
				NPRESULT resultShutdown = TT_Shutdown();

				if (resultShutdown == NPRESULT_SUCCESS)
				{
					this->m_LoadedTools.clear();
					fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]: System has been Shutdown Correctly\n");
					Sleep(2000);
					this->SetState(STATE_TRACKER_Idle);

					NPRESULT resultFinalCleanup = TT_FinalCleanup();
					if (resultFinalCleanup == NPRESULT_SUCCESS)
					{
						return SUCCESS;
					}
					fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalClose]: FinalCleanUp was not performed correctly!\n");
					return FAILURE;
				}
				else
				{
					fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]: System cannot ShutDown now. Trying again...\n");
				}
			}

			fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalClose]: System cannot ShutDown now\n");
			this->SetState(previous_state);
			return FAILURE;

		}
		else
		{
			fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalClose]: System cannot be closed from previous state!\n");
			this->SetState(previous_state);
			return FAILURE;
		}
    }

	/*! \brief Reset of the system.
	*
	* This function resets the system. The tracking is stopped and all currently loaded rigid body definitions are removed. This resetting enables the 
	* system to load a new calibration file.
	*
	* @return Result of the system reset: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::Reset( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::InternalReset]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToReset);

        if(previous_state != STATE_TRACKER_Tracking)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalReset]: System cannot be Reset from other State but Tracking\n");
            this->SetState(previous_state);
            return FAILURE;
        }
        else
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalReset]: Stopping the Tracking and Reset to calibration\n");
            ResultType resultStop = this->StopTracking();
            if(resultStop == FAILURE)
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalReset]: Cannot Stop the Tracking\n");
                this->SetState(previous_state);
                return FAILURE;
            }

			TT_ClearTrackableList(); //Añadido por DAVID
			this->SetState(STATE_TRACKER_CommunicationEstablished);
            return SUCCESS;
        }
    }

	/*! \brief Addition of tool to the system.
	*
	* This function allows to add a tool to the system. The tool is attached and enabled.
	*
	* @param OptiTrackTool object corresponding to the tool which wants to be added.
	* @return Result of the tool addition: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::AddTrackerTool( OptitrackTool::Pointer trackerTool )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::AddTrackerTool]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToAttachTrackerTool);

        if(previous_state == STATE_TRACKER_CalibratedState) 
        {
            ResultType resultAttach = trackerTool->AttachTrackable();

            if( (resultAttach == SUCCESS))
            {
                this->m_LoadedTools.push_back(trackerTool);
				fprintf(stdout, "<INFO> - [OptitrackTracker::AddTrackerTool]: Tool %s Added to the InternalContainer\n", trackerTool->GetToolName().c_str());
				this->SetState(STATE_TRACKER_CalibratedState);
                return SUCCESS;
            }
            else
            {
				fprintf(stdout, "#ERROR# - [OptitrackTracker::AddTrackerTool]: System cannot attach tool %s from previous state\n", trackerTool->GetToolName().c_str());
                this->SetState(previous_state);
                return FAILURE;
            }
        }
        else
        {
			fprintf(stdout, "#ERROR# - [OptitrackTracker::AddTrackerTool]: System cannot attach tool %s from previous state\n", trackerTool->GetToolName().c_str());
            this->SetState(previous_state);
            return FAILURE;
        }
    }

	/*! \brief Removal of tool from the system.
	*
	* This function allows to remove a tool from the system. The tool is disabled and dettached.
	*
	* @param OptiTrackTool object corresponding to the tool which wants to be removed.
	* @return Result of the tool removal: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::RemoveTrackerTool( OptitrackTool::Pointer trackerTool )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::RemoveTrackerTool]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToDetachTrackerTool);

        if( previous_state == STATE_TRACKER_CalibratedState )
        {
			ResultType resultDettach = trackerTool->DettachTrackable();

            if( (resultDettach == SUCCESS) )
            {
                int id = trackerTool->GetOptitrackID();
                m_LoadedTools.erase(m_LoadedTools.begin() + id);
                this->SetState(STATE_TRACKER_CalibratedState);
                return SUCCESS;
            }
            else
            {
				fprintf(stdout, "#ERROR# - [OptitrackTracker::RemoveTrackerTool]: System cannot dettach %s tool\n", trackerTool->GetToolName().c_str());
                this->SetState(previous_state);
                return FAILURE;
            }

        }
        else
        {
			fprintf(stdout, "#ERROR# - [OptitrackTracker::RemoveTrackerTool]: System cannot dettach tool %s from previous state\n", trackerTool->GetToolName().c_str());
            this->SetState(previous_state);
            return FAILURE;
        }
    }

	/*! \brief Obtain number of tools attached to the system.
	*
	* This function provides the number of tools which are attached to the system.
	*
	* @return Number of tools attached (unsigned int).
	*/
    unsigned int OptitrackTracker::GetNumberOfAttachedTools( void )
    {
        MutexLockHolder lock(*m_ToolsMutex);
        return this->m_LoadedTools.size();
    }

	/*! \brief Start the tracking of tools.
	*
	* This function starts the tracking of all tools attached by launching a thread.
	*
	* @return Result of the tracking initialization: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::StartTracking( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::StartTracking]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToStartTracking);

        // Check if there is at least a tool to be tracked
        if(this->GetNumberOfAttachedTools() < 1)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::StartTracking]: Number of Attached Tools is 0\n");
            this->SetState(previous_state);
            return FAILURE;
        }
        else
        {
            if(previous_state == STATE_TRACKER_CalibratedState) 
            {

                fprintf(stdout, "<INFO> - [OptitrackTracker::InternalStartTracking]: Tracking...\n");
                // Change the m_StopTracking Variable to false
                this->m_StopTrackingMutex->Lock();
                this->m_StopTracking = false;
                this->m_StopTrackingMutex->Unlock();
                this->m_TrackingFinishedMutex->Unlock(); // transfer the execution rights to tracking thread

                // Launch multiThreader using the Function ThreadStartTracking that executes the TrackTools() method
                m_ThreadID = m_MultiThreader->SpawnThread(this->ThreadStartTracking, this);
                this->SetState(STATE_TRACKER_Tracking);
                return SUCCESS;

            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalStartTracking]: Previous State is not valid to Start Tracking\n");
                this->SetState(previous_state);
                return FAILURE;
            }

        }
    }

	/*! \brief Tracking of attached tools.
	*
	* This function tracks all tools attached to the system.
	*/
    void OptitrackTracker::TrackTools()
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::TrackTools]\n");

        try
        {
            bool localStopTracking;       // Because m_StopTracking is used by two threads, access has to be guarded by a mutex. To minimize thread locking, a local copy is used here
            this->m_StopTrackingMutex->Lock();  // update the local copy of m_StopTracking ITK
            localStopTracking = this->m_StopTracking;

            /* lock the TrackingFinishedMutex to signal that the execution rights are now transfered to the tracking thread */
            if (!localStopTracking)
            {
                m_TrackingFinishedMutex->Lock();
            }

            this->m_StopTrackingMutex->Unlock();
            while ((this->GetState() == STATE_TRACKER_Tracking) && (localStopTracking == false))
            {
				if (TT_Update() == NPRESULT_SUCCESS)
				{
					for (unsigned int i = 0; i < this->GetNumberOfAttachedTools(); ++i)  // use mutexed methods to access tool container
					{
						OptitrackTool::Pointer currentTool = this->GetOptitrackTool(i);
						if (currentTool != nullptr)
						{
							currentTool->UpdateTool();
							//fprintf(stdout, "Updating tool named: %s", currentTool->GetToolName().c_str());
						}
						else
						{
							//std::cout << "Get data from tool number " << i << " failed" << std::endl; TODO
						}
					}

					/* Update the local copy of m_StopTracking */
					this->m_StopTrackingMutex->Lock();
					localStopTracking = this->m_StopTracking;
					this->m_StopTrackingMutex->Unlock();
					Sleep(2);
				}
				else{
					//fprintf(stdout, "#ERROR# - [OptitrackTracker::TrackTools]: Update Failed");
				}
            }

            m_TrackingFinishedMutex->Unlock(); // transfer control back to main thread
        }
        catch(...)
        {
            m_TrackingFinishedMutex->Unlock();
            fprintf(stdout, "#ERROR# - [OptitrackTracker::TrackTools]: Error while trying to track tools. Thread stopped.\n");
            this->StopTracking();
            this->SetState(STATE_TRACKER_CalibratedState);
            return;
        }
    }

	/*! \brief Tracking thread.
	*
	* This function handles the ITK thread for the tracking.
	*
	* @return Return 0.
	*/
    ITK_THREAD_RETURN_TYPE OptitrackTracker::ThreadStartTracking(void* pInfoStruct)
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::ThreadStartTracking]\n");

        /* extract this pointer from Thread Info structure */
        struct itk::MultiThreader::ThreadInfoStruct * pInfo = (struct itk::MultiThreader::ThreadInfoStruct*)pInfoStruct;

        if (pInfo == NULL)
        {
            return ITK_THREAD_RETURN_VALUE;
        }

        if (pInfo->UserData == NULL)
        {
            return ITK_THREAD_RETURN_VALUE;
        }

            OptitrackTracker *trackingDevice = static_cast<OptitrackTracker*>(pInfo->UserData);

        if (trackingDevice != NULL)
        {
            // Call the TrackTools function in this thread
            trackingDevice->TrackTools();
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::ThreadStartTracking]: trackingDevice is NULL\n");
        }

        trackingDevice->m_ThreadID = -1; // reset thread ID because we end the thread here
        return ITK_THREAD_RETURN_VALUE;

        // Delete next when thread is uncommented
        return 0;
    }

	/*! \brief Stop the tracking of tools.
	*
	* This function stops the tracking of tools.
	*
	* @return Result of the tracking stoppage: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::StopTracking( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::InternalStopTracking]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToStopTracking);


        if( previous_state == STATE_TRACKER_Tracking ||
            previous_state == STATE_TRACKER_AttemptingToCloseCommunication ||
            previous_state == STATE_TRACKER_AttemptingToReset)
        {

            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalStopTracking]: Stopping...\n");
            //Change the StopTracking value
            m_StopTrackingMutex->Lock();
            m_StopTracking = true;
            m_StopTrackingMutex->Unlock();
            m_TrackingFinishedMutex->Lock();
			this->SetState(STATE_TRACKER_CommunicationEstablished);
            return SUCCESS;

        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalStopTracking]: Previous State is not valid to Stop Tracking\n");
            this->SetState(previous_state);
            return FAILURE;
        }
    }

	/*! \brief Setting of camera parameter.
	*
	* This function allows to set some of the camera settings to the whole set of cameras, such as exposure, threshold, illumination, and video mode.	*
	* @param exposure: Valid values are 1-480.
	* @param threshold: Pixels with intensities darker than this value will be filtered out when using processed video modes. Valid values are 0-255.
	* @param intensity: This should be set to 15 for almost all situations. The recommended method for reducing IR LED brightness is to lower the
    * camera exposure setting, this has the effect of shortening the IR strobe duration. Valid values are 0-15.	* @param videoType: The desired in-camera video processing mode. Valid values are: Segment Mode (0), Grayscale Mode (1), Object Mode (2), 
	* Precision Mode (4), and MJPEG Mode (V100R2 only) (6).
	* @return Result of the camera parameters setting: SUCCESS or FAILURE.
	*/
    ResultType OptitrackTracker::SetCameraParams(int exposure, int threshold , int intensity, int videoType )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::SetCameraParams]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToSetCameraParams);

        int resultUpdate;
        bool resultSetCameraSettings;

        unsigned int camera_number = this->GetCameraNumber();
		fprintf(stdout, "<INFO> - [OptitrackTracker::SetCameraParams]: Number of Cameras #%d\n", camera_number);
        // If no cameras are connected
        if(camera_number == 0)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::SetCameraParams]: Number of Connected Cameras is 0\n");
            this->SetState(previous_state);
            return FAILURE;
        }

        for(int cam = 0; cam < camera_number; cam++) // for all connected cameras
        {
            for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
            {

                resultSetCameraSettings = TT_SetCameraSettings(cam,videoType,exposure,threshold,intensity);

                if(resultSetCameraSettings)
                {
                    this->SetVideoType(videoType);
                    this->SetExp(exposure);
                    this->SetThr(threshold);
                    this->SetLed(intensity);
                    fprintf(stdout, "<INFO> - [OptitrackTracker::SetCameraParams]: Camera #%d params are set\n",cam);
                    i = 0; // End attempts for camera #cam
                }
                else
                {
                    if(i == 1)
                    {
                        fprintf(stdout, "#ERROR# - [OptitrackTracker::SetCameraParams]: Carmera #%d failed\n",cam);
                        this->SetState(previous_state);
                        return FAILURE;
                    }
                }
            }
        }

        fprintf(stdout, "<INFO> - [OptitrackTracker::SetCameraParams]: Succsess\n");
        this->SetState(previous_state);
        return SUCCESS;
    }

	/*! \brief Get the number of connected cameras.
	*
	* This function provides the number of connected cameras.
	*
	* @return Number of connected cameras (unsigned int).
	*/
    unsigned int OptitrackTracker::GetCameraNumber( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::GetCameraNumber]\n");
        this->m_CameraNumber = 0;
        int resultUpdate;

        for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
        {

			this->m_CameraNumber = TT_CameraCount();
			i = 0;

        }

        return this->m_CameraNumber;
    }

	OptitrackTool::Pointer OptitrackTracker::GetOptitrackTool( unsigned int toolID)
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackTool]\n");
        OptitrackTool::Pointer t = nullptr;

        MutexLockHolder toolsMutexLockHolder(*m_ToolsMutex); // lock and unlock the mutex ITK
        if(toolID < this->GetNumberOfAttachedTools())
        {
            t = m_LoadedTools.at(toolID);
            //fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackTool]: Selected tool #%d\n",toolID);
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::GetOptitrackTool]: Tool Number %d does not exist\n",toolID);
        }

        return t;
    }

    OptitrackTool::Pointer OptitrackTracker::GetOptitrackToolByName( std::string toolName )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackToolByName]\n");
        OptitrackTool::Pointer t = nullptr;


        unsigned int toolCount = this->GetNumberOfAttachedTools();

        for (unsigned int i = 0; i < toolCount; ++i)
        {
                if (toolName == this->GetOptitrackTool(i)->GetToolName())
                {
                    //fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackToolByName]: Selected tool %s\n",toolName.c_str());
                    return this->GetOptitrackTool(i);
                }
        }


		fprintf(stdout, "#ERROR# - [OptitrackTracker::GetOptitrackToolByName]: Tool Named %s does not exist\n", toolName.c_str());
        return NULL;
    }

	void sleep(unsigned int mseconds)
	{
		clock_t goal = mseconds + clock();
		while (goal > clock());
	}

	ResultType OptitrackTracker::CheckNumberOfMarkers(void)
	{
		// Check number of Cameras
		int numberOfCameras = TT_CameraCount();

		// Variables for API results
		int resultUpdate, markerCount, cameraSettingsChanged, numberFailTrackingEvents;
		int iterations = 0, cameraMarkerCount;
		int maxIterationNumber = 50;

		int *trackingFailureArray = new int[numberOfCameras];
		std::fill_n(trackingFailureArray, numberOfCameras, 0);

		while (iterations < maxIterationNumber)
		{
			Sleep(5);
			resultUpdate = TT_UpdateSingleFrame();

			for (int cameraIndex = 0; cameraIndex < numberOfCameras; cameraIndex++)
			{
				cameraMarkerCount = TT_CameraMarkerCount(cameraIndex);
				//std::cout << " TT_CameraMarkerCount result: " << cameraMarkerCount << " (Camera " << cameraIndex << ")" << std::endl;

				if (cameraMarkerCount != 1)
				{
					trackingFailureArray[cameraIndex] = trackingFailureArray[cameraIndex] + 1;
				}

			}
			iterations++;
		}

		for (int cameraIndex = 0; cameraIndex < numberOfCameras; cameraIndex++){
			if ((trackingFailureArray[cameraIndex] / 50) >= 0.3)
			{
				// Camera pair is watching more than one marker
				fprintf(stdout, "Camera %i is not visualizing just one marker.\n", cameraIndex);
				fprintf(stdout, "[ABORTING]: Please make sure only one marker is visible in the field of view of each camera.\n");
				return ResultType::FAILURE;
			}
		}

		return ResultType::SUCCESS;
    }

	int CameraCorrespondeceBetweenAPIandTrackingTools(int numberOfCameras, int CameraNumber)
	{
		if (numberOfCameras == 8)
		{
			switch (CameraNumber)
			{
			case 1:
				return 8;
			case 2:
				return 7;
			case 3:
				return 6;
			case 4:
				return 5;
			case 5:
				return 2;
			case 6:
				return 1;
			case 7:
				return 4;
			case 8:
				return 3;
			default:
				return 0;
				break;
			}
		}
		else if (numberOfCameras == 2)
		{
			switch (CameraNumber)
			{
			case 1:
				return 2;
			case 2:
				return 1;
			default:
				return 0;
				break;
			}
		}
		else
		{
			return CameraNumber;
		}
	}

	int GetMarkerPosition2D3D(std::ostream* stream, int numberOfCameras, int Camera1, int Camera2)
	{
		NPRESULT resultUpdate;

		int MarkerCount = 0;
		bool *CameraUsed = new bool[numberOfCameras];
		for (int camIndex = 0; camIndex < numberOfCameras; camIndex++)
		{
			if (camIndex == Camera1 || camIndex == Camera2)
			{
				CameraUsed[camIndex] = true;
			}
			else
			{
				CameraUsed[camIndex] = false;
			}
		}

		int TrackingToolsCamera1 = CameraCorrespondeceBetweenAPIandTrackingTools(numberOfCameras, Camera1 + 1);
		int TrackingToolsCamera2 = CameraCorrespondeceBetweenAPIandTrackingTools(numberOfCameras, Camera2 + 1);

		std::string camera_pair = "_" + std::to_string(TrackingToolsCamera1) + std::to_string(TrackingToolsCamera2);

		float FrameMarkerX, FrameMarkerY, FrameMarkerZ;
		float X1, Y1, X2, Y2 = 0;
		bool Camera1ContributesToMarkerPos, Camera2ContributesToMarkerPos;

		for (int count = 0; count < 1000; count++)
		{
			resultUpdate = TT_Update();
			MarkerCount = TT_FrameMarkerCount();

			if (MarkerCount != 0)
			{
				for (int m = 0; m < MarkerCount; m++)
				{
					FrameMarkerX = TT_FrameMarkerX(m) * 1000;
					FrameMarkerY = TT_FrameMarkerY(m) * 1000;
					FrameMarkerZ = TT_FrameMarkerZ(m) * 1000;

					Camera1ContributesToMarkerPos = TT_FrameCameraCentroid(m, Camera1, X1, Y1);
					Camera2ContributesToMarkerPos = TT_FrameCameraCentroid(m, Camera2, X2, Y2);
					*stream << count << ";" << m << ";" << FrameMarkerX << ";" << FrameMarkerY << ";" << FrameMarkerZ << ";" << TrackingToolsCamera1 << ";" << TrackingToolsCamera2 << ";" << X1 << ";" << Y1 << ";" << X2 << ";" << Y2 << ";" << "Pair" << "\n";
				}
			}
			else
			{
				fprintf(stdout, "No markers detected \n");
				*stream << count << ";" << "NoMarkers" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << TrackingToolsCamera1 << ";" << TrackingToolsCamera2 << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "Pair" << "\n";
			}

			sleep(5);
		}

		return resultUpdate;
	}

	ResultType OptitrackTracker::TestCalibration(std::string FileName)
	{
		fprintf(stdout, "<INFO> - [OptitrackTracker::TestCalibration]\n");
		OPTITRACK_TRACKER_STATE previous_state = this->GetState();
		this->SetState(STATE_TRACKER_AttemptingToTestCalibration);


		if (previous_state == STATE_TRACKER_CalibratedState)
		{

			int numberOfCameras = TT_CameraCount();

			// Prepare the file
			std::ostream* stream; ///< the output stream
			stream = new std::ofstream(FileName);
			stream->precision(10);
			//File header
			*stream << "TimeStamp" << ";MarkerIndex" << ";X_3D" << ";Y_3D" << ";Z_3D" << ";CameraUsed1" << ";CameraUsed2" << ";XCam1" << ";YCam1" << ";XCam2" << ";YCam2" << ";CameraPair" << "\n";

			// Track All cameras
			for (int i = 0; i < numberOfCameras; i++)
			{
				//bool CameraSettingsChanged = TT_SetCameraSettings(i, this->GetVideoType(), this->GetExp(), this->GetThr(), this->GetLed());
				bool CameraSettingsChanged = TT_SetCameraSettings(i, 4, 7, 200, 15);

				std::cout << "Camera settings of all cameras have been set!:" << std::endl;
				std::cout << "  -Video type:" << this->GetVideoType() << std::endl;
				std::cout << "  -Exposure:" << this->GetExp() << std::endl;
				std::cout << "  -Threshold:" << this->GetThr() << std::endl;
				std::cout << "  -LED:" << this->GetLed() << std::endl;
			}

			int resultUpdate, MarkerCount;
			float FrameMarkerX, FrameMarkerY, FrameMarkerZ;

			for (int count = 0; count < 1000; count++)
			{
				sleep(5); 
				resultUpdate = TT_Update();
				std::cout << "Result Update:" << resultUpdate << std::endl;
				MarkerCount = TT_FrameMarkerCount();
				std::cout << "Frame marker count:" << MarkerCount << std::endl;

				if (MarkerCount != 0)
				{
					for (int m = 0; m < MarkerCount; m++)
					{
						FrameMarkerX = TT_FrameMarkerX(m) * 1000;
						FrameMarkerY = TT_FrameMarkerY(m) * 1000;
						FrameMarkerZ = TT_FrameMarkerZ(m) * 1000;
						*stream << count << ";" << m << ";" << FrameMarkerX << ";" << FrameMarkerY << ";" << FrameMarkerZ << ";" << "-1" << ";" << "-1" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "All" << "\n";
					}
				}
				else
				{
					fprintf(stdout, "No markers detected \n");
					*stream << count << ";" << "NoMarkers" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "-1" << ";" << "-1" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "All" << "\n";
				}

			}


			// Track Camera Pairs
			for (int i = 0; i < numberOfCameras - 1; i++)
			{
				//Set a 1st camera to normal threshold level and normal exp level so the camera can see the markers
				bool CameraSettingsChanged = TT_SetCameraSettings(i, 4, 7, 200, 15);


				for (int j = i + 1; j < numberOfCameras; j++)
				{
					//Set a 2nd camera to normal threshold level and normal exp level so the camera can see the markers
					CameraSettingsChanged = TT_SetCameraSettings(j, 4, 7, 200, 15);

					fprintf(stdout, "Starting tracking for pair %i - %i \n", i, j);
					GetMarkerPosition2D3D(stream, numberOfCameras, i, j);

					//Set 2nd camera to highest threshold level (255) and minimum exp level (1) so the camera can see the markers
					CameraSettingsChanged = TT_SetCameraSettings(j, 2, 1, 255, 15);

				}

				//Set 1st camera to highest threshold level (255) and minimum exp level (1) so the camera cannot see the markers
				CameraSettingsChanged = TT_SetCameraSettings(i, 2, 1, 255, 15);

			}

			stream->flush();
			this->SetState(previous_state);
			return SUCCESS;

		}
		else
		{
			fprintf(stdout, "#ERROR# - [OptitrackTracker::TestCalibration]: Previous State is not valid to Perform Test\n");
			this->SetState(previous_state);
			return FAILURE;
		}


	}

    ResultType OptitrackTracker::LoadXMLConfigurationFile(std::string nameFile)
    {
        //== XML Configuration File reading

        std::string calibrationFile, toolFile;
        float camparamExposure, camparamThreshold, camparamIntensity;
        int toolNumber;
        const int maxNumberTools = 10;
        std::string toolFilesArray[maxNumberTools];

        tinyxml2::XMLDocument xmlDoc;
        char* char_Path = (char*)nameFile.c_str(); //Conversion from string to char*.
        tinyxml2::XMLError eResult = xmlDoc.LoadFile(char_Path);
        XMLCheckResult(eResult);
        if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "[XML READING ERROR] Problem loading the file! %s\n", nameFile);
            return FAILURE;

        }

        tinyxml2::XMLElement * pRoot = xmlDoc.FirstChildElement("ConfigurationFile");
        if (pRoot == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_FILE_READ_ERROR;
        XMLCheckResult(eResult);
        if (eResult != tinyxml2::XMLError::XML_SUCCESS){
            fprintf(stdout, "[XML READING ERROR] Problem accesing to NPTrackingTools element! \n");
            return FAILURE;

        }

        //CalibrationFile
        tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("CalibrationFile");
        if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
        XMLCheckResult(eResult);
        if (eResult != tinyxml2::XMLError::XML_SUCCESS){
            fprintf(stdout, "[XML READING ERROR] Problem parsing the element CalibrationFile! \n");
            return FAILURE;

        }
        try
        {
            calibrationFile = pElement->Attribute("file");
        }
        catch (int e)
        {
            fprintf(stdout, "[XML READING ERROR] Problem reading attribute File from CalibrationFile element! \n");
            std::cout << e << std::endl;
            return FAILURE;
        }

        //CameraParameters
        pElement = pRoot->FirstChildElement("CameraParameters");
        if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
        if (eResult == tinyxml2::XMLError::XML_SUCCESS){ //If CalibrationError element do not exist, attributes are not accessed.
            eResult = pElement->QueryFloatAttribute("Exposure", &camparamExposure);
            XMLCheckResult(eResult);

            eResult = pElement->QueryFloatAttribute("Threshold", &camparamThreshold);
            XMLCheckResult(eResult);

            eResult = pElement->QueryFloatAttribute("Intensity", &camparamIntensity);
            XMLCheckResult(eResult);
        }


        //ToolNumber
        pElement = pRoot->FirstChildElement("ToolNumber");
        if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
        XMLCheckResult(eResult);
        if (eResult != tinyxml2::XMLError::XML_SUCCESS){
            fprintf(stdout, "[XML READING ERROR] Problem parsing the element ToolMarkersNum! \n");
            return FAILURE;

        }
        eResult = pElement->QueryIntAttribute("Ntool", &toolNumber);
        XMLCheckResult(eResult);

        //Tools
        pElement = pRoot->FirstChildElement("ToolsList");
        if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
        XMLCheckResult(eResult);
        if (eResult != tinyxml2::XMLError::XML_SUCCESS){
            fprintf(stdout, "[XML READING ERROR] Problem parsing the element Tools! \n");
            return FAILURE;
        }
        int counter = 0;
        tinyxml2::XMLElement * pListElement = pElement->FirstChildElement("Tool");
        while ((counter < toolNumber) && (pListElement != nullptr))
        {
            try
            {
                toolFile = pListElement->Attribute("confile");
                toolFilesArray[counter] = toolFile;
            }
            catch (int e)
            {
                fprintf(stdout, "[XML READING ERROR] Problem reading attribute File from CalibrationFile element! \n");
                std::cout << e << std::endl;
                return FAILURE;
            }

            counter = counter + 1;

            pListElement = pListElement->NextSiblingElement("Tool");

        }

        //== Tracking system initialization
        this->SetCalibrationFile(calibrationFile);
        int result = this->Open();
        result = this->LoadCalibration();

        for (int i = 0; i < toolNumber; i++)
        {
            Optitrack::OptitrackTool::Pointer objTool = Optitrack::OptitrackTool::New();
            objTool->ConfigureToolByXmlFile(toolFilesArray[i]);
            result = this->AddTrackerTool(objTool);
        }

        for (int i = 0; i < 3; i++){
            result = this->StartTracking();
            Sleep(100);
            result = this->StopTracking();
            system("PAUSE");
        }

        return SUCCESS;
    }

	vnl_vector_fixed<double, 3> OptitrackTracker::Pivoting( unsigned int optitrackID, unsigned int sampleNumber)
	{
		fprintf(stdout, "<INFO> - [OptitrackHelper::Pivoting]\n");
		vnl_vector_fixed<double, 3> PivotOffset;

		Optitrack::OptitrackTracker::Pointer tracker = this;

		if (tracker->GetState() != OptitrackTracker::STATE_TRACKER_Tracking)
		{
			fprintf(stdout, "#ERROR# - [OptitrackHelper::Pivoting]: tracker is not tracking!\n");
			PivotOffset[0] = 0;
			PivotOffset[1] = 0;
			PivotOffset[2] = 0;
		}
		else
		{
			fprintf(stdout, "<INFO> - [OptitrackHelper::Pivoting]: Acquiring positions\n");
			vnl_sparse_matrix<double> A(3 * sampleNumber, 6);
			vnl_matrix<double> T_acquired(3 * sampleNumber, 4);
			vnl_vector<double> b(3 * sampleNumber, 1);
			b = 0;

			OptitrackTool::Pointer mytool = tracker->GetOptitrackTool(optitrackID);

			vcl_vector<int> cols(6);
			vcl_vector<double> vals(6);
			vnl_matrix<double> T;
			for (unsigned int q = 0; q < 3 * sampleNumber; q = q + 3)
			{
				Sleep(500);
				//fprintf(stdout, "Sample %d", (q / 3) + 1);
				//Sleep(1000);

				T = mytool->GetTransformMatrix();

				//TEST DAVID
				fprintf(stdout, "<test David> T(0,0)=%f T(0,1)=%f T(0,2)=%f T(0,3)=%f \n", T[0][0], T[0][1], T[0][2], T[0][3]);
				fprintf(stdout, "<test David> T(1,0)=%f T(1,1)=%f T(1,2)=%f T(1,3)=%f \n", T[1][0], T[1][1], T[1][2], T[1][3]);
				fprintf(stdout, "<test David> T(2,0)=%f T(2,1)=%f T(2,2)=%f T(2,3)=%f \n", T[2][0], T[2][1], T[2][2], T[2][3]);
				fprintf(stdout, "<test David> T(3,0)=%f T(3,1)=%f T(3,2)=%f T(3,3)=%f \n", T[3][0], T[3][1], T[3][2], T[3][3]);

				// Save matrix
				for (unsigned int i = 0; i < 3; i++) //DAVID: he cambiado i < 6 por i < 4
				{
					for (unsigned int j = 0; j < 4; j++)//DAVID: he cambiado j < 3 por j < 4
					{
						T_acquired[i + q][j] = T.get(i, j);
					}
				}




				for (unsigned int _c = 0; _c < 6; _c++)
				{
					cols[_c] = _c;
				}


				vals[0] = T[0][0];
				vals[1] = T[0][1];
				vals[2] = T[0][2];
				vals[3] = -1.0;
				vals[4] = 0.0;
				vals[5] = 0.0;

				A.set_row(q, cols, vals);

				vals[0] = T[1][0];
				vals[1] = T[1][1];
				vals[2] = T[1][2];
				vals[3] = 0.0;
				vals[4] = -1.0;
				vals[5] = 0.0;

				A.set_row(q + 1, cols, vals);

				vals[0] = T[2][0];
				vals[1] = T[2][1];
				vals[2] = T[2][2];
				vals[3] = 0.0;
				vals[4] = 0.0;
				vals[5] = -1.0;


				A.set_row(q + 2, cols, vals);

				b[q] = -T[0][3];
				b[q + 1] = -T[1][3];
				b[q + 2] = -T[2][3];
			}

			// Solve the Linear System by LSQR
			vnl_sparse_matrix_linear_system<double> linear_system(A, b);
			vnl_lsqr lsqr(linear_system);
			vnl_vector<double> result(6);
			lsqr.minimize(result);
			lsqr.diagnose_outcome(vcl_cerr);

			// Calculate residuals

			vnl_vector<double> residuals(3 * sampleNumber, 1);
			A.mult(result, residuals);
			residuals = residuals - b;

			// Filtering Outliers 95% 2*sd
			double sd_residualsx = 0.0;
			double sd_residualsy = 0.0;
			double sd_residualsz = 0.0;
			for (unsigned int q = 0; q < 3 * sampleNumber; q = q + 3)
			{
				residuals[q] = std::abs(residuals[q]);
				residuals[q + 1] = std::abs(residuals[q + 1]);
				residuals[q + 2] = std::abs(residuals[q + 2]);

				sd_residualsx = sd_residualsx + residuals[q] * residuals[q];
				sd_residualsy = sd_residualsy + residuals[q + 1] * residuals[q + 1];
				sd_residualsz = sd_residualsz + residuals[q + 2] * residuals[q + 2];

			}
			// Limit of 2*sd -> 95% of the sample
			sd_residualsx = 2 * std::sqrt(sd_residualsx);
			sd_residualsy = 2 * std::sqrt(sd_residualsy);
			sd_residualsz = 2 * std::sqrt(sd_residualsz);

			vnl_vector<int> valid_data(sampleNumber, 1);
			unsigned int i = 0;
			for (unsigned int q = 0; q < 3 * sampleNumber; q = q + 3)
			{
				if ((sd_residualsx >= residuals[q]) &&
					(sd_residualsy >= residuals[q + 1]) &&
					(sd_residualsz >= residuals[q + 2])) //DAVID: cambio <= por >=
				{
					// Data is valid
					valid_data[i] = 1;

				}
				else
				{
					valid_data[i] = 0;
				}

				i++;
			}

			// Recalculate the result for the filtered points
			int number_valids = valid_data.one_norm();

			vnl_sparse_matrix<double> A_2(3 * number_valids, 6);
			vnl_vector<double> b_2(3 * number_valids, 1);
			b_2 = 0;

			int q = 0;
			for (unsigned int i = 0; i < number_valids; i = i++)
			{
				if (valid_data[i] == 1)
				{
					// Data valid, include

					for (unsigned int i = 0; i < 3; i++) //David: he cambiado i<6 por i<3
					{
						for (unsigned int j = 0; j < 4; j++)
						{
							T[i][j] = T_acquired[i + q][j];//David: he cambiado i*3 por i+q
						}
					}

					for (unsigned int _c = 0; _c <= 5; _c++)
					{
						cols[_c] = _c;
					}

					vals[0] = T[0][0];
					vals[1] = T[0][1];
					vals[2] = T[0][2];
					vals[3] = -1.0;
					vals[4] = 0.0;
					vals[5] = 0.0;

					A_2.set_row(q, cols, vals);

					vals[0] = T[1][0];
					vals[1] = T[1][1];
					vals[2] = T[1][2];
					vals[3] = 0.0;
					vals[4] = -1.0;
					vals[5] = 0.0;

					A_2.set_row(q + 1, cols, vals);

					vals[0] = T[2][0];
					vals[1] = T[2][1];
					vals[2] = T[2][2];
					vals[3] = 0.0;
					vals[4] = 0.0;
					vals[5] = -1.0;


					A_2.set_row(q + 2, cols, vals);

					b_2[q] = -T[0][3];
					b_2[q + 1] = -T[1][3];
					b_2[q + 2] = -T[2][3];


					q = q + 3;
				}
				else
				{
					// Data no valid, not include
				}
			}

			// Solve the Linear System by LSQR
			vnl_sparse_matrix_linear_system<double> linear_system_2(A_2, b_2);
			vnl_lsqr lsqr_2(linear_system_2);
			vnl_vector<double> result_2(6);
			lsqr_2.minimize(result_2);
			lsqr_2.diagnose_outcome(vcl_cerr);

			PivotOffset[0] = result_2[0];
			PivotOffset[1] = result_2[1];
			PivotOffset[2] = result_2[2];

			fprintf(stdout, "<Result Offset> p1=%f p2=%f p3=%f ", result[0], result[1], result[2]);
			fprintf(stdout, "<Result Offset> p4=%f p5=%f p6=%f ", result[3], result[4], result[5]);
			fprintf(stdout, "<Result 2 Offset> p1=%f p2=%f p3=%f ", result_2[0], result_2[1], result_2[2]);
			fprintf(stdout, "<Result 2 Offset> p4=%f p5=%f p6=%f ", result_2[3], result_2[4], result_2[5]);
		}

		return PivotOffset;
	}

}