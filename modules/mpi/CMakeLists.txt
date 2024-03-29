## Copyright 2009 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

cmake_dependent_option(
  OSPRAY_MODULE_MPI
  "Build the MPI module (Requires the CPU module)"
  OFF
  OSPRAY_MODULE_CPU
  OFF
)

if (NOT OSPRAY_MODULE_MPI)
  return()
endif()

find_package(MPI REQUIRED)
if(NOT TARGET MPI::MPI_CXX) # pre CMake 3.9
  add_library(MPI::MPI_CXX IMPORTED INTERFACE)
  set_property(TARGET MPI::MPI_CXX PROPERTY INTERFACE_COMPILE_OPTIONS ${MPI_CXX_COMPILE_FLAGS})
  set_property(TARGET MPI::MPI_CXX PROPERTY INTERFACE_INCLUDE_DIRECTORIES "${MPI_CXX_INCLUDE_DIRS}")
  set_property(TARGET MPI::MPI_CXX PROPERTY INTERFACE_LINK_LIBRARIES ${MPI_CXX_LINK_FLAGS} ${MPI_CXX_LIBRARIES})
endif()

find_package(Snappy REQUIRED)

rkcommon_configure_build_type()

add_subdirectory(common)
add_subdirectory(ospray)

if (OSPRAY_ENABLE_APPS_EXAMPLES OR OSPRAY_ENABLE_APPS_TUTORIALS)
  add_subdirectory(tutorials)
endif()

if (OSPRAY_ENABLE_APPS_TESTING)
  add_subdirectory(testing)
endif()
