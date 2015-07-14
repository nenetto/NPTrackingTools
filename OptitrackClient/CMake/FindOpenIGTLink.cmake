# Copyright (c) 2015, E. Marinetto emarinetto@hggm.es.
# All rights reserved.
#
###############################################################################
# Find OpenIGTLink
###############################################################################
#
#  OpenIGTLink_FOUND
#  OpenIGTLink_INCLUDE_DIR
#  OpenIGTLink_LIBRARY
#
###############################################################################
# Find library in the install folder
###############################################################################

find_path(OpenIGTLink_INCLUDE_DIR igtlWindows.h
    HINTS ${OpenIGTLink_DIR}/include/igtl
    PATH_SUFFIXES include
    )

find_library(OpenIGTLink_LIBRARY
    OpenIGTLink
    HINTS ${OpenIGTLink_DIR}/lib/igtl
    PATH_SUFFIXES lib64 lib
    )

include(FindPackageHandleStandardArgs)
FIND_PACKAGE_HANDLE_STANDARD_ARGS(OpenIGTLink DEFAULT_MSG OpenIGTLink_LIBRARY)


if(     OpenIGTLink_INCLUDE_DIR
    AND OpenIGTLink_LIBRARY)

    set(OpenIGTLink_FOUND ON)

endif()

mark_as_advanced(OpenIGTLink_LIBRARY OpenIGTLink_INCLUDE_DIR)

