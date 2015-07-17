
//====================================================================================-----
//== Tracking Tools DLL Library
//== Copyright NaturalPoint, Inc.
//==
//== The Rigid Body DLL Library is designed to be a simple yet full featured interface to
//== the Rigid Body Library.
//==
//====================================================================================-----

#ifndef NPTRACKINGTOOLS_H
#define NPTRACKINGTOOLS_H

//== Includes ========================================================================-----

#include "trackablesettings.h"

//== DLL EXPORT/IMPORT PREPROCESSOR DEFINES ==========================================-----

#ifdef NPTRACKINGTOOLS_EXPORTS
#define TTAPI __declspec(dllexport)
#else
#ifndef STATIC_TT_LINK
#define TTAPI __declspec(dllimport)
#else
#define TTAPI
#endif
#endif

namespace CameraLibrary
{
    class Camera;
    class CameraManager;
    class cCameraModule;
}

namespace Core
{
    struct DistortionModel;
}

struct Marker;


//== RIGID BODY STARTUP / SHUTDOWN ==================================================-----

/** \fn TT_Initialize()
 *  \brief This function attempts to initialize the Tracking Tools tracking API.
 *
 *  It should be called before attempting to use other compontents of the API.  It returns information about whether or not it succeeded.
 *
 *  @return 0  if succeeded, 10 if a valid license is not found, and 11 if it was unable to initialize.
 *
 */
#define NPRESULT int                  //== NPRESULT Defines Call Success/Failure ====-----

//== RIGID BODY STARTUP / SHUTDOWN ==================================================-----
TTAPI NPRESULT TT_Initialize(bool NPTracking_FAIL_INVALIDLICENSE = false, bool NPTracking_FAIL_UNABLETOINITIALIZE = false);      //== Initialize Library ======================----- -

TTAPI NPRESULT TT_Shutdown(bool NPTracking_FAIL_UNABLETOSHUTDOWN = false);        //== Shutdown Library ========================------

TTAPI NPRESULT TT_FinalCleanup(bool NPTracking_FAIL_UNABLETOCLEANUP = false);    //== This shuts down device driver, call =====------
//== this before exiting your application. ===-----


//== RIGID BODY INTERFACE ===========================================================-----

TTAPI NPRESULT TT_LoadCalibration(const char *filename, bool NPTracking_FAIL_INVALIDFILE = false, bool NPTracking_FAIL_FAILED = false); //== Load Calibration =====-----
TTAPI NPRESULT TT_LoadTrackables(const char *filename, bool NPTracking_FAIL_INVALIDFILE = false, bool NPTracking_FAIL_FAILED = false); //== Load Trackables ======-----
TTAPI NPRESULT TT_SaveTrackables(const char *filename, bool NPTracking_FAIL_FAILED = false); //== Save Trackables ======-----
TTAPI NPRESULT TT_AddTrackables(const char *filename, bool NPTracking_FAIL_INVALIDFILE = false, bool NPTracking_FAIL_FAILED = false); //== Add  Trackables ======-----
TTAPI NPRESULT TT_Update(bool NPTracking_FAIL_INVALIDLICENSE = false, bool NPTracking_FAIL_NOFRAMEAVAILABLE = false);                          //== Process incoming camera data --
TTAPI NPRESULT TT_UpdateSingleFrame(bool NPTracking_FAIL_INVALIDLICENSE = false, bool NPTracking_FAIL_NOFRAMEAVAILABLE = false);               //== Process incoming camera data --
TTAPI NPRESULT TT_LoadProject(const char *filename, bool NPTracking_FAIL_INVALIDFILE = false, bool NPTracking_FAIL_FAILED = false); //== Load Project File ==========---
TTAPI NPRESULT TT_SaveProject(const char *filename, bool NPTracking_FAIL_FAILED = false); //== Save Project File ==========---



//== FRAME ==========================================================================------

TTAPI int      TT_FrameMarkerCount(int variableTestNumberOfMarkers = 0);               //== Returns Frame Markers Count ---
TTAPI float    TT_FrameMarkerX(int index, bool NPTracking_FAIL_NOFRAMEAVAILABLE = false, float variableTestMarkerPositionX = 0);          //== Returns X Coord of Marker -----
TTAPI float    TT_FrameMarkerY(int index, bool NPTracking_FAIL_NOFRAMEAVAILABLE = false, float variableTestMarkerPositionY = 0);          //== Returns Y Coord of Marker -----
TTAPI float    TT_FrameMarkerZ(int index, bool NPTracking_FAIL_NOFRAMEAVAILABLE = false, float variableTestMarkerPositionZ = 0);          //== Returns Z Coord of Marker -----
TTAPI int      TT_FrameMarkerLabel(int index, int frameMarkerLabel = 0);      //== Returns Label of Marker -------
TTAPI double   TT_FrameTimeStamp(double frameTimeSpamp = 0.0);                 //== Time Stamp of Frame (seconds) -

//== TT_FrameCameraCentroid returns true if the camera is contributing
//== to this 3D marker.  It also returns the location of the 2D centroid
//== that is reconstructing to this 3D marker.

TTAPI bool     TT_FrameCameraCentroid(int index, int CameraIndex, float &X, float &Y, float Xdef = 10, float Ydef = 20, bool cameraContributionTo3DMarker = true);


//== TRACKABLES CONTROL =============================================================------

TTAPI bool     TT_IsTrackableTracked(int index, bool NPTracking_FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME = false); //== Is trackable currently tracked ---
TTAPI void     TT_TrackableLocation(int RigidIndex,       //== Trackable Index ======---
	float *x, float *y, float *z,                  //== Position ==---
	float *qx, float *qy, float *qz, float *qw,    //== Orientation --
	float *yaw, float *pitch, float *roll,
	bool NPTracking_FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME = false,
	float xdef = 0, float ydef = 0, float zdef = 0,                      //== Position ==---
	float qxdef = 0, float qydef = 0, float qzdef = 0, float qwdef = 0,  //== Orientation --
	float yawdef = 0, float pitchdef = 0, float rolldef = 0);        //== Orientation --

TTAPI void     TT_ClearTrackableList();             //== Clear all trackables   =====---
TTAPI NPRESULT TT_RemoveTrackable(int Index, bool NPTracking_FAIL_FAILED = false);       //== Remove single trackable ====---
TTAPI int      TT_TrackableCount(int numberOfTrackables = 0);                 //== Returns number of trackables  -

TTAPI int      TT_TrackableID(int index, int trackableID = 0);           //== Get Trackables ID ==========---
TTAPI void     TT_SetTrackableID(int index, int ID); //== Set Trackables ID ==========---
TTAPI const char* TT_TrackableName(int index);      //== Returns Trackable Name =====---

TTAPI void     TT_SetTrackableEnabled(int index, bool enabled);    //== Set Tracking   ====---
TTAPI bool     TT_TrackableEnabled(int index, bool stateOfTrackable = true);                     //== Get Tracking   ====---

TTAPI NPRESULT TT_TrackableTranslatePivot(int index, float x, float y, float z);

TTAPI int      TT_TrackableMarkerCount(int index, int numberOfTrackableMarkers = 0);             //== Get marker count   ====---

TTAPI void     TT_TrackableMarker(int RigidIndex,              //== Get Trackable mrkr ====---
	int MarkerIndex, float *x, float *y, float *z, float x_trackMarker_def = 0, float y_trackMarker_def = 0, float z_trackMarker_def = 0);

TTAPI void     TT_TrackablePointCloudMarker(int RigidIndex,    //== Get corresponding point cloud marker ======---
	int MarkerIndex, bool &Tracked,          //== If tracked is false, there is no
	float &x, float &y, float &z, bool TrackedDef=true, float x_pointCloudMarker_def = 0, float y_pointCloudMarker_def = 0, float z_pointCloudMarker_def = 0);           //== corresponding point cloud marker.


//== TT_CreateTrackable.  This creates a trackable based on the marker
//== count and marker list provided.  The MarkerList is a expected to
//== contain of list of marker coordinates in the order: x1,y1,z1,x2,
//== y2,z2,etc...xN,yN,zN.

TTAPI NPRESULT TT_CreateTrackable(const char* Name, int ID, int MarkerCount, float *MarkerList);
/*
NPRESULT TT_TrackableSettings(int Index, cTrackableSettings &Settings);  //== Get Trackable Settings =---
NPRESULT TT_SetTrackableSettings(int Index, cTrackableSettings &Settings);  //== Set Trackable Settings =---

*/

//== POINT CLOUD INTERFACE ==========================================================-----

TTAPI int      TT_CameraCount(int numberOfCameras = 0);                    //== Returns Camera Count =====-----
TTAPI float    TT_CameraXLocation(int index, float variableCameraPositionX = 0);       //== Returns Camera's X Coord =-----
TTAPI float    TT_CameraYLocation(int index, float variableCameraPositionY = 0);       //== Returns Camera's Y Coord =-----
TTAPI float    TT_CameraZLocation(int index, float variableCameraPositionZ = 0);       //== Returns Camera's Z Coord =-----
TTAPI float    TT_CameraOrientationMatrix(int camera, int index, float orientationMatrixCameraIndex = 1); //== Orientation -----

//= = Set camera settings.This function allows you to set the camera's
//== video mode, exposure, threshold, and illumination settings.

//== VideoType:
//==     0 = Segment Mode
//==     1 = Grayscale Mode
//==     2 = Object Mode
//==     4 = Precision Mode
//==     6 = MJPEG Mode     (V100R2 only)

//== Exposure: Valid values are:  1-480
//== Threshold: Valid values are: 0-255
//== Intensity: Valid values are: 0-15  (This should be set to 15 for all most all
//==                                     situations)
TTAPI bool     TT_SetCameraSettings(int CameraIndex, int VideoType, int Exposure, int Threshold, int Intensity, bool succesfulUpdateOfCameraSettings = true);

//== Fetch predistorted marker location.  This is basically where the
//== camera would see the marker if there was no lens distortion.
//== For most of our cameras/lenses, this location is only a few pixels
//== from the distorted (TT_CameraMarker) position.
TTAPI bool     TT_CameraMarkerPredistorted(int CameraIndex, int MarkerIndex, float &x, float &y, bool succesfulCameraMarkerPredistorted=true, float x_cameraMarker_def=0, float y_cameraMarker_def=0);

TTAPI   int      TT_CameraMarkerCount(int CameraIndex, int numberOfCameraMarkers=0); //== Camera's 2D Marker Count =---

//= = CameraMarker fetches the 2D centroid location of the marker as seen by the camera.
TTAPI bool     TT_CameraMarker(int CameraIndex, int MarkerIndex, float &x, float &y, bool succesfulCameraMarker = true,
	float x_cameraMarker_def = 0, float y_cameraMarker_def = 0);

//== Backproject from 3D space to 2D space.  If you give this function a 3D
//== location and select a camera, it will return where the point would land
//== on the imager of that camera in to 2D space.  This basically locates
//== where in the cameras FOV a 3D point would be located.
TTAPI void     TT_CameraBackproject(int CameraIndex, float X, float Y, float Z, float &CameraX, float &CameraY, float CameraX_def = 0, float CameraY_def = 0);



//== RESULT PROCESSING ========================================================================================-----

TTAPI const char *TT_GetResultString(NPRESULT result); //== Return Plain Text Message =======================------

#define NPRESULT_SUCCESS                0             //== Successful Result ================================-------
#define NPRESULT_FILENOTFOUND           1             //== File Not Found ===================================-------
#define NPRESULT_LOADFAILED             2             //== Load Failed ======================================-------
#define NPRESULT_FAILED                 3             //== Failed ===========================================-------
#define NPRESULT_INVALIDFILE            8             //== Invalid File =====================================-------
#define NPRESULT_INVALIDCALFILE         9             //== Invalid Calibration File =========================-------
#define NPRESULT_UNABLETOINITIALIZE     10            //== Unable To Initialize =============================-------
#define NPRESULT_INVALIDLICENSE         11            //== Invalid License ==================================-------
#define NPRESULT_NOFRAMEAVAILABLE       14            //== No Frames Available ==============================-------



//===============================================================================================================---

#endif