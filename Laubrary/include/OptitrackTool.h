#ifndef OPTITRACK_TOOL_H
#define OPTITRACK_TOOL_H

// Config from CMake
#include "LaubraryConfig.h"


// NPTrackingTools library
#include "NPTrackingTools.h"

// OptitrackTool
#include "OptitrackTracker.h"

// Extra std libs
#include <iostream>
#include <stdio.h>
#include <cstdlib>
#include <fstream>
#include <sstream>




namespace Optitrack{

    class Laubrary_EXPORT OptitrackTool{
    public:
        int test;

        friend class OptitrackTracker;

    };

}

#endif