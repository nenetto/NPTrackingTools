# Copyright (c) 2015, E. Marinetto emarinetto@hggm.es.
# All rights reserved.
#
#
###############################################################################
# Find TINYXML2
###############################################################################
#
#  TINYXML2_FOUND
#  TINYXML2_INCLUDE_DIR
#  TINYXML2_LIBRARY
#
###############################################################################

find_path(tinyxml2_INCLUDE_DIR tinyxml2.h
	HINTS ${tinyxml2_DIR}
	PATH_SUFFIXES include
	)

find_library(tinyxml2_LIBRARY
	tinyxml2
	HINTS ${tinyxml2_DIR}
	PATH_SUFFIXES lib64 lib
	)

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(tinyxml2 DEFAULT_MSG tinyxml2_LIBRARY)


if(		tinyxml2_INCLUDE_DIR
	AND tinyxml2_LIBRARY)

	set(tinyxml2_FOUND ON)

endif()

mark_as_advanced(tinyxml2_LIBRARY tinyxml2_INCLUDE_DIR)
