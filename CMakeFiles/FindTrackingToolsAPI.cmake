# Author: Eugenio Marinetto
# Date: 2015-01-16

# Find Libraries for TrackingTools API


# -----------------------------------------------------------------------------
if( NOT WIN32)

    set(EP_OPTION_NAME OFF)
    message( WARNING "Optitrack only have API dinamic link libraries for Windows Systems. Please complain to: http://forums.naturalpoint.com/" )
    set(TrackingToolsAPI_FOUND OFF)

else()

    # Find Tracking Tools installation
    find_path(TrackingToolsAPI_DIR TrackingTools.exe
          DOC  "Include directory of the Optitrack library."
          HINT "C:/Program Files/OptiTrack/Tracking Tools"
          )

    if(TrackingToolsAPI_DIR)
      #message("[OK] Optitrack Tracking Tools found @ ${TrackingToolsAPI_DIR}")
    else()
      message("[FAIL] Optitrack Tracking Tools NOT found, please set TrackingToolsAPI_DIR")
    endif()

    # Find Dll library
    find_path(TrackingToolsAPI_LIB_DIR NPTrackingToolsx64.dll
          DOC  "Dinamic Link Library of Tracking Tools API NPTrackingToolsx64.dll"
          HINT "${TrackingToolsAPI_DIR}/lib"
          )

    if(TrackingToolsAPI_LIB_DIR)
      #message("[OK] Optitrack Tracking Tools Library directory found @ ${TrackingToolsAPI_LIB_DIR}")
    else()
      message("[FAIL] Optitrack Tracking Tools Library directory NOT found, please set TrackingToolsAPI_LIB_DIR")
    endif()

    # Find Headers file
    find_path(TrackingToolsAPI_INC_DIR NPTrackingTools.h
          DOC  "Dinamic Link Library of Tracking Tools API NPTrackingTools.dll"
          HINT "${TrackingToolsAPI_DIR}/inc"
          )
    if(TrackingToolsAPI_INC_DIR)
      #message("[OK] Optitrack Tracking Tools Include directory found @ ${TrackingToolsAPI_INC_DIR}")
    else()
      message("[FAIL] Optitrack Tracking Tools Include directory NOT found, please set TrackingToolsAPI_INC_DIR")
    endif()

    set(TrackingToolsAPI_INC "${TrackingToolsAPI_INC_DIR}/NPTrackingTools.h")
    set(TrackingToolsAPI_INC2 "${TrackingToolsAPI_INC_DIR}/trackablesettings.h")

    # Load the proper library x64 or x32
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      find_file(TrackingToolsAPI_DLL NPTrackingToolsx64.dll ${TrackingToolsAPI_LIB_DIR})
      find_file(TrackingToolsAPI_LIB_FILE NPTrackingToolsx64.lib ${TrackingToolsAPI_LIB_DIR})
      set(TrackingToolsAPI_LIB "NPTrackingToolsx64.lib")

    else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      find_file(TrackingToolsAPI_DLL NPTrackingTools.dll ${TrackingToolsAPI_LIB_DIR})
      find_file(TrackingToolsAPI_LIB_FILE NPTrackingTools.lib ${TrackingToolsAPI_LIB_DIR})
      set(TrackingToolsAPI_LIB "NPTrackingTools.lib")

    endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

    if(TrackingToolsAPI_LIB_FILE AND TrackingToolsAPI_DLL)
      #message("[OK] Optitrack Tracking Tools Library found @ ${TrackingToolsAPI_LIB}")
    else()
      message("[FAIL] Optitrack Tracking Tools Library NOT found, please set TrackingToolsAPI_LIB")
      unset(TrackingToolsAPI_LIB CACHE)
    endif()

    # Find Headers file
    find_path(TrackingToolsAPI_EXE TrackingTools.exe
          DOC  "TrackingTools application"
          HINT "${TrackingToolsAPI_DIR}"
          )
    if(TrackingToolsAPI_EXE)
      #message("[OK] Optitrack Tracking Tools found @ ${TrackingToolsAPI_EXE}")
    else()
      message("[FAIL] Optitrack Tracking Tools NOT found, please set TrackingToolsAPI_EXE")
    endif()

    set(TrackingToolsAPI_EXE "${TrackingToolsAPI_DIR}/TrackingTools.exe")

    # Assume that if include and lib were found
    if(TrackingToolsAPI_DIR
     AND TrackingToolsAPI_LIB_DIR
     AND TrackingToolsAPI_INC_DIR
     AND TrackingToolsAPI_LIB
     AND TrackingToolsAPI_INC
     AND TrackingToolsAPI_DLL
     AND TrackingToolsAPI_INC2
     AND TrackingToolsAPI_EXE)

    set(TrackingToolsAPI_FOUND "YES")
    set(TrackingToolsAPI_INCLUDE_DIRS ${TrackingToolsAPI_INC_DIR} CACHE PATH "")
    set(TrackingToolsAPI_SOURCE_DIRS "" CACHE PATH "")
    set(TrackingToolsAPI_BINARY_DIRS "" CACHE PATH "")
    set(TrackingToolsAPI_EXECUTABLES ${TrackingToolsAPI_EXE} CACHE FILEPATH "")
    set(TrackingToolsAPI_LIBRARIES ${TrackingToolsAPI_LIB} CACHE STRING "")
    set(TrackingToolsAPI_SHARED ON)
    set(TrackingToolsAPI_DEFINITIONS "" CACHE sTRING "")

    unset(TrackingToolsAPI_DIR CACHE)
    unset(TrackingToolsAPI_LIB_DIR CACHE)
    unset(TrackingToolsAPI_INC_DIR CACHE)
    unset(TrackingToolsAPI_LIB CACHE)
    unset(TrackingToolsAPI_INC CACHE)
    unset(TrackingToolsAPI_DLL CACHE)
    unset(TrackingToolsAPI_INC2 CACHE)
    unset(TrackingToolsAPI_EXE CACHE)
    unset(TrackingToolsAPI_LIB_FILE CACHE)


      mark_as_advanced( FORCE
        TrackingToolsAPI_FOUND
        TrackingToolsAPI_INCLUDE_DIRS
        TrackingToolsAPI_SOURCE_DIRS
        TrackingToolsAPI_BINARY_DIRS
        TrackingToolsAPI_EXECUTABLES
        TrackingToolsAPI_LIBRARIES
        TrackingToolsAPI_SHARED
        TrackingToolsAPI_DEFINITIONS
      )


    endif()


endif()

