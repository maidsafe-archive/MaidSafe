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
project(snappy)

include(CheckIncludeFile)
include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)

enable_testing()

#==================================================================================================#
# Files                                                                                            #
#==================================================================================================#

include_directories(${CMAKE_CURRENT_SOURCE_DIR} include)

set(HEADER_FILES_SNAPPY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/snappy.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/snappy-internal.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/snappy-sinksource.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/snappy-stubs-internal.h
                            ${CMAKE_CURRENT_SOURCE_DIR}/snappy-stubs-public.h)

set(SOURCE_FILES_SNAPPY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/snappy.cc
                            ${CMAKE_CURRENT_SOURCE_DIR}/snappy-sinksource.cc
                            ${CMAKE_CURRENT_SOURCE_DIR}/snappy-stubs-internal.cc)

#==================================================================================================#
# Groups                                                                                           #
#==================================================================================================#

source_group("Header Files" FILES ${HEADER_FILES_SNAPPY_DIR})
source_group("Source Files" FILES ${SOURCE_FILES_SNAPPY_DIR})

set(ALL_FILES_SNAPPY_DIR ${HEADER_FILES_SNAPPY_DIR} ${SOURCE_FILES_SNAPPY_DIR})

#==================================================================================================#
# Library                                                                                          #
#==================================================================================================#
include_directories(${CMAKE_CURRENT_BINARY_DIR})
add_library(snappy STATIC ${ALL_FILES_SNAPPY_DIR})
target_link_libraries(snappy ${BoostChronoLibs} ${BoostDateTimeLibs} ${BoostFilesystemLibs} ${BoostSystemLibs} ${BoostThreadLibs})
set_target_properties(snappy PROPERTIES FOLDER "Third Party/Snappy")
if(JustThread_FOUND)
  target_link_libraries(snappy optimized ${JustThread_LIBRARY} debug ${JustThread_LIBRARY_DEBUG})
endif()

#==================================================================================================#
# Tests                                                                                            #
#==================================================================================================#

#macro(snappy_tests)
#  foreach(value ${ARGN})
#    get_filename_component(exe_name ${value} NAME_WE)
#    add_executable(${exe_name} ${value})
#    set(AllExesForCurrentProject ${AllExesForCurrentProject} ${exe_name})
#    add_test(NAME ${exe_name} COMMAND ${exe_name})
#    set_target_properties(${exe_name} PROPERTIES FOLDER "Third Party/Snappy/Tests")
#  endforeach()
#endmacro()

#snappy_tests(${CMAKE_CURRENT_SOURCE_DIR}/snappy-unittest.cc
#             ${CMAKE_CURRENT_SOURCE_DIR}/snappy-test.cc)

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_flags.cmake)
if(MSVC)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
endif()
