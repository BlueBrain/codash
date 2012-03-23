
# Copyright (c) 2012 Daniel Nachbaur <daniel.nachbaur@epfl.ch>

configure_file("Version.h.in" "${CMAKE_CURRENT_BINARY_DIR}/Version.h" @ONLY)

set(CODASH_HEADERS # public, installed
  api.h
  codash.h
  Serialization.h
  "${CMAKE_CURRENT_BINARY_DIR}/Version.h"
)

set(CODASH_DETAIL_HEADERS # private, installed
)

set(CODASH_SOURCES
  Serialization.cpp
)

set(CODASH_DETAIL_SOURCES
)

set(CODASH_ALL ${CODASH_HEADERS} ${CODASH_SOURCES})
set(CODASH_DETAIL_ALL ${CODASH_DETAIL_HEADERS} ${CODASH_DETAIL_SOURCES})

source_group(\\ FILES CMakeLists.txt ${CODASH_ALL})
source_group(detail FILES ${CODASH_DETAIL_ALL})

set(LIBRARY codash)
set(LIB_INCLUDE ${LIBRARY}${VERSION_MAJOR})

add_library(${LIBRARY} SHARED ${CODASH_ALL} ${CODASH_DETAIL_ALL})
target_link_libraries(${LIBRARY} ${DASH_LIBRARIES} ${COLLAGE_LIBRARIES})
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
install_symlink(${LIB_INCLUDE} ${LIBRARY} include)
