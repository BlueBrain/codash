
# Copyright (c) 2012-2015 Daniel Nachbaur <daniel.nachbaur@epfl.ch>

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(codash)

list(APPEND CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/CMake
  ${PROJECT_SOURCE_DIR}/CMake/common)

include(GitExternal)
include(SubProject)

set(VERSION_MAJOR "0")
set(VERSION_MINOR "1")
set(VERSION_PATCH "0")
set(VERSION_ABI 1)


include(Common)

set(CODASH_DESCRIPTION
  "Serialization & distribution support for DASH via Collage")
set(COMMON_PROJECT_DOMAIN ch.epfl.bluebrain)
set(CODASH_DEPENDENT_LIBRARIES Collage dash Lunchbox)

include(FindPackages)

add_subdirectory(codash)
add_subdirectory(tests)
add_subdirectory(tools)
add_subdirectory(doc)

add_custom_target(package COMMENT "Packaging not done yet")
# include(CPackConfig)
include(PackageConfig)

include(DoxygenRule)
