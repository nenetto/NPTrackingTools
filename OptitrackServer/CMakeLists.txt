cmake_minimum_required(VERSION 3.0.2)

# Project for the executable
project(OptitrackServer C CXX)

# Add cpp files that form the library
file(GLOB ${PROJECT_NAME}_SRC_FILES source/*.cpp)
include_directories(include)

# Create executable
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC_FILES})

####################################
# Search for BiiGOptitrackControl ##
####################################
option(BiiGOptitrackControl_DIR "")
find_package (BiiGOptitrackControl)
target_link_libraries(${PROJECT_NAME} PRIVATE BiiGOptitrackControl)

###############################
# Search for ITK             ##
###############################
find_package(ITK REQUIRED)
include(${ITK_USE_FILE})
target_link_libraries(${PROJECT_NAME} PUBLIC ${ITK_LIBRARIES})

###############################
# Search for OpenIGTLink     ##
###############################
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")
find_package (OpenIGTLink REQUIRED)
include_directories(${OpenIGTLink_INCLUDE_DIRS})
link_directories(${OpenIGTLink_LIBRARY_DIRS})
target_link_libraries(${PROJECT_NAME} PUBLIC optimized ${OpenIGTLink_LIBRARY_DIRS}/Release/OpenIGTLink.lib PUBLIC debug ${OpenIGTLink_LIBRARY_DIRS}/Debug/OpenIGTLink.lib)



# Documentation
option(OptitrackServer_BUILD_DOCUMENTATION "" OFF)

if(OptitrackServer_BUILD_DOCUMENTATION)
    find_package(Doxygen)
    option(OptitrackServer_BUILD_DOCUMENTATION "Create and install the HTML based API documentation (requires Doxygen)" ${DOXYGEN_FOUND})

    if(OptitrackServer_BUILD_DOCUMENTATION)
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