# Author: Ali Uneri
# Date: 2012-10-28

  set(EP_OPTION_NAME "USE_${EP_NAME}")
  set(EP_REQUIRED_PROJECTS Git)
  set(EP_PATCH "${CMAKE_CURRENT_LIST_DIR}/${EP_NAME}.patch")
  set(EP_OPTION_DESCRIPTION "Insight Segmentation and Registration Toolkit")
  set(EP_URL "git://itk.org/ITK.git")
  set(EP_VERSION "v4.6.1")


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

  set(EP_CMAKE_ARGS
    -DBUILD_EXAMPLES:BOOL=OFF
    -DBUILD_TESTING:BOOL=OFF
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DCMAKE_INSTALL_PREFIX:PATH=${${PROJECT_NAME}_INSTALL_DIR}
    -DITK_USE_GPU:BOOL=OFF
    -DITKV3_COMPATIBILITY:BOOL=OFF
    -DITK_BUILD_DEFAULT_MODULES:BOOL=OFF
    -DModule_ITKCommon:BOOL=ON
    -DModule_ITKUtilities:BOOL=ON
    -DModule_ITKCore:BOOL=ON)


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



