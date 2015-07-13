// A simple program that computes the square root of a number
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tinyxml2.h>
//#include "NPTrackingTools.h"
//#include "BiiGOptitrackControl.h"
#include "OptitrackTracker.h"
#include "OptitrackTool.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif

#include <sstream>
#include <string.h>
using namespace std;


int main(int argc, char *argv[])
{
	int option;
	cout << " " << endl;
	cout << "Option: (1) LoadXMLConfFile (2) Manual" << endl;
	cin >> option;
	
	if (option == 1)
	{
		Optitrack::OptitrackTracker::Pointer objTracker = Optitrack::OptitrackTracker::New();
		std::string myConfigurationFile = "C:/DavidGarciaMato/ConfigurationFile_TestDavid.xml";
		int result = objTracker->LoadXMLConfigurationFile(myConfigurationFile);
	} 
	else
	{
		Optitrack::OptitrackTracker::Pointer objTracker = Optitrack::OptitrackTracker::New();
		std::string myCalfile = "C:/DavidGarciaMato/Calibration.cal";
		objTracker->SetCalibrationFile(myCalfile);
		
		/*
		Optitrack::OptitrackTool::Pointer objToolPolaris = Optitrack::OptitrackTool::New();
		std::string namefilePolaris = "C:/DavidGarciaMato/PunteroPolaris.txt";
		objToolPolaris->ConfigureToolByTxtFile(namefilePolaris);
		/*/
		Optitrack::OptitrackTool::Pointer objToolPolaris = Optitrack::OptitrackTool::New();
		std::string namefilePolaris = "C:/DavidGarciaMato/PunteroPolarisXML.xml";
		objToolPolaris->ConfigureToolByXmlFile(namefilePolaris);
		
		/*
		Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
		std::string namefileOptitrack = "C:/DavidGarciaMato/OptitrackRigidBody.txt";
		objToolOptitrack->ConfigureToolByTxtFile(namefileOptitrack);
		/*/
		Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
		std::string namefileOptitrack = "C:/DavidGarciaMato/OptitrackRigidBodyXML.xml";
		objToolOptitrack->ConfigureToolByXmlFile(namefileOptitrack);
		
		int result;
		int currentState = objTracker->GetState();
		bool stop = false;
		int option;
		int numberAttachedTools;
		//int nextID, numberOfMarkers, optitrackID;
		string toolName;

		while (stop == false)
		{
			switch (currentState)
			{

			case 0: //IDLE STATE
				cout << " " << endl;
				cout << "Option:Open(0),LoadCalibration(1),AddTool(2),RemoveTool(3),StartTracking(4)" << endl;
				cout << "StopTracking(5),Reset(6),Close(7),InfoTools(8),SetCameraParameters(9)" << endl;
				option;
				cin >> option;
				switch (option)
				{
				case 0:
					result = objTracker->Open();
					cout << " Open result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 1:
					result = objTracker->LoadCalibration();
					cout << " LoadCalibration result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 2:
					cout << " " << endl;
					cout << "Which tool do you want to add?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolOptitrack);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						break;
					case 2:
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolPolaris);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}


					break;

				case 3:
					cout << " " << endl;
					cout << "Which tool do you want to remove?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						result = objTracker->RemoveTrackerTool(objToolOptitrack);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					case 2:
						result = objTracker->RemoveTrackerTool(objToolPolaris);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}
					break;
				case 4:
					for (int i = 0; i < 3; i++){
						result = objTracker->StartTracking();
						cout << " StartTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						Sleep(100);
						result = objTracker->StopTracking();
						cout << " StopTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						system("PAUSE");
					}
					break;

				case 5:
					result = objTracker->StopTracking();
					cout << " StopTracking result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 6:
					result = objTracker->Reset();
					cout << " Reset result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 7:
					result = objTracker->Close();
					cout << " Close result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					stop = true;
					break;
				case 8:
					numberAttachedTools = objTracker->GetNumberOfAttachedTools();
					cout << " Number of attached tools: " << numberAttachedTools << endl;
					//nextID = objTool->GetIDnext();
					//numberOfMarkers = objTool->GetNumberOfMarkers();
					//optitrackID = objTool->GetOptitrackID();
					//toolName = objTool->GetToolName();
					//cout << " Info Tool: " << "[numberOfMarkers] " << numberOfMarkers << "; [optitrackID] " << optitrackID << "; [toolName] " << toolName << endl;
					break;
				case 9:
					result = objTracker->SetCameraParams(7, 200, 14, 4);
					cout << " SetCamParams result: " << result << endl;
					break;
				}
				break;
			case 1: //TRACKER TOOL ATTACHED
				cout << "Tracker Tool Attached State does not exist anymore!" << endl;
				break;
			case 2: //COMMUNICATION ESTABLISHED
				cout << " " << endl;
				cout << "Option:Open(0),LoadCalibration(1),AddTool(2),RemoveTool(3),StartTracking(4)" << endl;
				cout << "StopTracking(5),Reset(6),Close(7),InfoTools(8),SetCameraParameters(9)" << endl;
				option;
				cin >> option;
				switch (option)
				{
				case 0:
					result = objTracker->Open();
					cout << " Open result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 1:
					result = objTracker->LoadCalibration();
					cout << " LoadCalibration result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 2:
					cout << " " << endl;
					cout << "Which tool do you want to add?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolOptitrack);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						break;
					case 2:
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolPolaris);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}


					break;

				case 3:
					cout << " " << endl;
					cout << "Which tool do you want to remove?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						result = objTracker->RemoveTrackerTool(objToolOptitrack);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					case 2:
						result = objTracker->RemoveTrackerTool(objToolPolaris);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}
					break;
				case 4:
					for (int i = 0; i < 3; i++){
						result = objTracker->StartTracking();
						cout << " StartTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						Sleep(100);
						result = objTracker->StopTracking();
						cout << " StopTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						system("PAUSE");
					}
					break;

				case 5:
					result = objTracker->StopTracking();
					cout << " StopTracking result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 6:
					result = objTracker->Reset();
					cout << " Reset result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 7:
					result = objTracker->Close();
					cout << " Close result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					stop = true;
					break;
				case 8:
					numberAttachedTools = objTracker->GetNumberOfAttachedTools();
					cout << " Number of attached tools: " << numberAttachedTools << endl;
					//nextID = objTool->GetIDnext();
					//numberOfMarkers = objTool->GetNumberOfMarkers();
					//optitrackID = objTool->GetOptitrackID();
					//toolName = objTool->GetToolName();
					//cout << " Info Tool: " << "[numberOfMarkers] " << numberOfMarkers << "; [optitrackID] " << optitrackID << "; [toolName] " << toolName << endl;
					break;
				case 9:
					result = objTracker->SetCameraParams(7, 200, 14, 4);
					cout << " SetCamParams result: " << result << endl;
					break;
				}
				break;
			case 3: //TRACKING STATE
				cout << " " << endl;
				cout << "Option:Open(0),LoadCalibration(1),AddTool(2),RemoveTool(3),StartTracking(4)" << endl;
				cout << "StopTracking(5),Reset(6),Close(7),InfoTools(8),SetCameraParameters(9)" << endl;
				option;
				cin >> option;
				switch (option)
				{
				case 0:
					result = objTracker->Open();
					cout << " Open result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 1:
					result = objTracker->LoadCalibration();
					cout << " LoadCalibration result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 2:
					cout << " " << endl;
					cout << "Which tool do you want to add?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolOptitrack);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						break;
					case 2:
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolPolaris);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}


					break;

				case 3:
					cout << " " << endl;
					cout << "Which tool do you want to remove?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						result = objTracker->RemoveTrackerTool(objToolOptitrack);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					case 2:
						result = objTracker->RemoveTrackerTool(objToolPolaris);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}
					break;
				case 4:
					for (int i = 0; i < 3; i++){
						result = objTracker->StartTracking();
						cout << " StartTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						Sleep(100);
						result = objTracker->StopTracking();
						cout << " StopTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						system("PAUSE");
					}
					break;

				case 5:
					result = objTracker->StopTracking();
					cout << " StopTracking result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 6:
					result = objTracker->Reset();
					cout << " Reset result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 7:
					result = objTracker->Close();
					cout << " Close result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					stop = true;
					break;
				case 8:
					numberAttachedTools = objTracker->GetNumberOfAttachedTools();
					cout << " Number of attached tools: " << numberAttachedTools << endl;
					//nextID = objTool->GetIDnext();
					//numberOfMarkers = objTool->GetNumberOfMarkers();
					//optitrackID = objTool->GetOptitrackID();
					//toolName = objTool->GetToolName();
					//cout << " Info Tool: " << "[numberOfMarkers] " << numberOfMarkers << "; [optitrackID] " << optitrackID << "; [toolName] " << toolName << endl;
					break;
				case 9:
					result = objTracker->SetCameraParams(7, 200, 14, 4);
					cout << " SetCamParams result: " << result << endl;
					break;
				}
				break;
			case 4: //CALIBRATED STATE
				cout << " " << endl;
				cout << "Option:Open(0),LoadCalibration(1),AddTool(2),RemoveTool(3),StartTracking(4)" << endl;
				cout << "StopTracking(5),Reset(6),Close(7),InfoTools(8),SetCameraParameters(9)" << endl;
				option;
				cin >> option;
				switch (option)
				{
				case 0:
					result = objTracker->Open();
					cout << " Open result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 1:
					result = objTracker->LoadCalibration();
					cout << " LoadCalibration result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 2:
					cout << " " << endl;
					cout << "Which tool do you want to add?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolOptitrack);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolOptitrack->GetState() << endl;
						cout << " OptitrackID: " << objToolOptitrack->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolOptitrack->GetToolName() << endl;
						numberAttachedTools = objTracker->GetNumberOfAttachedTools();
						cout << " Number of attached tools: " << numberAttachedTools << endl;
						cout << " Pointer state: " << objToolPolaris->GetState() << endl;
						cout << " Optitrack state: " << objToolOptitrack->GetState() << endl;
						break;
					case 2:
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						result = objTracker->AddTrackerTool(objToolPolaris);
						cout << " AddTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						cout << " Tool State: " << objToolPolaris->GetState() << endl;
						cout << " OptitrackID: " << objToolPolaris->GetOptitrackID() << endl;
						cout << " Tool Name: " << objToolPolaris->GetToolName() << endl;
						numberAttachedTools = objTracker->GetNumberOfAttachedTools();
						cout << " Number of attached tools: " << numberAttachedTools << endl;
						cout << " Pointer state: " << objToolPolaris->GetState() << endl;
						cout << " Optitrack state: " << objToolOptitrack->GetState() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}


					break;

				case 3:
					cout << " " << endl;
					cout << "Which tool do you want to remove?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << endl;
					option;
					cin >> option;
					switch (option)
					{
					case 1:
						result = objTracker->RemoveTrackerTool(objToolOptitrack);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					case 2:
						result = objTracker->RemoveTrackerTool(objToolPolaris);
						cout << " RemoveTracker result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						break;
					default:
						fprintf(stdout, "#ERROR# Non-valid option!\n");
						break;
					}
					break;
				case 4:
					for (int i = 0; i < 3; i++){
						result = objTracker->StartTracking();
						cout << " StartTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						Sleep(100);
						result = objTracker->StopTracking();
						cout << " StopTracking result: " << result << endl;
						cout << " State -> " << objTracker->GetState() << endl;
						system("PAUSE");
					}
					break;

				case 5:
					result = objTracker->StopTracking();
					cout << " StopTracking result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 6:
					result = objTracker->Reset();
					cout << " Reset result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					break;
				case 7:
					result = objTracker->Close();
					cout << " Close result: " << result << endl;
					cout << " State -> " << objTracker->GetState() << endl;
					stop = true;
					break;
				case 8:
					numberAttachedTools = objTracker->GetNumberOfAttachedTools();
					cout << " Number of attached tools: " << numberAttachedTools << endl;
					cout << " Pointer state: " << objToolPolaris->GetState() << endl;
					cout << " Optitrack state: " << objToolOptitrack->GetState() << endl;
					//nextID = objTool->GetIDnext();
					//numberOfMarkers = objTool->GetNumberOfMarkers();
					//optitrackID = objTool->GetOptitrackID();
					//toolName = objTool->GetToolName();
					//cout << " Info Tool: " << "[numberOfMarkers] " << numberOfMarkers << "; [optitrackID] " << optitrackID << "; [toolName] " << toolName << endl;
					break;
				case 9:
					result = objTracker->SetCameraParams(7, 200, 14, 4);
					cout << " SetCamParams result: " << result << endl;
					break;
				}
				break;
			default:
				break;
			}
			currentState = objTracker->GetState();

		}

	}

	system("PAUSE");

	return 0;

}

/*
ResultType LoadXMLConfigurationFile(std::string nameFile)
{
	//== XML Configuration File reading

	std::string calibrationFile, toolFile;
	float camparamExposure, camparamThreshold, camparamIntensity;
	int toolNumber;
	const int maxNumberTools = 10;
	string toolFilesArray[maxNumberTools];

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
		fprintf(stdout, "[XML READING ERROR] Problem accesing to NPTrackingTools element! \n");
		return FAILURE;

	}

	//CalibrationFile
	tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("CalibrationFile");
	if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	XMLCheckResult(eResult);
	if (eResult != tinyxml2::XMLError::XML_SUCCESS){
		fprintf(stdout, "[XML READING ERROR] Problem parsing the element CalibrationFile! \n");
		return FAILURE;

	}
	try
	{
		calibrationFile = pElement->Attribute("file");
	}
	catch (int e)
	{
		fprintf(stdout, "[XML READING ERROR] Problem reading attribute File from CalibrationFile element! \n");
		std::cout << e << std::endl;
		return FAILURE;
	}
	
	//CameraParameters
	pElement = pRoot->FirstChildElement("CameraParameters");
	if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	if (eResult == tinyxml2::XMLError::XML_SUCCESS){ //If CalibrationError element do not exist, attributes are not accessed.
		eResult = pElement->QueryFloatAttribute("Exposure", &camparamExposure);
		XMLCheckResult(eResult);

		eResult = pElement->QueryFloatAttribute("Threshold", &camparamThreshold);
		XMLCheckResult(eResult);

		eResult = pElement->QueryFloatAttribute("Intensity", &camparamIntensity);
		XMLCheckResult(eResult);
	}

	
	//ToolNumber
	pElement = pRoot->FirstChildElement("ToolNumber");
	if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	XMLCheckResult(eResult);
	if (eResult != tinyxml2::XMLError::XML_SUCCESS){
		fprintf(stdout, "[XML READING ERROR] Problem parsing the element ToolMarkersNum! \n");
		return FAILURE;

	}
	eResult = pElement->QueryIntAttribute("Ntool", &toolNumber);
	XMLCheckResult(eResult);

	//Tools
	pElement = pRoot->FirstChildElement("ToolsList");
	if (pElement == nullptr) eResult = tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	XMLCheckResult(eResult);
	if (eResult != tinyxml2::XMLError::XML_SUCCESS){
		fprintf(stdout, "[XML READING ERROR] Problem parsing the element Tools! \n");
		return FAILURE;
	}
	int counter = 0;
	tinyxml2::XMLElement * pListElement = pElement->FirstChildElement("Tool");
	while ((counter < toolNumber) && (pListElement != nullptr))
	{
		try
		{
			toolFile = pListElement->Attribute("confile");
			toolFilesArray[counter] = toolFile;
		}
		catch (int e)
		{
			fprintf(stdout, "[XML READING ERROR] Problem reading attribute File from CalibrationFile element! \n");
			std::cout << e << std::endl;
			return FAILURE;
		}

		counter = counter + 1;

		pListElement = pListElement->NextSiblingElement("Tool");

	}

	//Display results (TEST)
	std::cout << "<INFO TEST DAVID - LOADXMLCONFIGURATIONFILE() >" << std::endl;
	std::cout << "Calibration File: " << calibrationFile << std::endl;
	std::cout << "Camera parameters: " << " [Exposure] " << camparamExposure << " [Threshold] " << camparamThreshold << " [Intensity] " << camparamIntensity << std::endl;
	std::cout << "Number of tools: " << toolNumber << std::endl;
	std::cout << "Tool 1 File: " << toolFilesArray[0] << std::endl;
	std::cout << "Tool 2 File: " << toolFilesArray[1] << std::endl;


	//== Tracking system initialization
	Optitrack::OptitrackTracker::Pointer objTracker = Optitrack::OptitrackTracker::New();
	objTracker->SetCalibrationFile(calibrationFile);

	int result = objTracker->Open();
	cout << " Open result: " << result << endl;
	cout << " State -> " << objTracker->GetState() << endl;

	result = objTracker->LoadCalibration();
	cout << " LoadCalibration result: " << result << endl;
	cout << " State -> " << objTracker->GetState() << endl;

	for (int i = 0; i < toolNumber; i++)
	{
		Optitrack::OptitrackTool::Pointer objTool = Optitrack::OptitrackTool::New();
		objTool->ConfigureToolByXmlFile(toolFilesArray[i]);
				
		result = objTracker->AddTrackerTool(objTool);
		cout << " AddTracker result: " << result << endl;
		cout << " State -> " << objTracker->GetState() << endl;
		cout << " Tool State: " << objTool->GetState() << endl;
		cout << " OptitrackID: " << objTool->GetOptitrackID() << endl;
		cout << " Tool Name: " << objTool->GetToolName() << endl;
	}

	for (int i = 0; i < 3; i++){
		result = objTracker->StartTracking();
		cout << " StartTracking result: " << result << endl;
		cout << " State -> " << objTracker->GetState() << endl;
		Sleep(100);
		result = objTracker->StopTracking();
		cout << " StopTracking result: " << result << endl;
		cout << " State -> " << objTracker->GetState() << endl;
		system("PAUSE");
	}

	return SUCCESS;
}
*/


