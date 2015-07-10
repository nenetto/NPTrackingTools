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

	ResultType OptitrackTracker::LoadXMLConfigurationFile(std::string nameFile)
	{
		std::string calibrationFile;
		float camparamExposure, camparamThreshold, camparamIntensity;
		int toolNumber;


		tinyxml2::XMLDocument xmlDoc;
		char* char_Path = (char*)nameFile.c_str(); //Conversion from string to char*.
		tinyxml2::XMLError eResult = xmlDoc.LoadFile(char_Path);
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "[XML READING ERROR] Problem loading the file! \n");
			return FAILURE;

		}

		tinyxml2::XMLElement * pRoot = xmlDoc.FirstChildElement("NPTrackingTools");
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
			calibrationFile = pRoot->Attribute("file");
		}
		catch (int e)
		{
			fprintf(stdout, "[XML READING ERROR] Problem reading attribute File from CalibrationFile element! \n");
			std::cout << e << std::endl;
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
		eResult = pElement->QueryIntAttribute("NTool", &toolNumber);
		XMLCheckResult(eResult);

		//Tools
		/*
		pElement = pRoot->FirstChildElement("ToolMarkers");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
		fprintf(stdout, "[XML READING ERROR] Problem parsing the element ToolMarkers! \n");
		return FAILURE;

		}
		int counter = 0, markerLocation = 0;
		tinyxml2::XMLElement * pListElement = pElement->FirstChildElement("m1");
		while ((counter < (toolNumberOfMarkers * 3)) && (pListElement != nullptr))
		{
		eResult = pListElement->QueryFloatAttribute("x", &this->m_CalibrationPoints[counter * 3 + 0]);
		XMLCheckResult(eResult);
		eResult = pListElement->QueryFloatAttribute("y", &this->m_CalibrationPoints[counter * 3 + 1]);
		XMLCheckResult(eResult);
		eResult = pListElement->QueryFloatAttribute("z", &this->m_CalibrationPoints[counter * 3 + 2]);
		XMLCheckResult(eResult);

		counter = counter + 1;

		//Conversion from int to char*.
		std::stringstream strs;
		markerLocation = (counter / 3) + 1;
		strs << (markerLocation);
		std::string temp_str = "m" + strs.str();
		char* char_type = (char*)temp_str.c_str();
		pListElement = pListElement->NextSiblingElement(char_type);

		//Optitrack works with Left Handed System
		this->m_CalibrationPoints[counter * 3 + 0] = this->m_CalibrationPoints[counter * 3 + 0] / 1000;
		this->m_CalibrationPoints[counter * 3 + 1] = this->m_CalibrationPoints[counter * 3 + 1] / 1000;
		this->m_CalibrationPoints[counter * 3 + 2] = -this->m_CalibrationPoints[counter * 3 + 2] / 1000;
		*/
	º
		
		return FAILURE; //Failure=0 & Success=1
	}

}