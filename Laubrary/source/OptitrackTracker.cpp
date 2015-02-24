#include "OptitrackTracker.h"

namespace Optitrack{


    void OptitrackTracker::setState(OPTITRACK_STATE state_)
    {
        this->m_state = state_;
    }

    OptitrackTracker::OPTITRACK_STATE OptitrackTracker::getState( void )
    {
        return this->m_state;
    }

    //=======================================================
    // Constructor
    //=======================================================
    OptitrackTracker::OptitrackTracker()
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::OptitrackTracker]\n");
        //Clear List of tools
        this->m_LoadedTools.clear();
    }

    //=======================================================
    // Destructor
    //=======================================================
    OptitrackTracker::~OptitrackTracker()
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]\n");

        ResultType result;
        // If device is in Tracking mode, stop the Tracking first
        if (this->getState() == STATE_TRACKER_Tracking)
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: in Tracking State -> Stopping Tracking \n");
            result = this->InternalStopTracking();

            if(result == SUCCESS){
                fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: Device Stopped \n");
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::~OptitrackTracker]: Error Stopping the Device\n");
            }
        }

        // Call InternalClose first
        if (this->getState() != STATE_TRACKER_Idle)
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::~OptitrackTracker]: Calling InternalClose \n");
            result = this->InternalClose();

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

    OptitrackTracker::ResultType OptitrackTracker::InternalOpen( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::InternalOpen]\n");

        OPTITRACK_STATE previous_state = this->getState();
        this->setState(STATE_TRACKER_AttemptingToEstablishCommunication);

        if(previous_state == STATE_TRACKER_CommunicationEstablished)
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalOpen]: System was initialized before\n");
            this->setState(STATE_TRACKER_CommunicationEstablished);
            return SUCCESS;
        }

        NPRESULT result = TT_Initialize();

        if( result == NPRESULT_SUCCESS)
        {
            this->setState(STATE_TRACKER_CommunicationEstablished);
            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalOpen]: System was Initialized\n");
            return SUCCESS;
        }else
        {
            this->setState(STATE_TRACKER_Idle);
            fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalOpen]: TT_Initialize failed\n");
            return FAILURE;
        }
    }

    OptitrackTracker::ResultType OptitrackTracker::LoadCalibration( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::LoadCalibration]\n");
        OPTITRACK_STATE previous_state = getState();
        this->setState(STATE_TRACKER_AttemptingToLoadCalibration);

        if(previous_state != STATE_TRACKER_CommunicationEstablished)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::LoadCalibration]: System has not been Initialized/Open\n");
            this->setState(previous_state);
            return FAILURE;
        }

        if(this->m_CalibrationFile.empty()){
            fprintf(stdout, "#ERROR# - [OptitrackTracker::LoadCalibration]: Calibration File is empty\n");
            this->setState(STATE_TRACKER_CommunicationEstablished);
            return FAILURE;
        }

        for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
        {
            NPRESULT resultLoadCalibration = TT_LoadCalibration(this->m_CalibrationFile.c_str());

            if(resultLoadCalibration != NPRESULT_SUCCESS)
            {
                fprintf(stdout, "#ERROR# - [OptitrackTracker::LoadCalibration]: TT_LoadCalibration failed\n");
                this->setState(STATE_TRACKER_CommunicationEstablished);
                return FAILURE;
            }
            else
            {
                fprintf(stdout, "<INFO> - [OptitrackTracker::LoadCalibration]: Calibration was successfully loaded\n");
                this->setState(STATE_TRACKER_CalibratedState);
                return SUCCESS;
            }

        }
    }

    void OptitrackTracker::setCalibrationFile(std::string newCalibrationFile)
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::setCalibrationFile]\n");
          // Check the file path
        if(newCalibrationFile.empty())
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::setCalibrationFile]: Calibration File is empty\n");
            return;
        }

        this->m_CalibrationFile = newCalibrationFile;
        fprintf(stdout, "<INFO> - [OptitrackTracker::LoadCalibration]: Calibration was successfully set\n");
        return;
    }

    std::string OptitrackTracker::getCalibrationFile( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::getCalibrationFile]\n");
        return this->m_CalibrationFile;
    }

    //=======================================================
    // InternalClose
    //=======================================================
    OptitrackTracker::ResultType OptitrackTracker::InternalClose( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]\n");
        OPTITRACK_STATE previous_state = getState();
        this->setState(STATE_TRACKER_AttemptingToCloseCommunication);

        if(previous_state == STATE_TRACKER_Tracking)
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]: Stopping the Tracking\n");
            //ResultType resultStop = this->InternalStopTracking(); //Stop tracking on close
            //TO DO: Stop tracking
        }


        for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
        {
            TT_ClearTrackableList();
            NPRESULT resultShutdown = TT_Shutdown();

            if(resultShutdown == NPRESULT_SUCCESS)
            {
                fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]: System has been Shutdown Correctly\n");
                //Sleep(2000); -> Find Substitute using ¿ITK?
                this->setState(STATE_TRACKER_Idle);
                return SUCCESS;
            }
            else
            {
                fprintf(stdout, "<INFO> - [OptitrackTracker::InternalClose]: System cannot ShutDown now. Trying again...\n");
            }
        }

        fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalClose]: System cannot ShutDown now\n");
        this->setState(previous_state);
        return FAILURE;
    }

    OptitrackTracker::ResultType OptitrackTracker::InternalReset( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTracker::InternalReset]\n");
        OPTITRACK_STATE previous_state = getState();
        this->setState(STATE_TRACKER_AttemptingToReset);

        if(previous_state != STATE_TRACKER_Tracking)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTracker::InternalReset]: System cannot be Reset from other State but Tracking\n");
            this->setState(previous_state);
            return FAILURE;
        }
        else
        {
            fprintf(stdout, "<INFO> - [OptitrackTracker::InternalReset]: Stopping the Tracking and Reset to calibration\n");
            //TO DO: Stop tracking
            this->setState(STATE_TRACKER_CalibratedState);
            return SUCCESS;
        }
    }

}