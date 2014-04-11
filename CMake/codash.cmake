

set(CODASH_PACKAGE_VERSION 0.1.0)
set(CODASH_DEPENDS bluebrain REQUIRED dash Collage Boost)
set(CODASH_REPO_URL https://github.com/BlueBrain/codash.git)
set(CODASH_REPO_TAG master)
set(CODASH_BOOST_COMPONENTS "program_options serialization system thread date_time")
set(CODASH_DEB_DEPENDS libboost-program-options-dev libboost-serialization-dev
  libboost-date-time-dev libboost-system-dev libboost-thread-dev)
set(CODASH_SOURCE "${CMAKE_SOURCE_DIR}/src/codash")
set(CODASH_FORCE_BUILD ${CI_BUILD})

if(CI_BUILD_COMMIT)
  set(CODASH_REPO_TAG ${CI_BUILD_COMMIT})
else()
  set(CODASH_REPO_TAG master)
endif()
set(CODASH_FORCE_BUILD ON)
set(CODASH_SOURCE ${CMAKE_SOURCE_DIR})