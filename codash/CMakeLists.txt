
# Copyright (c) 2012 Daniel Nachbaur <daniel.nachbaur@epfl.ch>

update_file(${CMAKE_CURRENT_SOURCE_DIR}/version.in.h
  ${OUTPUT_INCLUDE_DIR}/codash/version.h)

set(CODASH_HEADERS # public, installed
  ${OUTPUT_INCLUDE_DIR}/codash/version.h
  api.h
  codash.h
  distributable.h
  receiver.h
  sender.h
)

set(CODASH_DETAIL_HEADERS # private, installed
)

set(CODASH_SOURCES
  codash.cpp
  receiver.cpp
  sender.cpp
  version.cpp
)

set(CODASH_DETAIL_SOURCES
  detail/communicator.h
  detail/communicator.cpp
  detail/objectFactory.h
  detail/types.h
)

set(CODASH_ALL ${CODASH_HEADERS} ${CODASH_SOURCES})
set(CODASH_DETAIL_ALL ${CODASH_DETAIL_HEADERS} ${CODASH_DETAIL_SOURCES})

source_group(\\ FILES CMakeLists.txt ${CODASH_ALL})
source_group(detail FILES ${CODASH_DETAIL_ALL})

set(LIBRARY codash)
set(LIB_INCLUDE ${LIBRARY})
if(NOT MSVC)
  set(LIB_INCLUDE ${LIB_INCLUDE}${VERSION_MAJOR})
endif()

add_library(${LIBRARY} SHARED ${CODASH_ALL} ${CODASH_DETAIL_ALL})
target_link_libraries(${LIBRARY} ${DASH_LIBRARIES} ${COLLAGE_LIBRARIES}
  ${Boost_LIBRARIES})
set_target_properties(${LIBRARY}
  PROPERTIES VERSION ${VERSION} SOVERSION ${VERSION_ABI}
  COMPILE_DEFINITIONS CODASH_SHARED
)

install(FILES ${CODASH_HEADERS}
        DESTINATION include/${LIB_INCLUDE} COMPONENT dev)
install(FILES ${CODASH_DETAIL_HEADERS}
        DESTINATION include/${LIB_INCLUDE}/detail COMPONENT dev)
install(TARGETS ${LIBRARY}
        ARCHIVE DESTINATION lib COMPONENT dev
        RUNTIME DESTINATION bin COMPONENT lib
        LIBRARY DESTINATION lib COMPONENT lib)
if(NOT MSVC)
  install_symlink("include/${LIB_INCLUDE}" "include/${LIBRARY}" dev)
endif()
