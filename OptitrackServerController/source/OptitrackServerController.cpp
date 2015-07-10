/*=========================================================================

Program:   Open IGT Link -- Example for Data Receiving Server Program
Module:    $RCSfile: $
Language:  C++
Date:      $Date: $
Version:   $Revision: $

Copyright (c) Insight Software Consortium. All rights reserved.

This software is distributed WITHOUT ANY WARRANTY; without even
the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
PURPOSE.  See the above copyright notices for more information.

=========================================================================*/

#include <iostream>
#include <math.h>
#include <cstdlib>
#include <cstring>

#include "igtlOSUtil.h"
#include "igtlClientSocket.h"
#include "igtlStatusMessage.h"

#if OpenIGTLink_PROTOCOL_VERSION >= 2
#include "igtlStringMessage.h"
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

#define N_STRINGS 1

const char * RequestString;


int ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader * header);

#if OpenIGTLink_PROTOCOL_VERSION >= 2
int SendString(igtl::Socket * socket, std::string message);
std::string ReceiveString(igtl::Socket * socket, igtl::MessageHeader * header);
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

int main(int argc, char* argv[])
{
	igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
	socket->ConnectToServer("10.140.18.88", 18944);
	

	if (socket.IsNotNull()) // if client connected
	{
		std::string Request;

		bool Control = true;
		int option;
		int numberAttachedTools;

		std::string toolName;

		int result;

		while (Control)
		{
			std::cout << " " << std::endl;
			std::cout << "Option:Open(0),LoadCalibration(1),AddTool(2),RemoveTool(3),StartTracking(4)" << std::endl;
			std::cout << "StopTracking(5),Reset(6),Close(7),InfoTools(8),SetCameraParameters(9)" << std::endl;
			option;
			std::cin >> option;
			switch (option)
			{
			case 0:
				Request = "Open";
				SendString(socket, Request);

				//TO-DO receive result
				//std::cout << " Open result: " << result << std::endl;			
				break;

			case 1:
				Request = "LoadCalibration";
				SendString(socket, Request);
				break;

			case 2:
				std::cout << " " << std::endl;
				std::cout << "Which tool do you want to add?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << std::endl;
				option;
				std::cin >> option;
				switch (option)
				{
				case 1:
					Request = "AddTool_OptitrackRB";
					SendString(socket, Request);

					break;
				case 2:
					Request = "AddTool_NDIPointerPolaris";
					SendString(socket, Request);
					break;
				default:
					fprintf(stdout, "#ERROR# Non-valid option!\n");
					break;
				}
				break;

			case 3:
				std::cout << " " << std::endl;
				std::cout << "Which tool do you want to remove?:OptiTrackRigidBody(1), or NDIPointerPolaris(2)" << std::endl;
				option;
				std::cin >> option;
				switch (option)
				{
				case 1:
					Request = "RemoveTool_OptitrackRB";
					SendString(socket, Request);

					break;
				case 2:
					Request = "RemoveTool_NDIPointerPolaris";
					SendString(socket, Request);
					break;
				default:
					fprintf(stdout, "#ERROR# Non-valid option!\n");
					break;
				}
				break;
			case 4:
				Request = "StartTracking";
				SendString(socket, Request);
				break;

			case 5:
				Request = "StopTracking";
				SendString(socket, Request);
				break;
			case 6:
				Request = "Reset";
				SendString(socket, Request);
				break;
				break;
			case 7:
				Request = "Close";
				SendString(socket, Request);
				Control = false;
				break;
			case 8:
				Request = "GetNumberOfAttachedTools";
				SendString(socket, Request);
				break;
				//numberAttachedTools = m_Tracker->GetNumberOfAttachedTools();
				//std::cout << " Number of attached tools: " << numberAttachedTools << std::endl;
				//nextID = objTool->GetIDnext();
				//numberOfMarkers = objTool->GetNumberOfMarkers();
				//optitrackID = objTool->GetOptitrackID();
				//toolName = objTool->GetToolName();
				//cout << " Info Tool: " << "[numberOfMarkers] " << numberOfMarkers << "; [optitrackID] " << optitrackID << "; [toolName] " << toolName << endl;
				break;
			case 9:
				Request = "SetCameraParams";
				SendString(socket, Request);
				//result = m_Tracker->SetCameraParams(7, 200, 14, 4);
				//std::cout << " SetCamParams result: " << result << std::endl;
				break;
			}
			break;


		}

		std::cout << "Exited while" << std::endl;


		/* Receive data
			Create a message buffer to receive header
			igtl::MessageHeader::Pointer headerMsg;
			headerMsg = igtl::MessageHeader::New();

			headerMsg->InitPack();

			// Receive generic header from the socket
			int r = socket->Receive(headerMsg->GetPackPointer(), headerMsg->GetPackSize());
			if (r == 0)
			{
			socket->CloseSocket();
			}
			if (r != headerMsg->GetPackSize())
			{
			continue;
			}

			// Deserialize the header
			headerMsg->Unpack();

			// Check data type and receive data body
			if (strcmp(headerMsg->GetDeviceType(), "STATUS") == 0)
			{
			int status = ReceiveStatus(socket, headerMsg);
			if (status == igtl::StatusMessage::STATUS_OK)
			{
			std::cerr << "Receiving : " << "OK" << std::endl;
			}
			}

			#if OpenIGTLink_PROTOCOL_VERSION >= 2
			else if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0)
			{

			std::string Str = ReceiveString(socket, headerMsg);
			std::cerr << "Receiving : " << Str << std::endl;
			}
			#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

			else
			{
			// if the data type is unknown, skip reading.
			std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
			socket->Skip(headerMsg->GetBodySizeToRead(), 0);
			}

			igtl::Sleep(10);

			}
			socket->CloseSocket();*/
	}

}


int ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader * header)
{

	std::cerr << "Receiving STATUS data type." << std::endl;

	// Create a message buffer to receive transform data
	igtl::StatusMessage::Pointer statusMsg;
	statusMsg = igtl::StatusMessage::New();
	statusMsg->SetMessageHeader(header);
	statusMsg->AllocatePack();

	// Receive transform data from the socket
	socket->Receive(statusMsg->GetPackBodyPointer(), statusMsg->GetPackBodySize());

	// Deserialize the transform data
	// If you want to skip CRC check, call Unpack() without argument.
	int c = statusMsg->Unpack(1);

	if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
	{
		std::cerr << "========== STATUS ==========" << std::endl;
		std::cerr << " Code      : " << statusMsg->GetCode() << std::endl;
		std::cerr << " SubCode   : " << statusMsg->GetSubCode() << std::endl;
		std::cerr << " Error Name: " << statusMsg->GetErrorName() << std::endl;
		std::cerr << " Status    : " << statusMsg->GetStatusString() << std::endl;
		std::cerr << "============================" << std::endl;
	}

	return c;

}


#if OpenIGTLink_PROTOCOL_VERSION >= 2

std::string ReceiveString(igtl::Socket * socket, igtl::MessageHeader * header)
{
	std::cerr << "Receiving STRING data type." << std::endl;

	// Create a message buffer to receive transform data
	igtl::StringMessage::Pointer stringMsg;
	stringMsg = igtl::StringMessage::New();
	stringMsg->SetMessageHeader(header);
	stringMsg->AllocatePack();

	// Receive transform data from the socket
	socket->Receive(stringMsg->GetPackBodyPointer(), stringMsg->GetPackBodySize());

	// Deserialize the transform data
	int c = stringMsg->Unpack(1);

	if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
	{
		std::cerr << "Encoding: " << stringMsg->GetEncoding() << "; " << "String: " << stringMsg->GetString() << std::endl;
	}

	return stringMsg->GetString();
}

int SendString(igtl::Socket * socket, std::string request)
{
	std::cerr << "Sending STRING data type." << std::endl;

	igtl::StringMessage::Pointer strMessage = igtl::StringMessage::New();
	strMessage->SetDeviceName("Optitrack");
	
	strMessage->SetString(request);
	strMessage->Pack();

	socket->Send(strMessage->GetPackPointer(), strMessage->GetPackSize());
	igtl::Sleep(10); // wait
	return 1;
}


#endif //OpenIGTLink_PROTOCOL_VERSION >= 2