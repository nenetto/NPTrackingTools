
set(EP_OPTION_NAME "USE_${EP_NAME}")
set(EP_REQUIRED_PROJECTS)
set(EP_OPTION_DESCRIPTION "NPTrackingTools")
set(EP_OPTION_DEFAULT ON)

cma_end_definition()
# -----------------------------------------------------------------------------

# Options pop up
cmake_dependent_option(${EP_NAME}_USE_NPTrackingTools_FAKE "Use Fake Optitrack API" ON ${EP_OPTION_NAME} ON)


set(EP_CMAKE_ARGS
  -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
  -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
  -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
  -DUSE_NPTrackingTools_FAKE:BOOL=${${EP_NAME}_USE_NPTrackingTools_FAKE}
  )

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
  INSTALL_DIR ${DESTDIR}
  # test
  )

set(TestOptitrack_${EP_NAME}_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}-build" CACHE INTERNAL "")