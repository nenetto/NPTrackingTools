# Author: Eugenio Marinetto
# Date: 2012-10-28

set(EP_OPTION_NAME "USE_${EP_NAME}")
set(EP_REQUIRED_PROJECTS BiiGOptitrackControl NPTrackingTools tinyxml2 ITK)
set(EP_OPTION_DESCRIPTION "Test Optitrack")
set(EP_OPTION_DEFAULT ON)


cma_end_definition()
# -----------------------------------------------------------------------------


set(EP_CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
  -DCMAKE_INSTALL_PREFIX:PATH=${${PROJECT_NAME}_INSTALL_DIR}
  -DTestOptitrack_BUILD_DOCUMENTATION:BOOL=${${PROJECT_NAME}_BUILD_DOCUMENTATION}
  -DNPTrackingTools_DIR:PATH=${${PROJECT_NAME}_NPTrackingTools_DIR}
  -DBiiGOptitrackControl_DIR:PATH=${${PROJECT_NAME}_BiiGOptitrackControl_DIR}
  -Dtinyxml2_DIR:PATH=${${PROJECT_NAME}_tinyxml2_DIR}
  -DITK_DIR:PATH=${${PROJECT_NAME}_ITK_DIR}
  )

if(${${PROJECT_NAME}_BUILD_DOCUMENTATION})
  list(APPEND EP_CMAKE_ARGS
    -DBUILD_DOCUMENTATION:BOOL=ON
    -DDOXYGEN_EXECUTABLE:PATH=${${PROJECT_NAME}_DOXYGEN_EXECUTABLE}
    )
endif()


ExternalProject_Add(${EP_NAME}
  DEPENDS ${EP_REQUIRED_PROJECTS}
  # download
  # patch
  # update
  UPDATE_COMMAND ""
  # configure
  SOURCE_DIR ${PROJECT_SOURCE_DIR}/${EP_NAME}
  CMAKE_ARGS ${EP_CMAKE_ARGS}
  # build
  BINARY_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-build
  # install
  INSTALL_COMMAND ""
  # test
  )

set(${PROJECT_NAME}_${EP_NAME}_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}-build" CACHE INTERNAL "")
