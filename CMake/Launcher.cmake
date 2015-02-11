
# command alias: Slicer
list(APPEND HELP "   Slicer - Slicer application.\n")
if(ARGV MATCHES "^Slicer$")
  string(REPLACE "${PATHSEP}" ";" SLICERPATH "$ENV{SLICERPATH}")
  set(ARGV ${CMD} $ENV{SLICER_EXECUTABLE} --additional-module-paths ${SLICERPATH})
endif()


# command alias: Slicelet
list(APPEND HELP "   Slicelet <path> - Slicer scripted module as a standalone application.\n")
if(ARGV MATCHES "^Slicelet")
  list(GET ARGV 1 MODULE)
  set(ARGV ${CMD} $ENV{SLICER_EXECUTABLE} --no-main-window --disable-cli-modules --disable-loadable-modules --disable-scripted-loadable-modules --python-script ${MODULE})
endif()


# command alias: BiiGTK
list(APPEND HELP "   BiiGTK - Experimental BiiGTK interface.\n")
if(ARGV MATCHES "^BiiGTK$")
  string(REPLACE "${PATHSEP}" ";" SLICERPATH "$ENV{SLICERPATH}")
  get_filename_component(PROJECT_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR} PATH)
  set(ARGV ${CMD} $ENV{SLICER_EXECUTABLE} --no-main-window --python-script ${PROJECT_SOURCE_DIR}/SlicerBiiGTK/Main/Main.py --additional-module-paths ${SLICERPATH})
endif()
