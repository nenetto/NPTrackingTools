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
		int result = objTracker->CheckNumberOfMarkers();

		if (result == 1)
		{
			fprintf(stdout, "SUCCESS for one marker\n");
			objTracker->TestCalibration(resultFile);
		}


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

