cmake_minimum_required(VERSION 2.8.10)

message("=======================================")
message(" Using NPTrackingTools fake API        ")
message("=======================================")


project(NPTrackingTools C CXX)

# Add cpp files that form the library
file(GLOB ${PROJECT_NAME}_SRC_FILES source/*.cpp)
include_directories(include)



# Check if x64
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set(bits "x64")
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  set(bits "")
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

# Create NPTrackingTools library
add_library(${PROJECT_NAME}${bits} SHARED ${${PROJECT_NAME}_SRC_FILES})

# Add a configuration file
set(INCLUDE_DIR_TEMPLATE    ${PROJECT_SOURCE_DIR}/include)
set(SOURCE_DIR_TEMPLATE     ${PROJECT_SOURCE_DIR})
set(EXECUTABLES_TEMPLATE    "")
set(LIBRARY_TEMPLATE        "${PROJECT_NAME}${bits}")
set(BINARY_DIR_TEMPLATE     ${PROJECT_BINARY_DIR})
set(SHARED_TEMPLATE         ON)
set(DEFINITIONS_TEMPLATE    "")


get_filename_component(PARENT_SRC_DIR ${PROJECT_SOURCE_DIR} DIRECTORY)
get_filename_component(PARENT_BIN_DIR ${PROJECT_BINARY_DIR} DIRECTORY)

configure_file (
  "${PARENT_SRC_DIR}/CMakeFiles/TemplateConfigFile.cmake.in"
  "${PARENT_BIN_DIR}/${PROJECT_NAME}${bits}Config.cmake"
  )




