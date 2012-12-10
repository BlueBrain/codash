
# Copyright (c) 2012, EFPL/Blue Brain Project
#                     Daniel Nachbaur <daniel.nachbaur@epfl.ch>

include_directories(BEFORE ${CMAKE_CURRENT_SOURCE_DIR})

file(GLOB_RECURSE TOOLS_FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} *.cpp)

foreach(FILE ${TOOLS_FILES})
  string(REGEX REPLACE ".cpp" "" NAME ${FILE})
  string(REGEX REPLACE "[./]" "_" NAME ${NAME})
  set(NAME "${NAME}codash")

  source_group(\\ FILES ${FILE})

  add_executable(${NAME} ${FILE})
  set_target_properties(${NAME} PROPERTIES FOLDER "Tools")
  target_link_libraries(${NAME} codash)
  install(TARGETS ${NAME} RUNTIME DESTINATION bin COMPONENT TOOLS)
endforeach()
