

set(CODASH_PACKAGE_VERSION 0.1)
set(CODASH_DEPENDS bluebrain REQUIRED Boost dash Collage Lunchbox)
set(CODASH_REPO_URL https://github.com/BlueBrain/codash.git)
set(CODASH_REPO_TAG master)
set(CODASH_BOOST_COMPONENTS "program_options serialization system thread date_time")
set(CODASH_DEB_DEPENDS libboost-program-options-dev libboost-serialization-dev
libboost-thread-dev libboost-system-dev libboost-date-time-dev)
set(CODASH_SOURCE "${CMAKE_SOURCE_DIR}/src/codash")
set(CODASH_FORCE_BUILD ${CI_BUILD})

if(CI_BUILD_COMMIT)
  set(CODASH_REPO_TAG ${CI_BUILD_COMMIT})
else()
  set(CODASH_REPO_TAG master)
endif()
set(CODASH_FORCE_BUILD ON)
set(CODASH_SOURCE ${CMAKE_SOURCE_DIR})