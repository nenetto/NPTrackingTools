# Author: Eugenio Marinetto
# Date: 2015-06-29

set(EP_OPTION_NAME "USE_${EP_NAME}")
set(EP_REQUIRED_PROJECTS Python Git)
set(EP_URL "https://github.com/nenetto/IGTLinkPython.git")
set(EP_OPTION_DESCRIPTION "Python IGTLink Client")
set(EP_OPTION_DEFAULT ON)

cma_envvar(PYTHONPATH PREPEND "@SOURCE_DIR@")

cma_end_definition()
# -----------------------------------------------------------------------------

set(EP_CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_INSTALL_PREFIX:PATH=${${PROJECT_NAME}_INSTALL_DIR}
  -D${EP_NAME}_BUILD_TESTING:BOOL=${${PROJECT_NAME}_BUILD_TESTING})

if(${PROJECT_NAME}_BUILD_TESTING)
  list(APPEND EP_CMAKE_ARGS -DPYTHON_EXECUTABLE:FILEPATH=${${PROJECT_NAME}_PYTHON_EXECUTABLE})
endif()

if(${EP_OPTION_NAME}_SOURCE_DIR)
  set(EP_DOWNLOAD
    DOWNLOAD_COMMAND ""
    SOURCE_DIR ${${EP_OPTION_NAME}_SOURCE_DIR})
else()
  set(EP_DOWNLOAD
    GIT_REPOSITORY ${EP_URL}
    GIT_TAG origin/master
    SOURCE_DIR ${PROJECT_BINARY_DIR}/${EP_NAME})
endif()

ExternalProject_Add(${EP_NAME}
  DEPENDS ${EP_REQUIRED_PROJECTS}
  # download
  "${EP_DOWNLOAD}"
  # patch
  # update
  UPDATE_COMMAND ""
  # configure
  CMAKE_ARGS ${EP_CMAKE_ARGS}
  # build
  BUILD_COMMAND ""
  # install
  INSTALL_DIR ${DESTDIR}
  # test
  )