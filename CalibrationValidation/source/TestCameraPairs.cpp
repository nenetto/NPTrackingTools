// A simple program that computes the square root of a number
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iostream>
#include <locale>
#include <time.h>
//#include <algorithm>

#include "NPTrackingTools.h"
#include "TestCameraPairs.h"

namespace NoInitShutdown
{
template <class charT, charT sep>
class punct_facet : public std::numpunct<charT> {
protected:
	charT do_decimal_point() const { return sep; }
};

void sleep(unsigned int mseconds)
{
	clock_t goal = mseconds + clock();
	while (goal > clock());
}

/*void median(int sample_array[], int numSamples)
{
	std::sort(sample_array, sample_array + numSamples);

	if (numSamples % 2 != 0){// is the # of elements odd?
		int temp = ((numSamples + 1) / 2) - 1;
		fprintf(stdout, "The median is %f \n", sample_array[temp]);
	}
	else
	{
		fprintf(stdout, "The median is %f \n", (sample_array[(numSamples / 2) - 1] + sample_array[numSamples / 2]) / 2);
	}
	return;
}*/

//Get Information about cameras: number of cameras, their position and orientation

int CamerasInfoAndDeactivation()
{
	int numberOfCameras = TT_CameraCount();
	float cameraOrientation[9];
	float xCameraPosition, yCameraPosition, zCameraPosition;
	for (int i = 0; i < numberOfCameras; i++)
	{
		TT_SetCameraSettings(i, 2, 1, 255, 15);

		xCameraPosition = TT_CameraXLocation(i);
		yCameraPosition = TT_CameraYLocation(i);
		zCameraPosition = TT_CameraZLocation(i);

		fprintf(stdout, "Position of the camera %d is %f, %f, %f \n", i, xCameraPosition, yCameraPosition, zCameraPosition);
		for (int j = 0; j < 9; j++){
			cameraOrientation[j] = TT_CameraOrientationMatrix(i, j);
		}
		fprintf(stdout, "Orientation of the camera %d is %f,%f,%f,%f,%f,%f,%f,%f,%f\n", i, cameraOrientation[0], cameraOrientation[1], cameraOrientation[2], cameraOrientation[3], cameraOrientation[4], cameraOrientation[5], cameraOrientation[6], cameraOrientation[7], cameraOrientation[8]);
	}

	return numberOfCameras;
}


int CameraCorrespondeceBetweenAPIandTrackingTools(int numberOfCameras, int CameraNumber)
{
	if (numberOfCameras == 8)
	{
		switch (CameraNumber)
		{
		case 1:
			return 8;
		case 2:
			return 7;
		case 3:
			return 6;
		case 4:
			return 5;
		case 5:
			return 2;
		case 6:
			return 1;
		case 7:
			return 4;
		case 8:
			return 3;
		default:
			return 0;
			break;
		}
	}
	else if (numberOfCameras == 2)
	{
		switch (CameraNumber)
		{
		case 1:
			return 2;
		case 2:
			return 1;
		default:
			return 0;
			break;
		}
	}
	else
	{
		return CameraNumber;
	}
}

// Track tool markers. For this, we are going to do the average X,Y,Z position of all the markers, out of 2000 samples

int GetMarkerPosition2D3D(std::ostream* stream, int numberOfCameras, int Camera1, int Camera2, std::string Experiment)
{
	NPRESULT resultUpdate;

	int MarkerCount = 0;
	bool *CameraUsed = new bool[numberOfCameras];
	for (int camIndex = 0; camIndex < numberOfCameras; camIndex++)
	{
		if (camIndex == Camera1 || camIndex == Camera2)
		{
			CameraUsed[camIndex] = true;
		}
		else
		{
			CameraUsed[camIndex] = false;
		}
	}

	int TrackingToolsCamera1 = CameraCorrespondeceBetweenAPIandTrackingTools(numberOfCameras,Camera1+1);
	int TrackingToolsCamera2 = CameraCorrespondeceBetweenAPIandTrackingTools(numberOfCameras,Camera2+1);

	std::string camera_pair = "_" + std::to_string(TrackingToolsCamera1) + std::to_string(TrackingToolsCamera2);

	//fprintf(stdout, "Camera pair is %d - %d \n", TrackingToolsCamera1, TrackingToolsCamera2);


	float FrameMarkerX, FrameMarkerY, FrameMarkerZ;

	//X,Y,Z using a number of samples (numsamples)
	for (int count = 0; count < 1000; count++)
	{
		//fprintf(stdout, "Attemting to do TT_Update...\n");
		resultUpdate = TT_Update();
		//fprintf(stdout, "TT_Update done.\n");


		// Here we read the number of markers in each frame, and the 3D position of the markers (position with reference to the whole tracking system)
		//fprintf(stdout, "Attemting to do TT_FrameMarkerCount...\n");
		MarkerCount = TT_FrameMarkerCount();
		//fprintf(stdout, "TT_FrameMarkerCount done\n");

		if (MarkerCount != 0)
		{
			//fprintf(stdout, "Attemting to do TT_FrameMarkerX and TT_FrameCameraCentroid...\n");
			for (int m = 0; m < MarkerCount; m++)
			{
				FrameMarkerX = TT_FrameMarkerX(m) * 1000;
				FrameMarkerY = TT_FrameMarkerY(m) * 1000;
				FrameMarkerZ = TT_FrameMarkerZ(m) * 1000;
				//fprintf(stdout, "Position of marker %d is %f, %f, %f \n", m, FrameMarkerX, FrameMarkerY, FrameMarkerZ);
				float X1, Y1, X2, Y2 = 0;
				bool Camera1ContributesToMarkerPos = TT_FrameCameraCentroid(m, Camera1, X1, Y1);
				bool Camera2ContributesToMarkerPos = TT_FrameCameraCentroid(m, Camera2, X2, Y2);
				//fprintf(stdout, "Storing marker data in file...\n");
				*stream << count << ";" << m << ";" << FrameMarkerX << ";" << FrameMarkerY << ";" << FrameMarkerZ << ";" << TrackingToolsCamera1 << ";" << TrackingToolsCamera2 << ";" << X1 << ";" << Y1 << ";" << X2 << ";" << Y2 << ";" << Experiment << "\n";
			}
		//fprintf(stdout, "TT_FrameMarkerX and TT_FrameCameraCentroid done...\n");
		}
		else
		{
		fprintf(stdout, "No markers detected \n");
		*stream << count << ";" << "NoMarkers" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << TrackingToolsCamera1 << ";" << TrackingToolsCamera2 << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << "NA" << ";" << Experiment << "\n";
		//fprintf(stdout, "Data stored in file...\n");
		}

		sleep(5);
	}

	return resultUpdate;
}


void TestCameraPairNoInitShutdown (int Exp, int Threshold, std::string Experiment)
{

	fprintf(stdout, "[TestOptitrack]: Calling TT_Initialize\n");
	NPRESULT result = TT_Initialize();
	fprintf(stdout, "[TestOptitrack]: End of TT_Initialize, result %i \n", result);

	NPRESULT resultCalibration = TT_LoadCalibration("Calibration.cal");
	fprintf(stdout, "[TestOptitrack]: End of Load Calibration, result %i \n", resultCalibration);

		//Set camera threshold to maximum (255) so they do not detect any marker.
		//Set camera exp to minimum (0) so they do not detect any marker.
	fprintf(stdout, "[TestOptitrack]: Camera settings...\n");
	CamerasInfoAndDeactivation();
	fprintf(stdout, "[TestOptitrack]: End of camera settings...\n");

	int numberOfCameras = TT_CameraCount();


	std::string FileName = "CalValidation" + Experiment + ".csv"; ///< stores the file name*/
	//open csv file
	std::ostream* stream; ///< the output stream
	stream = new std::ofstream(FileName);
	stream->precision(10);
	//File header
	*stream << "TimeStamp" << ";MarkerIndex" << ";X_3D" << ";Y_3D" << ";Z_3D" << ";CameraUsed1" << ";CameraUsed2" << ";XCam1" << ";YCam1" << ";XCam2" << ";YCam2" << ";Experiment" << "\n";


	for (int i = 0; i < numberOfCameras - 1; i++)
	{
		//Set a 1st camera to normal threshold level and normal exp level so the camera can see the markers
		bool CameraSettingsChanged = TT_SetCameraSettings(i, 2, Exp, Threshold, 15);
		//fprintf(stdout, "Camera settings enabled.\n");


		for (int j = i + 1; j < numberOfCameras; j++)
		{
			//Set a 2nd camera to normal threshold level and normal exp level so the camera can see the markers
			CameraSettingsChanged = TT_SetCameraSettings(j, 2, Exp, Threshold, 15);
			//fprintf(stdout, "Camera settings enabled.\n");

			fprintf(stdout, "Starting tracking...\n");
			GetMarkerPosition2D3D(stream, numberOfCameras, i, j, Experiment);
			fprintf(stdout, "Tracking finished.\n");

			//Set 2nd camera to highest threshold level (255) and minimum exp level (1) so the camera can see the markers
			CameraSettingsChanged = TT_SetCameraSettings(j, 2, 1, 255, 15);
			//fprintf(stdout, "Camera settings disabled.\n", j);

		}

		//Set 1st camera to highest threshold level (255) and minimum exp level (1) so the camera cannot see the markers
		CameraSettingsChanged = TT_SetCameraSettings(i, 2, 1, 255, 15);
		//fprintf(stdout, "Camera settings disabled.\n");

	}

	fprintf(stdout, "[TestOptitrack]: Closing file... \n");
	stream->flush();
	fprintf(stdout, "[TestOptitrack]: File closed... \n");

	fprintf(stdout, "[TestOptitrack]: Shutting down system... \n");
	int end = TT_Shutdown();
	fprintf(stdout, "[TestOptitrack]: System shutdown \n");

  return;
}
}