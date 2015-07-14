#include "OptitrackClient.h"
#include "igtlSocket.h"

int main( int argc, char *argv[] )
{
  try 
  {
    Optitrack::OptitrackClient *client = new Optitrack::OptitrackClient();

    return 0;
  }
  catch( std::exception &e )
  {
    std::cerr<<e.what()<<"\n";
    return 1;
  }
}
