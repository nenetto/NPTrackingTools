// A simple program that computes the square root of a number
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <tinyxml2.h>
#include "NPTrackingTools.h"
#include "Laubrary.h"

#ifndef XMLCheckResult
#define XMLCheckResult(a_eResult) if (a_eResult != tinyxml2::XML_SUCCESS) { printf("Error: %i\n", a_eResult); }
#endif

#include <sstream>
#include <string.h>
using namespace std;

void CreateXMLTool(std::string pathNewXMLFile, std::string toolName, int toolNumberOfMarkers, float* arrayPositionsMarkers, float* arrayPositionPivot,
	const char* calibrationFileName = NULL, int dayCurrentDate = NULL, int monthCurrentDate = NULL, int yearCurrentDate = NULL,
	float calibrationErrorRMS = NULL, float calibrationErrorMean = NULL, float calibrationErrorSD = NULL, float calibrationErrorMedian = NULL, 
	float calibrationErrorQ1 = NULL, float calibrationErrorQ3 = NULL)
{
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("NPTrackingTools");
	xmlDoc.InsertFirstChild(pRoot);

	tinyxml2::XMLElement * pElement = xmlDoc.NewElement("FileType");
	pElement->SetText(" ToolDefinition ");
	pRoot->InsertEndChild(pElement);

	if ((dayCurrentDate != NULL) && (monthCurrentDate != NULL) && (yearCurrentDate != NULL))
	{
		pElement = xmlDoc.NewElement("CalibrationDate");
		pElement->SetAttribute("D", dayCurrentDate);
		pElement->SetAttribute("M", monthCurrentDate);
		pElement->SetAttribute("Y", yearCurrentDate);
		pRoot->InsertEndChild(pElement);
	}

	if ((calibrationErrorRMS != NULL) && (calibrationErrorMean != NULL) && (calibrationErrorSD != NULL)
		&& (calibrationErrorMedian != NULL) && (calibrationErrorQ1 != NULL) && (calibrationErrorQ3 != NULL))
	{
		pElement = xmlDoc.NewElement("CalibrationError");
		pElement->SetAttribute("RMS", calibrationErrorRMS);
		pElement->SetAttribute("Mean", calibrationErrorMean);
		pElement->SetAttribute("SD", calibrationErrorSD);
		pElement->SetAttribute("Median", calibrationErrorMedian);
		pElement->SetAttribute("Q1", calibrationErrorQ1);
		pElement->SetAttribute("Q3", calibrationErrorQ3);
		pRoot->InsertEndChild(pElement);
	}

	pElement = xmlDoc.NewElement("ToolName");
	char* char_ToolName = (char*)toolName.c_str(); //Conversion from string to char*.
	pElement->SetText(char_ToolName);
	pRoot->InsertEndChild(pElement);
	
	pElement = xmlDoc.NewElement("ToolMarkersNum");
	pElement->SetText(toolNumberOfMarkers);
	pRoot->InsertEndChild(pElement);

	pElement = xmlDoc.NewElement("ToolMarkers");
	tinyxml2::XMLElement * pListElement;
	for (int i = 0; i < toolNumberOfMarkers; i++)
	{
        //Conversion from int to char*.
		stringstream strs;
		strs << (i+1); // i+1 in order for the first marker to be 1 and not  0.
		string temp_str = strs.str();
		char* char_type = (char*)temp_str.c_str();

		pListElement = xmlDoc.NewElement(char_type);
		pListElement->SetAttribute("x", arrayPositionsMarkers[3*i]);
		pListElement->SetAttribute("y", arrayPositionsMarkers[3*i + 1]);
		pListElement->SetAttribute("z", arrayPositionsMarkers[3*i + 2]);
		pElement->InsertEndChild(pListElement);
	}
	pRoot->InsertEndChild(pElement);

	pElement = xmlDoc.NewElement("ToolPivot");
	pElement->SetAttribute("x", arrayPositionPivot[0]);
	pElement->SetAttribute("y", arrayPositionPivot[1]);
	pElement->SetAttribute("z", arrayPositionPivot[2]);
	pRoot->InsertEndChild(pElement);
	
	if (calibrationFileName != NULL)
	{
		pElement = xmlDoc.NewElement("CalibrationFile");
		pElement->SetText(calibrationFileName);
		pRoot->InsertEndChild(pElement);
	}

	char* char_Path = (char*)pathNewXMLFile.c_str(); //Conversion from string to char*.
	tinyxml2::XMLError eResult = xmlDoc.SaveFile(char_Path);
	XMLCheckResult(eResult);
}

void CreateXMLToolCalibrationFile(std::string pathNewXMLFile, int toolNumberOfCalibrationPoints, float* arrayPositionsCalibrationPoints, 
	float* arrayTransformation,	int dayCurrentDate = NULL, int monthCurrentDate = NULL, int yearCurrentDate = NULL,	float calibrationErrorRMS = NULL,
	float calibrationErrorMean = NULL, float calibrationErrorSD = NULL,	float calibrationErrorMedian = NULL, float calibrationErrorQ1 = NULL, 
	float calibrationErrorQ3 = NULL)
{
	tinyxml2::XMLDocument xmlDoc;
	tinyxml2::XMLNode * pRoot = xmlDoc.NewElement("NPTrackingTools");
	xmlDoc.InsertFirstChild(pRoot);

	tinyxml2::XMLElement * pElement = xmlDoc.NewElement("FileType");
	pElement->SetText(" PivotingData ");
	pRoot->InsertEndChild(pElement);

	if ((dayCurrentDate != NULL) && (monthCurrentDate != NULL) && (yearCurrentDate != NULL))
	{
		pElement = xmlDoc.NewElement("Date");
		pElement->SetAttribute("D", dayCurrentDate);
		pElement->SetAttribute("M", monthCurrentDate);
		pElement->SetAttribute("Y", yearCurrentDate);
		pRoot->InsertEndChild(pElement);
	}

	if ((calibrationErrorRMS != NULL) && (calibrationErrorMean != NULL) && (calibrationErrorSD != NULL)
		&& (calibrationErrorMedian != NULL) && (calibrationErrorQ1 != NULL) && (calibrationErrorQ3 != NULL))
	{
		pElement = xmlDoc.NewElement("CalibrationError");
		pElement->SetAttribute("RMS", calibrationErrorRMS);
		pElement->SetAttribute("Mean", calibrationErrorMean);
		pElement->SetAttribute("SD", calibrationErrorSD);
		pElement->SetAttribute("Median", calibrationErrorMedian);
		pElement->SetAttribute("Q1", calibrationErrorQ1);
		pElement->SetAttribute("Q3", calibrationErrorQ3);
		pRoot->InsertEndChild(pElement);
	}

	pElement = xmlDoc.NewElement("CalibrationPoints");
	pElement->SetText(toolNumberOfCalibrationPoints);
	pRoot->InsertEndChild(pElement);

	pElement = xmlDoc.NewElement("Data");
	tinyxml2::XMLElement * pListElement;
	for (int i = 0; i < toolNumberOfCalibrationPoints; i++)
	{
		//Conversion from int to char*.
		stringstream strs;
		strs << (i + 1); // i+1 in order for the first marker to be 1 and not  0.
		string temp_str = strs.str();
		char* char_type = (char*)temp_str.c_str();

		pListElement = xmlDoc.NewElement(char_type);
		pListElement->SetAttribute("x", arrayPositionsCalibrationPoints[3 * i]);
		pListElement->SetAttribute("y", arrayPositionsCalibrationPoints[3 * i + 1]);
		pListElement->SetAttribute("z", arrayPositionsCalibrationPoints[3 * i + 2]);
		pElement->InsertEndChild(pListElement);
	}
	pRoot->InsertEndChild(pElement);

	pElement = xmlDoc.NewElement("Transformation");
	pElement->SetAttribute("x", arrayTransformation[0]);
	pElement->SetAttribute("y", arrayTransformation[1]);
	pElement->SetAttribute("z", arrayTransformation[2]);
	pRoot->InsertEndChild(pElement);

	char* char_Path = (char*)pathNewXMLFile.c_str(); //Conversion from string to char*.
	tinyxml2::XMLError eResult = xmlDoc.SaveFile(char_Path);
	XMLCheckResult(eResult);
}

void ReadXMLTool(std::string pathXMLFile, char &fileType, std::string &toolName, int &toolNumberOfMarkers, float* arrayPositionsMarkers, float* arrayPositionPivot,
	const char*calibrationFileName, int &dayCurrentDate, int &monthCurrentDate, int &yearCurrentDate, float &calibrationErrorRMS, float &calibrationErrorMean,
	float &calibrationErrorSD, float &calibrationErrorMedian, float &calibrationErrorQ1, float &calibrationErrorQ3)
{
	tinyxml2::XMLDocument xmlDoc;
	char* char_Path = (char*)pathXMLFile.c_str(); //Conversion from string to char*.
	tinyxml2::XMLError eResult = xmlDoc.LoadFile(char_Path);
	XMLCheckResult(eResult);

	tinyxml2::XMLNode * pRoot = xmlDoc.FirstChild();
	if (pRoot == nullptr) tinyxml2::XMLError::XML_ERROR_FILE_READ_ERROR;

	//FileType
	tinyxml2::XMLElement * pElement = pRoot->FirstChildElement("FileType");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	fileType = *pElement->GetText();
	cout << fileType << endl;

	//CalibrationDate
	pElement = pRoot->FirstChildElement("CalibrationDate");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	
	eResult = pElement->QueryIntAttribute("D", &dayCurrentDate);
	XMLCheckResult(eResult);

	eResult = pElement->QueryIntAttribute("M", &monthCurrentDate);
	XMLCheckResult(eResult);

	eResult = pElement->QueryIntAttribute("Y", &yearCurrentDate);
	XMLCheckResult(eResult);

	//CalibrationError
	pElement = pRoot->FirstChildElement("CalibrationError");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;

	eResult = pElement->QueryFloatAttribute("RMS", &calibrationErrorRMS);
	XMLCheckResult(eResult);

	eResult = pElement->QueryFloatAttribute("Mean", &calibrationErrorMean);
	XMLCheckResult(eResult);

	eResult = pElement->QueryFloatAttribute("SD", &calibrationErrorSD);
	XMLCheckResult(eResult);

	eResult = pElement->QueryFloatAttribute("Median", &calibrationErrorMedian);
	XMLCheckResult(eResult);

	eResult = pElement->QueryFloatAttribute("Q1", &calibrationErrorQ1);
	XMLCheckResult(eResult);

	eResult = pElement->QueryFloatAttribute("Q3", &calibrationErrorQ3);
	XMLCheckResult(eResult);

	//ToolName
	pElement = pRoot->FirstChildElement("ToolName");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	toolName = pElement->GetText(); //REvisaaaaar
	
	//ToolMarkersNum
	pElement = pRoot->FirstChildElement("ToolMarkersNum");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	eResult = pElement->QueryIntText(&toolNumberOfMarkers);
	XMLCheckResult(eResult);

	//ToolMarkers
	pElement = pRoot->FirstChildElement("ToolMarkers");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	int counter = 0;
	int markerLocation = 0;
	tinyxml2::XMLElement * pListElement = pElement->FirstChildElement("0");
	while ((counter<(toolNumberOfMarkers * 3)) && (pListElement != nullptr))
	{
		eResult = pListElement->QueryFloatAttribute("x", &arrayPositionsMarkers[counter]);
		XMLCheckResult(eResult);
		eResult = pListElement->QueryFloatAttribute("y", &arrayPositionsMarkers[counter + 1]);
		XMLCheckResult(eResult);
		eResult = pListElement->QueryFloatAttribute("z", &arrayPositionsMarkers[counter + 2]);
		XMLCheckResult(eResult);
		counter = counter + 3;

		//Conversion from int to char*.
		stringstream strs;
		markerLocation = (counter / 3) + 1;
		strs << (markerLocation);
		string temp_str = strs.str();
		char* char_type = (char*)temp_str.c_str();
		pListElement = pListElement->NextSiblingElement(char_type);
	}

	//ToolPivot
	pElement = pRoot->FirstChildElement("ToolPivot");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;

	eResult = pElement->QueryFloatAttribute("x", &arrayPositionPivot[0]);
	XMLCheckResult(eResult);

	eResult = pElement->QueryFloatAttribute("y", &arrayPositionPivot[1]);
	XMLCheckResult(eResult);

	eResult = pElement->QueryFloatAttribute("z", &arrayPositionPivot[2]);
	XMLCheckResult(eResult);

	//CalibrationFile
	pElement = pRoot->FirstChildElement("CalibrationFile");
	if (pElement == nullptr) tinyxml2::XMLError::XML_ERROR_PARSING_ELEMENT;
	char* char_calibrationFileName;
	calibrationFileName = pElement->GetText(); //REvisaaaaar
}


int main (int argc, char *argv[])
{

  Laubrary::message(); // Use of function in Laubrary

  string toolName = "Tool";
  int numberOfMarkers = 3;
  float arrayPositionsMarkers[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  float arrayPositionPivot[3] = { 11, 22, 33 };

  CreateXMLTool("D:/DAVID UC3M/SavedData.xml", toolName, numberOfMarkers, arrayPositionsMarkers, arrayPositionPivot, "CalFile.cal", 24, 02, 2015, 0.1, 0.2, 0.3, 0.4, 0.5, 0.6);

  int toolNumberOfCalibrationPoints = 3;
  float arrayPositionsCalibrationPoints[9] = { 1, 2, 3, 4, 5, 6, 7, 8, 9 };
  float arrayTransformation[3] = { 11, 22, 33 };

  CreateXMLToolCalibrationFile("D:/DAVID UC3M/SavedDataCalibration.xml", toolNumberOfCalibrationPoints, arrayPositionsCalibrationPoints, arrayTransformation,
	  24, 02, 2015, 1, 2, 3, 4, 5, 6);
  
  string pathXMLFileToRead = "D:/DAVID UC3M/SavedData.xml";
  char fileType_read;
  string toolName_read;
  int toolNumberOfMarkers_read;
  float* arrayPositionsMarkers_read;
  float* arrayPositionPivot_read;
  char* calibrationFileName_read;
  int dayCurrentDate_read, monthCurrentDate_read, yearCurrentDate_read   ;
  float calibrationErrorRMS_read=0, calibrationErrorMean_read=0, calibrationErrorSD_read=0, 
	  calibrationErrorMedian_read=0, calibrationErrorQ1_read=0, calibrationErrorQ3_read=0;

  ReadXMLTool(pathXMLFileToRead, fileType_read, toolName_read, toolNumberOfMarkers_read, arrayPositionsMarkers_read, arrayPositionPivot_read,
	  calibrationFileName_read, dayCurrentDate_read, monthCurrentDate_read, yearCurrentDate_read, calibrationErrorRMS_read, calibrationErrorMean_read,
	  calibrationErrorSD_read, calibrationErrorMedian_read, calibrationErrorQ1_read, calibrationErrorQ3_read);
  cout << "," << arrayPositionsMarkers_read[1] << "," << arrayPositionPivot_read[1] << "," << endl;


/*
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
  */
  system("PAUSE");
  
  return 0;
}