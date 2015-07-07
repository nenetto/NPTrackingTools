#ifndef OPTITRACK_TOOL_H
#define OPTITRACK_TOOL_H

// Configuration files
#include "BiiGOptitrackControlConfig.h" // Export
#include "ExtraDefinitions.h"

// ITK Libs
#include <itkFastMutexLock.h>
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>
#include <itkObject.h>

namespace Optitrack{

    class BiiGOptitrackControl_EXPORT OptitrackTool: public itk::Object{

    public:
        friend class OptitrackTracker;
        BiiGOptitrackControlClassMacro(Optitrack::OptitrackTool, itk::Object);
        itkNewMacro(Self);


        /**
        * \brief Definition of the states for the machine behaviour
        */
        typedef enum
        {
            // MAJOR STATES
            STATE_TOOL_NoState = -1,
            STATE_TOOL_Idle = 0,
            STATE_TOOL_Configurated = 1,
            STATE_TOOL_Attached = 2,
            STATE_TOOL_Enabled = 3,
            STATE_TOOL_Disabled = 4,
            // TRANSITIONAL STATES
            STATE_TOOL_AttemptingToReadTxtFile = 10,
            STATE_TOOL_AttemptingToGetIDnext = 11,
            STATE_TOOL_AttemptingToDettachTrackable = 12,
            STATE_TOOL_AttemptingToEnableTrackable = 13,
            STATE_TOOL_AttemptingToDisableTrackable = 14,
            STATE_TOOL_AttemptingToAttachTrackable = 15,
            STATE_TOOL_AttemptingToUpdateTrackable = 16,
			STATE_TOOL_AttemptingToReadXmlFile = 17
        } OPTITRACK_TOOL_STATE;

        /**
        * \brief Events that the class can launch NOT USED
        */
        typedef enum
        {
            // Events
            EVENT_TOOL_NoEvent = -1,
        } OPTITRACK_TOOL_EVENT;

        /**
        * \brief Different options for the result of the functions
        *
        typedef enum
        {
            FAILURE = 0,
            SUCCESS = 1
        } ResultType;
        */

        ResultType ConfigureToolByTxtFile(std::string nameFile);

		ResultType ConfigureToolByXmlFile(std::string nameFile);

        int GetIDnext( void );

        ResultType AttachTrackable( void );

        ResultType DettachTrackable( void );

        vnl_vector_fixed<double,3> GetPosition( void );

        vnl_quaternion<double> GetOrientation( void );

        ResultType Enable( void );

        ResultType Disable( void );

        bool IsTracked( void );

        bool IsDataValid( void ) const;

        void SetDataValid(bool validate);

        ResultType UpdateTool( void );

        /** @brief Sets the tool Name */
        itkSetMacro(ToolName,std::string);

        /** @brief Gets the tool Name. */
        itkGetMacro(ToolName,std::string);

        /** @brief Sets the tool NumberOfMarkers */
        itkSetMacro(NumberOfMarkers,unsigned int);

        /** @brief Gets the tool NumberOfMarkers. */
        itkGetMacro(NumberOfMarkers,unsigned int);

        /** @brief Sets the tool FileConfiguration */
        itkSetMacro(FileConfiguration,std::string);

        /** @brief Gets the tool FileConfiguration. */
        itkGetMacro(FileConfiguration,std::string);

        /** @brief Gets the tool m_OptitrackID. */
        itkGetMacro(OptitrackID,int);

        /** @brief Sets the tool TransformMatrix */
        itkSetMacro(TransformMatrix, vnl_matrix<double>);

        /** @brief Gets the tool TransformMatrix. */
        itkGetMacro(TransformMatrix, vnl_matrix<double>);

        /** @brief Gets the tool State */
        OPTITRACK_TOOL_STATE GetState( void );

    protected:

        OptitrackTool();
        ~OptitrackTool();

        OptitrackTool(const OptitrackTool&);
        const OptitrackTool& operator=(const OptitrackTool&);

        /** @brief Sets the tool State */
        void SetState(OPTITRACK_TOOL_STATE state_);

        ResultType SetPosition(vnl_vector_fixed<double,3> position);

        ResultType SetOrientation(vnl_quaternion<double> orientation);

        bool IsIndeterminateValue(const float pV);

        bool IsInfiniteValue(const float pV);

    private:

        /**
        * \brief State of the Tracker
        */
        OPTITRACK_TOOL_STATE m_State = STATE_TOOL_NoState;

        /**
        * \brief Last Signal/Event Launched by Tracker
        */
        OPTITRACK_TOOL_EVENT m_Event = EVENT_TOOL_NoEvent;

        /**
        * \brief Name of the tool
        */
        std::string m_ToolName;

        /**
        * \brief Optitrack ID number of the tool
        */
        int m_OptitrackID;

        /**
        * \brief Optitrack ID number of the tool
        */
        unsigned int m_NumberOfMarkers;

        /**
        * \brief Mutex to control concurrent access to the tool
        */
        itk::FastMutexLock::Pointer m_MyMutex;

        /**
        * \brief mutex to control access to m_state
        */
        itk::FastMutexLock::Pointer m_StateMutex;

        /**
        * \brief Is Trackable Visible
        */
        bool m_Visible;

        /**
        * \brief Is data saved Visible
        */
        bool m_DataValid;

        /**
        * \brief Orientation of the Tool
        */
        vnl_quaternion<double> m_Orientation;

        /**
        * \brief Location of the Tool
        */
        vnl_vector_fixed<double,3> m_Position;

        /**
        * \brief List of Markers locations in calibration position and orientation
        */
        float* m_CalibrationPoints;

        /**
        * \brief location of the pivot point during calibration
        */
        float* m_PivotPoint;

        /**
        * \brief Configuration File, can be a XML file or a TXT file
        */
        std::string m_FileConfiguration;

        /**
        * \brief Transformation Matrix of the tool
        */
        vnl_matrix<double> m_TransformMatrix;

    };

}

#endif


// TODO
//
// 1. Check State Machine for all functions
// 5. Create new std::out to log file.
// 6. Print Self