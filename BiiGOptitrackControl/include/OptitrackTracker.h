#ifndef OPTITRACK_TRACKER_H
#define OPTITRACK_TRACKER_H

// Configuration files
#include "BiiGOptitrackControlConfig.h"
#include "ExtraDefinitions.h"
#include "OptitrackTool.h"

// ITK Libs
#include <itkMultiThreader.h>
#include <itkFastMutexLock.h>
#include <itkObject.h>
#include <itksys/SystemTools.hxx>


namespace Optitrack{


    class BiiGOptitrackControl_EXPORT OptitrackTracker : public itk::Object{

    public:
        friend class OptitrackTool;
        BiiGOptitrackControlClassMacro(Optitrack::OptitrackTracker, itk::Object);
        itkNewMacro(Self);


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
            STATE_TRACKER_AttemptingToDetachTrackerTool = 20,
			STATE_TRACKER_AttemptingToTestCalibration = 21
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
        ResultType RemoveTrackerTool( OptitrackTool::Pointer trackerTool );

        /**
        * \brief Add tracker tool entry to internal containers
        * \return Returns SUCCESS if the tool was uploaded correctly, FAILURE otherwise.
        */
        ResultType AddTrackerTool( OptitrackTool::Pointer trackerTool );

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
        * \brief Study if the number of markers is higher than 1 in the field of view
        * \return Returns SUCCESS if the test is passed (only one marker is visible).
        */
        ResultType CheckNumberOfMarkers( void );

		/**
		* \brief Study if the calibration is correct by camera pairs study
		* \return Returns SUCCESS if test was performed
		*/
		ResultType TestCalibration(std::string FileName);

        /**
        * \brief Return the tool pointer of the tool number toolNumber
        * \param toolNumber The number of the tool which should be given back.
        * \return Returns the tool which the number "toolNumber". Returns NULL, if there is
        * no tool with this number.
        */
        OptitrackTool::Pointer GetOptitrackTool(unsigned int toolID);

        /**
        * \brief Return the tool pointer of the tool named toolNumber
        * \param toolNumber The number of the tool which should be given back.
        * \return Returns the tool which the number "toolNumber". Returns NULL, if there is
        * no tool with this number.
        */
        OptitrackTool::Pointer GetOptitrackToolByName( std::string toolName );

        /**
        * \brief Set the Cameras Exposure, Threshold and Intensity of IR LEDs. By Default it set the Video type to 4: Precision Mode for tracking
        * == VideoType:
        * ==     0 = Segment Mode
        * ==     1 = Grayscale Mode
        * ==     2 = Object Mode
        * ==     4 = Precision Mode
        * ==     6 = MJPEG Mode     (V100R2 only)
        * \return Returns ResultType SUCCESS if all cameras were set up correctly
        */
        ResultType SetCameraParams(int exposure, int threshold, int intensity, int videoType = 4);

        /** @brief Sets the directory where the calibration file of the MicronTracker can be found. */
        itkSetMacro(Exp,unsigned int);

        /** @brief Gets the current calibration directory. */
        itkGetMacro(Exp,unsigned int);

        /** @brief Sets the directory where the calibration file of the MicronTracker can be found. */
        itkSetMacro(Led,unsigned int);

        /** @brief Gets the current calibration directory. */
        itkGetMacro(Led,unsigned int);

        /** @brief Sets the directory where the calibration file of the MicronTracker can be found. */
        itkSetMacro(Thr,unsigned int);

        /** @brief Gets the current calibration directory. */
        itkGetMacro(Thr,unsigned int);

        /** @brief Sets the Video Type of Cameras. */
        itkSetMacro(VideoType,unsigned int);

        /** @brief Gets the Video Type of Cameras. */
        itkGetMacro(VideoType,unsigned int);

        /** @brief Gets the current calibration file. */
        itkGetMacro(CalibrationFile,std::string);

        /** @brief Sets current calibration file */
        itkSetMacro(CalibrationFile,std::string);

        /** @brief Gets Current number of Connected Cameras */
        unsigned int GetCameraNumber( void ); // TODO Delete using ITK

        /** @brief Gets the tool Event */
        OPTITRACK_TRACKER_EVENT GetEvent( void );

        /** @brief Gets the tool State */
        OPTITRACK_TRACKER_STATE GetState( void );

		ResultType LoadXMLConfigurationFile(std::string configurationFilePath);

    protected:
        OptitrackTracker();
        ~OptitrackTracker();
        OptitrackTracker(const OptitrackTracker&);
        const OptitrackTracker& operator=(const OptitrackTracker&);

        /** @brief Sets the tool Event */
        void SetEvent(OPTITRACK_TRACKER_EVENT event_);

        /** @brief Sets the tool State */
        void SetState(OPTITRACK_TRACKER_STATE state_);

		vnl_vector_fixed<double, 3> Pivoting(unsigned int optitrackID, unsigned int sampleNumber);


    private:

        /**
        * \brief State of the Tracker
        */
        OPTITRACK_TRACKER_STATE m_State = STATE_TRACKER_NoState;

        /**
        * \brief Last Signal/Event Launched by Tracker
        */
        OPTITRACK_TRACKER_EVENT m_Event = EVENT_TRACKER_NoEvent;

        /**
        * \brief Calibration File used for Tracking
        */
        std::string m_CalibrationFile;

        /**
        * \brief Vector of pointers pointing to all defined tools
        */
        //std::vector<Optitrack::OptitrackTool*> m_LoadedTools;
        std::vector<OptitrackTool::Pointer> m_LoadedTools;

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
        * \brief The Cameras Video Type
        */
        unsigned int m_VideoType = 2;

        /**
        * \brief The Cameras Thr
        */
        unsigned int m_CameraNumber;

        /**
        * \brief Mutex for coordinated access of tool container
        */
        itk::FastMutexLock::Pointer m_ToolsMutex;

        /**
        * \brief MultiThreader that starts continuous tracking update
        */
        itk::MultiThreader::Pointer m_MultiThreader;

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
        itk::FastMutexLock::Pointer m_StopTrackingMutex;

        /**
        * \brief mutex to manage control flow of StopTracking()
        */
        itk::FastMutexLock::Pointer m_TrackingFinishedMutex;

        /**
        * \brief mutex to control access to m_state
        */
        itk::FastMutexLock::Pointer m_StateMutex;

    };

}


#endif

// TODO
//
// 5. Create new std::out to log file.
// 6. Print Self
// 7. Add ITK matrices for transformation and function that allow select a tool as a Reference
