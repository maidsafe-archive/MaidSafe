cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
project(leveldb)

include(CheckIncludeFile)
include(FindBoost) # for Windows...

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)
include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_flags.cmake)

ENABLE_TESTING()

include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

SET (HEADER_FILES_LEVELDB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/cache.h
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
                              ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/write_batch.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/db/builder.h
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
                              ${CMAKE_CURRENT_SOURCE_DIR}/db/write_batch_internal.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/port/atomic_pointer.h
							  ${CMAKE_CURRENT_SOURCE_DIR}/port/port.h
							  ${CMAKE_CURRENT_SOURCE_DIR}/port/port_example.h
							  ${CMAKE_CURRENT_SOURCE_DIR}/port/thread_annotations.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/block.h
	                          ${CMAKE_CURRENT_SOURCE_DIR}/table/block_builder.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/filter_block.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/format.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/iterator_wrapper.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/merger.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/two_level_iterator.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/arena.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/coding.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/crc32c.h
	                          ${CMAKE_CURRENT_SOURCE_DIR}/util/hash.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/histogram.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/logging.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/mutexlock.h
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/random.h)

SET (SOURCE_FILES_LEVELDB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/db/builder.cc
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
                              ${CMAKE_CURRENT_SOURCE_DIR}/db/write_batch.cc
							  ${CMAKE_CURRENT_SOURCE_DIR}/table/block.cc
	                          ${CMAKE_CURRENT_SOURCE_DIR}/table/block_builder.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/filter_block.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/format.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/iterator.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/merger.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/table.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/table_builder.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/table/two_level_iterator.cc
                              ${CMAKE_CURRENT_SOURCE_DIR}/util/arena.cc
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



# OS detection
if(WIN32)
    add_definitions(-DLEVELDB_PLATFORM_WINDOWS -DOS_WIN -DWIN32_LEAN_AND_MEAN)
    if(MSVC)
        add_definitions(-DCOMPILER_MSVC)
		set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
    endif()
    list(APPEND HEADER_FILES_LEVELDB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/win32/env_win32.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/win32/leveldb.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/win32/port_win32.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/win32/stdint.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/win32/unistd.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/win32/win32exports.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-internal.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-stubs-public.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-sinksource.h
                                             ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-stubs-internal.h)
    list(APPEND SOURCE_FILES_LEVELDB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/win32/env_win32.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/win32/port_win32.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-sinksource.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-stubs-internal.cc)

elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD")
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_FREEBSD)
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
    # link pthread
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
#    list(REMOVE_ITEM leveldb_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/util/win_logger.cc
#                                     ${CMAKE_CURRENT_SOURCE_DIR}/port/port_win.cc)
#    list(REMOVE_ITEM leveldb_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/util/win_logger.h
#                                     ${CMAKE_CURRENT_SOURCE_DIR}/port/port_win.h)

    list(APPEND HEADER_FILES_LEVELDB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/port/port_posix.h
	                                 ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy.h
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-internal.h
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-stubs-public.h
	                                 ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-sinksource.h
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-stubs-internal.h
                                         ${CMAKE_CURRENT_SOURCE_DIR}/include/leveldb/c.h)

    list(APPEND SOURCE_FILES_LEVELDB_DIR ${CMAKE_CURRENT_SOURCE_DIR}//port/port_posix.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/util/env_posix.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-sinksource.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/snappy/snappy-stubs-internal.cc
                                         ${CMAKE_CURRENT_SOURCE_DIR}/db/c.cc)

elseif(CYGWIN)
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_LINUX)
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
elseif(APPLE)
    add_definitions(-DLEVELDB_PLATFORM_POSIX -DOS_MACOSX)
    set(PORT_SRCS port/port_posix.cc util/env_posix.cc)
    set(CIF_SRCS db/c.cc)
endif()

  
#LEVELDB_SOURCE_PROPERTIES(LevelDB "${HEADER_FILES_LEVELDB_DIR}" "$ {SOURCE_FILES_LEVELDB_DIR}") 

#MACRO(LEVELDB_SOURCE_PROPERTIES NAME HEADERS SOURCES)
#  source_group(${CMAKE_CURRENT_SOURCE_DIR}\\${NAME} FILES ${HEADERS} ${SOURCES})
#ENDMACRO (LEVELDB_SOURCE_PROPERTIES NAME HEADERS SOURCES)


# FIXME: check cstdatomic here..

set(TESTHARNESS_SRCS util/testutil.cc util/testharness.cc)

include_directories(${CMAKE_CURRENT_BINARY_DIR})

# CHECK_INCLUDE_FILE(stdint.h HAVE_STDINT_H)

#==================================================================================================#
# Define library                                                                                   #
#==================================================================================================#
add_library(leveldb STATIC ${HEADER_FILES_LEVELDB_DIR}  ${SOURCE_FILES_LEVELDB_DIR})
target_link_libraries(leveldb ${BoostChronoLibs} ${BoostDateTimeLibs} ${BoostFilesystemLibs} ${BoostSystemLibs} ${BoostThreadLibs})
set_target_properties(leveldb PROPERTIES FOLDER "Third Party/LevelDB")
if(JustThread_FOUND)
  target_link_libraries(leveldb optimized ${JustThread_LIBRARY} debug ${JustThread_LIBRARY_DEBUG})
endif()

#==================================================================================================#
# Tests                                                                                            #
#==================================================================================================#
add_library(leveldb-testharness STATIC ${TESTHARNESS_SRCS})
target_link_libraries(leveldb-testharness leveldb)
set_target_properties(leveldb-testharness PROPERTIES FOLDER "Third Party/LevelDB/Tests")

macro(leveldb_tests ${name})
endmacro()

macro(leveldb_tests)
  foreach(value ${ARGN})
    get_filename_component(name ${value} NAME)
      add_executable(${name} ${value})
      add_test(NAME ${name} COMMAND ${name})
      if(WIN32)
        target_link_libraries(${name} leveldb-testharness leveldb)
      else()
        target_link_libraries(${name} leveldb-testharness pthread)
      endif()
    set_target_properties(${name} PROPERTIES FOLDER "Third Party/LevelDB/Tests")
  endforeach()
endmacro()

leveldb_tests(db/db_bench.cc
              db/c_test.c
              db/corruption_test.cc
              db/db_test.cc
              db/dbformat_test.cc
              db/log_test.cc
              db/skiplist_test.cc
              db/version_edit_test.cc
              db/version_set_test.cc
              db/write_batch_test.cc
              table/table_test.cc
              util/arena_test.cc
              util/bloom_test.cc
              util/cache_test.cc
              util/coding_test.cc
              util/crc32c_test.cc
              util/env_test.cc
             )
