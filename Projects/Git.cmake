# Author: Ali Uneri
# Date: 2013-05-02

set(EP_OPTION_NAME "USE_${EP_NAME}")

cma_end_definition()
# -----------------------------------------------------------------------------

find_package(Git REQUIRED)

ExternalProject_Add(${EP_NAME}
  DOWNLOAD_COMMAND ""
  CONFIGURE_COMMAND ""
  BUILD_COMMAND ""
  INSTALL_COMMAND "")

set(GIT_EXECUTABLE "${GIT_EXECUTABLE}" CACHE INTERNAL "")
set(${PROJECT_NAME}_GIT_EXECUTABLE "${GIT_EXECUTABLE}" CACHE INTERNAL "")
