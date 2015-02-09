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

    # Find Header file
    find_path(TrackingToolsAPI_INC_DIR NPTrackingTools.h
          DOC  "Dinamic Link Library of Tracking Tools API NPTrackingTools.dll"
          HINT "${TrackingToolsAPI_DIR}/inc"
          )
    if(TrackingToolsAPI_INC_DIR)
      #message("[OK] Optitrack Tracking Tools Include directory found @ ${TrackingToolsAPI_INC_DIR}")
    else()
      message("[FAIL] Optitrack Tracking Tools Include directory NOT found, please set TrackingToolsAPI_INC_DIR")
    endif()

    set(TrackingToolsAPI_INC "${TrackingToolsAPI_INC_DIR}/NPTrackingTools.h" CACHE PATH "" FORCE)
    set(TrackingToolsAPI_INC2 "${TrackingToolsAPI_INC_DIR}/trackablesettings.h" CACHE PATH "" FORCE)

    # Load the proper library x64 or x32
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( TrackingToolsAPI_DLL "${TrackingToolsAPI_LIB_DIR}/NPTrackingToolsx64.dll" )
      find_library( TrackingToolsAPI_LIB
      NAMES  NPTrackingToolsx64 gmodule12
      PATHS  ${TrackingToolsAPI_LIB_DIR}
      )
    else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( TrackingToolsAPI_DLL "${TrackingToolsAPI_LIB_DIR}/NPTrackingTools.dll" )
      find_library( TrackingToolsAPI_LIB
      NAMES  NPTrackingTools gmodule12
      PATHS  ${TrackingToolsAPI_LIB_DIR}
      )
    endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set( CMAKE_EXE_LINKER_FLAGS ${TrackingToolsAPI_LIB_DIR} CACHE PATH "" FORCE)

    if(TrackingToolsAPI_LIB)
      #message("[OK] Optitrack Tracking Tools Library found @ ${TrackingToolsAPI_LIB}")
    else()
      message("[FAIL] Optitrack Tracking Tools Library found NOT found, please set TrackingToolsAPI_LIB")
    endif()

    # Assume that if include and lib were found
    if(TrackingToolsAPI_DIR
     AND TrackingToolsAPI_LIB_DIR
     AND TrackingToolsAPI_INC_DIR
     AND TrackingToolsAPI_LIB
     AND TrackingToolsAPI_INC
     AND TrackingToolsAPI_DLL
     AND TrackingToolsAPI_INC2)


      set(TrackingToolsAPI_FOUND "YES" CACHE PATH "" FORCE)

      set( TrackingToolsAPI_INCLUDE_DIRS  ${TrackingToolsAPI_INC_DIR} CACHE PATH "" FORCE)
      set( TrackingToolsAPI_LIBRARIES ${TrackingToolsAPI_LIB} CACHE PATH "" FORCE)

      mark_as_advanced(
        TrackingToolsAPI_DIR
        TrackingToolsAPI_INC_DIR
        TrackingToolsAPI_INC
        TrackingToolsAPI_LIB_DIR
        TrackingToolsAPI_LIB
        TrackingToolsAPI_DLL
        TrackingToolsAPI_INC2
      )

    endif()


endif()

