#include "OptitrackTool.h"

#include "OptitrackHelpers.h"

// NPTrackingTools library
#include "NPTrackingTools.h"

// ITK Libs
#include <itkMultiThreader.h>
#include <itksys/SystemTools.hxx>

// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>
#include <limits.h>
#include <math.h>

namespace Optitrack
{

    OptitrackTool::OptitrackTool()
    {
        this->m_StateMutex = itk::FastMutexLock::New();
        this->m_MyMutex = itk::FastMutexLock::New();
        this->m_OptitrackID = -1;
        this->m_Visible = true;
        this->m_DataValid = false;

        this->m_Position[0] = 0.0f;
        this->m_Position[1] = 0.0f;
        this->m_Position[2] = 0.0f;
        this->m_Orientation[0] = 0.0f;
        this->m_Orientation[1] = 0.0f;
        this->m_Orientation[2] = 0.0f;
        this->m_Orientation[3] = 0.0f;

        this->m_TransformMatrix = vnl_matrix<double>(4,4);
        this->m_TransformMatrix.set_identity();
    }

    OptitrackTool::~OptitrackTool()
    {
        this->m_MyMutex->Unlock();
        this->m_MyMutex = NULL;
        this->m_StateMutex->Unlock();
        this->m_StateMutex = NULL;

        if(this->m_CalibrationPoints != NULL)
            delete this->m_CalibrationPoints;
        if(this->m_PivotPoint != NULL)
            delete this->m_PivotPoint;
    }

    void OptitrackTool::SetState(OptitrackTool::OPTITRACK_TOOL_STATE state_)
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

    OptitrackTool::OPTITRACK_TOOL_STATE OptitrackTool::GetState( void )
    {
        return this->m_State;
    }

    ResultType OptitrackTool::ConfigureToolByTxtFile(std::string nameFile)
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToReadTxtFile);

        int resultFscan, resultUpdate, resultCreateTrackable, resultTrackableTranslatePivot;

        // Check the file path
        if(nameFile.empty())
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot Read file\n");
            this->SetFileConfiguration("");
            this->SetState(previous_state);
            return FAILURE;
        }

        // Open the file
        FILE* calib_file = fopen(nameFile.c_str(),"r");
        if (calib_file == NULL)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot open file\n");
            this->SetFileConfiguration("");
            this->SetState(previous_state);
            return FAILURE;
        }


        // Get the name
        this->SetToolName("");
        char* aux = new char[200];
        resultFscan = fscanf(calib_file,"%s\n",aux);
        this->SetToolName(aux);
        delete aux;
        if ((resultFscan < 1) || this->GetToolName().empty())
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: No name found in the tool configuration file\n");
            this->SetFileConfiguration("");
            this->SetState(previous_state);
            return FAILURE;
        }

        fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]: Tool Name %d\n", this->GetToolName());


        // Get the number of of points
        int num_mk = 0;
        resultFscan = fscanf(calib_file,"%i\n",&num_mk);
        this->SetNumberOfMarkers(num_mk);
        if (this->GetNumberOfMarkers() < 3)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: The minimum number for define a tool is 3 markers\n");
            this->SetFileConfiguration("");
            this->SetNumberOfMarkers(0);
            this->SetState(previous_state);
            fclose(calib_file);
            return FAILURE;
        }

        fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]: Numer of Markers %i\n", this->GetNumberOfMarkers());

        // Read the Calibration Point locations and save them
        this->m_CalibrationPoints = new float[3*this->GetNumberOfMarkers()];

        for(int i=0; i<this->GetNumberOfMarkers(); i++)
        {
             resultFscan = fscanf(calib_file,"%fe ",    &this->m_CalibrationPoints[i*3+0]);
             if (resultFscan < 1)
             {
                fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot read X location for marker i%\n",i);
                this->SetFileConfiguration("");
                this->SetNumberOfMarkers(0);
                delete this->m_CalibrationPoints;
                this->SetState(previous_state);
                fclose(calib_file);
                return FAILURE;
             }

             resultFscan = fscanf(calib_file,"%fe ",    &this->m_CalibrationPoints[i*3+1]);
             if (resultFscan < 1)
             {
                fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot read Y location for marker %i\n",i);
                this->SetFileConfiguration("");
                this->SetNumberOfMarkers(0);
                delete this->m_CalibrationPoints;
                this->SetState(previous_state);
                fclose(calib_file);
                return FAILURE;
             }

             resultFscan = fscanf(calib_file,"%fe\n",    &this->m_CalibrationPoints[i*3+2]);
             if (resultFscan < 1)
             {
                fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot read Y location for marker %i\n",i);
                this->SetFileConfiguration("");
                this->SetNumberOfMarkers(0);
                delete this->m_CalibrationPoints;
                this->SetState(previous_state);
                fclose(calib_file);
                return FAILURE;
             }
             fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]: Marker %i: [%f, %f, %f]\n", i, this->m_CalibrationPoints[i*3+0], this->m_CalibrationPoints[i*3+1], this->m_CalibrationPoints[i*3+2]);


             this->m_CalibrationPoints[i*3+0] = this->m_CalibrationPoints[i*3+0]/1000;
             this->m_CalibrationPoints[i*3+1] = this->m_CalibrationPoints[i*3+1]/1000;
             this->m_CalibrationPoints[i*3+2] = - this->m_CalibrationPoints[i*3+2]/1000;// !!!!!!! Optitrack works with Left Handed System !!!!!!!

        }

        // Read the Pivot Point location
        this->m_PivotPoint = new float[3];
        resultFscan = fscanf(calib_file,"%fe ",    &this->m_PivotPoint[0]);
        if (resultFscan < 1)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot read X location for Pivot Point\n");
            this->SetFileConfiguration("");
            this->SetNumberOfMarkers(0);
            delete this->m_CalibrationPoints;
            delete this->m_PivotPoint;
            this->SetState(previous_state);
            fclose(calib_file);
            return FAILURE;
        }

        resultFscan = fscanf(calib_file,"%fe ",    &this->m_PivotPoint[1]);
        if (resultFscan < 1)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot read Y location for Pivot Point\n");
            this->SetFileConfiguration("");
            this->SetNumberOfMarkers(0);
            delete this->m_CalibrationPoints;
            delete this->m_PivotPoint;
            this->SetState(previous_state);
            fclose(calib_file);
            return FAILURE;
        }

        resultFscan = fscanf(calib_file,"%fe\n",    &this->m_PivotPoint[2]);
        if (resultFscan < 1)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByTxtFile]: Cannot read Z location for Pivot Point\n");
            this->SetFileConfiguration("");
            this->SetNumberOfMarkers(0);
            delete this->m_CalibrationPoints;
            delete this->m_PivotPoint;
            this->SetState(previous_state);
            fclose(calib_file);
            return FAILURE;
         }
        fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]: Pivot: [%f, %f, %f]\n", m_PivotPoint[0], m_PivotPoint[1], m_PivotPoint[2]);

        // mm -> m
        this->m_PivotPoint[0] = this->m_PivotPoint[0]/1000;
        this->m_PivotPoint[1] = this->m_PivotPoint[1]/1000;
        this->m_PivotPoint[2] = -this->m_PivotPoint[2]/1000; // !!!!!!! Optitrack works with Left Handed System !!!!!!!



        fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]: Configuration Successfull\n");
                    fclose(calib_file);
                    this->SetState(STATE_TOOL_Configurated);
                    return SUCCESS;
    }

    int OptitrackTool::GetIDnext( void )
    {

        fprintf(stdout, "<INFO> - [OptitrackTool::GetIDnext]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToGetIDnext);

        int num_trackables = -1;
        int resultUpdate;

        for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
        {
            resultUpdate = TT_Update();
            if(NPRESULT_SUCCESS == resultUpdate)
            {
                num_trackables =    TT_TrackableCount();
                if(num_trackables > -1)
                {
                    fprintf(stdout, "<INFO> - [OptitrackTool::GetIDnext]: Next ID %i \n",num_trackables);
                    this->SetState(previous_state);
                    return num_trackables;
                }
                else
                {
                    fprintf(stdout, "#ERROR# - [OptitrackTool::GetIDnext]: GetIDnext FAILED\n");
                    this->SetState(previous_state);
                    return -1;
                }

            }

        }

        fprintf(stdout, "#ERROR# - [OptitrackTool::GetIDnext]: GetIDnext FAILED\n");
        this->SetState(previous_state);
        return -1;
    }

    ResultType OptitrackTool::DettachTrackable( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::DeleteTrackable]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToDettachTrackable);

        int resultRemoveTrackable;
        resultRemoveTrackable = TT_RemoveTrackable(this->GetOptitrackID());

        if(resultRemoveTrackable != NPRESULT_SUCCESS)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::DeleteTrackable]: Cannot Remove Trackable %d \n",this->GetToolName());
            this->SetState(previous_state);
            return FAILURE;
        }
        else
        {
            fprintf(stdout, "<INFO> - [OptitrackTool::DeleteTrackable]: %d Deleted \n",this->GetToolName());
            this->SetState(STATE_TOOL_Configurated);
            return SUCCESS;
        }

        fprintf(stdout, "#ERROR# - [OptitrackTool::DeleteTrackable]: Cannot Remove Trackable %d \n",this->GetToolName());
        this->SetState(previous_state);
        return FAILURE;
    }

    ResultType OptitrackTool::SetPosition(vnl_vector_fixed<double,3> position)
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::SetPosition]\n");
        // sets the position
        this->m_Position[0] = position[0];
        this->m_Position[1] = position[1];
        this->m_Position[2] = position[2];

        OptitrackHelper::ConvertMatrix(this->m_TransformMatrix,  this->m_Position , this->m_Orientation);


        fprintf(stdout, "<INFO> - [OptitrackTool::SetPosition]: SUCESS \n");
        return SUCCESS;
    }

    ResultType OptitrackTool::SetOrientation(vnl_quaternion<double> orientation)
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::SetOrientation]\n");
        // sets the orientation as a quaternion
        this->m_Orientation.x() = orientation.x();
        this->m_Orientation.y() = orientation.y();
        this->m_Orientation.z() = orientation.z();
        this->m_Orientation.r() = orientation.r();

        OptitrackHelper::ConvertMatrix(this->m_TransformMatrix,  this->m_Position , this->m_Orientation);

        fprintf(stdout, "<INFO> - [OptitrackTool::SetPosition]: SUCESS \n");
        return SUCCESS;
    }

    vnl_vector_fixed<double,3> OptitrackTool::GetPosition( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::GetPosition]\n");
        fprintf(stdout, "<INFO> - [OptitrackTool::GetPosition]: SUCESS \n");
        return this->m_Position;
    }

    vnl_quaternion<double> OptitrackTool::GetOrientation( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::GetOrientation]\n");
        fprintf(stdout, "<INFO> - [OptitrackTool::GetOrientation]: SUCESS \n");
        return this->m_Orientation;
    }

    ResultType OptitrackTool::Enable( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::Enable]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToEnableTrackable);

        if( (previous_state == STATE_TOOL_Disabled) ||
            (previous_state == STATE_TOOL_Attached))
        {
            TT_SetTrackableEnabled(this->GetOptitrackID(), true);
            if(TT_TrackableEnabled(this->GetOptitrackID()) == true)
            {
                fprintf(stdout, "<INFO> - [OptitrackTool::Enable]: %d Enabled \n",this->GetToolName());
                this->SetState(STATE_TOOL_Enabled);
                return SUCCESS;
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTool::Enable]: Enable Failed %d \n",this->GetToolName());
                this->SetState(previous_state);
                return FAILURE;
            }
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::Enable]: Tool %d is not Disabled or Attached \n",this->GetToolName());
            this->SetState(previous_state);
            return FAILURE;

        }
    }

    ResultType OptitrackTool::Disable( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::Disable]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToEnableTrackable);

        if( (previous_state == STATE_TOOL_Disabled) ||
            (previous_state == STATE_TOOL_Attached))
        {
            TT_SetTrackableEnabled(this->GetOptitrackID(), false);
            if(TT_TrackableEnabled(this->GetOptitrackID()) == false)
            {
                fprintf(stdout, "<INFO> - [OptitrackTool::Disable]: %d Disabled \n",this->GetToolName());
                this->SetState(STATE_TOOL_Enabled);
                return SUCCESS;
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTool::Disable]: Disable Failed %d \n",this->GetToolName());
                this->SetState(previous_state);
                return FAILURE;
            }
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::Disable]: Tool %d is not Enabled or Attached \n",this->GetToolName());
            this->SetState(previous_state);
            return FAILURE;

        }
    }

    ResultType OptitrackTool::AttachTrackable( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::AttachTrackable]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToAttachTrackable);

        if( previous_state == STATE_TOOL_Configurated ){


            // get the ID for next tool in Optitrack System
            this->m_OptitrackID = this->GetIDnext();
            fprintf(stdout, "<INFO> - [OptitrackTool::AttachTrackable]: ID %i\n",this->m_OptitrackID);

            // Create the Tool
            NPRESULT resultCreateTrackable;
            for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
            {
                resultCreateTrackable = TT_CreateTrackable(m_ToolName.c_str(), this->m_OptitrackID,this->GetNumberOfMarkers(),this->m_CalibrationPoints);
                if(NPRESULT_SUCCESS == resultCreateTrackable)
                {
                    fprintf(stdout, "<INFO> - [OptitrackTool::AttachTrackable]: Trackable Created Successfully\n");
                    i = -1;
                }
                else
                {
                    if(i == 1)
                    {
                        fprintf(stdout, "#ERROR# - [OptitrackTool::AttachTrackable]: Cannot Create Tool\n");
                        this->SetFileConfiguration("");
                        this->SetNumberOfMarkers(0);
                        delete this->m_CalibrationPoints;
                        delete this->m_PivotPoint;
                        this->m_OptitrackID = -1;
                        this->SetState(previous_state);
                        return FAILURE;
                    }
                }
            }

            NPRESULT resultUpdate;
            NPRESULT resultTrackableTranslatePivot;
            for( int i=OPTITRACK_ATTEMPTS; i>0; i--)
            {
                resultUpdate = TT_Update();
                if(NPRESULT_SUCCESS == resultUpdate)
                {
                    resultTrackableTranslatePivot = TT_TrackableTranslatePivot(this->m_OptitrackID,this->m_PivotPoint[0],this->m_PivotPoint[1],this->m_PivotPoint[2]);
                    if(NPRESULT_SUCCESS == resultCreateTrackable)
                    {
                        fprintf(stdout, "<INFO> - [OptitrackTool::AttachTrackable]: Pivot Translation Successfull\n");
                        this->SetState(STATE_TOOL_Attached);
                        return SUCCESS;
                    }
                    else
                    {
                        if(i == 1)
                        {
                            fprintf(stdout, "#ERROR# - [OptitrackTool::AttachTrackable]: Cannot do Pivot Translation\n");
                            this->SetFileConfiguration("");
                            this->SetNumberOfMarkers(0);
                            delete this->m_CalibrationPoints;
                            delete this->m_PivotPoint;
                            this->m_OptitrackID = -1;
                            this->SetState(previous_state);
                            NPRESULT resultRemoveTrackable = TT_RemoveTrackable(this->GetOptitrackID());
                            if(resultRemoveTrackable != NPRESULT_SUCCESS)
                            {
                                fprintf(stdout, "#FATAL ERROR# - [OptitrackTool::AttachTrackable]: Tool Was created but Pivot was not set correctly\n");
                            }
                            return FAILURE;
                        }
                    }
                }
                else
                {
                    if(i == 1)
                        {
                            fprintf(stdout, "#ERROR# - [OptitrackTool::AttachTrackable]: Cannot do Pivot Translation\n");
                            this->SetFileConfiguration("");
                            this->SetNumberOfMarkers(0);
                            delete this->m_CalibrationPoints;
                            delete this->m_PivotPoint;
                            this->m_OptitrackID = -1;
                            this->SetState(previous_state);
                            NPRESULT resultRemoveTrackable = TT_RemoveTrackable(this->GetOptitrackID());
                            if(resultRemoveTrackable != NPRESULT_SUCCESS)
                            {
                                fprintf(stdout, "#FATAL ERROR# - [OptitrackTool::AttachTrackable]: Tool Was created but Pivot was not set correctly\n");
                            }
                            return FAILURE;
                        }
                }
            }

        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::AttachTrackable]: Tool %d is not Configurated \n",this->GetToolName());
            this->SetState(previous_state);
            return FAILURE;
        }
    }

    bool OptitrackTool::IsTracked( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::IsTracked]\n");
        return TT_IsTrackableTracked(this->GetOptitrackID());
    }

    bool OptitrackTool::IsDataValid( void ) const
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::IsDataValid]\n");
        // returns true if the current position data is valid (no error during tracking, tracking error below threshold, ...)
        return this->m_DataValid;
    }

    void OptitrackTool::SetDataValid(bool validate)
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::SetDataValid]\n");
        // sets if the tracking data (position & Orientation) is valid
        this->m_DataValid = validate;
    }

    bool OptitrackTool::IsIndeterminateValue(const float pV)
    {
        return (pV != pV);
    }

    bool OptitrackTool::IsInfiniteValue(const float pV)
    {
        return (fabs(pV) == std::numeric_limits<float>::infinity());
    }

    ResultType OptitrackTool::UpdateTool( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToUpdateTrackable);

        if( previous_state != STATE_TOOL_Enabled)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::UpdateTool]: Tool %d is not in Enabled State \n",this->GetToolName());
            this->SetState(previous_state);
            this->SetDataValid(false);
            return FAILURE;
        }

        float yaw,pitch,roll;
        float data[7];

        if(TT_Update() == NPRESULT_SUCCESS)
        {
            if(this->IsTracked())
            {
                TT_TrackableLocation(this->m_OptitrackID,   &data[0],    &data[1],    &data[2],                 // Position
                                                            &data[3],    &data[4],    &data[5],    &data[6],    // Orientation
                                                            &yaw,        &pitch,      &roll);                   // Orientation

                for( int i=0; i<7; i++)
                {
                    if( this->IsInfiniteValue(data[i]) || this->IsInfiniteValue(data[i])) // Possible Tracking check for INF numbers
                    {
                        this->SetDataValid(false);
                        fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]: Data set to INF by the system\n");
                        return FAILURE;
                    }
                }

            // m -> mm
                this->m_Position[0] = data[0]*1000;
                this->m_Position[1] = data[1]*1000;
                this->m_Position[2] = -data[2]*1000; // Correction from LeftHanded to RightHanded system

                this->m_Orientation.x() = data[3];
                this->m_Orientation.y() = data[4];
                this->m_Orientation.z() = -data[5];
                this->m_Orientation.r() = data[6];

                OptitrackHelper::ConvertMatrix(this->m_TransformMatrix,  this->m_Position , this->m_Orientation);

                this->SetDataValid(true);

                fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]: Success  Tool %s\n",this->GetToolName());
                fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]:          Pos = [ %f , %f , %f ]\n",this->m_Position[0],this->m_Position[1],this->m_Position[2]);
                fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]:          Ori = [ %f , %f , %f , %f ]\n",this->m_Orientation.x(),this->m_Orientation.y(),this->m_Orientation.z(),this->m_Orientation.r());

                this->SetState(STATE_TOOL_Enabled);
                return SUCCESS;
            }
            else
            {
                this->SetDataValid(false);
            fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]: Trackable %s NOT tracked\n", this->GetToolName());
            }
        }
        else
        {
         fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]: Failed API Updating\n");
         this->SetState(previous_state);
         this->SetDataValid(false);
         return FAILURE;
        }
    }
}