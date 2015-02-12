cmake_minimum_required(VERSION 2.8.10)

message("=======================================")
message(" Using NPTrackingTools fake API        ")
message("=======================================")

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

# Create NPTrackingTools library
add_library(${PROJECT_NAME} SHARED ${${PROJECT_NAME}_SRC_FILES})

# Setting the public headers of the library
file(GLOB ${PROJECT_NAME}_SOURCE_HDR_FILES ${PROJECT_SOURCE_DIR}/include/*.h)
file(GLOB ${PROJECT_NAME}_CMAKE_HDR_FILES ${PROJECT_BINARY_DIR}/CmakeFilesHeader/*.h)
list(APPEND ${PROJECT_NAME}_HDR_FILES ${${PROJECT_NAME}_SOURCE_HDR_FILES} ${${PROJECT_NAME}_CMAKE_HDR_FILES})

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
  $<INSTALL_INTERFACE:${CMAKE_INSTALL_PREFIX}/inc>
)

export(TARGETS ${PROJECT_NAME} FILE ${PROJECT_NAME}Config.cmake)

# Add to the search path of find_package
export(PACKAGE ${PROJECT_NAME})







