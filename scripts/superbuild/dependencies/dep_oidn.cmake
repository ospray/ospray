## Copyright 2009-2021 Intel Corporation
## SPDX-License-Identifier: Apache-2.0

set(COMPONENT_NAME oidn)

set(COMPONENT_PATH ${INSTALL_DIR_ABSOLUTE})
if (INSTALL_IN_SEPARATE_DIRECTORIES)
  set(COMPONENT_PATH ${INSTALL_DIR_ABSOLUTE}/${COMPONENT_NAME})
endif()

if (OIDN_HASH)
  set(OIDN_URL_HASH URL_HASH SHA256=${OIDN_HASH})
endif()

if (BUILD_OIDN_FROM_SOURCE)
  string(REGEX REPLACE "(^[0-9]+\.[0-9]+\.[0-9]+$)" "v\\1" OIDN_ARCHIVE ${OIDN_VERSION})
  ExternalProject_Add(${COMPONENT_NAME}
    PREFIX ${COMPONENT_NAME}
    DOWNLOAD_DIR ${COMPONENT_NAME}
    STAMP_DIR ${COMPONENT_NAME}/stamp
    SOURCE_DIR ${COMPONENT_NAME}/src
    BINARY_DIR ${COMPONENT_NAME}/build
    LIST_SEPARATOR | # Use the alternate list separator
    URL "https://github.com/OpenImageDenoise/oidn/archive/${OIDN_ARCHIVE}.zip"
    ${OIDN_URL_HASH}
    CMAKE_ARGS
      -DCMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}
      -DCMAKE_CXX_COMPILER=${CMAKE_CXX_COMPILER}
      -DCMAKE_C_COMPILER=${CMAKE_C_COMPILER}
      -DCMAKE_INSTALL_PREFIX:PATH=${COMPONENT_PATH}
      -DCMAKE_INSTALL_INCLUDEDIR=${CMAKE_INSTALL_INCLUDEDIR}
      -DCMAKE_INSTALL_LIBDIR=${CMAKE_INSTALL_LIBDIR}
      -DCMAKE_INSTALL_DOCDIR=${CMAKE_INSTALL_DOCDIR}
      -DCMAKE_INSTALL_BINDIR=${CMAKE_INSTALL_BINDIR}
      $<$<BOOL:${DOWNLOAD_TBB}>:-DTBB_ROOT=${TBB_PATH}>
      $<$<BOOL:${DOWNLOAD_ISPC}>:-DISPC_EXECUTABLE=${ISPC_PATH}>
      -DCMAKE_BUILD_TYPE=${DEPENDENCIES_BUILD_TYPE}
      -DOIDN_APPS=OFF
    BUILD_COMMAND ${DEFAULT_BUILD_COMMAND}
    BUILD_ALWAYS ${ALWAYS_REBUILD}
  )

  if (DOWNLOAD_TBB)
    ExternalProject_Add_StepDependencies(${COMPONENT_NAME} configure tbb)
  endif()
  if (DOWNLOAD_ISPC)
    ExternalProject_Add_StepDependencies(${COMPONENT_NAME} configure ispc)
  endif()
else()

  if (APPLE)
    set(OIDN_OSSUFFIX "x86_64.macos.tar.gz")
  elseif (WIN32)
    set(OIDN_OSSUFFIX "x64.vc14.windows.zip")
  else()
    set(OIDN_OSSUFFIX "x86_64.linux.tar.gz")
  endif()
  set(OIDN_URL "https://github.com/OpenImageDenoise/oidn/releases/download/v${OIDN_VERSION}/oidn-${OIDN_VERSION}.${OIDN_OSSUFFIX}")

  ExternalProject_Add(${COMPONENT_NAME}
    PREFIX ${COMPONENT_NAME}
    DOWNLOAD_DIR ${COMPONENT_NAME}
    STAMP_DIR ${COMPONENT_NAME}/stamp
    SOURCE_DIR ${COMPONENT_NAME}/src
    BINARY_DIR ${COMPONENT_NAME}
    URL ${OIDN_URL}
    ${OIDN_URL_HASH}
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    INSTALL_COMMAND "${CMAKE_COMMAND}" -E copy_directory
      <SOURCE_DIR>/
      ${COMPONENT_PATH}
    BUILD_ALWAYS OFF
  )
endif()

list(APPEND CMAKE_PREFIX_PATH ${COMPONENT_PATH})
string(REPLACE ";" "|" CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH}")

