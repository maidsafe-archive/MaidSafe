#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.maidsafe.net/licenses                                                            #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
#                                                                                                  #
#==================================================================================================#


project(leveldb)

include(CheckIncludeFile)
include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)

enable_testing()


#==================================================================================================#
# Files                                                                                            #
#==================================================================================================#
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
  list(APPEND HEADER_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_win.h)
  set(SOURCE_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_win.cc)
  list(APPEND SOURCE_FILES_LEVELDB_UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util/env_win.cc)
else()
  list(APPEND HEADER_FILES_LEVELDB_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/c.h)
  list(APPEND SOURCE_FILES_LEVELDB_DB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/db/c.cc)
  list(APPEND HEADER_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_posix.h)
  set(SOURCE_FILES_LEVELDB_PORT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_posix.cc)
  list(APPEND SOURCE_FILES_LEVELDB_UTIL_DIR ${CMAKE_CURRENT_SOURCE_DIR}/util/env_posix.cc)
endif()


#==================================================================================================#
# Definitions                                                                                      #
#==================================================================================================#
if(WIN32)
  add_definitions(-DLEVELDB_PLATFORM_WINDOWS -DOS_WIN -DWIN32_LEAN_AND_MEAN) # -DSNAPPY)
  if(MSVC)
    add_definitions(-DCOMPILER_MSVC)
  endif()
elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_FREEBSD)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
elseif(CYGWIN)
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
elseif(APPLE)
  add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_MACOSX)
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
add_library(leveldb STATIC ${ALL_FILES_LEVELDB_DIR})
ms_target_include_system_dirs(leveldb PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR} ${BoostSourceDir})
if(MSVC)
  target_link_libraries(leveldb ${BoostFilesystemLibs} ${BoostSystemLibs} ${JustThread_LIBRARIES})
else()
  target_link_libraries(leveldb ${BoostFilesystemLibs} ${BoostSystemLibs} ${JustThread_LIBRARIES} -lpthread)
endif()
if(TARGET check_sanitizer_blacklist)
  add_dependencies(leveldb check_sanitizer_blacklist)
endif()
set_target_properties(leveldb PROPERTIES FOLDER "Third Party/LevelDB")


#==================================================================================================#
# Tests                                                                                            #
#==================================================================================================#
if(MaidsafeTesting)
  set(SOURCE_FILES_LEVELDB_TESTHARNESS ${CMAKE_CURRENT_SOURCE_DIR}/util/testutil.cc ${CMAKE_CURRENT_SOURCE_DIR}/util/testharness.cc)
  add_library(leveldb-testharness STATIC ${SOURCE_FILES_LEVELDB_TESTHARNESS})
  ms_target_include_system_dirs(leveldb-testharness PRIVATE ${PROJECT_SOURCE_DIR})
  if(WIN32)
    target_link_libraries(leveldb-testharness leveldb)
  else()
    target_link_libraries(leveldb-testharness leveldb pthread)
  endif()
  target_compile_options(leveldb-testharness
    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/wd4996>
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w>)
  set_target_properties(leveldb-testharness PROPERTIES FOLDER "Third Party/LevelDB")

  macro(leveldb_tests)
    foreach(value ${ARGN})
      get_filename_component(exe_name ${value} NAME_WE)
      get_filename_component(Extension ${value} EXT)
      string(REGEX REPLACE "(.*)_test" "test_leveldb_\\1" exe_name ${exe_name})
      ms_add_executable(${exe_name} "." ${value})
      set_target_properties(${exe_name} PROPERTIES FOLDER "Third Party/LevelDB")
      ms_target_include_system_dirs(${exe_name} PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)
      target_link_libraries(${exe_name} leveldb-testharness)
      target_compile_options(${exe_name}
        PRIVATE
          $<$<CXX_COMPILER_ID:MSVC>:/wd4018 /wd4244 /wd4267 /wd4800 /wd4996>
          $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w $<$<STREQUAL:"${Extension}",".cc">:-std=c++11> ${LibCXX}>)
      list(APPEND AllTests ${exe_name})
    endforeach()
  endmacro()

  leveldb_tests(${CMAKE_CURRENT_SOURCE_DIR}/db/autocompact_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/corruption_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/db_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/dbformat_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/filename_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/log_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/skiplist_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/version_edit_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/version_set_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/db/write_batch_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/issues/issue178_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/issues/issue200_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/table/filter_block_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/table/table_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/util/arena_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/util/bloom_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/util/cache_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/util/coding_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/util/crc32c_test.cc
                ${CMAKE_CURRENT_SOURCE_DIR}/util/env_test.cc)
  if(NOT MSVC)
    leveldb_tests(${CMAKE_CURRENT_SOURCE_DIR}/db/c_test.c)
  endif()

  ms_add_executable(leveldb_benchmark "." ${CMAKE_CURRENT_SOURCE_DIR}/db/db_bench.cc)
  set_target_properties(leveldb_benchmark PROPERTIES FOLDER "Third Party/LevelDB")
  ms_target_include_system_dirs(leveldb_benchmark PRIVATE ${PROJECT_SOURCE_DIR} ${PROJECT_SOURCE_DIR}/include)
  target_link_libraries(leveldb_benchmark leveldb-testharness)
  target_compile_options(leveldb_benchmark
    PRIVATE
      $<$<CXX_COMPILER_ID:MSVC>:/wd4018 /wd4244 /wd4267 /wd4800 /wd4996>
      $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w -std=c++11 ${LibCXX} ${LibCXXAbi}>)

  set(AllLeveldbTests ${AllTests} CACHE INTERNAL "Full list of LevelDB tests.")
endif()

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_flags.cmake)
target_compile_options(leveldb
  PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:/wd4018 /wd4244 /wd4267 /wd4996>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-w -std=c++11>
  PUBLIC
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-pthread ${LibCXX}>)

target_compile_definitions(leveldb PRIVATE $<$<BOOL:${MSVC}>:UNICODE _UNICODE BOOST_ALL_NO_LIB>)


#==================================================================================================#
# Package                                                                                          #
#==================================================================================================#
# install(TARGETS leveldb COMPONENT Development CONFIGURATIONS Debug Release ARCHIVE DESTINATION lib)
# install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ COMPONENT Development DESTINATION include/maidsafe/third_party_libs)

if(MaidsafeTesting)
  install(TARGETS leveldb-testharness ${AllLeveldbTests} COMPONENT Tests CONFIGURATIONS Debug RUNTIME DESTINATION bin/debug ARCHIVE DESTINATION lib)
  install(TARGETS leveldb-testharness ${AllLeveldbTests} COMPONENT Tests CONFIGURATIONS Release RUNTIME DESTINATION bin ARCHIVE DESTINATION lib)
  install(TARGETS leveldb_benchmark COMPONENT Benchmarkss CONFIGURATIONS Release RUNTIME DESTINATION bin)
endif()
