#include "OptitrackTool.h"

//tinyxml2 lib
#include <tinyxml2.h>
#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif

// NPTrackingTools (Optitrack API)
#include <NPTrackingTools.h>

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
		this->m_TransformMatrix = vnl_matrix<double>(4, 4);
    }

    OptitrackTool::~OptitrackTool()
    {
        this->m_MyMutex->Unlock();
        this->m_StateMutex->Unlock();
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
        //fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]\n");
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

        fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByTxtFile]: Tool Name %s\n", this->GetToolName().c_str());


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
		//fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByXmlFile]\n");
		OPTITRACK_TOOL_STATE previous_state = this->GetState();
		this->SetState(STATE_TOOL_AttemptingToReadXmlFile);

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
			fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem loading the file! \n");
			this->SetFileConfiguration("");
			this->SetState(previous_state);
			return FAILURE;
		}

		tinyxml2::XMLElement * pRoot = xmlDoc.FirstChildElement("NPTrackingTools");
		if (pRoot == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_FILE_READ_ERROR;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem accesing to NPTrackingTools element! \n");
			return FAILURE;
		}

		//FileType
		tinyxml2::XMLElement * pFileType = pRoot->FirstChildElement("FileType");
		if (pFileType == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the element FileType! \n");
			this->SetFileConfiguration("");
			this->SetState(previous_state);
			return FAILURE;
		}


		//CalibrationDate
		tinyxml2::XMLElement * pCalibrationDate = pRoot->FirstChildElement("CalibrationDate");
		if (pCalibrationDate != NULL)
		{
			eResult = pCalibrationDate->QueryIntAttribute("D", &dayCurrentDate);
			XMLCheckResult(eResult);
			eResult = pCalibrationDate->QueryIntAttribute("M", &monthCurrentDate);
			XMLCheckResult(eResult);
			eResult = pCalibrationDate->QueryIntAttribute("Y", &yearCurrentDate);
			XMLCheckResult(eResult);
		}

		//CalibrationError
		tinyxml2::XMLElement * pCalibrationError = pRoot->FirstChildElement("CalibrationError");
		if (pCalibrationDate != NULL)
		{
			eResult = pCalibrationError->QueryFloatAttribute("RMS", &calibrationErrorRMS);
			XMLCheckResult(eResult);
			if (eResult != tinyxml2::XMLError::XML_SUCCESS){
				fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the attribute RMS! \n");
				this->SetFileConfiguration("");
				this->SetState(previous_state);
				return FAILURE;
			}

			eResult = pCalibrationError->QueryFloatAttribute("Mean", &calibrationErrorMean);
			XMLCheckResult(eResult);
			if (eResult != tinyxml2::XMLError::XML_SUCCESS){
				fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the attribute Mean! \n");
				this->SetFileConfiguration("");
				this->SetState(previous_state);
				return FAILURE;
			}

			eResult = pCalibrationError->QueryFloatAttribute("SD", &calibrationErrorSD);
			XMLCheckResult(eResult);
			if (eResult != tinyxml2::XMLError::XML_SUCCESS){
				fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the attribute SD! \n");
				this->SetFileConfiguration("");
				this->SetState(previous_state);
				return FAILURE;
			}

			eResult = pCalibrationError->QueryFloatAttribute("Median", &calibrationErrorMedian);
			XMLCheckResult(eResult);
			if (eResult != tinyxml2::XMLError::XML_SUCCESS){
				fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the attribute Median! \n");
				this->SetFileConfiguration("");
				this->SetState(previous_state);
				return FAILURE;
			}

			eResult = pCalibrationError->QueryFloatAttribute("Q1", &calibrationErrorQ1);
			XMLCheckResult(eResult);
			if (eResult != tinyxml2::XMLError::XML_SUCCESS){
				fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the attribute Q1! \n");
				this->SetFileConfiguration("");
				this->SetState(previous_state);
				return FAILURE;
			}


			eResult = pCalibrationError->QueryFloatAttribute("Q3", &calibrationErrorQ3);
			XMLCheckResult(eResult);
			if (eResult != tinyxml2::XMLError::XML_SUCCESS){
				fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the attribute Q3! \n");
				this->SetFileConfiguration("");
				this->SetState(previous_state);
				return FAILURE;
			}
		}


		//ToolName
		tinyxml2::XMLElement *pToolName = pRoot->FirstChildElement("ToolName");
		if (pToolName == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the element ToolName! \n");
			this->SetFileConfiguration("");
			this->SetState(previous_state);
			return FAILURE;
		}
		toolName = pToolName->GetText();
		this->SetToolName(toolName);
		fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByXmlFile]: Tool Name %s\n", this->GetToolName().c_str());

		//ToolMarkersNum
		tinyxml2::XMLElement *pToolMarkersNum = pRoot->FirstChildElement("ToolMarkersNum");
		if (pToolMarkersNum == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the element ToolMarkersNum! \n");
			this->SetFileConfiguration("");
			this->SetState(previous_state);
			return FAILURE;
		}
		eResult = pToolMarkersNum->QueryIntText(&toolNumberOfMarkers);
		XMLCheckResult(eResult);
		this->SetNumberOfMarkers(toolNumberOfMarkers);
		fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByXmlFile]: Number of Markers %i\n", this->GetNumberOfMarkers());

		//ToolMarkers
		tinyxml2::XMLElement *pElement = pRoot->FirstChildElement("ToolMarkers");
		if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the element ToolMarkers! \n");
			this->SetFileConfiguration("");
			this->SetState(previous_state);
			return FAILURE;
		}
		this->m_CalibrationPoints = new float[3 * this->GetNumberOfMarkers()];
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

			fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByXmlFile]: Marker %d: [%f, %f, %f]\n", counter + 1, this->m_CalibrationPoints[counter * 3 + 0], this->m_CalibrationPoints[counter * 3 + 1], this->m_CalibrationPoints[counter * 3 + 2]);

			//Optitrack works with Left Handed System
			this->m_CalibrationPoints[counter * 3 + 0] = this->m_CalibrationPoints[counter * 3 + 0] / 1000;
			this->m_CalibrationPoints[counter * 3 + 1] = this->m_CalibrationPoints[counter * 3 + 1] / 1000;
			this->m_CalibrationPoints[counter * 3 + 2] = -this->m_CalibrationPoints[counter * 3 + 2] / 1000;

			counter = counter + 1;

			//Conversion from int to char*.
			std::stringstream strs;
			markerLocation = counter + 1;
			strs << (markerLocation);
			std::string temp_str = "m" + strs.str();
			char* char_type = (char*)temp_str.c_str();
			pListElement = pListElement->NextSiblingElement(char_type);
		}

		//ToolPivot
		this->m_PivotPoint = new float[3];
		tinyxml2::XMLElement *pToolPivot = pRoot->FirstChildElement("ToolPivot");
		if (pToolPivot == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
		XMLCheckResult(eResult);
		if (eResult != tinyxml2::XMLError::XML_SUCCESS){
			fprintf(stdout, "#ERROR# - [OptitrackTool::SetToolByXmlFile]: Problem parsing the element ToolPivot! \n");
			this->SetFileConfiguration("");
			this->SetState(previous_state);
			return FAILURE;
		}
		eResult = pToolPivot->QueryFloatAttribute("x", &this->m_PivotPoint[0]);
		XMLCheckResult(eResult);
		eResult = pToolPivot->QueryFloatAttribute("y", &this->m_PivotPoint[1]);
		XMLCheckResult(eResult);
		eResult = pToolPivot->QueryFloatAttribute("z", &this->m_PivotPoint[2]);
		XMLCheckResult(eResult);

		fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByXmlFile]: Pivot: [%f, %f, %f]\n", this->m_PivotPoint[0], this->m_PivotPoint[1], this->m_PivotPoint[2]);

		this->m_PivotPoint[0] = this->m_PivotPoint[0] / 1000;
		this->m_PivotPoint[1] = this->m_PivotPoint[1] / 1000;
		this->m_PivotPoint[2] = -this->m_PivotPoint[2] / 1000; // !!!!!!! Optitrack works with Left Handed System !!!!!!!

		//CalibrationFile
		tinyxml2::XMLElement *pCalibrationFile = pRoot->FirstChildElement("CalibrationFile");
		if (pCalibrationFile != NULL)
		{
			calibrationFileName = pCalibrationFile->GetText();
		}

		fprintf(stdout, "<INFO> - [OptitrackTool::SetToolByXmlFile]: Configuration Successfull\n");
		this->SetState(STATE_TOOL_Configurated);
		return SUCCESS;
	}

    int OptitrackTool::GetIDnext( void )
    {

        //fprintf(stdout, "<INFO> - [OptitrackTool::GetIDnext]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToGetIDnext);
		int num_trackables = TT_TrackableCount();
		//fprintf(stdout, "<INFO> - [OptitrackTool::GetIDnext]: Number of Trackables %i \n", num_trackables);

        if(num_trackables > -1)
        {
            //fprintf(stdout, "<INFO> - [OptitrackTool::GetIDnext]: Next ID %i \n",num_trackables);
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

    ResultType OptitrackTool::DettachTrackable( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTool::DeleteTrackable]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToDettachTrackable);

        int resultRemoveTrackable;
        resultRemoveTrackable = TT_RemoveTrackable(this->GetOptitrackID());

        if(resultRemoveTrackable != NPRESULT_SUCCESS)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::DeleteTrackable]: Cannot Remove Trackable %s \n",this->GetToolName().c_str());
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
        //fprintf(stdout, "<INFO> - [OptitrackTool::SetPosition]\n");
        // sets the position
        this->m_Position[0] = position[0];
        this->m_Position[1] = position[1];
        this->m_Position[2] = position[2];

        //fprintf(stdout, "<INFO> - [OptitrackTool::SetPosition]: SUCESS \n");
        return SUCCESS;
    }

	ResultType OptitrackTool::SetTransformMatrix(vnl_matrix<double> transform)
	{
		//fprintf(stdout, "<INFO> - [OptitrackTool::SetTransformMatrix]\n");
		// sets the position

		for (int i = 0; i < 4; i++){
			for (int j = 0; j < 4; j++){
				this->m_TransformMatrix(j, i) = transform(j, i);
			}
		}

		//fprintf(stdout, "<INFO> - [OptitrackTool::SetTransformMatrix]: SUCESS \n");
		return SUCCESS;
	}

    ResultType OptitrackTool::SetOrientation(vnl_quaternion<double> orientation)
    {
        //fprintf(stdout, "<INFO> - [OptitrackTool::SetOrientation]\n");
        // sets the orientation as a quaternion
        this->m_Orientation.x() = orientation.x();
        this->m_Orientation.y() = orientation.y();
        this->m_Orientation.z() = orientation.z();
        this->m_Orientation.r() = orientation.r();

        //fprintf(stdout, "<INFO> - [OptitrackTool::SetPosition]: SUCESS \n");
        return SUCCESS;
    }

    vnl_vector_fixed<double,3> OptitrackTool::GetPosition( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTool::GetPosition]\n");
        //fprintf(stdout, "<INFO> - [OptitrackTool::GetPosition]: SUCESS \n");
        return this->m_Position;
    }

    vnl_quaternion<double> OptitrackTool::GetOrientation( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTool::GetOrientation]\n");
        //fprintf(stdout, "<INFO> - [OptitrackTool::GetOrientation]: SUCESS \n");
        return this->m_Orientation;
    }

	vnl_matrix<double> OptitrackTool::GetTransformMatrix(void)
	{
		//fprintf(stdout, "<INFO> - [OptitrackTool::GetTransformMatrix]\n");
		//fprintf(stdout, "<INFO> - [OptitrackTool::GetTransformMatrix]: SUCESS \n");
		return this->m_TransformMatrix;
	}

    ResultType OptitrackTool::AttachTrackable( void )
    {
        //fprintf(stdout, "<INFO> - [OptitrackTool::AttachTrackable]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToAttachTrackable);

        if( previous_state == STATE_TOOL_Configurated ){


            // get the ID for next tool in Optitrack System
            this->m_OptitrackID = this->GetIDnext();
            //fprintf(stdout, "<INFO> - [OptitrackTool::AttachTrackable]: ID %i\n",this->m_OptitrackID);

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
                        //fprintf(stdout, "<INFO> - [OptitrackTool::AttachTrackable]: Pivot Translation Successfull\n");
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
        //fprintf(stdout, "<INFO> - [OptitrackTool::IsTracked]\n");
        return TT_IsTrackableTracked(this->GetOptitrackID());
    }

    bool OptitrackTool::IsDataValid( void ) const
    {
        //fprintf(stdout, "<INFO> - [OptitrackTool::IsDataValid]\n");
        return this->m_DataValid;
    }

    void OptitrackTool::SetDataValid(bool validate)
    {
        //fprintf(stdout, "<INFO> - [OptitrackTool::SetDataValid]\n");
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
        //fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]\n");
        OPTITRACK_TOOL_STATE previous_state = this->GetState();
        this->SetState(STATE_TOOL_AttemptingToUpdateTrackable);

        if( previous_state != STATE_TOOL_Attached)
        {
            fprintf(stdout, "#ERROR# - [OptitrackTool::UpdateTool]: Tool %s is not in Attached State \n",this->GetToolName());
            this->SetState(previous_state);
            this->SetDataValid(false);
            return FAILURE;
        }

        float yaw,pitch,roll;
        float data[7];
        unsigned int numberOfSeenMarkers;

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
					this->SetState(previous_state);
                    fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]: Data set to INF by the system\n");
                    return FAILURE;
                }
            }

        // m -> mm

			vnl_vector_fixed<double, 3> position;
			vnl_quaternion<double> orientation;

			position[0] = data[0] * 1000;
			position[1] = data[1] * 1000;
			position[2] = -data[2] * 1000; // Correction from LeftHanded to RightHanded system
			this->SetPosition(position);

			//fprintf(stdout, "%s Position:[%f,%f,%f]\n", this->GetToolName().c_str(),this->m_Position[0], this->m_Position[1], this->m_Position[2]);

			orientation.x() = data[3];
			orientation.y() = data[4];
			orientation.z() = data[5];
			orientation.r() = data[6];
			this->SetOrientation(orientation);
			//fprintf(stdout, "%s Orientation:[%f,%f,%f]\n",this->GetToolName().c_str(), this->m_Orientation.x(), this->m_Orientation.y(), this->m_Orientation.z(), this->m_Orientation.r());

            //this->m_Position[0] = data[0]*1000;
            //this->m_Position[1] = data[1]*1000;
            //this->m_Position[2] = -data[2]*1000; // Correction from LeftHanded to RightHanded system

            //this->m_Orientation.x() = data[3];
            //this->m_Orientation.y() = data[4];
            //this->m_Orientation.z() = -data[5];
            //this->m_Orientation.r() = data[6];

			vnl_matrix<double> transformMatrix = vnl_matrix<double>(4, 4);
			//transformMatrix(0, 0) = (-2)*pow(this->m_Orientation.y(), 2) - 2 * pow(this->m_Orientation.z(), 2) + 1;
			//transformMatrix(0, 1) = 2 * this->m_Orientation.r()*this->m_Orientation.z() + 2 * this->m_Orientation.x()*this->m_Orientation.y();
			//transformMatrix(0, 2) = 2 * this->m_Orientation.r()*this->m_Orientation.y() - 2 * this->m_Orientation.x()*this->m_Orientation.z();
			//transformMatrix(0, 3) = this->m_Position[0];
			//transformMatrix(1, 0) = 2 * this->m_Orientation.x()*this->m_Orientation.y() - 2 * this->m_Orientation.r()*this->m_Orientation.z();
			//transformMatrix(1, 1) = (-2) * pow(this->m_Orientation.x(), 2) - 2 * pow(this->m_Orientation.z(), 2) + 1;
			//transformMatrix(1, 2) = -2 * this->m_Orientation.r()*this->m_Orientation.x() - 2 * this->m_Orientation.y()*this->m_Orientation.z();
			//transformMatrix(1, 3) = this->m_Position[1];
			//transformMatrix(2, 0) = -2 * this->m_Orientation.r()*this->m_Orientation.y() - 2 * this->m_Orientation.x()*this->m_Orientation.z();
			//transformMatrix(2, 1) = 2 * this->m_Orientation.r()*this->m_Orientation.x() - 2 * this->m_Orientation.y()*this->m_Orientation.z();
			//transformMatrix(2, 2) = (-2)*pow(this->m_Orientation.x(), 2) - 2 * pow(this->m_Orientation.y(), 2) + 1;
			//transformMatrix(2, 3) = this->m_Position[2];
			//transformMatrix(3, 0) = 0.0;
			//transformMatrix(3, 1) = 0.0;
			//transformMatrix(3, 2) = 0.0;
			//transformMatrix(3, 3) = 1.0;

			this->ConvertMatrix(transformMatrix, this->m_Position, this->m_Orientation);

			this->SetTransformMatrix(transformMatrix);

			//fprintf(stdout, "%s Matrix:[%f,%f,%f,%f]\n", this->GetToolName().c_str(), this->m_TransformMatrix(0, 0), this->m_TransformMatrix(0, 1), m_TransformMatrix(0, 2), m_TransformMatrix(0, 3));

            numberOfSeenMarkers = TT_TrackableMarkerCount(this->m_OptitrackID);
            this->SetNumberOfSeenMarkers(numberOfSeenMarkers);

            this->SetDataValid(true);

			this->SetState(STATE_TOOL_Attached);
            return SUCCESS;
        }
        else
        {
            this->SetDataValid(false);
			this->SetState(previous_state);
        fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]: Trackable %s NOT tracked\n", this->GetToolName());
        }

         //fprintf(stdout, "<INFO> - [OptitrackTool::UpdateTool]: Failed API Updating\n");
         this->SetState(previous_state);
         this->SetDataValid(false);
         return FAILURE;
    }

	void OptitrackTool::ConvertMatrix(vnl_matrix<double> &R, vnl_vector_fixed<double, 3> position, vnl_quaternion<double> orientation)
	{
		//fprintf(stdout, "<INFO> - [OptitrackTool::ConvertMatrix]\n");
		double x, y, z, qx, qy, qz, qw;
		x = position[0];
		y = position[1];
		z = position[2];
		qx = orientation[0];
		qy = orientation[1];
		qz = orientation[2];
		qw = orientation[3];


		R(0, 0) = -2 * qy*qy - 2 * qz*qz + 1;
		R(0, 1) = 2 * qw*qz + 2 * qx*qy;
		R(0, 2) = 2 * qw*qy - 2 * qx*qz;
		R(0, 3) = x;

		R(1, 0) = 2 * qx*qy - 2 * qw*qz;
		R(1, 1) = -2 * qx*qx - 2 * qz*qz + 1;
		R(1, 2) = -2 * qw*qx - 2 * qy*qz;
		R(1, 3) = y;

		R(2, 0) = -2 * qw*qy - 2 * qx*qz;
		R(2, 1) = 2 * qw*qx - 2 * qy*qz;
		R(2, 2) = -2 * qx*qx - 2 * qy*qy + 1;
		R(2, 3) = z;

		R(3, 0) = 0.0;
		R(3, 1) = 0.0;
		R(3, 2) = 0.0;
		R(3, 3) = 1.0;

	}

}