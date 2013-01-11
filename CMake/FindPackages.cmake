# generated by Buildyard, do not edit.

include(System)
set(FIND_PACKAGES_FOUND ${SYSTEM} ${FIND_PACKAGES_FOUND_EXTRA})

find_package(dash 1.1.0 REQUIRED)
if(dash_FOUND)
  set(dash_name dash)
endif()
if(DASH_FOUND)
  set(dash_name DASH)
endif()
if(dash_name)
  list(APPEND FIND_PACKAGES_FOUND CODASH_USE_DASH)
  link_directories(${${dash_name}_LIBRARY_DIRS})
  include_directories(${${dash_name}_INCLUDE_DIRS})
endif()

find_package(Collage 0.7.0 REQUIRED)
if(Collage_FOUND)
  set(Collage_name Collage)
endif()
if(COLLAGE_FOUND)
  set(Collage_name COLLAGE)
endif()
if(Collage_name)
  list(APPEND FIND_PACKAGES_FOUND CODASH_USE_COLLAGE)
  link_directories(${${Collage_name}_LIBRARY_DIRS})
  include_directories(${${Collage_name}_INCLUDE_DIRS})
endif()

find_package(Boost 1.41.0 REQUIRED program_options serialization)
if(Boost_FOUND)
  set(Boost_name Boost)
endif()
if(BOOST_FOUND)
  set(Boost_name BOOST)
endif()
if(Boost_name)
  list(APPEND FIND_PACKAGES_FOUND CODASH_USE_BOOST)
  link_directories(${${Boost_name}_LIBRARY_DIRS})
  include_directories(SYSTEM ${${Boost_name}_INCLUDE_DIRS})
endif()


# Write defines.h and options.cmake
if(NOT FIND_PACKAGES_INCLUDE)
  set(FIND_PACKAGES_INCLUDE
    "${CMAKE_BINARY_DIR}/include/${CMAKE_PROJECT_NAME}/defines${SYSTEM}.h")
endif()
if(NOT OPTIONS_CMAKE)
  set(OPTIONS_CMAKE ${CMAKE_BINARY_DIR}/options.cmake)
endif()
set(DEFINES_FILE ${FIND_PACKAGES_INCLUDE})
set(DEFINES_FILE_IN ${DEFINES_FILE}.in)
file(WRITE ${DEFINES_FILE_IN}
  "// generated by Buildyard, do not edit.\n\n"
  "#ifndef ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n"
  "#define ${CMAKE_PROJECT_NAME}_DEFINES_${SYSTEM}_H\n\n")
file(WRITE ${OPTIONS_CMAKE} "# Optional modules enabled during build\n")
foreach(DEF ${FIND_PACKAGES_FOUND})
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
