cmake_minimum_required(VERSION 2.8.10)

message("=======================================")
message(" Using NPTrackingTools fake API        ")
message("=======================================")


project(NPTrackingTools C CXX)

# Add cpp files that form the library
file(GLOB ${PROJECT_NAME}_SRC_FILES source/*.cpp)
include_directories(include)


# Create NPTrackingTools library
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRC_FILES})

# Add a configuration file
set(INCLUDE_DIR_TEMPLATE    ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_DIR_TEMPLATE     ${PROJECT_SOURCE_DIR})
set(EXECUTABLES_TEMPLATE    "")
set(LIBRARY_TEMPLATE        "${PROJECT_NAME}.lib")
set(BINARY_DIR_TEMPLATE     ${PROJECT_BINARY_DIR})
set(SHARED_TEMPLATE         ON)
set(DEFINITIONS_TEMPLATE    "")


set(TrackingToolsAPI_INCLUDE_DIRS ${INCLUDE_DIR_TEMPLATE} CACHE PATH "" FORCE)
set(TrackingToolsAPI_SOURCE_DIRS ${SOURCE_DIR_TEMPLATE} CACHE PATH "" FORCE)
set(TrackingToolsAPI_BINARY_DIRS ${BINARY_DIR_TEMPLATE} CACHE PATH "" FORCE)
set(TrackingToolsAPI_EXECUTABLES ${EXECUTABLES_TEMPLATE} CACHE FILEPATH "" FORCE)
set(TrackingToolsAPI_LIBRARIES ${LIBRARY_TEMPLATE} CACHE STRING "" FORCE)
set(TrackingToolsAPI_SHARED ON FORCE)
set(TrackingToolsAPI_DEFINITIONS "" FORCE)



get_filename_component(PARENT_SRC_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_BIN_DIR ${PROJECT_BINARY_DIR} DIRECTORY)

configure_file (
  "${PARENT_SRC_DIR}/CMakeFiles/TemplateConfigFile.cmake.in"
  "${PARENT_BIN_DIR}/${PROJECT_NAME}Config.cmake"
  )




