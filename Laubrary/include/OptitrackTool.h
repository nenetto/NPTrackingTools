#ifndef OPTITRACK_TOOL_H
#define OPTITRACK_TOOL_H

// Config from CMake
#include "LaubraryConfig.h"


// NPTrackingTools library
#include "NPTrackingTools.h"

// OptitrackTool
//#include "OptitrackTracker.h"

// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>

// ITK Libs
#include <itkMultiThreader.h>
#include <itkFastMutexLock.h>
#include <itksys/SystemTools.hxx>
#include <itkMutexLockHolder.h>
#include <itkObject.h>


namespace Optitrack{

    class Laubrary_EXPORT OptitrackTool: public itk::Object{

    public:
        friend class OptitrackTracker;
        LaubraryClassMacro(Optitrack::OptitrackTool, itk::Object);
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
            STATE_TRACKER_AttemptingToDetachTrackerTool = 20
        } OPTITRACK_TOOL_STATE;

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
        } OPTITRACK_TOOL_EVENT;

        /** @brief Sets the tool Name */
        itkSetMacro(ToolName,std::string);

        /** @brief Gets the tool Name. */
        itkGetMacro(ToolName,std::string);

    protected:

        OptitrackTool();
        ~OptitrackTool();

        OptitrackTool(const OptitrackTool&);
        const OptitrackTool& operator=(const OptitrackTool&);

    private:

        /**
        * \brief Name of the tool
        */
        std::string m_ToolName;

        /**
        * \brief Mutex to control concurrent access to the tool
        */
        itk::FastMutexLock::Pointer m_MyMutex;

    };

}

#endif