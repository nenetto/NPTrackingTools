#ifndef LAUBRARY_IGSTKLIKE_H
#define LAUBRARY_IGSTKLIKE_H

// Config from CMake
#include "LaubraryConfig.h"

// NPTrackingTools library
#include "NPTrackingTools.h"

// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>

// UTIL
#define LAUBRARY_IGSTKLIKE_STATE int
#define LAUBRARY_IGSTKLIKE_EVENT int

// MAJOR STATES
#define ST_TRACKER_NoState -1
#define ST_TRACKER_Idle 0
#define ST_TRACKER_TrackerToolAttached 1
#define ST_TRACKER_CommunicationEstablished 2
#define ST_TRACKER_Tracking 3

// TRANSITIONAL STATES
#define ST_TRACKER_AttemptingToEstablishCommunication 4
#define ST_TRACKER_AttemptingToCloseCommunication 5
#define ST_TRACKER_AttemptingToTrack 6
#define ST_TRACKER_AttemptingToStopTracking 7
#define ST_TRACKER_AttemptingToUpdate 8

// Events
#define EV_TRACKER_NoEvent -1
#define EV_TRACKER_TrackerOpenEvent 0
#define EV_TRACKER_TrackerOpenErrorEvent 1
#define EV_TRACKER_TrackerCloseEvent 2
#define EV_TRACKER_TrackerCloseErrorEvent 3
#define EV_TRACKER_TrackerInitializeEvent 4
#define EV_TRACKER_TrackerInitializeErrorEvent 5
#define EV_TRACKER_TrackerStartTrackingEvent 6
#define EV_TRACKER_TrackerStartTrackingErrorEvent 7
#define EV_TRACKER_TrackerStopTrackingEvent 8
#define EV_TRACKER_TrackerStopTrackingErrorEvent 9
#define EV_TRACKER_TrackerUpdateStatusEvent 10
#define EV_TRACKER_TrackerUpdateStatusErrorEvent 11
#define EV_TRACKER_TrackerEvent 12
#define EV_TRACKER_TrackerErrorEvent 13


// MAJOR STATES
#define ST_TOOL_NoState -1
#define ST_TOOL_Idle 0
#define ST_TOOL_Configured 1
#define ST_TOOL_Attached 2
#define ST_TOOL_NotAvailable 3
#define ST_TOOL_Tracked 4

// TRANSITIONAL STATES
#define ST_TOOL_AttemptingToConfigureTrackerTool 5
#define ST_TOOL_AttemptingToAttachTrackerTool 6
#define ST_TOOL_AttemptingToDetachTrackerToolFromTracker 7

// Events
#define EV_TOOL_NoEvent -1
#define EV_TOOL_TrackerToolConfigurationEvent
#define EV_TOOL_TrackerToolConfigurationErrorEvent
#define EV_TOOL_InvalidRequestToAttachTrackerToolErrorEvent
#define EV_TOOL_InvalidRequestToDetachTrackerToolErrorEvent
#define EV_TOOL_TrackerToolAttachmentToTrackerEvent
#define EV_TOOL_TrackerToolAttachmentToTrackerErrorEvent
#define EV_TOOL_TrackerToolMadeTransitionToTrackedStateEvent
#define EV_TOOL_TrackerToolNotAvailableToBeTrackedEvent
#define EV_TOOL_ToolTrackingStartedEvent
#define EV_TOOL_ToolTrackingStoppedEvent
#define EV_TOOL_TrackerToolEvent
#define EV_TOOL_TrackerToolErrorEvent




namespace LaubraryIGSTKlike{

    class IGSTKTracker{

        int state;
        int event;


        // Interface Methods
        void RequestOpen();
        void RequestClose();
        void RequestReset();
        void RequestStartTracking();
        void RequestStopTracking();
        void RequestSetFrequency();
        void RequestSetReferenceTool();

        void setEvent(LAUBRARY_IGSTKLIKE_EVENT event_);
        void setState(LAUBRARY_IGSTKLIKE_STATE state_);
        LAUBRARY_IGSTKLIKE_EVENT getEvent(void);
        LAUBRARY_IGSTKLIKE_STATE getState(void);


    };

    class IGSTKTrackerTool{

    private:
        int state;
        int event;

    public:
        // Interface Methods
        void SetCalibrationTransform();
        void GetCalibratedTransform();
        void RequestConfigure();
        void RequestAttachToTracker();
        void GetTrackerToolIdentifier();

        // Extra functions
        void setEvent(LAUBRARY_IGSTKLIKE_EVENT event_);
        void setState(LAUBRARY_IGSTKLIKE_STATE state_);
        LAUBRARY_IGSTKLIKE_EVENT getEvent(void);
        LAUBRARY_IGSTKLIKE_STATE getState(void);

    };



}



#endif

//TODO
//
// 1. Add Comments 