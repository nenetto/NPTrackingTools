#ifndef OPTITRACK_HELPERS_H
#define OPTITRACK_HELPERS_H

// Configuration files
#include "BiiGOptitrackControlConfig.h"
#include "OptitrackTracker.h"
#include "OptitrackTool.h"

// ITK
#include <vnl/vnl_quaternion.h>
#include <vnl/vnl_vector_fixed.h>
#include <vnl/vnl_matrix.h>




namespace Optitrack
{
    class BiiGOptitrackControl_EXPORT  OptitrackHelper
    {
        public:

            static void ConvertMatrix(vnl_matrix<double> &R, vnl_vector_fixed<double,3> position, vnl_quaternion<double> orientation);

            static vnl_vector_fixed<double,3> Pivoting(OptitrackTracker::Pointer tracker, unsigned int optitrackID, unsigned int sampleNumber);


    };
}



#endif