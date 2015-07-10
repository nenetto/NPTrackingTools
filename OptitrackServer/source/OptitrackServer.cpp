/*=========================================================================

  Program:   Open IGT Link -- Example for Tracker Server Program
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

#include "OptitrackServer.h"

//A�adido por David (7-Julio)
#include <stdio.h>
#include <stdlib.h>
#include "OptitrackTracker.h"
#include "OptitrackTool.h"
#include <sstream>
#include <string.h>
using namespace std;


namespace Optitrack
{

	OptitrackServer::OptitrackServer()
	{
		m_PortNumber = 18944;
		m_Interval = 10;
	
		m_ServerSocket = igtl::ServerSocket::New();
		int r = m_ServerSocket->CreateServer(m_PortNumber);

		if (r < 0)
		{
			std::cerr << "Cannot create a server socket." << std::endl;
			exit(0);
		}
	
		//m_ListOfSockets = std::vector<igtl::Socket::Pointer>();

		this->m_MultiThreader = itk::MultiThreader::New();

		this->m_RequestMessageMutex = itk::FastMutexLock::New();

		// Launch multiThreader using the Function ThreadStartTracking that executes the TrackTools() method
		m_RequestMessage = "";

	}

	OptitrackServer::~OptitrackServer()
	{
		int result = m_Tracker->Close();
		std::cout << " Close result: " << result << std::endl;
		std::cout << " State -> " << m_Tracker->GetState() << std::endl;
		result = m_Tracker->Reset();

		std::cout << " Reset result: " << result << std::endl;
		std::cout << " State -> " << m_Tracker->GetState() << std::endl;

		m_ServerSocket->CloseSocket();
		
		m_MultiThreader->Delete();
	}


	void OptitrackServer::InitializeSystem()
	{

		m_Tracker = Optitrack::OptitrackTracker::New();
		std::string myCalfile = "C:/DavidGarciaMato/Calibration.cal";
		m_Tracker->SetCalibrationFile(myCalfile);

		m_Tool = Optitrack::OptitrackTool::New();
		std::string namefilePolaris = "C:/DavidGarciaMato/PunteroPolaris.txt";
		m_Tool->ConfigureToolByTxtFile(namefilePolaris);
	
		Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
		std::string namefilePolaris = "C:/David/PunteroPolarisXML.xml";
		objToolOptitrack->ConfigureToolByXmlFile(namefilePolaris);
		

		Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
		std::string namefileOptitrack = "C:/DavidGarciaMato/OptitrackRigidBody.txt";
		objToolOptitrack->ConfigureToolByTxtFile(namefileOptitrack);
		
		Optitrack::OptitrackTool::Pointer objToolOptitrack = Optitrack::OptitrackTool::New();
		std::string namefileOptitrack = "C:/David/OptitrackRigidBodyXML.xml";
		objToolOptitrack->ConfigureToolByXmlFile(namefileOptitrack);

	}


	std::string OptitrackServer::ReceiveTransform(igtl::Socket * socket, igtl::MessageHeader * header)
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

	void OptitrackServer::Connections()
	{



		int result;
		int currentState = m_Tracker->GetState();
		bool stop = false;
		int option;
		int numberAttachedTools;
		//int nextID, numberOfMarkers, optitrackID;
		string toolName;


		int port = 18944;

		double fps = 10;
		int interval = (int)(1000.0 / fps);
		
		igtl::TransformMessage::Pointer transMsg;
		transMsg = igtl::TransformMessage::New();
		transMsg->SetDeviceName("Tracker");


		igtl::Socket::Pointer socket;

	
		while (1)
		{
			//------------------------------------------------------------
			// Waiting for Connection
			socket = m_ServerSocket->WaitForConnection(1000);

			if (socket.IsNotNull()) // if client connected
			{
				m_Socket = socket;
				     
				m_ThreadID = m_MultiThreader->SpawnThread(this->ThreadSendNavigationInfo, this);
				break;
			}
		}
		
	}

	std::string OptitrackServer::ReceiveInstructions(igtl::Socket::Pointer instSocket)
	{
		//A�adido por David (09-07-2015)
		m_RequestMessage = ReceiveString(m_Socket, /* �?header?�*/);
		return m_RequestMessage;
	}


	ITK_THREAD_RETURN_TYPE OptitrackServer::ThreadSendNavigationInfo(void* pInfoStruct)
	{

		fprintf(stdout, "<INFO> - [OptitrackServer::ThreadSendNavigationInfo]\n");

		/* extract this pointer from Thread Info structure */
		struct itk::MultiThreader::ThreadInfoStruct * pInfo = (struct itk::MultiThreader::ThreadInfoStruct*)pInfoStruct;

		if (pInfo == NULL)
		{
			return ITK_THREAD_RETURN_VALUE;
		}

		if (pInfo->UserData == NULL)
		{
			return ITK_THREAD_RETURN_VALUE;
		}

		OptitrackServer *server = static_cast<OptitrackServer*>(pInfo->UserData);

		if (server != NULL)
		{
			// Call the TrackTools function in this thread
			server->SendInformation();
		}
		else
		{
			fprintf(stdout, "#ERROR# - [OptitrackServer::ThreadStartTracking]: server is NULL\n");
		}

		//m_Tracker->StopTracking();

		server->m_ThreadID = -1; // reset thread ID because we end the thread here
		return ITK_THREAD_RETURN_VALUE;

		// Delete next when thread is uncommented
		return 0;

	}

	void OptitrackServer::SendInformation()
	{
		igtl::TransformMessage::Pointer transMsg;
		transMsg = igtl::TransformMessage::New();
		transMsg->SetDeviceName("Tracker");

		while (1)
		{
			this->m_RequestMessageMutex->Lock();
			std::string localRequestMessage = m_RequestMessage;// Launch multiThreader using the Function ThreadStartTracking that executes the TrackTools() method
			this->m_RequestMessageMutex->Unlock();

			if (localRequestMessage == "Close")
			{
				break;
			}
			else if (localRequestMessage == "Start")
			{
				igtl::Matrix4x4 matrix;
				if (m_Socket.IsNotNull())
				{
					igtl::Matrix4x4 matrix;
					GetOptitrackToolTransformMatrix(matrix);
					//GetRandomTestMatrix(matrix);
					transMsg->SetMatrix(matrix);
					transMsg->Pack();
					m_Socket->Send(transMsg->GetPackPointer(), transMsg->GetPackSize());
					igtl::Sleep(m_Interval); // wait

				}
			}
		}

	}

	int OptitrackServer::SendTransform(igtl::Socket * socket, igtl::MessageHeader * header)
	{
		std::cerr << "Receiving TRANSFORM data type." << std::endl;

		// Create a message buffer to receive transform data
		igtl::TransformMessage::Pointer transMsg;
		transMsg = igtl::TransformMessage::New();
		transMsg->SetMessageHeader(header);
		transMsg->AllocatePack();

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
			return 1;
		}

		return 0;

	}

	int OptitrackServer::SendPosition(igtl::Socket * socket)
	{
		std::cerr << "Sending POSITION data type." << std::endl;

		//------------------------------------------------------------
		// Allocate Transform Message Class

		igtl::PositionMessage::Pointer positionMsg;
		positionMsg = igtl::PositionMessage::New();
		positionMsg->SetDeviceName("Tracker");
		positionMsg->SetPackType(igtl::PositionMessage::ALL); // default
		float position[3];
		float quaternion[4];

		GetRandomTestVectors(position, quaternion);
		positionMsg->SetPosition(position);
		positionMsg->SetQuaternion(quaternion);
		positionMsg->Pack();
		socket->Send(positionMsg->GetPackPointer(), positionMsg->GetPackSize());
		igtl::Sleep(10); // wait

		return 1;
	}


	int OptitrackServer::SendStatus(igtl::Socket * socket, int status)
	{

		std::cerr << "Sending STATUS data type." << std::endl;

		// Allocate Status Message Class

		igtl::StatusMessage::Pointer statusMsg;
		statusMsg = igtl::StatusMessage::New();
		statusMsg->SetDeviceName("Optitrack");

		//statusMsg->SetCode(igtl::StatusMessage::STATUS_OK);
		statusMsg->SetCode(status);
		statusMsg->SetSubCode(128);
		statusMsg->SetErrorName("OK!");
		statusMsg->SetStatusString("This is a test to send status message.");
		statusMsg->Pack();
		socket->Send(statusMsg->GetPackPointer(), statusMsg->GetPackSize());
		igtl::Sleep(10); // wait

		return 1;

	}


#if OpenIGTLink_PROTOCOL_VERSION >= 2

	std::string OptitrackServer::ReceiveString(igtl::Socket * socket, igtl::MessageHeader * header)
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

	int OptitrackServer::SendString(igtl::Socket * socket, std::string Str)
	{
		//------------------------------------------------------------
		// Allocate Transform Message Class

		igtl::StringMessage::Pointer stringMsg = igtl::StringMessage::New();
		stringMsg->SetDeviceName("Optitrack");

		stringMsg->SetString(Str);

		std::cout << "Sending string: " << Str << std::endl;

		stringMsg->Pack();
		socket->Send(stringMsg->GetPackPointer(), stringMsg->GetPackSize());
		igtl::Sleep(10); // wait

		return 1;

	}


#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

	//------------------------------------------------------------
	// Function to generate random matrix.

	void OptitrackServer::GetRandomTestVectors(float* position, float* quaternion)
	{

		// random position
		static float phi = 0.0;
		position[0] = 50.0 * cos(phi);
		position[1] = 50.0 * sin(phi);
		position[2] = 50.0 * cos(phi);
		phi = phi + 0.2;

		// random orientation
		static float theta = 0.0;
		quaternion[0] = 0.0;
		quaternion[1] = 0.6666666666*cos(theta);
		quaternion[2] = 0.577350269189626;
		quaternion[3] = 0.6666666666*sin(theta);
		theta = theta + 0.1;

		std::cerr << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;
		std::cerr << "quaternion = (" << quaternion[0] << ", " << quaternion[1] << ", "
			<< quaternion[2] << ", " << quaternion[3] << ")" << std::endl << std::endl;
	}

	void OptitrackServer::GetRandomTestMatrix(igtl::Matrix4x4& matrix)
	{
		float position[3];
		float orientation[4];

		// random position
		static float phi = 0.0;
		position[0] = 50.0 * cos(phi);
		position[1] = 50.0 * sin(phi);
		position[2] = 50.0 * cos(phi);
		phi = phi + 0.2;

		// random orientation
		static float theta = 0.0;
		orientation[0] = 0.0;
		orientation[1] = 0.6666666666*cos(theta);
		orientation[2] = 0.577350269189626;
		orientation[3] = 0.6666666666*sin(theta);
		theta = theta + 0.1;

		//igtl::Matrix4x4 matrix;
		igtl::QuaternionToMatrix(orientation, matrix);

		matrix[0][3] = position[0];
		matrix[1][3] = position[1];
		matrix[2][3] = position[2];

		igtl::PrintMatrix(matrix);
	}

	void OptitrackServer::GetOptitrackToolTransformMatrix(igtl::Matrix4x4& matrix)
	{
		vnl_vector_fixed<double,3> position;
		float orientation[4];
	    
		position[0] = this->m_Tool->GetPosition()[0];
		position[1] = this->m_Tool->GetPosition()[1];
		position[2] = this->m_Tool->GetPosition()[2];

		std::cout << "position   =" << m_Tool->GetPosition() << std::endl;

		std::cout << "position   = (" << position[0] << ", " << position[1] << ", " << position[2] << ")" << std::endl;

	
		orientation[0] = this->m_Tool->GetOrientation()[0];
		orientation[1] = this->m_Tool->GetOrientation()[1];
		orientation[2] = this->m_Tool->GetOrientation()[2];
		orientation[3] = this->m_Tool->GetOrientation()[3];
	
		igtl::QuaternionToMatrix(orientation, matrix);

		matrix[0][3] = position[0];
		matrix[1][3] = position[1];
		matrix[2][3] = position[2];

		igtl::PrintMatrix(matrix);
	}
}