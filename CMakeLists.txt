
# Minimum version of CMake required
cmake_minimum_required (VERSION 2.8.11)
message("Adding minimum required CMake version 2.8.11")

# Local Generator Name for project
set(MYPROJECT_NAME "NPTrackingTools")
project(${MYPROJECT_NAME})

# Version Control of the project
set (${MYPROJECT_NAME}_VERSION_MAJOR 0)
set (${MYPROJECT_NAME}_VERSION_MINOR 0)
message("Version: ${${MYPROJECT_NAME}_VERSION_MAJOR}.${${MYPROJECT_NAME}_VERSION_MINOR}")


# Find TrackingTools API (if exist)
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMakeFiles")
find_package (TrackingToolsAPI)


# Add option for the use of the library


if(TrackingToolsAPI_FOUND)
    set (${MYPROJECT_NAME}_USE_TTFAKE OFF CACHE BOOL "Use TrackingTools Fake")
else()
    set (${MYPROJECT_NAME}_USE_TTFAKE ON CACHE BOOL "Use TrackingTools Fake")
    message("TrackingToolsAPI is not installed in the system. Using Fake Version instead")
endif()


# If there is a API installed, add the project and copy the files
if(${MYPROJECT_NAME}_USE_TTFAKE)
    message(" Use Fake TrackingTools API instead")

    # Include directories
    include_directories("${PROJECT_SOURCE_DIR}/src/")
    # Create the library
    set(PROJECT_SOURCE_FILES "${PROJECT_SOURCE_DIR}/src/${MYPROJECT_NAME}.cpp")

    # Load the proper library x64 or x32
    if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set(LIBRARY_NAME "${MYPROJECT_NAME}x64")
    else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
      set(LIBRARY_NAME "${MYPROJECT_NAME}")
    endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

    # Now we use the SHARED word to tell cmake that it is a DLL
    add_library(${LIBRARY_NAME} SHARED ${PROJECT_SOURCE_FILES})

    set(${MYPROJECT_NAME}_DLL "${PROJECT_BINARY_DIR}/binary/${LIBRARY_NAME}.dll" CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_INC "${PROJECT_BINARY_DIR}/include/${MYPROJECT_NAME}.h" CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_INC2 "${PROJECT_BINARY_DIR}/include/trackablesettings.h" CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_DIR ${PROJECT_BINARY_DIR} CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_LIB_DIR "${PROJECT_BINARY_DIR}/binary" CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_INC_DIR "${PROJECT_BINARY_DIR}/include" CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_LIB "${PROJECT_BINARY_DIR}/binary/${LIBRARY_NAME}.lib" CACHE PATH "" FORCE PARENT_SCOPE)

    set( ${MYPROJECT_NAME}_INCLUDE_DIRS  ${${MYPROJECT_NAME}_INC_DIR} CACHE PATH "" FORCE PARENT_SCOPE)
    set( ${MYPROJECT_NAME}_LIBRARIES ${${MYPROJECT_NAME}_LIB} CACHE PATH "" FORCE PARENT_SCOPE)

    # Copy the .h and dll files and create the project NPTrackingTools
    add_custom_command(  TARGET ${LIBRARY_NAME} POST_BUILD
                        COMMAND ${CMAKE_COMMAND} -E make_directory  "${PROJECT_BINARY_DIR}/include"
                        COMMAND ${CMAKE_COMMAND} -E make_directory  "${PROJECT_BINARY_DIR}/binary"
                        COMMAND ${CMAKE_COMMAND} -E copy            "${PROJECT_SOURCE_DIR}/src/NPTrackingTools.h"    "${PROJECT_BINARY_DIR}/include"
                        COMMAND ${CMAKE_COMMAND} -E copy            "${PROJECT_SOURCE_DIR}/src/trackablesettings.h"    "${PROJECT_BINARY_DIR}/include"
                        COMMAND ${CMAKE_COMMAND} -E copy            "${PROJECT_BINARY_DIR}/$<CONFIGURATION>/${LIBRARY_NAME}.dll"    "${PROJECT_BINARY_DIR}/binary"
                        COMMAND ${CMAKE_COMMAND} -E copy            "${PROJECT_BINARY_DIR}/$<CONFIGURATION>/${LIBRARY_NAME}.lib"    "${PROJECT_BINARY_DIR}/binary")


else()

    # Copy the .h and dll files and create the project NPTrackingTools
    add_custom_target(  ${MYPROJECT_NAME} ALL
                        COMMAND ${CMAKE_COMMAND} -E make_directory  "${PROJECT_BINARY_DIR}/include"
                        COMMAND ${CMAKE_COMMAND} -E make_directory  "${PROJECT_BINARY_DIR}/binary"
                        COMMAND ${CMAKE_COMMAND} -E copy            ${TrackingToolsAPI_INC}    "${PROJECT_BINARY_DIR}/include"
                        COMMAND ${CMAKE_COMMAND} -E copy            ${TrackingToolsAPI_INC2}    "${PROJECT_BINARY_DIR}/include"
                        COMMAND ${CMAKE_COMMAND} -E copy            ${TrackingToolsAPI_DLL}    "${PROJECT_BINARY_DIR}/binary"
                        COMMAND ${CMAKE_COMMAND} -E copy            ${TrackingToolsAPI_LIB}    "${PROJECT_BINARY_DIR}/binary")


    set(${MYPROJECT_NAME}_DLL ${TrackingToolsAPI_DLL} CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_INC ${TrackingToolsAPI_INC} CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_INC2 ${TrackingToolsAPI_INC2} CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_DIR ${TrackingToolsAPI_DIR} CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_LIB_DIR ${TrackingToolsAPI_LIB_DIR} CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_INC_DIR ${TrackingToolsAPI_INC_DIR} CACHE PATH "" FORCE PARENT_SCOPE)
    set(${MYPROJECT_NAME}_LIB ${TrackingToolsAPI_LIB} CACHE PATH "" FORCE PARENT_SCOPE)

    set( ${MYPROJECT_NAME}_INCLUDE_DIRS  ${TrackingToolsAPI_INCLUDE_DIRS} CACHE PATH "" FORCE PARENT_SCOPE)
    set( ${MYPROJECT_NAME}_LIBRARIES ${TrackingToolsAPI_LIBRARIES} CACHE PATH "" FORCE PARENT_SCOPE)

endif()


