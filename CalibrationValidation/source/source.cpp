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

int main(int argc, char *argv[])
{

	// Check number of parameters

	if (argc < 5)
	{
		fprintf(stdout, "[ERROR] The number of parameters is wrong.\n");
		printHelp();
	}
	else
	{

		// Read parameters
		std::string calFile(argv[1]);
		int Thr = atoi(argv[2]);
		int Exp = atoi(argv[3]);
		int Led = atoi(argv[4]);
		std::string resultFile(argv[5]);
		
		fprintf(stdout, "Calibration File: %s\n", calFile.c_str());
		fprintf(stdout, "Result File: %s\n", resultFile.c_str());

		// Create Tracker
		Optitrack::OptitrackTracker::Pointer objTracker = Optitrack::OptitrackTracker::New();
		
		// Load Calibration File
		objTracker->SetCalibrationFile(calFile);
		
		int result = objTracker->Close();
		std::cout << " Close result: " << result << std::endl;
		std::cout << " State -> " << objTracker->GetState() << std::endl;

		// Open Connection
		result = objTracker->Open();
		std::cout << " Open result: " << result << std::endl;
		std::cout << " State -> " << objTracker->GetState() << std::endl;

		result = objTracker->LoadCalibration();
		std::cout << " LoadCalibration result: " << result << std::endl;
		std::cout << " State -> " << objTracker->GetState() << std::endl;

		int numCam = objTracker->GetCameraNumber();

		// Set Camera Params
		//objTracker->SetCameraParams(Exp, Thr, Led);
		result = objTracker->SetCameraParams(Exp, Thr, Led, 4);
		std::cout << " SetCamParams result: " << result << std::endl;
		
		// Testing Number of viewed markers
		result = objTracker->CheckNumberOfMarkers();

		if (result == 1)
		{
			fprintf(stdout, "SUCCESS for one marker\n");
			objTracker->TestCalibration(resultFile);
		}
		
		result = objTracker->Close();
		std::cout << " Close result: " << result << std::endl;
		std::cout << " State -> " << objTracker->GetState() << std::endl;

	}

	system("PAUSE");
    return 0;
}

void printHelp(void)
{
	fprintf(stdout, "[Usage of CalibrationValidation]\n");
	fprintf(stdout, "\n");
	fprintf(stdout, "CalibrationValidation.exe CalibrationFile Thr Exp Led OutputResultFile");
	fprintf(stdout, "\n");
	fprintf(stdout, "CalibrationFile: Path to the tested calibration file ");
	fprintf(stdout, "Thr: Set Camera Threshold ");
	fprintf(stdout, "Exp: Set Camera Exposure ");
	fprintf(stdout, "Led: Set Camera Illumination ");
	fprintf(stdout, "OutputResultFile: Path to the CSV file where results will be saved");
}

