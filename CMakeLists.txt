cmake_minimum_required(VERSION 3.0.2)


set(PROJECT_NAME "NPTrackingTools" CACHE STRING "")
project(${PROJECT_NAME})
set(DESTDIR "${CMAKE_CURRENT_BINARY_DIR}/install" CACHE PATH "" FORCE)

# CMakeAll
list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/CMake")
find_package(CMakeAll REQUIRED)

# options
option(${PROJECT_NAME}_BUILD_DOCUMENTATION "Support for Doxygen documentation" OFF)
option(${PROJECT_NAME}_BUILD_PACKAGING "Support for CPack packaging" ON)

# version
set(${PROJECT_NAME}_VERSION_MAJOR 1)
set(${PROJECT_NAME}_VERSION_MINOR 1)
cma_git_revision(${PROJECT_SOURCE_DIR} ${PROJECT_NAME}_VERSION_PATCH)
set(${PROJECT_NAME}_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_PATCH}")


# configure projects
file(GLOB ${PROJECT_NAME}_PROJECTS "${PROJECT_SOURCE_DIR}/Projects/*.cmake")
cma_add_projects(${${PROJECT_NAME}_PROJECTS})

cma_print_projects()
message(STATUS "${PROJECT_NAME} ${${PROJECT_NAME}_VERSION}")

if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang" AND APPLE)
  set(CMAKE_CXX_FLAGS "-stdlib=libstdc++ ${CMAKE_CXX_FLAGS}")
endif()

cma_configure_projects()

if(${PROJECT_NAME}_BUILD_PACKAGING)
  set(CPACK_GENERATOR "ZIP")
  set(CPACK_PACKAGE_NAME "${PROJECT_NAME}")
  set(CPACK_PACKAGE_VENDOR "nenetto")
  set(CPACK_PACKAGE_VERSION_MAJOR ${${PROJECT_NAME}_VERSION_MAJOR})
  set(CPACK_PACKAGE_VERSION_MINOR ${${PROJECT_NAME}_VERSION_MINOR})
  set(CPACK_PACKAGE_VERSION_PATCH ${${PROJECT_NAME}_VERSION_PATCH})
  set(CPACK_PACKAGE_VERSION ${${PROJECT_NAME}_VERSION})
endif()

# Documentation
if(${PROJECT_NAME}_BUILD_DOCUMENTATION AND NOT USE_Doxygen)
  message("${PROJECT_NAME}_BUILD_DOCUMENTATION is enabling Doxygen")
  set(USE_Doxygen ON CACHE BOOL "" FORCE)
endif()


# configure files
file(WRITE ${PROJECT_BINARY_DIR}/CMake.h "#ifndef __CMake_h\n#define __CMake_h\n\n")
file(WRITE ${PROJECT_BINARY_DIR}/CMake.py "")
file(WRITE ${PROJECT_BINARY_DIR}/CMake.m "")
get_cmake_property(VARIABLES VARIABLES)
foreach (KEY ${VARIABLES})
  if(KEY MATCHES "^${PROJECT_NAME}_")
    file(TO_NATIVE_PATH "${${KEY}}" VALUE)
    if(WIN32)
      string(REPLACE "\\" "\\\\" VALUE "${VALUE}")
    endif()
    file(APPEND ${PROJECT_BINARY_DIR}/CMake.h "#define ${KEY} \"${VALUE}\"\n")
    file(APPEND ${PROJECT_BINARY_DIR}/CMake.py "${KEY} = '${VALUE}'\n")
    file(APPEND ${PROJECT_BINARY_DIR}/CMake.m "${KEY} = '${VALUE}';\n")
  endif()
endforeach()
file(APPEND ${PROJECT_BINARY_DIR}/CMake.h "\n#endif  // __CMake_h\n")


# configure launcher
cma_configure_launcher("${PROJECT_SOURCE_DIR}/CMake/Launcher.cmake")

cma_launcher_target(Terminal
  NAME run_Terminal)
cma_launcher_shortcut(Terminal
  NAME Terminal)


