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
#include "NPTrackingTools.h"

namespace CameraProjections
{
typedef struct{
	float *xPixel2D;
	float *yPixel2D;
}Camera2DProjections;

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

//Get Information about cameras: number of cameras, their position and orientation

int CamerasInfoAndSettings(int Exp, int Threshold)
{
	int numberOfCameras = TT_CameraCount();
	float cameraOrientation[9];
	float xCameraPosition, yCameraPosition, zCameraPosition;
	for (int i = 0; i < numberOfCameras; i++)
	{
		TT_SetCameraSettings(i, 2, Exp, Threshold, 15);

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

// Create trackable using tool definition file (.txt). TO-DO: change to xml format

int CreateTrackableByFileName(std::string nameFile)
{
	FILE* calib_file;
	// Open the file
	try
	{
		calib_file = fopen(nameFile.c_str(), "r");
	}
	catch (std::ios_base::failure &fail)
	{
		fprintf(stdout, "[TestOptitrack]: Opening the file failed!");
	}

	int TrackableID = 0; // Trackable ID, we're just validating one

	// Get the name
	std::string m_ToolName = "";
	char* aux = new char[200];
	int resultFscan = fscanf(calib_file, "%s\n", aux);
	m_ToolName.append(aux);
	delete aux;

	fprintf(stdout, "[TestOptitrack]: Toolname: %s\n", m_ToolName.c_str());

	int numMarkers = 0;
	// Get the number of of points
	resultFscan = fscanf(calib_file, "%i\n", &numMarkers);


	fprintf(stdout, "[TestOptitrack]: Number of markers: %i \n", numMarkers);

	// Read the Calibration Point locations and save them
	float *calibrationPoints = new float[3 * numMarkers];


	for (int i = 0; i < numMarkers; i++)
	{
		resultFscan = fscanf(calib_file, "%fe", &calibrationPoints[i * 3 + 0]);

		if (resultFscan < 1)
		{
			fprintf(stdout, "[TestOptitrack]: Cannot read X location for marker \n");
			return 1;
		}

		resultFscan = fscanf(calib_file, "%f", &calibrationPoints[i * 3 + 1]);
		if (resultFscan < 1)
		{
			fprintf(stdout, "[TestOptitrack]: Cannot read Y location for marker \n");
			return 1;
		}

		resultFscan = fscanf(calib_file, "%f\n", &calibrationPoints[i * 3 + 2]);
		if (resultFscan < 1)
		{
			fprintf(stdout, "[TestOptitrack]: Cannot read Z location for marker \n");
			return 1;
		}

		calibrationPoints[i * 3 + 0] = calibrationPoints[i * 3 + 0] / 1000;
		calibrationPoints[i * 3 + 1] = calibrationPoints[i * 3 + 1] / 1000;
		calibrationPoints[i * 3 + 2] = -calibrationPoints[i * 3 + 2] / 1000;// Optitrack works with Left Handed System

	}

	// Read the Pivot Point location
	float *pivotPoint = new float[3];
	resultFscan = fscanf(calib_file, "%fe ", &pivotPoint[0]);
	if (resultFscan < 1)
	{
		fprintf(stdout, "[TestOptitrack]: Cannot read X location for Pivot Point \n");
		return 1;
	}

	resultFscan = fscanf(calib_file, "%fe ", &pivotPoint[1]);
	if (resultFscan < 1)
	{
		fprintf(stdout, "[TestOptitrack]: Cannot read Y location for Pivot Point \n");
		return 1;
	}

	resultFscan = fscanf(calib_file, "%fe\n", &pivotPoint[2]);
	if (resultFscan < 1)
	{
		fprintf(stdout, "[TestOptitrack]: Cannot read Z location for Pivot Point \n");
		return 1;
	}

	fprintf(stdout, "[TestOptitrack] \tPivotPoint \n");
	fprintf(stdout, "[TestOptitrack]: PivotPoint: %f, %f, %f \n", pivotPoint[0], pivotPoint[1], pivotPoint[2]);

	// mm -> m
	pivotPoint[0] = pivotPoint[0] / 1000;
	pivotPoint[1] = pivotPoint[1] / 1000;
	pivotPoint[2] = -pivotPoint[2] / 1000;

	// Create the Trackable


	NPRESULT resultCreateTrackable = TT_CreateTrackable(m_ToolName.c_str(), TrackableID, numMarkers, calibrationPoints);
	if (NPRESULT_SUCCESS == resultCreateTrackable)
	{
		fprintf(stdout, "[TestOptitrack]: Trackable Created Successfully \n");


		// Get marker positions according to configuration file (that is, the calibrated tool)
		float *CalibrationMarkerX = new float[numMarkers];
		float *CalibrationMarkerY = new float[numMarkers];
		float *CalibrationMarkerZ = new float[numMarkers];

		for (int i = 0; i < numMarkers; i++)
		{
			CalibrationMarkerX[i] = 0;
			CalibrationMarkerY[i] = 0;
			CalibrationMarkerZ[i] = 0;
			TT_TrackableMarker(TrackableID, i, &CalibrationMarkerX[i], &CalibrationMarkerY[i], &CalibrationMarkerZ[i]);
			CalibrationMarkerX[i] = CalibrationMarkerX[i] * 1000;
			CalibrationMarkerY[i] = CalibrationMarkerY[i] * 1000;
			CalibrationMarkerZ[i] = CalibrationMarkerZ[i] * 1000;
			//fprintf(stdout, "Calibration position of the marker %i is %f, %f, %f \n", i, CalibrationMarkerX[i], CalibrationMarkerY[i], CalibrationMarkerZ[i]);
		}

		//Calculate distance values between calibrated markers
		for (int i = 0; i < numMarkers; i++)
		{
			for (int j = 0; j < numMarkers; j++)
			{
				if (i < j)
				{
					float CaldX = CalibrationMarkerX[i] - CalibrationMarkerX[j];
					float CaldY = CalibrationMarkerY[i] - CalibrationMarkerY[j];
					float CaldZ = CalibrationMarkerZ[i] - CalibrationMarkerZ[j];

					float CalDistance = sqrt((CaldX*CaldX) + (CaldY*CaldY) + (CaldZ*CaldZ));
					//fprintf(stdout, "Distance between Calibrated markers %i and %i is %f \n", i, j, CalDistance);
				}
			}
		}

		return NPRESULT_SUCCESS;

	}
	else
	{
		fprintf(stdout, "[TestOptitrack]: Trackable Created Unsuccessfully \n");
		return NPRESULT_FAILED;
	}

}


// Track tool markers. For this, we are going to do the average X,Y,Z position of all the markers, out of 2000 samples

int GetMarkerPosition2D3D(std::string FileName, std::string Date, std::string Case, std::string Quality, std::string Phantom_Position, std::string Rep)
{
	NPRESULT resultUpdate;

	//open csv file

	std::ostream* stream; ///< the output stream
	stream = new std::ofstream(FileName);
	stream->precision(10);

	//stream->imbue(std::locale(std::cout.getloc(), new punct_facet<char, ','>));

	//csv header

	*stream << "TimeStamp" << ";Rep" << ";MarkerIndex" << ";MarkerIsTracked" << ";X_3D" << ";Y_3D" << ";Z_3D" << ";Camera" << ";X_2D_Real" << ";Y_2D_Real" << ";X_2D_Cal" << ";Y_2D_Cal" << ";ProjectionContributesTo3DPosition" << "\n";

	float *PointCloudMarkerX = new float[TT_TrackableMarkerCount(0)];
	float *PointCloudMarkerY = new float[TT_TrackableMarkerCount(0)];
	float *PointCloudMarkerZ = new float[TT_TrackableMarkerCount(0)];

	Camera2DProjections *RealCameraProjection = new Camera2DProjections[TT_CameraCount()];

	Camera2DProjections *CalibratedCameraProjection = new Camera2DProjections[TT_CameraCount()];

	for (int camCount = 0; camCount < TT_CameraCount(); camCount++)
	{
		RealCameraProjection[camCount].xPixel2D = new float[TT_TrackableMarkerCount(0)];
		RealCameraProjection[camCount].yPixel2D = new float[TT_TrackableMarkerCount(0)];
		CalibratedCameraProjection[camCount].xPixel2D = new float[TT_TrackableMarkerCount(0)];
		CalibratedCameraProjection[camCount].yPixel2D = new float[TT_TrackableMarkerCount(0)];
	}

	int MarkerCount = 0;
	int numSamples = 0;


	//X,Y,Z, Mx, My, Mz using a number of samples (numsamples)
	for (int count = 0; count < 10000; count++)
	{
		resultUpdate = TT_Update();

		if (!TT_IsTrackableTracked(0))
		{
			fprintf(stdout, "Trackable not tracked!!!!!");
		}

		//Only read frame markers if trackable is tracked
		//if (true)
		if (TT_IsTrackableTracked(0))
		{
			for (int MarkerIndex = 0; MarkerIndex < TT_TrackableMarkerCount(0); MarkerIndex++)
			{
				bool Tracked;
				float mX, mY, mZ;
				TT_TrackablePointCloudMarker(0, MarkerIndex, Tracked, mX, mY, mZ);
				PointCloudMarkerX[MarkerIndex] = mX * 1000;
				PointCloudMarkerY[MarkerIndex] = mY * 1000;
				PointCloudMarkerZ[MarkerIndex] = mZ * 1000;

				for (int camCount = 0; camCount < TT_CameraCount(); camCount++)
					{
						float x2D, y2D = 0;
						bool ProjectionContributesTo3DPosition = false;
						//TT_CameraMarker(camCount, MarkerIndex, x2D, y2D);
						ProjectionContributesTo3DPosition = TT_CameraMarkerPredistorted(camCount, MarkerIndex, x2D, y2D);
						RealCameraProjection[camCount].xPixel2D[MarkerIndex] = x2D;
						RealCameraProjection[camCount].yPixel2D[MarkerIndex] = y2D;

						float x_Cal, y_Cal;
						TT_CameraBackproject(camCount, PointCloudMarkerX[MarkerIndex] / 1000, PointCloudMarkerY[MarkerIndex] / 1000, PointCloudMarkerZ[MarkerIndex] / 1000, x_Cal, y_Cal);
						CalibratedCameraProjection[camCount].xPixel2D[MarkerIndex] = x_Cal;
						CalibratedCameraProjection[camCount].yPixel2D[MarkerIndex] = y_Cal;
						*stream << numSamples + 1 << ";" << Rep << ";" << MarkerIndex << ";" << Tracked << ";" << PointCloudMarkerX[MarkerIndex] << ";" << PointCloudMarkerY[MarkerIndex] << ";" << PointCloudMarkerZ[MarkerIndex] << ";" << camCount << ";" << RealCameraProjection[camCount].xPixel2D[MarkerIndex] << ";" << RealCameraProjection[camCount].yPixel2D[MarkerIndex] << ";" << CalibratedCameraProjection[camCount].xPixel2D[MarkerIndex] << ";" << CalibratedCameraProjection[camCount].yPixel2D[MarkerIndex] << ";" << ProjectionContributesTo3DPosition << "\n";
					}
			}

			//numSamples grows when TT_TrackableIsTracked
			numSamples++;
		}

		sleep(10);
		//std::this_thread::sleep_for(std::chrono::milliseconds(50));

	}

	stream->flush();

	return resultUpdate;
}


void Test2DCameraProjectionsModif(int Exp, int Threshold, std::string ExpType, std::string Repetition)
{

//define variables for logging
	std::string Date = "13032015";
	std::string Case = ExpType;
	std::string Quality = "VeryHigh";
	std::string Phantom_Position = "1";
	std::string Rep = Repetition;

  fprintf(stdout, "[TestOptitrack]: Getting arguments\n");

  /*std::string Date = argv[1];
  std::string Quality = argv[2];
  std::string Case = argv[3];
  std::string Rep = argv[4];
  std::string Phantom_Position = "1";*/

  std::string FileName = "CalValidation_" + Date +"_" + Quality + "_" + Case + "_" + Rep + ".csv"; ///< stores the file name and path

  std::cout << "Filename: " << FileName;

  fprintf(stdout, "[TestOptitrack]: Calling TT_Initialize\n");

  NPRESULT result = TT_Initialize();

  fprintf(stdout, "[TestOptitrack]: End of TT_Initialize, result %i \n", result);

  NPRESULT resultCalibration = TT_LoadCalibration("Calibration.cal");
  fprintf(stdout, "[TestOptitrack]: End of Load Calibration, result %i \n", resultCalibration);


  int NumOfCameras = CamerasInfoAndSettings(Exp, Threshold);

  TT_ClearTrackableList();

  fprintf(stdout, "[TestOptitrack]: End of Clear trackable list \n");

  CreateTrackableByFileName("PunteroPolaris.txt");

  fprintf(stdout, "[TestOptitrack]: End of creating trackable \n");

  GetMarkerPosition2D3D(FileName, Date, Case, Quality, Phantom_Position, Rep);

  fprintf(stdout, "[TestOptitrack]: End of tracking \n");


  TT_RemoveTrackable(0);


  int end = TT_Shutdown();

  return;
}
}