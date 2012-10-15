
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
include(FindPackages)
include(UpdateFile)

add_subdirectory(codash)
add_subdirectory(tests)
add_subdirectory(doc)
include(DoxygenRule)

include(CTest)
add_custom_target(package COMMENT "Packaging not done yet")
# include(CPackConfig)
