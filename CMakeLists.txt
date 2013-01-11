
# Copyright (c) 2012 Daniel Nachbaur <daniel.nachbaur@epfl.ch>

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(codash)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")

set(VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH})
if(VERSION_MINOR VERSION_LESS 10)
  set(VERSION_ABI ${VERSION_MAJOR}0${VERSION_MINOR})
else()
  set(VERSION_ABI ${VERSION_MAJOR}${VERSION_MINOR})
endif()

include(Common)
include(revision)
include(UpdateFile)

include(FindPackages)
set(CODASH_TRANSIENT_LIBRARIES Collage dash)

add_subdirectory(codash)
add_subdirectory(tests)
add_subdirectory(tools)
add_subdirectory(doc)
include(DoxygenRule)

add_custom_target(package COMMENT "Packaging not done yet")
# include(CPackConfig)

set(CTEST_CUSTOM_WARNING_EXCEPTION ${CTEST_CUSTOM_WARNING_EXCEPTION}
  # http://public.kitware.com/Bug/view.php?id=10179
  "ld: warning: duplicate dylib")
include(CTest)

include(PackageConfig)
