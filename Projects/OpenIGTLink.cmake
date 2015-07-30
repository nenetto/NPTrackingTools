# Author: Eugenio Marinetto
# Date: 2015-01-15

set(EP_OPTION_NAME "USE_${EP_NAME}")
set(EP_REQUIRED_PROJECTS Git)
set(EP_URL "https://github.com/openigtlink/OpenIGTLink.git")
set(EP_OPTION_DESCRIPTION "OpenIGTLink")

cma_envvar(@LIBRARYPATH@ PREPEND "@BINARY_DIR@/@LIBDIR@/@INTDIR@")

cma_end_definition()
# -----------------------------------------------------------------------------

if(${PROJECT_NAME}_OpenIGTLink_EXTERNAL_DIR)

  ExternalProject_Add(${EP_NAME}
       DOWNLOAD_COMMAND ""
       CONFIGURE_COMMAND ""
       BUILD_COMMAND ""
       INSTALL_COMMAND "")

  set(${PROJECT_NAME}_${EP_NAME}_DIR ${${PROJECT_NAME}_OpenIGTLink_EXTERNAL_DIR} CACHE INTERNAL "")

else()

  set(EP_CMAKE_ARGS
    -DBUILD_SHARED_LIBS:BOOL=OFF
    -DBUILD_STATIC_LIBS:BOOL=ON
    -DCMAKE_BUILD_TYPE:STRING=${CMAKE_BUILD_TYPE}
    -DCMAKE_C_FLAGS:STRING=${CMAKE_C_FLAGS}
    -DCMAKE_CXX_FLAGS:STRING=${CMAKE_CXX_FLAGS}
    -DBUILD_EXAMPLES:BOOL=ON
    -DCMAKE_INSTALL_PREFIX:PATH=${PROJECT_BINARY_DIR}/${EP_NAME}-install
    )

  ExternalProject_Add(${EP_NAME}
    DEPENDS ${EP_REQUIRED_PROJECTS}
    # download
    GIT_REPOSITORY ${EP_URL}
    GIT_TAG 5a501817c2da52e81db4db3eca6dd5111f94fed9
    # patch
    # update
    UPDATE_COMMAND ""
    # configure
    SOURCE_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}
    CMAKE_ARGS ${EP_CMAKE_ARGS}
    # build
    BINARY_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-build
    # install
    INSTALL_DIR ${PROJECT_BINARY_DIR}/${EP_NAME}-install
    INSTALL_COMMAND
    # test
    )

  set(${PROJECT_NAME}_${EP_NAME}_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}-install" CACHE INTERNAL "")


endif()


