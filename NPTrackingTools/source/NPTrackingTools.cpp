#include "Config.h"
#include "NPTrackingTools.h"

#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>

using namespace std;

/**
 * \defgroup NPTrackingToolsFake
 * @{
 */


//== NPRIGIDBODY PUBLIC INTERFACE ===================================================-----
//== NPRESULT Defines Call Success/Failure ====-----

//== RIGID BODY STARTUP / SHUTDOWN ==================================================-----

NPRESULT TT_Initialize(bool NPTracking_FAIL_INVALIDLICENSE, bool NPTracking_FAIL_UNABLETOINITIALIZE) {

	if (NPTracking_FAIL_INVALIDLICENSE) {
		return NPRESULT_INVALIDLICENSE; //A valid Tracking tools license was not found.
	}
	else if (NPTracking_FAIL_UNABLETOINITIALIZE){
		return NPRESULT_UNABLETOINITIALIZE;
	}
	else{
		return NPRESULT_SUCCESS;
	}
}

NPRESULT TT_Shutdown(bool NPTracking_FAIL_UNABLETOSHUTDOWN)  {
	if (NPTracking_FAIL_UNABLETOSHUTDOWN){
		return NPRESULT_FAILED;
	}
	else{
		return NPRESULT_SUCCESS;
	}
}

NPRESULT TT_FinalCleanup(bool NPTracking_FAIL_UNABLETOCLEANUP){
	if (NPTracking_FAIL_UNABLETOCLEANUP){
		return NPRESULT_FAILED;
	}
	else {
		return NPRESULT_SUCCESS;
	}
}
//== this before exiting your application. ===----- 


NPRESULT TT_LoadCalibration(const char *filename, bool NPTracking_FAIL_INVALIDFILE, bool NPTracking_FAIL_FAILED){
	if (NPTracking_FAIL_INVALIDFILE) {
		return NPRESULT_INVALIDCALFILE; //Invalid calibration file.
	}
	else if (NPTracking_FAIL_FAILED){
		return NPRESULT_LOADFAILED; //The file or path specified is no valid.
	}
	else {
		return NPRESULT_SUCCESS; //Method succeeded.
	}

}

NPRESULT TT_LoadTrackables(const char *filename, bool NPTracking_FAIL_INVALIDFILE, bool NPTracking_FAIL_FAILED){ //== Load Trackables ======-----
	if (NPTracking_FAIL_INVALIDFILE) {
		return NPRESULT_INVALIDFILE; //Invalid file.
	}
	else if (NPTracking_FAIL_FAILED){

		return NPRESULT_LOADFAILED; //The file or path specified is no valid.
	}
	else {
		return NPRESULT_SUCCESS; //Method succeeded.
	}

}
NPRESULT TT_SaveTrackables(const char *filename, bool NPTracking_FAIL_FAILED){ //== Save Trackables ======---- 
	if (NPTracking_FAIL_FAILED){
		return NPRESULT_FAILED;
	}
	else {
		return (NPRESULT_SUCCESS);
	}

}
NPRESULT TT_AddTrackables(const char *filename, bool NPTracking_FAIL_INVALIDFILE, bool NPTracking_FAIL_FAILED){ //== Add  Trackables ======-----
	if (NPTracking_FAIL_INVALIDFILE) {
		return NPRESULT_INVALIDFILE; //Invalid file.
	}
	else if (NPTracking_FAIL_FAILED){

		return NPRESULT_LOADFAILED; //The file or path specified is no valid.
	}
	else {
		return NPRESULT_SUCCESS; //Method succeeded.
	}

}

NPRESULT TT_Update(bool NPTracking_FAIL_INVALIDLICENSE, bool NPTracking_FAIL_NOFRAMEAVAILABLE){                     //== Process incoming camera data --
	if (NPTracking_FAIL_INVALIDLICENSE){
		return NPRESULT_INVALIDLICENSE; //A valid Rigid Body license was not found.
	}
	else if (NPTracking_FAIL_NOFRAMEAVAILABLE){
		return NPRESULT_NOFRAMEAVAILABLE; // No tracking data was found.
	}
	else {
		return NPRESULT_SUCCESS; // Method succeeded.
	}
}

NPRESULT TT_UpdateSingleFrame(bool NPTracking_FAIL_INVALIDLICENSE, bool NPTracking_FAIL_NOFRAMEAVAILABLE){               //== Process incoming camera data --
	if (NPTracking_FAIL_INVALIDLICENSE){
		return NPRESULT_INVALIDLICENSE; //A valid Rigid Body license was not found.
	}
	else if (NPTracking_FAIL_NOFRAMEAVAILABLE){
		return NPRESULT_NOFRAMEAVAILABLE; // No tracking data was found.
	}
	else {
		return NPRESULT_SUCCESS; // Method succeeded.
	}
}

NPRESULT TT_LoadProject(const char *filename, bool NPTracking_FAIL_INVALIDFILE, bool NPTracking_FAIL_FAILED){ //== Load Project File ==========---
	if (NPTracking_FAIL_INVALIDFILE) {
		return NPRESULT_INVALIDFILE; //Invalid file.
	}
	else if (NPTracking_FAIL_FAILED){

		return NPRESULT_LOADFAILED; //The file or path specified is no valid.
	}
	else {
		return NPRESULT_SUCCESS; //Method succeeded.
	}
}

NPRESULT TT_SaveProject(const char *filename, bool NPTracking_FAIL_FAILED){ //== Save Project File ==========---
	if (NPTracking_FAIL_FAILED){
		return NPRESULT_FAILED;
	}
	else {
		return (NPRESULT_SUCCESS);
	}
}

//== FRAME ==========================================================================------

int      TT_FrameMarkerCount(int variableTestNumberOfMarkers){              //== Returns Frame Markers Count ---
	return (variableTestNumberOfMarkers);
}

float    TT_FrameMarkerX(int index, bool NPTracking_FAIL_NOFRAMEAVAILABLE, float variableTestMarkerPositionX){          //== Returns X Coord of Marker -----
	if (NPTracking_FAIL_NOFRAMEAVAILABLE){
		return (0); //No frame data was found
	}
	else{
		return (variableTestMarkerPositionX);
	}

}

float    TT_FrameMarkerY(int index, bool NPTracking_FAIL_NOFRAMEAVAILABLE, float variableTestMarkerPositionY){          //== Returns Y Coord of Marker -----
	if (NPTracking_FAIL_NOFRAMEAVAILABLE){
		return (0); //No frame data was found
	}
	else{
		return (variableTestMarkerPositionY);
	}
}

float    TT_FrameMarkerZ(int index, bool NPTracking_FAIL_NOFRAMEAVAILABLE, float variableTestMarkerPositionZ){          //== Returns Z Coord of Marker -----
	if (NPTracking_FAIL_NOFRAMEAVAILABLE){
		return (0); //No frame data was found
	}
	else{
		return (variableTestMarkerPositionZ);
	}
}

int      TT_FrameMarkerLabel(int index, int frameMarkerLabel){      //== Returns Label of Marker -------
	return (frameMarkerLabel);
}

double   TT_FrameTimeStamp(double frameTimeSpamp){                 //== Time Stamp of Frame (seconds) 
	return (frameTimeSpamp);
}

//== TT_FrameCameraCentroid returns true if the camera is contributing
//== to this 3D marker.  It also returns the location of the 2D centroid
//== that is reconstructing to this 3D marker.
bool     TT_FrameCameraCentroid(int index, int CameraIndex, float &X, float &Y, float Xdef, float Ydef, bool cameraContributionTo3DMarker){

	X = Xdef;
	Y = Ydef;

	return (cameraContributionTo3DMarker);

}



//== TRACKABLES CONTROL =============================================================------

bool     TT_IsTrackableTracked(int index, bool NPTracking_FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME){ //== Is trackable currently tracked ---
	if (NPTracking_FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME){
		return (false);
	}
	else {
		return (true);
	}
}

void     TT_TrackableLocation(int RigidIndex,      //== Trackable Index ======---
	float *x, float *y, float *z,                  //== Position ==---
	float *qx, float *qy, float *qz, float *qw,    //== Orientation -- 
	float *yaw, float *pitch, float *roll,         //== Orientation --
	bool NPTracking_FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME,
	float xdef, float ydef, float zdef,                      //== Position ==---
	float qxdef, float qydef, float qzdef, float qwdef,  //== Orientation -- 
	float yawdef, float pitchdef, float rolldef)             //== Orientation --
{

	if (NPTracking_FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME){
		cout << "The rigid body was not tracked in the current frame" << endl;
	}
	else {
		*x = xdef;
		*y = ydef;
		*z = zdef;
		*qx = qxdef;
		*qy = qydef;
		*qz = qzdef;
		*qw = qwdef;
		*yaw = yawdef;
		*pitch = pitchdef;
		*roll = rolldef;
	}

}

void     TT_ClearTrackableList(){             //== Clear all trackables   =====---
	cout << "  List of rigid body definitions cleared!\n";
}

NPRESULT TT_RemoveTrackable(int Index, bool NPTracking_FAIL_FAILED){       //== Remove single trackable ====---
	if (NPTracking_FAIL_FAILED){
		return (NPRESULT_FAILED);
	}
	else{
		return (NPRESULT_SUCCESS);
	}
}

int      TT_TrackableCount(int numberOfTrackables){                 //== Returns number of trackables  -
	return (numberOfTrackables);
}

int      TT_TrackableID(int index, int trackableID){           //== Get Trackables ID ==========---
	return (trackableID);
}

void     TT_SetTrackableID(int index, int ID){ //== Set Trackables ID ==========---
	cout << "  Trackable ID set!\n";
}

const char* TT_TrackableName(int index){      //== Returns Trackable Name =====---
	string trackableName = "Name of the trackable";
	const char* nameAddress = static_cast<const char*>(trackableName.c_str());
	return (nameAddress);
}

void     TT_SetTrackableEnabled(int index, bool enabled){    //== Set Tracking   ====---
	cout << "  Trackable Set Enabled!\n";
}

bool     TT_TrackableEnabled(int index, bool stateOfTrackable){                     //== Get Tracking   ====---
	return (stateOfTrackable);
}

NPRESULT TT_TrackableTranslatePivot(int index, float x, float y, float z){
	cout << "  Pivot translated! " << x << y << z << "\n";
	return (NPRESULT_SUCCESS);
}

int      TT_TrackableMarkerCount(int index, int numberOfTrackableMarkers){             //== Get marker count   ====---
	return (numberOfTrackableMarkers);
}
void     TT_TrackableMarker(int RigidIndex,              //== Get Trackable mrkr ====---
	int MarkerIndex, float *x, float *y, float *z,
	float x_trackMarker_def, float y_trackMarker_def, float z_trackMarker_def){

	*x = x_trackMarker_def;
	*y = y_trackMarker_def;
	*z = z_trackMarker_def;
}

void     TT_TrackablePointCloudMarker(int RigidIndex,    //== Get corresponding point cloud marker ======---
	int MarkerIndex, bool &Tracked,          //== If tracked is false, there is no
	float &x, float &y, float &z, bool TrackedDef,
	float x_pointCloudMarker_def, float y_pointCloudMarker_def, float z_pointCloudMarker_def){           //== corresponding point cloud marker.

	Tracked = TrackedDef;
	if (Tracked){
		x = x_pointCloudMarker_def;
		y = y_pointCloudMarker_def;
		z = z_pointCloudMarker_def;
	}

}

//== TT_CreateTrackable.  This creates a trackable based on the marker
//== count and marker list provided.  The MarkerList is a expected to
//== contain of list of marker coordinates in the order: x1,y1,z1,x2,
//== y2,z2,etc...xN,yN,zN.

NPRESULT TT_CreateTrackable(const char* Name, int ID, int MarkerCount, float *MarkerList){
	cout << "  Trackable created successfully! \n";
	return (NPRESULT_SUCCESS);
}

//== POINT CLOUD INTERFACE ==========================================================-----

int      TT_CameraCount(int numberOfCameras){                    //== Returns Camera Count =====-----
	return numberOfCameras;
}

float    TT_CameraXLocation(int index, float variableCameraPositionX){       //== Returns Camera's X Coord =-----
	return variableCameraPositionX;
}

float    TT_CameraYLocation(int index, float variableCameraPositionY){       //== Returns Camera's Y Coord =-----
	return variableCameraPositionY;
}

float    TT_CameraZLocation(int index, float variableCameraPositionZ){       //== Returns Camera's Z Coord =-----
	return variableCameraPositionZ;
}

float    TT_CameraOrientationMatrix(int camera, int index, float orientationMatrixCameraIndex){ //== Orientation -----
	return orientationMatrixCameraIndex;
}

//= = Set camera settings.This function allows you to set the camera's video mode, exposure, threshold, and illumination settings.

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
bool     TT_SetCameraSettings(int CameraIndex, int VideoType, int Exposure, int Threshold, int Intensity, bool succesfulUpdateOfCameraSettings){
	return (succesfulUpdateOfCameraSettings);
}


//== Fetch predistorted marker location.  This is basically where the
//== camera would see the marker if there was no lens distortion.
//== For most of our cameras/lenses, this location is only a few pixels
//== from the distorted (TT_CameraMarker) position.
bool     TT_CameraMarkerPredistorted(int CameraIndex, int MarkerIndex, float &x, float &y, bool succesfulCameraMarkerPredistorted, float x_cameraMarker_def, float y_cameraMarker_def){
	x=x_cameraMarker_def;
	y=y_cameraMarker_def;
	return (succesfulCameraMarkerPredistorted);
}



//= = CameraMarker fetches the 2D centroid location of the marker as seen by the camera.
bool     TT_CameraMarker(int CameraIndex, int MarkerIndex, float &x, float &y, bool succesfulCameraMarker,
	float x_cameraMarker_def, float y_cameraMarker_def){
	x = x_cameraMarker_def;
	y = y_cameraMarker_def;
	return succesfulCameraMarker;
}

//== Backproject from 3D space to 2D space.  If you give this function a 3D
//== location and select a camera, it will return where the point would land
//== on the imager of that camera in to 2D space.  This basically locates
//== where in the cameras FOV a 3D point would be located.
void     TT_CameraBackproject(int CameraIndex, float X, float Y, float Z, float &CameraX, float &CameraY, float CameraX_def, float CameraY_def){
	CameraX = CameraX_def;
	CameraY = CameraY_def;
}

//== RESULT PROCESSING ========================================================================================-----

const char *TT_GetResultString(NPRESULT result){ //== Return Plain Text Message =======================------
	string messageResult;
	const char *location;
	switch (result)
	{
	case NPRESULT_SUCCESS:  messageResult = "Successful Result";
		location = messageResult.c_str();
		return (location);
		break;

	case NPRESULT_FILENOTFOUND:messageResult = "File Not Found";
		location = messageResult.c_str();
		return (location);
		break;
	case NPRESULT_LOADFAILED:messageResult = "Load Failed";
		location = messageResult.c_str();
		return (location);
		break;
	case NPRESULT_FAILED: messageResult = "Failed";
		location = messageResult.c_str();
		return (location);
		break;
	case NPRESULT_INVALIDFILE: messageResult = "Invalid File";
		location = messageResult.c_str();
		return (location);
		break;
	case NPRESULT_INVALIDCALFILE:messageResult = "Invalid Calibration File";
		location = messageResult.c_str();
		return (location);
		break;
	case NPRESULT_UNABLETOINITIALIZE: messageResult = "Unable To Initialize";
		location = messageResult.c_str();
		return (location);
		break;
	case NPRESULT_INVALIDLICENSE:messageResult = "Invalid License";
		location = messageResult.c_str();
		return (location);
		break;
	case NPRESULT_NOFRAMEAVAILABLE: messageResult = "No Frames Available";
		location = messageResult.c_str();
		return (location);
		break;
	default:messageResult = "Unknown Error";
		location = messageResult.c_str();
		return (location);
		break;

	}

}



/**@}*/

