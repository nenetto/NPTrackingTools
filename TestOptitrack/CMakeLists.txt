cmake_minimum_required(VERSION 2.8.7)

# Project for the executable
project(TestOptitrack C CXX)
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/../install) ## Where to install the lib

# Add cpp files that form the library
file(GLOB ${PROJECT_NAME}_SRC_FILES source/*.cpp)
include_directories(include)

# Create executable
add_executable(${PROJECT_NAME} ${${PROJECT_NAME}_SRC_FILES})

###############################
# Search for NPTrackingTools ##
###############################
find_package ( NPTrackingTools )
target_link_libraries(${PROJECT_NAME} NPTrackingTools)



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