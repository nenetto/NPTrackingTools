#ifndef OPTITRACK_TRACKER_H
#define OPTITRACK_TRACKER_H

// Config from CMake
#include "LaubraryConfig.h"

// NPTrackingTools library
#include "NPTrackingTools.h"
#define OPTITRACK_ATTEMPTS 10

// OptitrackTol
#include "OptitrackTool.h"

// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>

// ITK Libs
//#include <itkMultiThreader.h>
//#include <itkFastMutexLock.h>
//#include <itksys/SystemTools.hxx>
//#include <itkMutexLockHolder.h>
//#include <itkObject.h>

/**
* \brief MutexHolder to keep rest of Mutex
*/
//typedef itk::MutexLockHolder<itk::FastMutexLock> MutexLockHolder;


#define ITK_THREAD_RETURN_TYPE int // Delete next when thread is uncommented

namespace Optitrack{

    class Laubrary_EXPORT OptitrackTracker /*: public itk::Object*/{

    public:

        //LaubraryClassMacro(OptitrackTool, itk::Object);
        //itkNewMacro(Self);

        friend class OptitrackTool;

        /**
        * \brief Definition of the states for the machine behaviour
        */
        typedef enum
        {
            // MAJOR STATES
            STATE_TRACKER_NoState = -1,
            STATE_TRACKER_Idle = 0,
            STATE_TRACKER_TrackerToolAttached = 1,
            STATE_TRACKER_CommunicationEstablished = 2,
            STATE_TRACKER_Tracking = 3,
            STATE_TRACKER_CalibratedState = 4,
            // TRANSITIONAL STATES
            STATE_TRACKER_AttemptingToEstablishCommunication = 10,
            STATE_TRACKER_AttemptingToCloseCommunication = 11,
            STATE_TRACKER_AttemptingToTrack = 12,
            STATE_TRACKER_AttemptingToStopTracking = 13,
            STATE_TRACKER_AttemptingToUpdate = 14,
            STATE_TRACKER_AttemptingToLoadCalibration = 15,
            STATE_TRACKER_AttemptingToReset = 16,
            STATE_TRACKER_AttemptingToAttachTrackerTool = 17,
            STATE_TRACKER_AttemptingToStartTracking = 18,
            STATE_TRACKER_AttemptingToSetCameraParams = 19,
            STATE_TRACKER_AttemptingToDetachTrackerTool = 20
        } OPTITRACK_TRACKER_STATE;

        /**
        * \brief Events that the class can launch NOT USED
        */
        typedef enum
        {
            // Events
            EVENT_TRACKER_NoEvent = -1,
            EVENT_TRACKER_TrackerEvent = 0,
            EVENT_TRACKER_TrackerErrorEvent = 1,
            EVENT_TRACKER_TrackerOpenEvent = 2,
            EVENT_TRACKER_TrackerOpenErrorEvent = 3,
            EVENT_TRACKER_TrackerCloseEvent = 4,
            EVENT_TRACKER_TrackerCloseErrorEvent = 5,
            EVENT_TRACKER_TrackerInitializeEvent = 6,
            EVENT_TRACKER_TrackerInitializeErrorEvent = 7,
            EVENT_TRACKER_TrackerStartTrackingEvent = 8,
            EVENT_TRACKER_TrackerStartTrackingErrorEvent = 9,
            EVENT_TRACKER_TrackerStopTrackingEvent = 10,
            EVENT_TRACKER_TrackerStopTrackingErrorEvent = 11,
            EVENT_TRACKER_TrackerUpdateStatusEvent = 12,
            EVENT_TRACKER_TrackerUpdateStatusErrorEvent = 13
        } OPTITRACK_TRACKER_EVENT;

        /**
        * \brief Different options for the result of the functions
        */
        typedef enum
        {
            FAILURE = 0,
            SUCCESS = 1
        } ResultType;

        /**
        * \brief Open the Connection with the Tracker. Calls LoadCalibration function and set the system up with the calibration file.
        * Remember that you have to set a calibration file first to open a correct connection to the Optical Tracking System.
        * \return Returns SUCCESS if the connection is well done. , FAILURE otherwise.
        */
        ResultType Open( void );


        /**
        * \brief Close the Connection with the Tracker. Calls StopTracking function, delete the tool containes and shut down the system.
        * \return Returns SUCCESS if the disconnection was well performed. , FAILURE otherwise.
        */
        ResultType Close( void );

        /**
        * \brief Set the system into Calibration State. Calls StopTracking function, does not delete the tool container.
        * \return Returns SUCCESS if the reset was well performed. , FAILURE otherwise.
        */
        ResultType Reset( void );

        /**
        * \brief Start to Track the tools already defined. If no tools are defined for this tracker, it returns an error.
        * Tools can be added using  AddTrackerTool
        * \return Returns SUCCESS at least one tool was defined and the tracking is correct, FAILURE otherwise.
        */
        ResultType StartTracking( void );

        /**
        * \brief Stop the Tracking Thread and tools will not longer be updated.
        * \return Returns SUCCESS if Tracking thread could be stopped, FAILURE otherwise.
        */
        ResultType StopTracking( void );

        /**
        * \brief Start the Tracking Thread for the tools
        */
        static ITK_THREAD_RETURN_TYPE ThreadStartTracking(void* pInfoStruct);

        /**
        * \brief Update each tool location in the container m_LoadedTools
        */
        void TrackTools();

        /**
        * \brief Print the object information in a stream
        */
        void PrintSelf( std::ostream& os) const;

        /**
        * \brief Remove tracker tool entry from internal containers
        * \return Returns SUCCESS if the tool was removed correctly, FAILURE otherwise.
        */
        ResultType RemoveTrackerTool( OptitrackTool * trackerTool );

        /**
        * \brief Add tracker tool entry to internal containers
        * \return Returns SUCCESS if the tool was uploaded correctly, FAILURE otherwise.
        */
        ResultType AddTrackerTool( OptitrackTool * trackerTool );

        /**
        * \brief Load the Calibration file to the Optitrack System and set the cameras in calibrated locations
        * \return Returns SUCCESS if the calibration was uploaded correctly, FAILURE otherwise.
        */
        ResultType LoadCalibration( void );

        /**
        * \brief Returns the number of defined tools
        * \return Returns the number of defined tools in the Optitrack device.
        */
        unsigned int GetNumberOfAttachedTools( void );

        /**
        * \brief Return the tool pointer of the tool number toolNumber
        * \param toolNumber The number of the tool which should be given back.
        * \return Returns the tool which the number "toolNumber". Returns NULL, if there is
        * no tool with this number.
        */
        OptitrackTool* GetOptitrackTool(unsigned int toolID);

        /**
        * \brief Return the tool pointer of the tool named toolNumber
        * \param toolNumber The number of the tool which should be given back.
        * \return Returns the tool which the number "toolNumber". Returns NULL, if there is
        * no tool with this number.
        */
        OptitrackTool* OptitrackTracker::GetOptitrackToolByName( std::string toolName );

        /**
        * \brief Set the Cameras Exposure, Threshold and Intensity of IR LEDs. By Default it set the Video type to 4: Precision Mode for tracking
        * //== VideoType:
            * //==     0 = Segment Mode
            * //==     1 = Grayscale Mode
            * //==     2 = Object Mode
            * //==     4 = Precision Mode
            * //==     6 = MJPEG Mode     (V100R2 only)
        * \return Returns true if all cameras were set up correctly
        * @throw mitk::IGTException Throws an exception if System is not Initialized
        */
        ResultType SetCameraParams(int exposure, int threshold, int intensity, int videoType = 4);


        void SetEvent(OPTITRACK_TRACKER_EVENT event_);
        void SetState(OPTITRACK_TRACKER_STATE state_);
        OPTITRACK_TRACKER_EVENT GetEvent( void );
        OPTITRACK_TRACKER_STATE GetState( void );

        /** @brief Sets the directory where the calibration file of the MicronTracker can be found. */
        //itkSetMacro(Exp,unsigned int);

        /** @brief Gets the current calibration directory. */
        //itkGetMacro(Exp,unsigned int);

        /** @brief Sets the directory where the calibration file of the MicronTracker can be found. */
        //itkSetMacro(Led,unsigned int);

        /** @brief Gets the current calibration directory. */
        //itkGetMacro(Led,unsigned int);

        /** @brief Sets the directory where the calibration file of the MicronTracker can be found. */
        //itkSetMacro(Thr,unsigned int);

        /** @brief Gets the current calibration directory. */
        //itkGetMacro(Thr,unsigned int);

        /** @brief Gets the current calibration file. */
        //itkGetMacro(calibrationPath,std::string);

        /** @brief Sets current calibration file */
        //itkSetMacro(calibrationPath,std::string);

        void SetCalibrationFile(std::string newCalibrationFile); // TODO Delete using ITK
        std::string GetCalibrationFile( void ); // TODO Delete using ITK

        /** @brief Gets Current number of Connected Cameras */
        unsigned int GetCameraNumber( void ); // TODO Delete using ITK

    protected:
        OptitrackTracker();
        ~OptitrackTracker();

    private:

        /**
        * \brief State of the Tracker
        */
        OPTITRACK_TRACKER_STATE m_state = STATE_TRACKER_NoState;

        /**
        * \brief Last Signal/Event Launched by Tracker
        */
        OPTITRACK_TRACKER_EVENT m_event = EVENT_TRACKER_NoEvent;

        /**
        * \brief Calibration File used for Tracking
        */
        std::string m_CalibrationFile;

        /**
        * \brief Vector of pointers pointing to all defined tools
        */
        std::vector<Optitrack::OptitrackTool*> m_LoadedTools;
        //std::vector<Optitrack::OptitrackTool::Pointer> m_LoadedTools;

        /**
        * \brief The Cameras Exposition
        */
        unsigned int m_Exp;

        /**
        * \brief The Cameras LED power
        */
        unsigned int m_Led;

        /**
        * \brief The Cameras Thr
        */
        unsigned int m_Thr;

        /**
        * \brief The Cameras Thr
        */
        unsigned int m_CameraNumber;

        /**
        * \brief Mutex for coordinated access of tool container
        */
        //itk::FastMutexLock::Pointer m_ToolsMutex;

        /**
        * \brief MultiThreader that starts continuous tracking update
        */
        //itk::MultiThreader::Pointer m_MultiThreader;

        /**
        * \brief ThreadID number identification
        */
        int m_ThreadID;

        /**
        * \brief MultiThreader that starts continuous tracking update
        */
        bool m_StopTracking;


        /**
        * \brief MultiThreader that starts continuous tracking update
        */
        //itk::MultiThreader::Pointer m_StopTrackingMutex;

        /**
        * \brief mutex to manage control flow of StopTracking()
        */
        //itk::FastMutexLock::Pointer m_TrackingFinishedMutex;

        /**
        * \brief mutex to control access to m_state
        */
        //itk::FastMutexLock::Pointer m_StateMutex;

    };

}


#endif