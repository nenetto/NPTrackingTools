# Minimum version of CMake required
cmake_minimum_required (VERSION 3.0.2)
set(CMAKE_MACOSX_RPATH 0)

project(Laubrary C CXX)


# Delete previous configured files
if(EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-debug.cmake")
  file(REMOVE "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-debug.cmake")
endif()

if(EXISTS "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
  file(REMOVE "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.cmake")
endif()

if(EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-minsizerel.cmake")
  file(REMOVE "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-minsizerel.cmake")
endif()

if(EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-release.cmake")
  file(REMOVE "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-release.cmake")
endif()

if(EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-relwithdebinfo.cmake")
  file(REMOVE "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets-relwithdebinfo.cmake")
endif()

if(EXISTS "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets.cmake")
  file(REMOVE "${CMAKE_INSTALL_PREFIX}/lib/${PROJECT_NAME}/${PROJECT_NAME}-targets.cmake")
endif()


# Add cpp files that form the library
file(GLOB ${PROJECT_NAME}_SRC_FILES source/*.cpp)

# Create  library
option(Laubrary_BUILD_SHARED ON)

if(Laubrary_BUILD_SHARED)
  add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRC_FILES})
else()
  add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRC_FILES})
endif()

# Include the Generator for the export Header in Windows.
# It contains functions for helping in the exports preprocessor commands
include (GenerateExportHeader)

# We generate the Exports header using the name of the library
generate_export_header(${PROJECT_NAME}
             BASE_NAME ${PROJECT_NAME}
             EXPORT_MACRO_NAME ${PROJECT_NAME}_EXPORT
             EXPORT_FILE_NAME ${PROJECT_BINARY_DIR}/CmakeFilesHeader/${PROJECT_NAME}_Export.h
             STATIC_DEFINE ${PROJECT_NAME}_BUILT_AS_STATIC
)

configure_file (
  "${PROJECT_SOURCE_DIR}/CMake/Config.h.in"
  "${PROJECT_BINARY_DIR}/CmakeFilesHeader/${PROJECT_NAME}Config.h"
  FORCE)

# Setting the public headers of the library
file(GLOB ${PROJECT_NAME}_SOURCE_HDR_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB ${PROJECT_NAME}_CMAKE_HDR_FILES ${PROJECT_BINARY_DIR}/CmakeFilesHeader/*.h)
list(APPEND ${PROJECT_NAME}_HDR_FILES ${${PROJECT_NAME}_SOURCE_HDR_FILES} ${${PROJECT_NAME}_CMAKE_HDR_FILES})

###############################
# Search for NPTrackingTools ##
###############################
option(NPTrackingTools_DIR "")
find_package (NPTrackingTools)
target_link_libraries(${PROJECT_NAME} NPTrackingTools)

<<<<<<< HEAD
=======

###############################
# Search for ITK ##
###############################
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
#target_link_libraries(${PROJECT_NAME} ITKCommon ITKIO ITKCore)
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES})

>>>>>>> 0c5d8962b782c26a505fec1762992a81b2162c85
# Headers (all are public)
include_directories(include)
include_directories(${PROJECT_BINARY_DIR}/CmakeFilesHeader)

set_target_properties(${PROJECT_NAME} PROPERTIES PUBLIC_HEADER "${${PROJECT_NAME}_HDR_FILES}")

##################################################################################################################
# INSTALLATION TARGET and Configuration Files
#################################################################################################################


install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        PUBLIC_HEADER DESTINATION inc
        )
install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/${PROJECT_NAME})

#################################################################
# Config File that will be used if the package is not installed
#################################################################

# Includes of the library for exporting purposes
target_include_directories(${PROJECT_NAME} PUBLIC
  $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/CmakeFilesHeader>
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/inc>
)

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)

# Add to the search path of find_package
export(PACKAGE ${PROJECT_NAME})


# Documentation
option(Laubrary_BUILD_DOCUMENTATION "" OFF)

if(Laubrary_BUILD_DOCUMENTATION)
    find_package(Doxygen)
    option(Laubrary_BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if(Laubrary_BUILD_DOCUMENTATION)
        if(NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to build the documentation.")
        endif()

        set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
        set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

        if(EXISTS ${doxyfile})
          file(REMOVE ${doxyfile})
        endif()
        configure_file(${doxyfile_in} ${doxyfile} @ONLY)

        add_custom_command(
            TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND ${DOXYGEN_EXECUTABLE} ${doxyfile}
            WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)

        install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/html DESTINATION doc/${PROJECT_NAME})
    endif()
endif()







