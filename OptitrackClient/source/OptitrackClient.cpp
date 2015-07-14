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

#include "igtlTransformMessage.h"
#include "igtlServerSocket.h"
#include "igtlStatusMessage.h"
#include "igtlPositionMessage.h"

#if OpenIGTLink_PROTOCOL_VERSION >= 2
#include "igtlStringMessage.h"
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

#include "OptitrackClient.h"

#define N_STRINGS 1

const char * RequestString;

namespace Optitrack
{
	OptitrackClient::OptitrackClient()
	{
		igtl::ClientSocket::Pointer socket = igtl::ClientSocket::New();
		socket->ConnectToServer("10.140.18.210", 18944);
		igtl::MessageHeader::Pointer headerMsg;

		if (socket.IsNotNull()) // if client connected
		{
			std::string Request;

			bool Connection = true;
			int option;
			int numberAttachedTools;

			std::string toolName;

			int result;

			while (Connection)
			{
				std::cout << "Option: SendConfigurationFilePath(0), ReceiveData(1), SendOutputFilePath(2)" << std::endl;
				int option;
				std::cin >> option;
				if (option == 0)
				{
					result = this->SendString(socket, "J:/");
					std::cout << " Send ConfigurationFilePath result: " << result << std::endl;
				}
				else if (option == 1)
				{
					bool Receiving = true;
					std::cout << " Receive data: " << result << std::endl; //It can be update info about the tracking (i.e. number of tools) or tool transform for every tool

					// Receive data
					//Create a message buffer to receive header
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
						else if (status == igtl::StatusMessage::STATUS_DISABLED)
						{
							std::cerr << "Receiving : " << "Disabled" << std::endl;
							Connection = false;
						}
					}

#if OpenIGTLink_PROTOCOL_VERSION >= 2
					else if (strcmp(headerMsg->GetDeviceType(), "STRING") == 0)
					{
						std::string Str = ReceiveString(socket, headerMsg);
						std::cerr << "Receiving : " << Str << std::endl;
					}
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

					else if (strcmp(headerMsg->GetDeviceType(), "TRANSFORM") == 0)
					{
						std::string Str = ReceiveToolTransform(socket, headerMsg);
						std::cerr << "Receiving : " << Str << std::endl;
					}

					else
					{
						// if the data type is unknown, skip reading.
						std::cerr << "Receiving : " << headerMsg->GetDeviceType() << std::endl;
						socket->Skip(headerMsg->GetBodySizeToRead(), 0);
					}

					igtl::Sleep(10);
					break;
				}
				else
				{

					break;
				}
			}


		}
		socket->CloseSocket();
	}


	OptitrackClient::~OptitrackClient()
	{
		
	}

	int OptitrackClient::ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader * header)
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

	std::string OptitrackClient::ReceiveToolTransform(igtl::Socket * socket, igtl::MessageHeader * header)
	{
		std::cerr << "Receiving TRANSFORM data type." << std::endl;

		// Create a message buffer to receive transform data
		igtl::TransformMessage::Pointer transMsg;
		transMsg = igtl::TransformMessage::New();
		transMsg->SetMessageHeader(header);
		transMsg->AllocatePack();

		std::cerr << "Receiving TRANSFORM device name: " << header->GetDeviceName() << std::endl;
		// Receive transform data from the socket
		socket->Receive(transMsg->GetPackBodyPointer(), transMsg->GetPackBodySize());

		// Deserialize the transform data
		// If you want to skip CRC check, call Unpack() without argument.
		int c = transMsg->Unpack(1);

		if (c & igtl::MessageHeader::UNPACK_BODY) // if CRC check is OK
		{
			// Retrive the transform data
			igtl::Matrix4x4 matrix;
			transMsg->GetMatrix(matrix);
			igtl::PrintMatrix(matrix);
			return header->GetDeviceName();
		}

		return header->GetDeviceName();

	}


#if OpenIGTLink_PROTOCOL_VERSION >= 2

	std::string OptitrackClient::ReceiveString(igtl::Socket * socket, igtl::MessageHeader * header)
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

	int OptitrackClient::SendString(igtl::Socket * socket, std::string request)
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
}