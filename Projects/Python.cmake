# Author: Ali Uneri
# Date: 2013-05-02

set(EP_OPTION_NAME "USE_${EP_NAME}")

if(${EP_OPTION_NAME}_Conda)
  cma_envvar(CONDA_DEFAULT_ENV "${PROJECT_BINARY_DIR}/${EP_NAME}")
  cma_envvar(PYTHONHOME "${PROJECT_BINARY_DIR}/${EP_NAME}")
  if(WIN32)
    cma_envvar(PATH PREPEND
      "${PROJECT_BINARY_DIR}/${EP_NAME}"
      "${PROJECT_BINARY_DIR}/${EP_NAME}/Scripts")
  elseif(UNIX)
    cma_envvar(PATH PREPEND "${PROJECT_BINARY_DIR}/${EP_NAME}/bin")
  else()
    message(FATAL_ERROR "Platform is not supported.")
  endif()
endif()

cma_end_definition()
# -----------------------------------------------------------------------------

find_package(PythonInterp REQUIRED)

if(USE_Slicer AND ${PYTHON_EXECUTABLE} MATCHES "conda")
  cmake_dependent_option(${EP_OPTION_NAME}_Conda "" ON ${EP_OPTION_NAME} OFF)
else()
  cmake_dependent_option(${EP_OPTION_NAME}_Conda "" OFF ${EP_OPTION_NAME} OFF)
endif()

if(${EP_OPTION_NAME}_Conda)
  set(CONDA_PACKAGES
    numpy=1.9.1
    pip=1.5.6)
  if(USE_mlabwrap)
    list(APPEND CONDA_PACKAGES scipy=0.14)
  endif()
  if(USE_pywinauto)
    list(APPEND CONDA_PACKAGES pywin32=219)
  endif()

  get_filename_component(CONDA_BIN_DIR ${PYTHON_EXECUTABLE} PATH)
  find_program(CONDA_EXECUTABLE conda
    PATHS "${CONDA_BIN_DIR}")
  if(NOT CONDA_EXECUTABLE)
    message(FATAL_ERROR "Please specify CONDA_EXECUTABLE")
  endif()

  if(WIN32)
    set(PYTHON_EXECUTABLE "${PROJECT_BINARY_DIR}/${EP_NAME}/python.exe")
    set(PYTHON_INCLUDE_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}/include")
    set(PYTHON_NUMPY_INCLUDE_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}/Lib/site-packages/numpy/core/include/numpy")
    set(PYTHON_LIBRARY "${PROJECT_BINARY_DIR}/${EP_NAME}/libs/python27.lib")
  elseif(UNIX)
    set(PYTHON_EXECUTABLE "${PROJECT_BINARY_DIR}/${EP_NAME}/bin/python2.7")
    set(PYTHON_INCLUDE_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}/include/python2.7")
    set(PYTHON_NUMPY_INCLUDE_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}/lib/python2.7/site-packages/numpy/core/include/numpy")
    if(APPLE)
      set(PYTHON_LIBRARY "${PROJECT_BINARY_DIR}/${EP_NAME}/lib/libpython2.7.dylib")
    else()
      set(PYTHON_LIBRARY "${PROJECT_BINARY_DIR}/${EP_NAME}/lib/libpython2.7.so")
    endif()
  else()
    message(FATAL_ERROR "Platform is not supported.")
  endif()

  if(UNIX)
    set(WARNING "Please call the following prior to compilation:\n")
    list(APPEND WARNING "sudo ln -fnsv ${PROJECT_BINARY_DIR}/${EP_NAME} /opt/anaconda1anaconda2anaconda3\n")
    if(APPLE)
      list(APPEND WARNING "sudo mkdir -pv /usr/local/lib\n")
      list(APPEND WARNING "sudo ln -sv /usr/lib/libgcc_s.1.dylib /usr/local/lib/libgcc_s.1.dylib\n")
    endif()
    message(WARNING ${WARNING})
  endif()

  ExternalProject_Add(${EP_NAME}
    # download
    DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/${EP_NAME}"
    DOWNLOAD_COMMAND ""
    # patch
    # update
    # configure
    CONFIGURE_COMMAND "${CMAKE_COMMAND}" -E remove_directory "${PROJECT_BINARY_DIR}/${EP_NAME}"
    # build
    BUILD_COMMAND "${CONDA_EXECUTABLE}" create --yes --no-default-packages --prefix "${PROJECT_BINARY_DIR}/${EP_NAME}" python=2.7.3
    # install
    INSTALL_COMMAND "${CONDA_EXECUTABLE}" install --yes --prefix "${PROJECT_BINARY_DIR}/${EP_NAME}" ${CONDA_PACKAGES} python=2.7.3
    )
else()
  if(APPLE)
    if(PYTHON_EXECUTABLE MATCHES "^/opt/local/bin/python")
      set(PYTHON_INCLUDE_DIR "/opt/local/Library/Frameworks/Python.framework/Headers" CACHE PATH "")
      set(PYTHON_LIBRARY "/opt/local/Library/Frameworks/Python.framework/Versions/Current/Python" CACHE PATH "")
    elseif(PYTHON_EXECUTABLE MATCHES "conda")
      get_filename_component(CONDA_BIN_DIR ${PYTHON_EXECUTABLE} PATH)
      get_filename_component(CONDA_DIR ${CONDA_BIN_DIR} PATH)
      set(PYTHON_INCLUDE_DIR "${CONDA_DIR}/include/python2.7" CACHE PATH "")
      set(PYTHON_LIBRARY "${CONDA_DIR}/lib/libpython2.7.dylib" CACHE PATH "")
    endif()
  endif()

  find_package(PythonLibs ${PYTHON_VERSION_STRING} REQUIRED)

  set(PYTHON_RELEASE_LIBRARY ${PYTHON_LIBRARY})
  if(WIN32)
    list(GET PYTHON_LIBRARY 0 IS_OPTIMIZED)
    if(${IS_OPTIMIZED} STREQUAL "optimized")
      list(GET PYTHON_LIBRARY 1 PYTHON_RELEASE_LIBRARY)
    endif()
  endif()

  find_package(NumPy REQUIRED)

  ExternalProject_Add(${EP_NAME}
    DOWNLOAD_COMMAND ""
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "")
endif()

set(PYTHON_EXECUTABLE "${PYTHON_EXECUTABLE}" CACHE INTERNAL "")
set(${PROJECT_NAME}_PYTHON_EXECUTABLE "${PYTHON_EXECUTABLE}" CACHE INTERNAL "")
set(${PROJECT_NAME}_PYTHON_LIBRARY "${PYTHON_LIBRARY}" CACHE INTERNAL "")
set(${PROJECT_NAME}_PYTHON_INCLUDE_DIR "${PYTHON_INCLUDE_DIR}" CACHE INTERNAL "")
set(${PROJECT_NAME}_PYTHON_NUMPY_INCLUDE_DIR "${PYTHON_NUMPY_INCLUDE_DIR}" CACHE INTERNAL "")
