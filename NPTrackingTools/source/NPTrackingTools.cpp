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


// Definición de parametros para comprobar la respuesta del programa ante errores 
// En estado "false" no deben dar error.
#define FAIL_INVALIDLICENSE false
#define FAIL_UNABLETOINITIALIZE false
#define FAIL_UNABLETOSHUTDOWN false
#define FAIL_UNABLETOCLEANUP false
#define FAIL_NOFRAMEAVAILABLE false
#define FAIL_FAILED false
#define FAIL_INVALIDFILE false
#define FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME false

extern int variableTestNumberOfMarkers;
extern float variableTestMarkerPositionX, variableTestMarkerPositionY, variableTestMarkerPositionZ;
extern float rigidBodyPositionX, rigidBodyPositionY, rigidBodyPositionZ;
extern float rigidBodyPositionX2, rigidBodyPositionY2, rigidBodyPositionZ2;
extern float rigidBodyQuaternionX, rigidBodyQuaternionY, rigidBodyQuaternionZ, rigidBodyQuaternionW;
extern float rigidBodyYaw, rigidBodyPitch, rigidBodyRoll;
extern int numberOfTrackables, trackableID, numberOfTrackableMarkers;
extern bool stateOfTrackable;
extern float trackableMarkerPositionX, trackableMarkerPositionY, trackableMarkerPositionZ;
extern float trackableMarkerPositionXPointCloud, trackableMarkerPositionYPointCloud, trackableMarkerPositionZPointCloud;



// Variables para probar funciones:
int variableTestNumberOfMarkers;
int frameMarkerLabel = 1000;
double frameTimeSpamp = 0.0;
bool cameraContributionTo3DMarker = true;
float variableTestMarkerPositionX, variableTestMarkerPositionY, variableTestMarkerPositionZ;
float rigidBodyPositionX, rigidBodyPositionY, rigidBodyPositionZ, rigidBodyQuaternionX, rigidBodyQuaternionY, rigidBodyQuaternionZ, rigidBodyQuaternionW;
float rigidBodyPositionX2, rigidBodyPositionY2, rigidBodyPositionZ2;
float rigidBodyYaw, rigidBodyPitch, rigidBodyRoll;
int numberOfTrackables;
int trackableID = 1000;
bool stateOfTrackable = false;
int numberOfTrackableMarkers; /*!< Detailed description of the variable "numberOfTrackableMarkers" */
float trackableMarkerPositionX, trackableMarkerPositionY, trackableMarkerPositionZ;
float trackableMarkerPositionXPointCloud, trackableMarkerPositionYPointCloud, trackableMarkerPositionZPointCloud;


//== NPRIGIDBODY PUBLIC INTERFACE ===================================================-----
                 //== TTAPI   NPRESULT Defines Call Success/Failure ====-----

//== RIGID BODY STARTUP / SHUTDOWN ==================================================-----


/** \fn TT_Initialize()
 *  \brief This function attempts to initialize the Tracking Tools tracking API.
 *
 *  It should be called before attempting to use other compontents of the API.  It returns information about whether or not it succeeded.
 *   
 *  @return 0  if succeeded, 10 if a valid license is not found, and 11 if it was unable to initialize.
 *  
 */
TTAPI   NPRESULT TT_Initialize() {
    
    if (FAIL_INVALIDLICENSE) {
        return TTAPI   NPRESULT_INVALIDLICENSE; //A valid Tracking tools license was not found.
    }
    else if (FAIL_UNABLETOINITIALIZE){
        return TTAPI   NPRESULT_UNABLETOINITIALIZE;
    }
    else{
        return TTAPI   NPRESULT_SUCCESS;
    }
}   

/*! \fn TT_Shutdown()
    \brief This function attempts to shutdown the Tracking Tools tracking API.

    It should be called before the application using the Traking Tools tracking API closes.  It returns information about whether or not it succeeded.

    \return 0  if succeeded, and 3 if it was unable to initialize.

*/
TTAPI   NPRESULT TT_Shutdown()  {
    if (FAIL_UNABLETOSHUTDOWN){
        return TTAPI   NPRESULT_FAILED;
    }
    else{
        return TTAPI   NPRESULT_SUCCESS;
    }
}

/*! \fn TT_FinalCleanup()
    \brief This function shuts down the camera device driver and ensures all the driver threads are terminated properly.

    The TT_FinalCleanup() function should be called when applications are done using the Tracking Tools API services. This is 
    typically when an application is closing, but can be performed sooner to recover resources when the Tracking Tools API is 
    not being used.

    \return 0  if succeeded, and 3 if it was unable to initialize.

*/
TTAPI   NPRESULT TT_FinalCleanup(){
    if (FAIL_UNABLETOCLEANUP){
        return TTAPI   NPRESULT_FAILED;
    }
    else {
        return TTAPI   NPRESULT_SUCCESS;
    }
}   
//== this before exiting your application. ===----- 


TTAPI   NPRESULT TT_LoadCalibration(const char *filename){
    ifstream calFile;
    calFile.open(filename, ios::in);
    std::string stringFilename(filename); //Convert filename from const char to string.
    
    if (stringFilename.substr(stringFilename.find_last_of(".")) != ".cal") { 
    //Check that the extension of the file is ".cal". First a substring is created from the last point of the original 
    //string until the end, and then this substring is compared to the desired extension: ".cal".
        return TTAPI   NPRESULT_INVALIDCALFILE; //Invalid calibration file.
    }else if (FAIL_FAILED){ //Maybe !calFile.good() or !calFile.is_open().
    //If calFile.good(), which is a boolean, is false --> the file does not exist.
        return TTAPI   NPRESULT_LOADFAILED; //The file or path specified is no valid.
    }
    else {
        return TTAPI   NPRESULT_SUCCESS; //Method succeeded.
    }
    calFile.close();

}

TTAPI   NPRESULT TT_LoadTrackables(const char *filename){ //== Load Trackables ======----[Si]
    ifstream trackablesFile;
    trackablesFile.open(filename, ios::in); //ios::in --> abre para lectura, situándose al principio.
    std::string stringFilename(filename); //Convert filename from const char to string.

    if (FAIL_INVALIDFILE) { 
        //ASSUMING THAT THE FILE MUST BE .TXT: if (stringFilename.substr(stringFilename.find_last_of(".")) != ".txt")
        //
        //Check that the extension of the file is ".txt". First a substring is created from the last point of the original 
        //string until the end, and then this substring is compared to the desired extension: ".txt".
        return TTAPI   NPRESULT_INVALIDFILE; //Invalid file.
    }
    else if (FAIL_FAILED){ 
        
        return TTAPI   NPRESULT_LOADFAILED; //The file or path specified is no valid.
    }
    else {
        return TTAPI   NPRESULT_SUCCESS; //Method succeeded.
    }
    trackablesFile.close();
}
TTAPI   NPRESULT TT_SaveTrackables(const char *filename){ //== Save Trackables ======---- (NO HAY INFO DE FUNCIÓN EN DOCUMENTO)
    ofstream trackablesFile;
    trackablesFile.open(filename, ios::app);
    //INCOMPLETE
    trackablesFile.close();
    return (TTAPI   NPRESULT_SUCCESS);

}
TTAPI   NPRESULT TT_AddTrackables(const char *filename){ //== Add  Trackables ======----[Si]
    ifstream trackablesFile;
    trackablesFile.open(filename, ios::app); //ios::app --> abre el archivo para añadir data al final.
    std::string stringFilename(filename); //Convert filename from const char to string.

    if (FAIL_INVALIDFILE) {
        //ASSUMING THAT THE FILE MUST BE .TXT: if (stringFilename.substr(stringFilename.find_last_of(".")) != ".txt")
        //
        //Check that the extension of the file is ".txt". First a substring is created from the last point of the original 
        //string until the end, and then this substring is compared to the desired extension: ".txt".
        return TTAPI   NPRESULT_INVALIDFILE; //Invalid file.
    }
    else if (FAIL_FAILED){

        return TTAPI   NPRESULT_LOADFAILED; //The file or path specified is no valid.
    }
    else {
        return TTAPI   NPRESULT_SUCCESS; //Method succeeded.
    }
    trackablesFile.close();
}

TTAPI   NPRESULT TT_Update(){                     //== Process incoming camera data -[Si]
    if (FAIL_INVALIDLICENSE){
        return TTAPI   NPRESULT_INVALIDLICENSE; //A valid Rigid Body license was not found.
    }
    else if (FAIL_NOFRAMEAVAILABLE){
        return TTAPI   NPRESULT_NOFRAMEAVAILABLE; // No tracking data was found.
    }
    else {
        return TTAPI   NPRESULT_SUCCESS; // Method succeeded.
    }
}

TTAPI   NPRESULT TT_UpdateSingleFrame(){               //== Process incoming camera data -[Si]
    if (FAIL_INVALIDLICENSE){
        return TTAPI   NPRESULT_INVALIDLICENSE; //A valid Rigid Body license was not found.
    }
    else if (FAIL_NOFRAMEAVAILABLE){
        return TTAPI   NPRESULT_NOFRAMEAVAILABLE; // No tracking data was found.
    }
    else {
        return TTAPI   NPRESULT_SUCCESS; // Method succeeded.
    }
}

TTAPI   NPRESULT TT_LoadProject(const char *filename){ //== Load Project File ==========--[Si]
    ifstream projectFile;
    projectFile.open(filename, ios::in); //ios::in --> abre para lectura, situándose al principio.
    std::string stringFilename(filename); //Convert filename from const char to string.

    if (FAIL_INVALIDFILE) {
        //ASSUMING THAT THE FILE MUST BE .TXT: if (stringFilename.substr(stringFilename.find_last_of(".")) != ".txt")
        //
        //Check that the extension of the file is ".txt". First a substring is created from the last point of the original 
        //string until the end, and then this substring is compared to the desired extension: ".txt".
        return TTAPI   NPRESULT_INVALIDFILE; //Invalid file.
    }
    else if (FAIL_FAILED){

        return TTAPI   NPRESULT_LOADFAILED; //The file or path specified is no valid.
    }
    else {
        return TTAPI   NPRESULT_SUCCESS; //Method succeeded.
    }
    projectFile.close();
}

TTAPI   NPRESULT TT_SaveProject(const char *filename){ //== Save Project File ==========--[Si]
    ofstream projectFile;
    projectFile.open(filename, ios::app);
    //INCOMPLETE
    projectFile.close();
    return (TTAPI   NPRESULT_SUCCESS);
}

//== FRAME ==========================================================================-----[Si]

int      TT_FrameMarkerCount(){              //== Returns Frame Markers Count ---
    variableTestNumberOfMarkers += 100;  
    return (variableTestNumberOfMarkers);
}

float    TT_FrameMarkerX(int index){          //== Returns X Coord of Marker -----
    if (FAIL_NOFRAMEAVAILABLE){
        return (0); //No frame data was found
    }
    else{
        variableTestMarkerPositionX += 10;
        return (variableTestMarkerPositionX);
    }
    
}

float    TT_FrameMarkerY(int index){          //== Returns Y Coord of Marker -----
    if (FAIL_NOFRAMEAVAILABLE){
        return (0); //No frame data was found
    }
    else{
        variableTestMarkerPositionY += 100;
        return (variableTestMarkerPositionY);
    }
}

float    TT_FrameMarkerZ(int index){          //== Returns Z Coord of Marker -----
    if (FAIL_NOFRAMEAVAILABLE){
        return (0); //No frame data was found
    }
    else{
        variableTestMarkerPositionZ += 1000;
        return (variableTestMarkerPositionZ);
    }
}

int      TT_FrameMarkerLabel(int index){      //== Returns Label of Marker -------
    frameMarkerLabel += 1;
    return (frameMarkerLabel);
}

double   TT_FrameTimeStamp(){                 //== Time Stamp of Frame (seconds) 
    frameTimeSpamp += 0.01;
    return (frameTimeSpamp);
}

//== TT_FrameCameraCentroid returns true if the camera is contributing
//== to this 3D marker.  It also returns the location of the 2D centroid
//== that is reconstructing to this 3D marker.


bool     TT_FrameCameraCentroid(int index, int CameraIndex, float &X, float &Y){
    

    return (cameraContributionTo3DMarker);

}


//== TRACKABLES CONTROL =============================================================-----[Si]

bool     TT_IsTrackableTracked(int index){ //== Is trackable currently tracked ---
    if (FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME){
        return (false);
    }
    else {
        return (true);
    }
}

void     TT_TrackableLocation(int RigidIndex,       //== Trackable Index ======---
    float *x, float *y, float *z,                  //== Position ==---
    float *qx, float *qy, float *qz, float *qw,    //== Orientation -- 
    float *yaw, float *pitch, float *roll){        //== Orientation --

    if (FAIL_RIGIDBODYNOTFOUNDINCURRENTFRAME){
        cout << "The rigid body was not tracked in the current frame" << endl;
    }
    else {
        rigidBodyPositionX += 1;
        *x = rigidBodyPositionX;
        rigidBodyPositionY += 2;
        *y = rigidBodyPositionY;
        rigidBodyPositionZ += 3;
        *z = rigidBodyPositionZ;
        rigidBodyQuaternionX += 11;
        *qx = rigidBodyQuaternionX;
        rigidBodyQuaternionY += 22;
        *qy = rigidBodyQuaternionY;
        rigidBodyQuaternionZ += 33;
        *qz = rigidBodyQuaternionZ;
        rigidBodyQuaternionW += 44;
        *qw = rigidBodyQuaternionW;
        rigidBodyYaw += 111;
        *yaw = rigidBodyYaw;
        rigidBodyPitch += 222;
        *pitch = rigidBodyPitch;
        rigidBodyRoll += 333;
        *roll = rigidBodyRoll;
        
    }

}

void     TT_ClearTrackableList(){             //== Clear all trackables   =====---
    cout << "  List of rigid body definitions cleared!\n";
}

TTAPI   NPRESULT TT_RemoveTrackable(int Index){       //== Remove single trackable ====---
    if (FAIL_FAILED){
        return (TTAPI   NPRESULT_FAILED);
    }
    else{
        return (TTAPI   NPRESULT_SUCCESS);
    }
}

int      TT_TrackableCount(){                 //== Returns number of trackables  -
    numberOfTrackables += 5;
    return (numberOfTrackables);
}

int      TT_TrackableID(int index){           //== Get Trackables ID ==========---
    return (trackableID);
}

void     TT_SetTrackableID(int index, int ID){ //== Set Trackables ID ==========---
    trackableID = ID;
    
}

const char* TT_TrackableName(int index){      //== Returns Trackable Name =====---
    string trackableName = "Name of certain trackable";
    const char* nameAddress = static_cast<const char*>(trackableName.c_str());
    return (nameAddress);
}

void     TT_SetTrackableEnabled(int index, bool enabled){    //== Set Tracking   ====---
    stateOfTrackable = enabled;
}

bool     TT_TrackableEnabled(int index){                     //== Get Tracking   ====---
    return (stateOfTrackable);
}

TTAPI   NPRESULT TT_TrackableTranslatePivot(int index, float x, float y, float z){
    rigidBodyPositionX2 = rigidBodyPositionX;   
    rigidBodyPositionY2 = rigidBodyPositionY;
    rigidBodyPositionZ2 = rigidBodyPositionZ;
    rigidBodyPositionX2 += x;
    rigidBodyPositionY2 += y;
    rigidBodyPositionZ2 += z;
    return (TTAPI   NPRESULT_SUCCESS);
}

int      TT_TrackableMarkerCount(int index){             //== Get marker count   ====---
    numberOfTrackableMarkers += 10;
    return (numberOfTrackableMarkers);
}
void     TT_TrackableMarker(int RigidIndex,              //== Get Trackable mrkr ====---
    int MarkerIndex, float *x, float *y, float *z){
    
    trackableMarkerPositionX += 1;
    *x = trackableMarkerPositionX;
    trackableMarkerPositionY += 2;
    *y = trackableMarkerPositionY;
    trackableMarkerPositionZ += 3;
    *z = trackableMarkerPositionZ;
}

void     TT_TrackablePointCloudMarker(int RigidIndex,    //== Get corresponding point cloud marker ======---
    int MarkerIndex, bool &Tracked,          //== If tracked is false, there is no
    float &x, float &y, float &z){           //== corresponding point cloud marker.

    trackableMarkerPositionXPointCloud += 1;
    x = trackableMarkerPositionXPointCloud;
    trackableMarkerPositionYPointCloud += 2;
    y = trackableMarkerPositionY;
    trackableMarkerPositionZPointCloud += 3;
    z = trackableMarkerPositionZPointCloud;

}
/* No admite el archivo "trackablesettings.h"

 //== TT_CreateTrackable.  This creates a trackable based on the marker
//== count and marker list provided.  The MarkerList is a expected to
//== contain of list of marker coordinates in the order: x1,y1,z1,x2,
//== y2,z2,etc...xN,yN,zN.

TTAPI   NPRESULT TT_CreateTrackable(const char* Name, int ID, int MarkerCount, float *MarkerList){
    return (TTAPI   NPRESULT_SUCCESS);
}

TTAPI   NPRESULT TT_TrackableSettings(int Index, cTrackableSettings &Settings){  //== Get Trackable Settings =---

}
TTAPI   NPRESULT TT_SetTrackableSettings(int Index, cTrackableSettings &Settings){  //== Set Trackable Settings =---

}


//== RESULT PROCESSING ========================================================================================-----

*/

const char *TT_GetResultString(TTAPI   NPRESULT result){ //== Return Plain Text Message =======================-----[Si]
    string messageResult;
    const char *location;
    switch (result)
    {
    case TTAPI   NPRESULT_SUCCESS:  messageResult= "Successful Result";
        location = static_cast<const char*>(messageResult.c_str()); //Convert the string "messageResult" to a const char* "location"
        return (location);
        break;
    case TTAPI   NPRESULT_FILENOTFOUND:messageResult = "File Not Found";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    case TTAPI   NPRESULT_LOADFAILED:messageResult = "Load Failed";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    case TTAPI   NPRESULT_FAILED: messageResult = "Failed";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    case TTAPI   NPRESULT_INVALIDFILE: messageResult = "Invalid File";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    case TTAPI   NPRESULT_INVALIDCALFILE:messageResult = "Invalid Calibration File";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    case TTAPI   NPRESULT_UNABLETOINITIALIZE: messageResult = "Unable To Initialize";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    case TTAPI   NPRESULT_INVALIDLICENSE:messageResult = "Invalid License";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    case TTAPI   NPRESULT_NOFRAMEAVAILABLE: messageResult = "No Frames Available";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
    default:messageResult = "Unknown Error";
        location = static_cast<const char*>(messageResult.c_str());
        return (location);
        break;
        
    }
    
}

/**@}*/

