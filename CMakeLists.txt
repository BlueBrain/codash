
# Copyright (c) 2012-2014 Daniel Nachbaur <daniel.nachbaur@epfl.ch>

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(codash)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/CMake
                              ${CMAKE_CURRENT_SOURCE_DIR}/CMake/common)

include(GitExternal)
include(Buildyard)
if(BUILDYARD_STOP)
  return()
endif()

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")

set(CODASH_DESCRIPTION
  "Serialization & distribution support for DASH via Collage")
set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)

include(Common)
include(FindPackages)
set(CODASH_DEPENDENT_LIBRARIES Collage dash Lunchbox)

add_subdirectory(codash)
add_subdirectory(tests)
add_subdirectory(tools)
add_subdirectory(doc)
include(DoxygenRule)

add_custom_target(package COMMENT "Packaging not done yet")
# include(CPackConfig)
include(PackageConfig)
