#include "OptitrackTracker.h"

// NPTrackingTools library
#include <NPTrackingTools.h>

// ITK Libs
#include <itksys/SystemTools.hxx>


// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>
#include <time.h>

//tinyxml2
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
        fprintf(stdout, "<INFO> - [OptitrackTracker::OptitrackTracker]\n");
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
        fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]\n");

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
            fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: Calling InternalClose \n");
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

    ResultType OptitrackTracker::Open( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::InternalOpen]\n");

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

    ResultType OptitrackTracker::LoadCalibration( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::LoadCalibration]\n");
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
                return SUCCESS;
            }

        }
    }

    ResultType OptitrackTracker::Close( void )
    {
		fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToCloseCommunication);

		if ((previous_state == STATE_TRACKER_CommunicationEstablished) || (previous_state == STATE_TRACKER_CalibratedState))
		{

			fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]: Stopping the Tracking\n");
			ResultType resultStop = this->StopTracking();
			if (resultStop == FAILURE)
			{
				fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalClose]: Cannot Stop the Tracking\n");
				this->SetState(previous_state);
				return FAILURE;
			}


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

    ResultType OptitrackTracker::Reset( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::InternalReset]\n");
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

    ResultType OptitrackTracker::AddTrackerTool( OptitrackTool::Pointer trackerTool )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::AddTrackerTool]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToAttachTrackerTool);

        if(previous_state == STATE_TRACKER_CalibratedState) 
        {
            ResultType resultAttach = trackerTool->AttachTrackable();
            ResultType resultEnable = trackerTool->Enable();

            if( (resultAttach == SUCCESS) && (resultEnable == SUCCESS) )
            {
                this->m_LoadedTools.push_back(trackerTool);
                fprintf(stdout, "<INFO> - [OptitrackTracker::AddTrackerTool]: Tool Added to the InternalContainer\n");
				this->SetState(STATE_TRACKER_CalibratedState);
                return SUCCESS;
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::AddTrackerTool]: System cannot attach tool from previous state\n");
                this->SetState(previous_state);
                return FAILURE;
            }
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::AddTrackerTool]: System cannot attach tool from previous state\n");
            this->SetState(previous_state);
            return FAILURE;
        }
    }

    ResultType OptitrackTracker::RemoveTrackerTool( OptitrackTool::Pointer trackerTool )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::RemoveTrackerTool]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToDetachTrackerTool);

        if( previous_state == STATE_TRACKER_CalibratedState )
        {
            ResultType resultDisable = trackerTool->Disable();
			ResultType resultDettach = trackerTool->DettachTrackable();

            if( (resultDettach == SUCCESS) && (resultDisable == SUCCESS) )
            {
                int id = trackerTool->GetOptitrackID();
                m_LoadedTools.erase(m_LoadedTools.begin() + id);
                this->SetState(STATE_TRACKER_CalibratedState);
                return SUCCESS;
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::RemoveTrackerTool]: System cannot dettach tool\n");
                this->SetState(previous_state);
                return FAILURE;
            }

        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::RemoveTrackerTool]: System cannot dettach tool from previous state\n");
            this->SetState(previous_state);
            return FAILURE;
        }
    }

    unsigned int OptitrackTracker::GetNumberOfAttachedTools( void )
    {
        MutexLockHolder lock(*m_ToolsMutex);
        return this->m_LoadedTools.size();
    }

    ResultType OptitrackTracker::StartTracking( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::StartTracking]\n");
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

                if(TT_Update() == NPRESULT_SUCCESS)
                {
                    fprintf(stdout, "<INFO> - [OptitrackTracker::InternalStartTracking]: Ready for Tracking\n");
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
                    fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalStartTracking]: Could not Update System at least once\n");
                    this->SetState(previous_state);
                    return FAILURE;
                }
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalStartTracking]: Previous State is not valid to Start Tracking\n");
                this->SetState(previous_state);
                return FAILURE;
            }

        }
    }

    void OptitrackTracker::TrackTools()
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::TrackTools]\n");

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
                for ( unsigned int i = 0; i < this->GetNumberOfAttachedTools(); ++i)  // use mutexed methods to access tool container
                {
                    OptitrackTool::Pointer currentTool = this->GetOptitrackTool(i);
                    if(currentTool != nullptr)
                    {
                        currentTool->UpdateTool();
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
                Sleep(1);
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

    ITK_THREAD_RETURN_TYPE OptitrackTracker::ThreadStartTracking(void* pInfoStruct)
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::ThreadStartTracking]\n");

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

    ResultType OptitrackTracker::StopTracking( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::InternalStopTracking]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToStopTracking);


        if( previous_state == STATE_TRACKER_Tracking ||
            previous_state == STATE_TRACKER_AttemptingToCloseCommunication ||
            previous_state == STATE_TRACKER_AttemptingToReset)
        {

            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalStopTracking]: Ready for Stop\n");
            //Change the StopTracking value
            m_StopTrackingMutex->Lock();
            m_StopTracking = true;
            m_StopTrackingMutex->Unlock();
            m_TrackingFinishedMutex->Lock();
            this->SetState(STATE_TRACKER_CalibratedState);
            return SUCCESS;

        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalStopTracking]: Previous State is not valid to Stop Tracking\n");
            this->SetState(previous_state);
            return FAILURE;
        }
    }

    ResultType OptitrackTracker::SetCameraParams(int exposure, int threshold , int intensity, int videoType )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::SetCameraParams]\n");
        OPTITRACK_TRACKER_STATE previous_state = this->GetState();
        this->SetState(STATE_TRACKER_AttemptingToSetCameraParams);

        int resultUpdate;
        bool resultSetCameraSettings;

        unsigned int camera_number = this->GetCameraNumber();
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
                resultUpdate = TT_Update(); // Get Update for the Optitrack API

                if(resultUpdate == NPRESULT_SUCCESS)
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
        }

        fprintf(stdout, "<INFO> - [OptitrackTracker::SetCameraParams]: Succsess\n");
        this->SetState(previous_state);
        return SUCCESS;
    }

    unsigned int OptitrackTracker::GetCameraNumber( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::GetCameraNumber]\n");
        this->m_CameraNumber = 0;
        int resultUpdate;

        for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
        {
          resultUpdate = TT_Update(); // Get Update for the Optitrack API
          if(resultUpdate == NPRESULT_SUCCESS)
          {
            this->m_CameraNumber = TT_CameraCount();
            i = 0;
          }
          else
          {
            Sleep(30);
          }
        }

        return this->m_CameraNumber;
    }

    OptitrackTool::Pointer OptitrackTracker::GetOptitrackTool( unsigned int toolID)
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackTool]\n");
        OptitrackTool::Pointer t = nullptr;

        MutexLockHolder toolsMutexLockHolder(*m_ToolsMutex); // lock and unlock the mutex ITK
        if(toolID < this->GetNumberOfAttachedTools())
        {
            t = m_LoadedTools.at(toolID);
            fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackTool]: Selected tool #%d\n",toolID);
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::GetOptitrackTool]: Tool Number %d does not exist\n",toolID);
        }

        return t;
    }

    OptitrackTool::Pointer OptitrackTracker::GetOptitrackToolByName( std::string toolName )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackToolByName]\n");
        OptitrackTool::Pointer t = nullptr;


        unsigned int toolCount = this->GetNumberOfAttachedTools();

        for (unsigned int i = 0; i < toolCount; ++i)
        {
                if (toolName == this->GetOptitrackTool(i)->GetToolName())
                {
                    fprintf(stdout, "<INFO> - [OptitrackTracker::GetOptitrackToolByName]: Selected tool %s\n",toolName);
                    return this->GetOptitrackTool(i);
                }
        }


        fprintf(stdout, "#ERROR# - [OptitrackTracker::GetOptitrackToolByName]: Tool Named %s does not exist\n",toolName);
        return NULL;
    }

	void sleep(unsigned int mseconds)
	{
		clock_t goal = mseconds + clock();
		while (goal > clock());
	}

    ResultType OptitrackTracker::CheckNumberOfMarkers( void )
    {
        // Check number of Cameras
        int numberOfCameras = TT_CameraCount();

        // Variables for API results
        int resultUpdate, markerCount, cameraSettingsChanged;
        bool abort = false;
        bool *CameraUsed = new bool[numberOfCameras];


        for (int camera1 = 0; camera1 < numberOfCameras; camera1++)
        {
            for (int camera2 = camera1 + 1; camera2 < numberOfCameras; camera2++)
            {

                if (abort)
                {
                    return ResultType::FAILURE;
                }

                // Shutdown rest of cameras

                for (int camIndex = 0; camIndex < numberOfCameras; camIndex++)
                {
                    if (camIndex == camera1 || camIndex == camera2)
                    {
                        cameraSettingsChanged = TT_SetCameraSettings(camIndex, this->GetVideoType(), this->GetExp(), this->GetThr(), this->GetLed());
                    }
                    else
                    {
                        // Turn off the camera
                        cameraSettingsChanged = TT_SetCameraSettings(camIndex, 2, 1, 255, 15);
                    }
                }

                // Message
                fprintf(stdout, "Testing Pair: %i - %i\n", camera1, camera2);

                // Read 100 frames and check that only one marker is in the field of view
                for (int count = 0; count < 50; count++)
                {
                    resultUpdate = TT_Update();
                    markerCount = TT_FrameMarkerCount();

                    if (markerCount != 1)
                    {
                        // Camera pair is watching more than one marker
						fprintf(stdout, "Camera Pair: %i - %i is watching %i markers\n", camera1, camera2, markerCount);
                        fprintf(stdout, "[ABORTING]: Please make sure only one marker is visible in the field of view\n");
                        abort = true;
                        break;
                    }


                    sleep(5);
                }


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

					float X1, Y1, X2, Y2 = 0;
					bool Camera1ContributesToMarkerPos = TT_FrameCameraCentroid(m, Camera1, X1, Y1);
					bool Camera2ContributesToMarkerPos = TT_FrameCameraCentroid(m, Camera2, X2, Y2);
					*stream << count << ";" << m << ";" << FrameMarkerX << ";" << FrameMarkerY << ";" << FrameMarkerZ << ";" << TrackingToolsCamera1 << ";" << TrackingToolsCamera2 << ";" << X1 << ";" << Y1 << ";" << X2 << ";" << Y2 << ";" << "\n";
				}
			}
			else
			{
				fprintf(stdout, "No markers detected \n");
				*stream << count << ";" << "NoMarkers" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << TrackingToolsCamera1 << ";" << TrackingToolsCamera2 << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "\n";
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
			*stream << "TimeStamp" << ";MarkerIndex" << ";X_3D" << ";Y_3D" << ";Z_3D" << ";CameraUsed1" << ";CameraUsed2" << ";XCam1" << ";YCam1" << ";XCam2" << ";YCam2" << "\n";

			for (int i = 0; i < numberOfCameras - 1; i++)
			{
				//Set a 1st camera to normal threshold level and normal exp level so the camera can see the markers
				bool CameraSettingsChanged = TT_SetCameraSettings(i, this->GetVideoType(), this->GetExp(), this->GetThr(), this->GetLed());


				for (int j = i + 1; j < numberOfCameras; j++)
				{
					//Set a 2nd camera to normal threshold level and normal exp level so the camera can see the markers
					CameraSettingsChanged = TT_SetCameraSettings(j, this->GetVideoType(), this->GetExp(), this->GetThr(), this->GetLed());

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
            fprintf(stdout, "[XML READING ERROR] Problem loading the file! \n");
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

        //Display results (TEST)
        std::cout << "<INFO TEST DAVID - LOADXMLCONFIGURATIONFILE() >" << std::endl;
        std::cout << "Calibration File: " << calibrationFile << std::endl;
        std::cout << "Camera parameters: " << " [Exposure] " << camparamExposure << " [Threshold] " << camparamThreshold << " [Intensity] " << camparamIntensity << std::endl;
        std::cout << "Number of tools: " << toolNumber << std::endl;
        std::cout << "Tool 1 File: " << toolFilesArray[0] << std::endl;
        std::cout << "Tool 2 File: " << toolFilesArray[1] << std::endl;


        //== Tracking system initialization
        this->SetCalibrationFile(calibrationFile);

        int result = this->Open();
        std::cout << " Open result: " << result << std::endl;
        std::cout << " State -> " << this->GetState() << std::endl;

        result = this->LoadCalibration();
        std::cout << " LoadCalibration result: " << result << std::endl;
        std::cout << " State -> " << this->GetState() << std::endl;

        for (int i = 0; i < toolNumber; i++)
        {
            Optitrack::OptitrackTool::Pointer objTool = Optitrack::OptitrackTool::New();
            objTool->ConfigureToolByXmlFile(toolFilesArray[i]);

            result = this->AddTrackerTool(objTool);
            std::cout << " AddTracker result: " << result << std::endl;
            std::cout << " State -> " << this->GetState() << std::endl;
            std::cout << " Tool State: " << objTool->GetState() << std::endl;
            std::cout << " OptitrackID: " << objTool->GetOptitrackID() << std::endl;
            std::cout << " Tool Name: " << objTool->GetToolName() << std::endl;
        }

        for (int i = 0; i < 3; i++){
            result = this->StartTracking();
            std::cout << " StartTracking result: " << result << std::endl;
            std::cout << " State -> " << this->GetState() << std::endl;
            Sleep(100);
            result = this->StopTracking();
            std::cout << " StopTracking result: " << result << std::endl;
            std::cout << " State -> " << this->GetState() << std::endl;
            system("PAUSE");
        }

        return SUCCESS;
    }
}