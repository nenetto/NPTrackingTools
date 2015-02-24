#ifndef OPTITRACK_TRACKER_H
#define OPTITRACK_TRACKER_H

// Config from CMake
#include "LaubraryConfig.h"

// NPTrackingTools library
#include "NPTrackingTools.h"

// OptitrackTool
#include "OptitrackTool.h"

// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <vector>


#define OPTITRACK_ATTEMPTS 10

namespace Optitrack{

    class OptitrackTracker{

    public:
        friend class OptitrackTool;

        typedef OptitrackTool   TrackerToolType;

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
            STATE_TRACKER_AttemptingToLoadCalibration = 15
        } OPTITRACK_STATE;

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
        } OPTITRACK_EVENT;

        // Interface Methods from IGSTK model for Tracker
        typedef enum
        {
            FAILURE=0,
            SUCCESS
        } ResultType;

        /** The "InternalOpen" method opens communication with a tracking device.
          This method is to be implemented by a descendant class
          and responsible for device-specific processing */
        ResultType InternalOpen( void );

        /** The "InternalClose" method closes communication with a tracking device.
          This method is to be implemented by a descendant class
          and responsible for device-specific processing */
        ResultType InternalClose( void );

        /** The "InternalReset" method resets tracker to a known configuration.
          This method is to be implemented by a descendant class
          and responsible for device-specific processing */
        ResultType InternalReset( void );

        /** The "InternalStartTracking" method starts tracking.
          This method is to be implemented by a descendant class
          and responsible for device-specific processing */
        ResultType InternalStartTracking( void );

        /** The "InternalStopTracking" method stops tracking.
          This method is to be implemented by a descendant class
          and responsible for device-specific processing */
        ResultType InternalStopTracking( void );


        /** The "InternalUpdateStatus" method updates tracker status.
          This method is to be implemented by a descendant class
          and responsible for device-specific processing */
        ResultType InternalUpdateStatus( void );

        /** The "InternalThreadedUpdateStatus" method updates tracker status.
          This method is called in a separate thread.
          This method is to be implemented by a descendant class
          and responsible for device-specific processing */
        ResultType InternalThreadedUpdateStatus( void );

        /** Print the object information in a stream. */
        void PrintSelf( std::ostream& os) const;

        /** Verify if a tracker tool information is correct before attaching
        *  it to the tracker. This method is used to verify the information supplied
        *  by the user about the tracker tool. The information depends on the
        *  tracker type. For example, during the configuration step of the
        *  MicronTracker, location of the directory containing marker template files
        *  is specified. If the user tries to attach a tracker tool with a marker
        *  type whose template file is not stored in this directory, this method
        *  will return failure. Similarly, for PolarisTracker, the method returns
        *  failure,  if the tool part number specified by the user during the tracker
        *  tool configuration step does not match with the part number read from the
        *  SROM file.
        */
        ResultType
            VerifyTrackerToolInformation( const TrackerToolType * );

        /** The "ValidateSpecifiedFrequency" method checks if the specified
        * frequency is valid for the tracking device that is being used. This
        * method is to be overridden in the derived tracking-device specific
        * classes to take into account the maximum frequency possible in the
        * tracking device
        */
        ResultType ValidateSpecifiedFrequency( double frequencyInHz );

        /** This method will remove entries of the traceker tool from internal
        * data containers */
        ResultType RemoveTrackerToolFromInternalDataContainers(
                                         const TrackerToolType * trackerTool );

        /** Add tracker tool entry to internal containers */
        ResultType AddTrackerToolToInternalDataContainers(
                                        const TrackerToolType * trackerTool );

        ///////////////////////////////////////////////////////////////////////////////////////////
        OptitrackTracker();
        ~OptitrackTracker();


        ResultType LoadCalibration( void );
        void setCalibrationFile(std::string newCalibrationFile);
        std::string getCalibrationFile( void );

        void setEvent(OPTITRACK_EVENT event_);
        void setState(OPTITRACK_STATE state_);
        OPTITRACK_EVENT getEvent( void );
        OPTITRACK_STATE getState( void );


    private:

        /**
        * \brief State of the Tracker
        */
        OPTITRACK_STATE m_state = STATE_TRACKER_NoState;

        /**
        * \brief Last Signal/Event Launched by Tracker
        */
        OPTITRACK_EVENT m_event = EVENT_TRACKER_NoEvent;

        /**
        * \brief Calibration File used for Tracking
        */
        std::string m_CalibrationFile;

        /**
        * \brief Vector of pointers pointing to all defined tools
        */
        std::vector<OptitrackTool*> m_LoadedTools;

    };

}


#endif