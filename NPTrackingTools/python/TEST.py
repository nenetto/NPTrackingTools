import NPTrackingTools as otk
import array

tracker = otk.NPTrackingTools(r'C:\Program Files\OptiTrack\Tracking Tools\lib\NPTrackingToolsx64.dll', True)


# Second TEST

print "[TestOptitrack]: Calling TT_Initialize\n"

result = tracker.TT_Initialize()
print '[TestOptitrack]: End of TT_Initialize, result {} \n'.format(result)

resultCalibration = tracker.TT_LoadCalibration("J:/Calibration13022015.cal")
print "[TestOptitrack]: End of Load Calibration, result {} \n".format(resultCalibration)


numberOfCameras = tracker.TT_CameraCount();
cameraOrientation = [1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0]

for i in range(0, numberOfCameras):
    xCameraPosition = tracker.TT_CameraXLocation(i);
    yCameraPosition = tracker.TT_CameraYLocation(i);
    zCameraPosition = tracker.TT_CameraZLocation(i);
    print "Position of the camera {} is {}, {}, {}".format(i, xCameraPosition, yCameraPosition, zCameraPosition)

    for j in range(0,9):
        cameraOrientation[j] = tracker.TT_CameraOrientationMatrix(i, j);
    print  "Orientation of the camera {} is {}, {}, {}, {}, {}, {}, {}, {}, {}".format(i, cameraOrientation[0], cameraOrientation[1], cameraOrientation[2], cameraOrientation[3], cameraOrientation[4], cameraOrientation[5], cameraOrientation[6], cameraOrientation[7], cameraOrientation[8])


numberOfMarkers = tracker.TT_FrameMarkerCount();

for i in range(0,numberOfMarkers):
    xMarkerPosition = tracker.TT_FrameMarkerX(i);
    yMarkerPosition = tracker.TT_FrameMarkerY(i);
    zMarkerPosition = tracker.TT_FrameMarkerZ(i);
    print 'Position of the marker {} is {}, {}, {}'.format(i, xMarkerPosition, yMarkerPosition, zMarkerPosition)
