// A simple program that computes the square root of a number
// A simple program that computes the square root of a number
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "NPTrackingTools.h"

int main (int argc, char *argv[])
{
  fprintf(stdout, "[TestOptitrack]: Calling TT_Initialize\n");

  NPRESULT result = TT_Initialize();
  //int result = 0;

  fprintf(stdout, "[TestOptitrack]: End of TT_Initialize, result %i \n",result);
  return 0;
}