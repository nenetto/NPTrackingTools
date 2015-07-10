#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <locale>
#include "TestCameraPairs.h"
#include "Test2DCameraProjections.h"

int main(int argc, char *argv[])
{
	if (argc == 5)
	{	
		int Exp = atoi(argv[1]);
		if (Exp <= 0 || Exp > 480)
		{
			fprintf(stdout, "Argv[1]- Exposition level must be between 1 and 480\n");
			return 1;
		}
		int Threshold = atoi(argv[2]);
		if (Threshold < 0 || Threshold > 255)
		{
			fprintf(stdout, "Argv[3]- Threshold level must be between 0 and 255\n");
			return 1;
		}

		int Experiment = atoi(argv[3]);
		std::string Case = argv[4];
		

		if (Experiment == 1) /*1st test 2D Camera Projections of 3D Marker*/
		{	
			std::string Rep = "Rep1";
			CameraProjections::Test2DCameraProjectionsModif(Exp, Threshold , Case, Rep);
		}
		else if (Experiment == 2)  /*2nd test Camera Pairs without Init and Shutdown between iterations*/
		{
			NoInitShutdown::TestCameraPairNoInitShutdown(Exp, Threshold, Case);
		}
		else
		{
			fprintf(stdout, "Argv[3]- Experiment must equal 1 or 2\n");
			return 1;
		}
	}
	else
	{
		fprintf(stdout, "TestOptitrack takes four arguments: \n - Exposition: integer(1->480)\n - Threshold: integer (0->255)\n - Experiment number : integer(1, 2)\n - Case: string('AllCamerasCalib', 'Cam3Moved', 'Cam38Moved', etc)\n");
	}
	
    return 0;
}