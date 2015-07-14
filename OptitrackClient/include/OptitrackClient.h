#ifndef __OptitrackClient_h
#define __OptitrackClient_h

#include "igtlClientSocket.h"
#include "igtlSocket.h"
#include "igtlMessageHeader.h"


/**
* \class Optitrack Client
*/

namespace Optitrack
{

	class OptitrackClient
	{
	public:

		OptitrackClient();

		~OptitrackClient();

	protected:

		int ReceiveStatus(igtl::Socket * socket, igtl::MessageHeader * header);
		std::string ReceiveToolTransform(igtl::Socket * socket, igtl::MessageHeader * header);

#if OpenIGTLink_PROTOCOL_VERSION >= 2
		int SendString(igtl::Socket * socket, std::string Str);
		std::string ReceiveString(igtl::Socket * socket, igtl::MessageHeader * header);
#endif //OpenIGTLink_PROTOCOL_VERSION >= 2

	private:

	};

}
#endif //__OptitrackClient_h
