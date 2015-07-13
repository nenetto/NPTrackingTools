#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <locale>
#include "OptitrackTracker.h"
#include "OptitrackTool.h"
#include "NPTrackingTools.h"


void printHelp(void);
unsigned int checkNumberOfMarkers(Optitrack::OptitrackTracker* objTracker);

int main(int argc, char *argv[])
{

	// Check number of parameters

	if (argc < 3)
	{
		fprintf(stdout, "[ERROR] The number of parameters is wrong.\n");
		printHelp();
	}
	else
	{

		// Read parameters

		std::string calFile(argv[1]);
		std::string resultFile(argv[2]);

		fprintf(stdout, "Calibration File: %s\n", calFile.c_str());
		fprintf(stdout, "Result File: %s\n", resultFile.c_str());


		// Create Tracker
		Optitrack::OptitrackTracker::Pointer objTracker = Optitrack::OptitrackTracker::New();
		

		// Load Calibration File
		objTracker->SetCalibrationFile(calFile);
		

		// Open Connection
		objTracker->Open();
		objTracker->LoadCalibration();

		// Testing Number of viewed markers
		int result = objTracker->checkNumberOfMarkers();


	}


    return 0;
}

void printHelp(void)
{
	fprintf(stdout, "[Usage of CalibrationValidation]\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "CalibrationValidation.exe CalibrationFile OutputResultFile");
	fprintf(stdout, "\n");
	fprintf(stdout, "CalibrationFile: Path to the tested calibration file ");
	fprintf(stdout, "OutputResultFile: Path to the CSV file where results will be saved");
}

void sleep(unsigned int mseconds)
{
	clock_t goal = mseconds + clock();
	while (goal > clock());
}


/*
unsigned int checkNumberOfMarkers(Optitrack::OptitrackTracker* objTracker)
{

	// Check number of Cameras
	int numberOfCameras = TT_CameraCount(8);

	// Variables for API results
	int resultUpdate, markerCount, cameraSettingsChanged;
	bool abort = false;
	bool *CameraUsed = new bool[numberOfCameras];


	for (int camera1 = 0; camera1 < numberOfCameras; camera1++)
	{
		for (int camera2 = camera1 + 1; camera2 < numberOfCameras; camera2++)
		{

			if (abort)
			{
				return 1;
			}

			// Shutdown rest of cameras

			for (int camIndex = 0; camIndex < numberOfCameras; camIndex++)
			{
				if (camIndex == camera1 || camIndex == camera2)
				{
					cameraSettingsChanged = TT_SetCameraSettings(camIndex, objTracker->GetVideoType(), objTracker->GetExp(), objTracker->GetThr(), objTracker->GetLed());
				}
				else
				{
					// Turn off the camera
					cameraSettingsChanged = TT_SetCameraSettings(camIndex, 2, 1, 255, 15); 
				}
			}

			// Message
			fprintf(stdout, "Testing Pair: %i - %i\n", camera1, camera2);

			// Read 100 frames and check that only one marker is in the field of view
			for (int count = 0; count < 50; count++)
			{
				resultUpdate = TT_Update();
				markerCount = TT_FrameMarkerCount(1);

				if (camera1 == 0 && camera2 == 7) markerCount = 3;
				

				if (markerCount > 1)
				{
					// Camera pair is watching more than one marker
					fprintf(stdout, "Camera Pair: %i - %i is watching more than one marker\n", camera1, camera2);
					fprintf(stdout, "[ABORTING]: Please make sure only one marker is visible in the field of view\n");
					abort = true;
					break;
				}


				sleep(5);
			}


		}
	}
	
	return 0;

}

*/