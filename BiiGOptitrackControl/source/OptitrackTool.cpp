#include "OptitrackTool.h"
#include <NPTrackingTools.h>

//tinyxml2
#include <tinyxml2.h>
#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif

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
            return;
        }
        this->m_State = state_;
        this->Modified();
        this->m_StateMutex->Unlock();
    }

    OptitrackTool::OPTITRACK_TOOL_STATE OptitrackTool::GetState( void )
    {
        MutexLockHolder lock(*m_StateMutex);
        return this->m_State;
        this->m_StateMutex->Unlock();
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

        fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]: Tool Name %s\n", this->GetToolName());


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

	ResultType OptitrackTool::ConfigureToolByXmlFile(std::string nameFile)
	{
		//Variable definition	
		//Calibration Date
		int dayCurrentDate, monthCurrentDate, yearCurrentDate;
		//Calibration Error
		float calibrationErrorRMS, calibrationErrorMean, calibrationErrorSD;
		float calibrationErrorMedian, calibrationErrorQ1, calibrationErrorQ3;
		//Tool name
		std::string toolName;
		//Number of markers
		int toolNumberOfMarkers;
		//Calibration file name
		std::string calibrationFileName;

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
			fprintf(stdout, "[XML READING ERROR] Problem accesing to ConfigurationFile element! \n");
			return FAILURE;

		}

		//FileType
		tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("FileType");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "[XML READING ERROR] Problem parsing the element FileType! \n");
			return FAILURE;

		}

		//CalibrationDate
		pElement = pRoot->FirstChildElement("CalibrationDate");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		if (eResult == tinyxml2::XMLError::XML_SUCCESS){ //If CalibrationDate element do not exist, attributes are not accessed.
			eResult = pElement->QueryIntAttribute("D", &dayCurrentDate);
			XMLCheckResult(eResult);

			eResult = pElement->QueryIntAttribute("M", &monthCurrentDate);
			XMLCheckResult(eResult);

			eResult = pElement->QueryIntAttribute("Y", &yearCurrentDate);
			XMLCheckResult(eResult);

		}

		//CalibrationError
		pElement = pRoot->FirstChildElement("CalibrationError");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		if (eResult == tinyxml2::XMLError::XML_SUCCESS){ //If CalibrationError element do not exist, attributes are not accessed.
			eResult = pElement->QueryFloatAttribute("RMS", &calibrationErrorRMS);
			XMLCheckResult(eResult);

			eResult = pElement->QueryFloatAttribute("Mean", &calibrationErrorMean);
			XMLCheckResult(eResult);

			eResult = pElement->QueryFloatAttribute("SD", &calibrationErrorSD);
			XMLCheckResult(eResult);

			eResult = pElement->QueryFloatAttribute("Median", &calibrationErrorMedian);
			XMLCheckResult(eResult);

			eResult = pElement->QueryFloatAttribute("Q1", &calibrationErrorQ1);
			XMLCheckResult(eResult);

			eResult = pElement->QueryFloatAttribute("Q3", &calibrationErrorQ3);
			XMLCheckResult(eResult);

		}

		//ToolName
		pElement = pRoot->FirstChildElement("ToolName");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "[XML READING ERROR] Problem parsing the element ToolName! \n");
			return FAILURE;

		}
		toolName = pElement->GetText();
		this->SetToolName(toolName);

		//ToolMarkersNum
		pElement = pRoot->FirstChildElement("ToolMarkersNum");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "[XML READING ERROR] Problem parsing the element ToolMarkersNum! \n");
			return FAILURE;

		}
		eResult = pElement->QueryIntText(&toolNumberOfMarkers);
		XMLCheckResult(eResult);
		this->SetNumberOfMarkers(toolNumberOfMarkers);

		//ToolMarkers
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

		}



		//ToolPivot
		pElement = pRoot->FirstChildElement("ToolPivot");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "[XML READING ERROR] Problem parsing the element ToolPivot! \n");
			return FAILURE;

		}
		eResult = pElement->QueryFloatAttribute("x", &this->m_PivotPoint[0]);
		XMLCheckResult(eResult);
		eResult = pElement->QueryFloatAttribute("y", &this->m_PivotPoint[1]);
		XMLCheckResult(eResult);
		eResult = pElement->QueryFloatAttribute("z", &this->m_PivotPoint[2]);
		XMLCheckResult(eResult);

		//CalibrationFile
		pElement = pRoot->FirstChildElement("CalibrationFile");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		if (eResult == tinyxml2::XMLError::XML_SUCCESS){ //If CalibrationFile element do not exist, value is not read.
			calibrationFileName = pElement->GetText();
		}
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
            fprintf(stdout, "#ERROR# - [OptitrackTool::DeleteTrackable]: Cannot Remove Trackable %s \n",this->GetToolName());
            this->SetState(previous_state);
            return FAILURE;
        }
        else
        {
            fprintf(stdout, "<INFO> - [OptitrackTool::DeleteTrackable]: %s Deleted \n",this->GetToolName());
            this->SetState(STATE_TOOL_Configurated);
            return SUCCESS;
        }

        fprintf(stdout, "#ERROR# - [OptitrackTool::DeleteTrackable]: Cannot Remove Trackable %s \n",this->GetToolName());
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
                fprintf(stdout, "<INFO> - [OptitrackTool::Enable]: %s Enabled \n",this->GetToolName());
                this->SetState(STATE_TOOL_Enabled);
                return SUCCESS;
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTool::Enable]: Enable Failed %s \n",this->GetToolName());
                this->SetState(previous_state);
                return FAILURE;
            }
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::Enable]: Tool %s is not Disabled or Attached \n",this->GetToolName());
            this->SetState(previous_state);
            return FAILURE;

        }
    }

    ResultType OptitrackTool::Disable( void )
    {
        fprintf(stdout, "<INFO> - [OptitrackTool::Disable]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
		fprintf(stdout, "<INFO TEST> - [OptitrackTool::Disable] Tool State --> %d\n", previous_state);
		this->SetState(STATE_TOOL_AttemptingToDisableTrackable);

        if( (previous_state == STATE_TOOL_Enabled) ||
            (previous_state == STATE_TOOL_Attached))
        {
            TT_SetTrackableEnabled(this->GetOptitrackID(), false);
            if(TT_TrackableEnabled(this->GetOptitrackID()) == false)
            {
                fprintf(stdout, "<INFO> - [OptitrackTool::Disable]: %s Disabled \n",this->GetToolName());
                this->SetState(STATE_TOOL_Disabled);
                return SUCCESS;
            }
            else
            {
                fprintf(stdout, "#ERROR# - [OptitrackTool::Disable]: Disable Failed %s \n",this->GetToolName());
                this->SetState(previous_state);
                return FAILURE;
            }
        }
        else
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::Disable]: Tool %s is not Enabled or Attached \n",this->GetToolName());
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
            fprintf(stdout, "#ERROR# - [OptitrackTool::AttachTrackable]: Tool %s is not Configurated \n",this->GetToolName());
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
            fprintf(stdout, "#ERROR# - [OptitrackTool::UpdateTool]: Tool %s is not in Enabled State \n",this->GetToolName());
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