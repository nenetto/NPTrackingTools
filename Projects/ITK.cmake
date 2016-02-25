# Author: Ali Uneri
# Date: 2012-10-28

set(EP_OPTION_NAME "USE_${EP_NAME}")
set(EP_REQUIRED_PROJECTS Git)
set(EP_PATCH "${CMAKE_CURRENT_LIST_DIR}/Patches/${EP_NAME}.patch")
set(EP_OPTION_DESCRIPTION "Insight Segmentation and Registration Toolkit")
set(EP_URL "git://itk.org/ITK.git")
set(EP_VERSION "v4.9.0")

cma_envvar(@LIBRARYPATH@ PREPEND "@BINARY_DIR@/@LIBDIR@/@INTDIR@")

cma_end_definition()
# -----------------------------------------------------------------------------


if(${PROJECT_NAME}_ITK_EXTERNAL_DIR)

  ExternalProject_Add(${EP_NAME}
       DOWNLOAD_COMMAND ""
       CONFIGURE_COMMAND ""
       BUILD_COMMAND ""
       INSTALL_COMMAND "")

  set(${PROJECT_NAME}_${EP_NAME}_DIR ${${PROJECT_NAME}_ITK_EXTERNAL_DIR} CACHE INTERNAL "")

else()

  if(WIN32)
    string(LENGTH "${PROJECT_BINARY_DIR}/${EP_NAME}-build" LENGTH)
    if(LENGTH GREATER 50)
      message(FATAL_ERROR "Shorter path for ${PROJECT_NAME} build directory is required, since ITK path is ${LENGTH} > 50 characters")
    endif()
  endif()

  set(EP_CMAKE_ARGS
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DITKV3_COMPATIBILITY:BOOL=OFF
    -DModule_ITKReview:BOOL=ON
    -DModule_ITKVideoBridgeOpenCV:BOOL=OFF)


  ExternalProject_Add(${EP_NAME}
    DEPENDS ${EP_REQUIRED_PROJECTS}
    # download
    GIT_REPOSITORY ${EP_URL}
    GIT_TAG ${EP_VERSION}
    # patch
    # update
    UPDATE_COMMAND ""
    # configure
    SOURCE_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}
    CMAKE_ARGS ${EP_CMAKE_ARGS}
    # build
    BINARY_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-b
    # install
    INSTALL_COMMAND ""
    # test
    )

  set(${PROJECT_NAME}_${EP_NAME}_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}-b" CACHE INTERNAL "")

endif()



