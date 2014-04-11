# Copyright (c) 2012-2014 Daniel Nachbaur <daniel.nachbaur@epfl.ch>

set(CODASH_PUBLIC_HEADERS # public, installed
  ${COMMON_INCLUDES}
  distributable.h
  receiver.h
  sender.h
  types.h
)

set(CODASH_SOURCES
  ${COMMON_SOURCES}
  codash.cpp
  receiver.cpp
  sender.cpp
  detail/communicator.h
  detail/communicator.cpp
  detail/objectFactory.h
  detail/types.h)

set(CODASH_ALL
  ${CODASH_PUBLIC_HEADERS}
  ${CODASH_SOURCES})

set(CODASH_LINK_LIBRARIES ${LUNCHBOX_LIBRARIES} ${DASH_LIBRARIES}
  ${COLLAGE_LIBRARIES} ${Boost_LIBRARIES})

common_library(codash)

source_group(\\ FILES CMakeLists.txt)
source_group(codash FILES ${CODASH_PUBLIC_HEADERS} ${CODASH_SOURCES})

