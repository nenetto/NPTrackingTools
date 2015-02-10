# Minimum version of CMake required
cmake_minimum_required (VERSION 2.8.11)

# Create option for fake API use
option(USE_NPTrackingTools_FAKE OFF)

if(USE_NPTrackingTools_FAKE)
  add_subdirectory(NPTrackingToolsFake NPTrackingTools)

else()
  message("=======================================")
  message(" Using REAL NPTrackingTools API        ")
  message("=======================================")

  message(" Attempt to find NPTrackingTools in the system...")

  # Find TrackingTools API (if exist)
  include("${PROJECT_SOURCE_DIR}/CMakeFiles/FindTrackingToolsAPI.cmake")

  if(TrackingToolsAPI_FOUND)
    message(" REAL NPTrackingTools API  FOUND       ")
    message(" Generating target and CMake Configuration File")

    # Generate the NPTrackingTools target
    set(PROJECT_NAME NPTrackingTools)

    # Add a configuration file
    set(INCLUDE_DIR_TEMPLATE  ${TrackingToolsAPI_INCLUDE_DIRS})
    set(SOURCE_DIR_TEMPLATE   ${TrackingToolsAPI_SOURCE_DIRS})
    set(EXECUTABLES_TEMPLATE  ${TrackingToolsAPI_EXECUTABLES})
    set(LIBRARY_TEMPLATE      ${TrackingToolsAPI_LIBRARIES})
    set(BINARY_DIR_TEMPLATE   ${TrackingToolsAPI_BINARY_DIRS})
    set(SHARED_TEMPLATE       ${TrackingToolsAPI_SHARED})
    set(DEFINITIONS_TEMPLATE  ${TrackingToolsAPI_DEFINITIONS})

    # Check if x64
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set(bits "x64")
    else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set(bits "")
    endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

    configure_file (
      "${PROJECT_SOURCE_DIR}/CMakeFiles/TemplateConfigFile.cmake.in"
      "${PROJECT_BINARY_DIR}/${PROJECT_NAME}${bits}Config.cmake"
      )


  else()
    message(WARNING "NPTrackingTools NOT FOUND, using fake NPTrackingTools...")
    message(WARNING "                setting USE_NPTrackingTools_FAKE to ON..")
    set(USE_NPTrackingTools_FAKE ON FORCE)
    add_subdirectory(NPTrackingToolsFake)
  endif()




endif()