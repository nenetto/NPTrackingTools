# Author: Ali Uneri
# Date: 2014-01-09

set(EP_OPTION_NAME "USE_${EP_NAME}")

cma_list(APPEND EP_REQUIRED_PROJECTS Git IF UNIX)

if(WIN32)
  set(EP_URL "http://ftp.stack.nl/pub/users/dimitri/doxygen-1.8.8.windows.x64.bin.zip")
  set(EP_URL_MD5 da44e0ea9124ea6a3ec9015155aac81c)
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
elseif(UNIX)
  find_package(BISON REQUIRED)
  find_package(FLEX REQUIRED)

  ExternalProject_Add(${EP_NAME}
    DEPENDS ${EP_REQUIRED_PROJECTS}
    # download
    GIT_REPOSITORY ${EP_URL}
    GIT_TAG Release_1_8_8
    # patch
    # update
    UPDATE_COMMAND ""
    # configure
    SOURCE_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}
    CONFIGURE_COMMAND <SOURCE_DIR>/configure --prefix <INSTALL_DIR>
    # build
    BINARY_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}
    # install
    INSTALL_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-install
    # test
    )
else()
  message(FATAL_ERROR "Platform is not supported.")
endif()

set(DOXYGEN_EXECUTABLE "${PROJECT_BINARY_DIR}/${EP_NAME}-install/bin/doxygen" CACHE INTERNAL "")
set(${PROJECT_NAME}_DOXYGEN_EXECUTABLE "${DOXYGEN_EXECUTABLE}" CACHE INTERNAL "")
