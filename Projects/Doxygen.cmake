# Author: Ali Uneri
# Date: 2014-01-09

set(EP_OPTION_NAME "USE_${EP_NAME}")

cma_list(APPEND EP_REQUIRED_PROJECTS Git IF UNIX)

set(EP_VERSION 1.8.11)
if(WIN32)
  set(EP_URL "http://downloads.sourceforge.net/project/doxygen/rel-${EP_VERSION}/doxygen-${EP_VERSION}.windows.x64.bin.zip")
  set(EP_URL_MD5 c398249c497052278c7ada474d4c71de)
elseif(UNIX)
  set(EP_URL "git://github.com/doxygen/doxygen.git")
else()
  message(FATAL_ERROR "Platform is not supported.")
endif()

cma_end_definition()
# -----------------------------------------------------------------------------

if(WIN32)
  ExternalProject_Add(${EP_NAME}
    DEPENDS ${EP_REQUIRED_PROJECTS}
    # download
    URL ${EP_URL}
    URL_MD5 ${EP_URL_MD5}
    TIMEOUT 300
    # patch
    # update
    UPDATE_COMMAND ""
    # configure
    SOURCE_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-install/bin
    CONFIGURE_COMMAND ""
    # build
    BUILD_COMMAND ""
    # install
    INSTALL_COMMAND ""
    # test
    )
  set(DOXYGEN_EXECUTABLE "${PROJECT_BINARY_DIR}/${EP_NAME}-install/bin/doxygen" CACHE INTERNAL "")

elseif(UNIX)
  find_package(BISON REQUIRED)
  find_package(FLEX REQUIRED)

  string(REPLACE "." "_" GIT_TAG "${EP_VERSION}")
  set(GIT_TAG "Release_${GIT_TAG}")

  set(EP_CMAKE_ARGS
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS})

  if(APPLE)
    list(APPEND EP_CMAKE_ARGS
      -DICONV_INCLUDE_DIR:PATH=/usr/include
      -DICONV_LIBRARY:FILEPATH=/usr/lib/libiconv.dylib)
  endif()
 
  ExternalProject_Add(${EP_NAME}
    DEPENDS ${EP_REQUIRED_PROJECTS}
    # download
    GIT_REPOSITORY ${EP_URL}
    GIT_TAG ${GIT_TAG}
    # patch
    # update
    UPDATE_COMMAND ""
    # configure
    SOURCE_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}
    CMAKE_ARGS ${EP_CMAKE_ARGS}
    # build
    BINARY_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-build
    # install
    INSTALL_COMMAND ""
    # test
    )
  set(DOXYGEN_EXECUTABLE "${PROJECT_BINARY_DIR}/${EP_NAME}-build/bin/doxygen" CACHE INTERNAL "")

else()
  message(FATAL_ERROR "Platform is not supported.")
endif()

set(${PROJECT_NAME}_DOXYGEN_EXECUTABLE "${DOXYGEN_EXECUTABLE}" CACHE INTERNAL "")
