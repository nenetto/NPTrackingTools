// A simple program that computes the square root of a number
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "NPTrackingTools.h"

int main (int argc, char *argv[])
{
  fprintf(stdout, "[TestOptitrack]: Calling TT_Initialize\n");

  NPRESULT result = TT_Initialize();
  //int result = 0;


  fprintf(stdout, "[TestOptitrack]: End of TT_Initialize, result %i \n",result);

  NPRESULT resultCalibration = TT_LoadCalibration("J:/Calibration13022015.cal");
  fprintf(stdout, "[TestOptitrack]: End of Load Calibration, result %i \n", resultCalibration);

  int numberOfCameras = TT_CameraCount();
  float cameraOrientation[9];
  float xCameraPosition, yCameraPosition, zCameraPosition;
  for (int i = 0; i < numberOfCameras; i++){
      xCameraPosition = TT_CameraXLocation(i);
      yCameraPosition = TT_CameraYLocation(i);
      zCameraPosition = TT_CameraZLocation(i);
      
      fprintf(stdout, "Position of the camera %d is %f, %f, %f \n", i, xCameraPosition, yCameraPosition, zCameraPosition);
      for (int j = 0; j < 9; j++){
          cameraOrientation[j] = TT_CameraOrientationMatrix(i, j);
      }
      fprintf(stdout, "Orientation of the camera %d is %f,%f,%f,%f,%f,%f,%f,%f,%f\n", i, cameraOrientation[0], cameraOrientation[1], cameraOrientation[2], cameraOrientation[3], cameraOrientation[4], cameraOrientation[5], cameraOrientation[6], cameraOrientation[7], cameraOrientation[8]);
  }
  
  int numberOfMarkers = TT_FrameMarkerCount();
  float xMarkerPosition, yMarkerPosition, zMarkerPosition;
  for (int i = 0; i < numberOfMarkers; i++){
      xMarkerPosition = TT_FrameMarkerX(i);
      yMarkerPosition = TT_FrameMarkerY(i);
      zMarkerPosition = TT_FrameMarkerZ(i);

      fprintf(stdout, "Position of the marker %d is %f, %f, %f \n", i, xMarkerPosition, yMarkerPosition, zMarkerPosition);
  }

  system("PAUSE");
  return 0;
}