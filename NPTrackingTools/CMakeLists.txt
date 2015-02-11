# Minimum version of CMake required
cmake_minimum_required (VERSION 2.8.11)

project(NPTrackingTools C CXX)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/../install) ## Where to install the lib

# Create option for fake API use
option(USE_NPTrackingTools_FAKE OFF)

if(USE_NPTrackingTools_FAKE)
  include(NPTrackingToolsFakeConfiguration.cmake)

else()
  message("=======================================")
  message(" Using REAL NPTrackingTools API        ")
  message("=======================================")

  message(" Attempt to find NPTrackingTools in the system...")

  # Find TrackingTools API (if exist)
  include("${PROJECT_SOURCE_DIR}/CMake/FindTrackingToolsAPI.cmake")

  if(TrackingToolsAPI_FOUND)
    message(" REAL NPTrackingTools API  FOUND       ")
    message(" Generating target and CMake Configuration File")

    # Add a configuration file
    set(INCLUDE_DIR_TEMPLATE  ${TrackingToolsAPI_INCLUDE_DIRS})
    set(SOURCE_DIR_TEMPLATE   ${TrackingToolsAPI_SOURCE_DIRS})
    set(EXECUTABLES_TEMPLATE  ${TrackingToolsAPI_EXECUTABLES})
    set(LIBRARY_TEMPLATE      ${TrackingToolsAPI_LIBRARIES})
    set(BINARY_DIR_TEMPLATE   ${TrackingToolsAPI_BINARY_DIRS})
    set(SHARED_TEMPLATE       ${TrackingToolsAPI_SHARED})
    set(DEFINITIONS_TEMPLATE  ${TrackingToolsAPI_DEFINITIONS})


    # Create installation and export targets for real NPTrackingTools
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}Config.cmake.in"
      "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
      @ONLY
      FORCE
      )

    # Installation
    #set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR}/install) ## Where to install the lib

    # install dll, lib and headers
    install(FILES "${TrackingToolsAPI_LIB_DIR}/${TrackingToolsAPI_LIBRARIES}.lib" DESTINATION lib)
    install(FILES "${TrackingToolsAPI_LIB_DIR}/${TrackingToolsAPI_LIBRARIES}.dll" DESTINATION bin)

    file(GLOB ${PROJECT_NAME}_HDR_FILES ${TrackingToolsAPI_INCLUDE_DIRS}/*.h)
    install(FILES ${${PROJECT_NAME}_HDR_FILES} DESTINATION inc)

    # install the configure files
    # Create installation and export targets for real NPTrackingTools
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-debug.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-debug.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-minsizerel.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-minsizerel.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-release.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-release.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets-relwithdebinfo.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-relwithdebinfo.cmake"
      @ONLY
      FORCE
      )
    configure_file (
      "${PROJECT_SOURCE_DIR}/CMake/${PROJECT_NAME}/${PROJECT_NAME}-targets.cmake.in"
      "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets.cmake"
      @ONLY
      FORCE
      )

  else()
    message(WARNING "NPTrackingTools NOT FOUND, using fake NPTrackingTools...")
    message(WARNING "                setting USE_NPTrackingTools_FAKE to ON..")
    set(USE_NPTrackingTools_FAKE ON FORCE)
    include(NPTrackingToolsFakeConfiguration.cmake)
  endif()

endif()


