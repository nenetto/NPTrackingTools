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
	Optitrack::OptitrackTracker::Pointer objTracker = Optitrack::OptitrackTracker::New();
	std::string myCalfile = "C:/DavidGarciaMato/Calibration.cal";
	objTracker->SetCalibrationFile(myCalfile);

	Optitrack::OptitrackTool::Pointer objToolPolaris = Optitrack::OptitrackTool::New();
	std::string namefilePolaris = "C:/DavidGarciaMato/PunteroPolaris.txt";
	objToolPolaris->ConfigureToolByTxtFile(namefilePolaris);
	/*
	Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
	std::string namefilePolaris = "C:/David/PunteroPolarisXML.xml";
	objToolOptitrack->ConfigureToolByXmlFile(namefilePolaris);
	*/

	Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
	std::string namefileOptitrack = "C:/DavidGarciaMato/OptitrackRigidBody.txt";
	objToolOptitrack->ConfigureToolByTxtFile(namefileOptitrack);
	/*
	Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
	std::string namefileOptitrack = "C:/David/OptitrackRigidBodyXML.xml";
	objToolOptitrack->ConfigureToolByXmlFile(namefileOptitrack);
	*/
	
		
	/*
	result = objTracker->Open();
	cout << " Open result: " << result << endl;
	cout << " State -> " << objTracker->GetState() << endl;

	cout << " LoadCalibration result: " << objTracker->LoadCalibration() << endl;
	cout << " State -> " << objTracker->GetState() << endl;

	result = objTracker->SetCameraParams(7, 200, 14, 4);
	cout << " SetCamParams result: " << result << endl;
	
	Optitrack::OptitrackTool::Pointer objTool = Optitrack::OptitrackTool::New();
	std::string namefile = "C:/David/OptitrackRigidBody.txt";
	objTool->ConfigureToolByTxtFile(namefile);
	*/
	/*
	Optitrack::OptitrackTool::Pointer objTool = Optitrack::OptitrackTool::New();
	std::string namefile = "J:\PunteroPolarisXML.xml";
	objTool->ConfigureToolByXmlFile(namefile);
	*/

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
		default:
			break;
		}
		currentState = objTracker->GetState();

	}
	
	system("PAUSE");

	return 0;
}