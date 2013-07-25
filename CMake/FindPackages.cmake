# generated by Buildyard, do not edit.

include(System)
list(APPEND FIND_PACKAGES_DEFINES ${SYSTEM})

find_package(dash 1.1.0 REQUIRED)
if(dash_FOUND)
  set(dash_name dash)
endif()
if(DASH_FOUND)
  set(dash_name DASH)
endif()
if(dash_name)
  list(APPEND FIND_PACKAGES_DEFINES CODASH_USE_DASH)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} dash")
  link_directories(${${dash_name}_LIBRARY_DIRS})
  if(NOT "${${dash_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${dash_name}_INCLUDE_DIRS})
  endif()
endif()

find_package(Collage 1.0.0 REQUIRED)
if(Collage_FOUND)
  set(Collage_name Collage)
endif()
if(COLLAGE_FOUND)
  set(Collage_name COLLAGE)
endif()
if(Collage_name)
  list(APPEND FIND_PACKAGES_DEFINES CODASH_USE_COLLAGE)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Collage")
  link_directories(${${Collage_name}_LIBRARY_DIRS})
  if(NOT "${${Collage_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(${${Collage_name}_INCLUDE_DIRS})
  endif()
endif()

find_package(Boost 1.41.0 REQUIRED program_options serialization)
if(Boost_FOUND)
  set(Boost_name Boost)
endif()
if(BOOST_FOUND)
  set(Boost_name BOOST)
endif()
if(Boost_name)
  list(APPEND FIND_PACKAGES_DEFINES CODASH_USE_BOOST)
  set(FIND_PACKAGES_FOUND "${FIND_PACKAGES_FOUND} Boost")
  link_directories(${${Boost_name}_LIBRARY_DIRS})
  if(NOT "SYSTEM ${${Boost_name}_INCLUDE_DIRS}" MATCHES "-NOTFOUND")
    include_directories(SYSTEM ${${Boost_name}_INCLUDE_DIRS})
  endif()
endif()


if(EXISTS ${CMAKE_SOURCE_DIR}/CMake/FindPackagesPost.cmake)
  include(${CMAKE_SOURCE_DIR}/CMake/FindPackagesPost.cmake)
endif()

set(CODASH_BUILD_DEBS cmake;git;git-svn;libavahi-compat-libdnssd-dev;libboost-date-time-dev;libboost-program-options-dev;libboost-regex-dev;libboost-serialization-dev;libboost-system-dev;libhwloc-dev;libibverbs-dev;libjpeg-turbo8-dev;librdmacm-dev;libturbojpeg;libudt-dev;pkg-config;subversion)

set(CODASH_DEPENDS dash;Collage;Boost)

# Write defines.h and options.cmake
if(NOT PROJECT_INCLUDE_NAME)
  set(PROJECT_INCLUDE_NAME ${CMAKE_PROJECT_NAME})
endif()
if(NOT OPTIONS_CMAKE)
  set(OPTIONS_CMAKE ${CMAKE_BINARY_DIR}/options.cmake)
endif()
set(DEFINES_FILE "${CMAKE_BINARY_DIR}/include/${PROJECT_INCLUDE_NAME}/defines${SYSTEM}.h")
set(DEFINES_FILE_IN ${DEFINES_FILE}.in)
file(WRITE ${DEFINES_FILE_IN}
  "// generated by CMake/FindPackages.cmake, do not edit.\n\n"
  "#ifndef ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n"
  "#define ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n\n")
file(WRITE ${OPTIONS_CMAKE} "# Optional modules enabled during build\n")
foreach(DEF ${FIND_PACKAGES_DEFINES})
  add_definitions(-D${DEF})
  file(APPEND ${DEFINES_FILE_IN}
  "#ifndef ${DEF}\n"
  "#  define ${DEF}\n"
  "#endif\n")
if(NOT DEF STREQUAL SYSTEM)
  file(APPEND ${OPTIONS_CMAKE} "set(${DEF} ON)\n")
endif()
endforeach()
file(APPEND ${DEFINES_FILE_IN}
  "\n#endif\n")

include(UpdateFile)
update_file(${DEFINES_FILE_IN} ${DEFINES_FILE})
if(Boost_FOUND) # another WAR for broken boost stuff...
  set(Boost_VERSION ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION})
endif()
if(CUDA_FOUND)
  string(REPLACE "-std=c++11" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
  string(REPLACE "-std=c++0x" "" CUDA_HOST_FLAGS "${CUDA_HOST_FLAGS}")
endif()
if(FIND_PACKAGES_FOUND)
  message(STATUS "Configured with ${CMAKE_BUILD_TYPE}${FIND_PACKAGES_FOUND}")
endif()
