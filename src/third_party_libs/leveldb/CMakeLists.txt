#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2012 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#

cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
project(leveldb)

include(CheckIncludeFile)
include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)

enable_testing()

#==================================================================================================#
# Files                                                                                            #
#==================================================================================================#
include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

set(HEADER_FILES_LEVELDB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/cache.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/comparator.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/db.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/env.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/filter_policy.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/iterator.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/options.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/slice.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/status.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/table.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/table_builder.h
                                     ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/write_batch.h)

set(HEADER_FILES_LEVELDB_DB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/db/builder.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/db_impl.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/db_iter.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/dbformat.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/filename.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/log_format.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/log_reader.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/log_writer.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/memtable.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/skiplist.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/snapshot.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/table_cache.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/version_edit.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/version_set.h
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/write_batch_internal.h)

set(SOURCE_FILES_LEVELDB_DB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/db/builder.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/db_impl.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/db_iter.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/dbformat.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/filename.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/log_reader.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/log_writer.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/memtable.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/repair.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/table_cache.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/version_edit.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/version_set.cc
                                ${CMAKE_CURRENT_SOURCE_DIR}/db/write_batch.cc)

set(HEADER_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/atomic_pointer.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/port/port.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/port/port_example.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/port/thread_annotations.h)

set(HEADER_FILES_LEVELDB_TABLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/table/block.h
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/block_builder.h
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/filter_block.h
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/format.h
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/iterator_wrapper.h
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/merger.h
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/two_level_iterator.h)

set(SOURCE_FILES_LEVELDB_TABLE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/table/block.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/block_builder.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/filter_block.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/format.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/iterator.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/merger.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/table.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/table_builder.cc
                                   ${CMAKE_CURRENT_SOURCE_DIR}/table/two_level_iterator.cc)

set(HEADER_FILES_LEVELDB_UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util/arena.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/coding.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/crc32c.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/hash.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/histogram.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/logging.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/mutexlock.h
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/random.h)

set(SOURCE_FILES_LEVELDB_UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util/arena.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/bloom.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/cache.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/coding.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/comparator.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/crc32c.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/env.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/filter_policy.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/hash.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/histogram.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/logging.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/options.cc
                                  ${CMAKE_CURRENT_SOURCE_DIR}/util/status.cc)

if(WIN32)
  add_definitions(-DLEVELDB_PLATFORM_WINDOWS -DOS_WIN -DWIN32_LEAN_AND_MEAN) # -DSNAPPY)
  if(MSVC)
    add_definitions(-DCOMPILER_MSVC)
  endif()
  list(APPEND HEADER_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_win.h)
  set(SOURCE_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_win.cc)
  list(APPEND SOURCE_FILES_LEVELDB_UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util/env_win.cc)
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_FREEBSD)
  set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
  set(CIF_SRCS db/c.cc)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
  # link pthread
  list(APPEND HEADER_FILES_LEVELDB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/c.h)
  list(APPEND SOURCE_FILES_LEVELDB_DB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/db/c.cc)
  list(APPEND HEADER_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_posix.h)
  set(SOURCE_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_posix.cc)
  list(APPEND SOURCE_FILES_LEVELDB_UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util/env_posix.cc)
elseif(CYGWIN)
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
  set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
  set(CIF_SRCS db/c.cc)
elseif(APPLE)
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_MACOSX)
  set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
  set(CIF_SRCS db/c.cc)
endif()

#==================================================================================================#
# Groups                                                                                           #
#==================================================================================================#
source_group("Include\\leveldb" FILES ${HEADER_FILES_LEVELDB_INCLUDE_DIR})
source_group("DB\\Header Files" FILES ${HEADER_FILES_LEVELDB_DB_DIR})
source_group("DB\\Source Files" FILES ${SOURCE_FILES_LEVELDB_DB_DIR})
source_group("Port\\Header Files" FILES ${HEADER_FILES_LEVELDB_PORT_DIR})
source_group("Port\\Source Files" FILES ${SOURCE_FILES_LEVELDB_PORT_DIR})
source_group("Table\\Header Files" FILES ${HEADER_FILES_LEVELDB_TABLE_DIR})
source_group("Table\\Source Files" FILES ${SOURCE_FILES_LEVELDB_TABLE_DIR})
source_group("Util\\Header Files" FILES ${HEADER_FILES_LEVELDB_UTIL_DIR})
source_group("Util\\Source Files" FILES ${SOURCE_FILES_LEVELDB_UTIL_DIR})

set(ALL_FILES_LEVELDB_DIR ${HEADER_FILES_LEVELDB_INCLUDE_DIR}
                          ${HEADER_FILES_LEVELDB_DB_DIR}
                          ${SOURCE_FILES_LEVELDB_DB_DIR}
                          ${HEADER_FILES_LEVELDB_PORT_DIR}
                          ${SOURCE_FILES_LEVELDB_PORT_DIR}
                          ${HEADER_FILES_LEVELDB_TABLE_DIR}
                          ${SOURCE_FILES_LEVELDB_TABLE_DIR}
                          ${HEADER_FILES_LEVELDB_UTIL_DIR}
                          ${SOURCE_FILES_LEVELDB_UTIL_DIR})

#==================================================================================================#
# Library                                                                                          #
#==================================================================================================#
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(leveldb STATIC ${ALL_FILES_LEVELDB_DIR})
target_link_libraries(leveldb ${BoostChronoLibs} ${BoostDateTimeLibs} ${BoostFilesystemLibs} ${BoostSystemLibs} ${BoostThreadLibs} ${JustThread_LIBRARIES})
set_target_properties(leveldb PROPERTIES FOLDER "Third Party/LevelDB")

#==================================================================================================#
# Tests                                                                                            #
#==================================================================================================#
set(SOURCE_FILES_LEVELDB_TESTHARNESS ${CMAKE_CURRENT_SOURCE_DIR}/util/testutil.cc ${CMAKE_CURRENT_SOURCE_DIR}/util/testharness.cc)
add_library(leveldb-testharness STATIC ${SOURCE_FILES_LEVELDB_TESTHARNESS})
target_link_libraries(leveldb-testharness leveldb)
set_target_properties(leveldb-testharness PROPERTIES FOLDER "Third Party/LevelDB/Tests")

macro(leveldb_tests)
  foreach(value ${ARGN})
    get_filename_component(exe_name ${value} NAME_WE)
    add_executable(${exe_name} ${value})
    set(AllExesForCurrentProject ${AllExesForCurrentProject} ${exe_name})
    add_test(NAME ${exe_name} COMMAND ${exe_name})
    if(WIN32)
      target_link_libraries(${exe_name} leveldb-testharness)
    else()
      target_link_libraries(${exe_name} leveldb-testharness pthread)
    endif()
    set_target_properties(${exe_name} PROPERTIES FOLDER "Third Party/LevelDB/Tests")
  endforeach()
endmacro()

leveldb_tests(${CMAKE_CURRENT_SOURCE_DIR}/db/db_bench.cc
              # ${CMAKE_CURRENT_SOURCE_DIR}/db/c_test.c
              ${CMAKE_CURRENT_SOURCE_DIR}/db/corruption_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/db/db_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/db/dbformat_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/db/log_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/db/skiplist_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/db/version_edit_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/db/version_set_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/db/write_batch_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/table/table_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/util/arena_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/util/cache_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/util/coding_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/util/crc32c_test.cc
              ${CMAKE_CURRENT_SOURCE_DIR}/util/env_test.cc)

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_flags.cmake)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error -w")
endif()
