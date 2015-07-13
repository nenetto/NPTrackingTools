#include "OptitrackServer.h"
#include "igtlSocket.h"

int main( int argc, char *argv[] )
{
  try 
  {
    Optitrack::OptitrackServer *server = new Optitrack::OptitrackServer();
	
	server->Connections();
	server->~OptitrackServer();
    return 0;
  }
  catch( std::exception &e )
  {
    std::cerr<<e.what()<<"\n";
    return 1;
  }
}
