cmake_minimum_required(VERSION 3.0.2)

# Project for the executable
project(CalibrationValidation C CXX)

# Add cpp files that form the library
file(GLOB ${PROJECT_NAME}_SRC_FILES source/*.cpp)
include_directories(include)

# Create executable
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC_FILES})

###############################
# Search for NPTrackingTools ##
###############################
option(NPTrackingTools_DIR "")
find_package (NPTrackingTools)
target_link_libraries(${PROJECT_NAME} NPTrackingTools)

####################################
# Search for BiiGOptitrackControl ##
####################################
option(BiiGOptitrackControl_DIR "")
find_package (BiiGOptitrackControl)
target_link_libraries(${PROJECT_NAME} BiiGOptitrackControl)

###############################
# Search for tinyxml2 ##
###############################
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")
option(tinyxml2_DIR "")
find_package (tinyxml2)
include_directories(${tinyxml2_INCLUDE_DIR})
message(tinyxml2_INCLUDE_DIR ${tinyxml2_INCLUDE_DIR})
target_link_libraries(${PROJECT_NAME} ${tinyxml2_LIBRARY})

###############################
# Search for ITK ##
###############################
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES})

# Documentation
option(${PROJECT_NAME}_BUILD_DOCUMENTATION "" OFF)

if(${PROJECT_NAME}_BUILD_DOCUMENTATION)
    find_package(Doxygen)
    option(${PROJECT_NAME}_BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if(${PROJECT_NAME}_BUILD_DOCUMENTATION)
        if(NOT DOXYGEN_FOUND)
            message(FATAL_ERROR "Doxygen is needed to build the documentation.")
        endif()

        if(EXISTS ${doxyfile})
          file(REMOVE ${doxyfile})
        endif()

        set(doxyfile_in ${CMAKE_CURRENT_SOURCE_DIR}/doc/Doxyfile.in)
        set(doxyfile ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)

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



##################################################################################################################
# INSTALLATION TARGET and Configuration Files
#################################################################################################################


#################################################################
# Config File if INSTALL target and installation
#################################################################

install(TARGETS ${PROJECT_NAME} EXPORT ${PROJECT_NAME}-targets
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        )
install(EXPORT ${PROJECT_NAME}-targets DESTINATION lib/${PROJECT_NAME})


#################################################################
# Config File that will be used if the package is not installed
#################################################################

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)
export(PACKAGE ${PROJECT_NAME})