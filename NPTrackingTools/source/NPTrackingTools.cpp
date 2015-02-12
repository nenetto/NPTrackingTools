
#include <stdio.h>
#include "Config.h"
#include "NPTrackingTools.h"

TTAPI   NPRESULT TT_Initialize()
{
	fprintf(stdout, "[NPTrackingTools Fake]: Initializing the System...\n");
	return NPRESULT_SUCCESS;
}