cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
project(leveldb)

include(CheckIncludeFile)
include(FindBoost) # for Windows...

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)

# version

ENABLE_TESTING()

file(GLOB leveldb_SOURCES  "${CMAKE_CURRENT_SOURCE_DIR}/db/*.cpp"
                           "${CMAKE_CURRENT_SOURCE_DIR}/table/*.cpp"
                           "${CMAKE_CURRENT_SOURCE_DIR}/util/*.cpp")

file(GLOB leveldb_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
                          "${CMAKE_CURRENT_SOURCE_DIR}/db/*.h"
                          "${CMAKE_CURRENT_SOURCE_DIR}/table/*.h"
                          "${CMAKE_CURRENT_SOURCE_DIR}/util/*.h")

#file(GLOB TESTleveldb_DIR "${CMAKE_CURRENT_SOURCE_DIR}/db/tests/*.cc")
#                          "${CMAKE_CURRENT_SOURCE_DIR}/table/tests/*.cc"
#						   "${CMAKE_CURRENT_SOURCE_DIR}/util/tests/*.cc")

include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

# OS detection
if(WIN32)
    add_definitions(-DLEVELDB_PLATFORM_WINDOWS -DOS_WIN -DWIN32_LEAN_AND_MEAN)
    if(MSVC)
        add_definitions(-DCOMPILER_MSVC)
    endif()
    set(PORT_SRCS port/port_win.cc util/env_boost.cc)
    set(CIF_SRCS)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_FREEBSD)
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
    # link pthread
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
elseif(CYGWIN)
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
elseif(APPLE)
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_MACOSX)
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
endif()

# FIXME: check cstdatomic here..

#set(SRCS
#    db/builder.cc
#    db/db_impl.cc
#    db/db_iter.cc
#    db/filename.cc
#    db/dbformat.cc
#    db/log_reader.cc
#    db/log_writer.cc
#    db/memtable.cc
#    db/repair.cc
#    db/table_cache.cc
#    db/version_edit.cc
#    db/version_set.cc
#    db/write_batch.cc
#    table/block.cc
#    table/block_builder.cc
#    table/format.cc
#    table/filter_block.cc
#    table/iterator.cc
#    table/merger.cc
#    table/table.cc
#    table/table_builder.cc
#    table/two_level_iterator.cc
#    util/env.cc
#    util/arena.cc
#    util/bloom.cc
#    util/cache.cc
#    util/coding.cc
#    util/comparator.cc
#    util/crc32c.cc
#    util/filter_policy.cc
#    util/hash.cc
#    util/histogram.cc
#    util/logging.cc
#    util/options.cc
#    util/status.cc
#    ${PORT_SRCS}
#    ${CIF_SRCS}
#    )

set(leveldb_SOURCES ${PORT_SRCS} ${CIF_SRCS})

set(TESTHARNESS_SRCS
    util/tests/testutil.cc
    util/tests/testharness.cc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)


add_library(leveldb STATIC ${leveldb_SOURCES}  ${leveldb_HEADERS})

set_target_properties(leveldb PROPERTIES FOLDER "Third Party/LevelDB")
# TESTS

add_library(leveldb-testharness STATIC ${TESTHARNESS_SRCS})
set_target_properties(leveldb-testharness PROPERTIES FOLDER "Third Party/LevelDB/Tests")


macro(leveldb_test ${nam})
endmacro()

macro(leveldb_tests)
  foreach(t ${ARGN})
    get_filename_component(_tname ${t} NAME_WE)
      add_executable(${_tname} ${t})
      add_test(NAME ${_tname} COMMAND ${_tname})
      target_link_libraries(${_tname} leveldb-testharness leveldb)
      if(WIN32)
        target_link_libraries(${_tname} boost)
      else()
        target_link_libraries(${_tname} pthread)
      endif()
    set_target_properties(${_tname} PROPERTIES FOLDER "Third Party/LevelDB/Tests")
  endforeach()
endmacro()

leveldb_tests(db/tests/db_bench.cc
              db/tests/c_test.c
              db/tests/corruption_test.cc
              db/tests/db_test.cc
              db/tests/dbformat_test.cc
              db/tests/log_test.cc
              db/tests/skiplist_test.cc
              db/tests/version_edit_test.cc
              db/tests/version_set_test.cc
              db/tests/write_batch_test.cc
              table/tests/table_test.cc
              util/tests/arena_test.cc
              util/tests/bloom_test.cc
              util/tests/cache_test.cc
              util/tests/coding_test.cc
              util/tests/crc32c_test.cc
              util/tests/env_test.cc
             )

# TODO: memenv_test
