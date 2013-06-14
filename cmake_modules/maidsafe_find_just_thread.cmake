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
#                                                                                                  #
#  Module used to locate the just::thread include directories and libraries.                       #
#                                                                                                  #
#  To enable checked libraries, set the following variable to ON:                                  #
#    JUST_THREAD_DEADLOCK_CHECK                                                                    #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    JustThread_INCLUDE_DIR, JustThread_LIBRARY_DIR, JustThread_LIBRARY,                           #
#    JustThread_LIBRARY_DEBUG, JustThread_LIBRARIES, and JustThread_FOUND.                         #
#                                                                                                  #
#==================================================================================================#


unset(JustThread_INCLUDE_DIR CACHE)
unset(JustThread_LIBRARY_DIR CACHE)
unset(JustThread_LIBRARY CACHE)
unset(JustThread_LIBRARY_DEBUG CACHE)
unset(JustThread_LIBRARIES CACHE)
unset(JustThread_FOUND CACHE)

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(USE_JUST_THREADS FALSE)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" AND CMAKE_CXX_COMPILER_VERSION VERSION_GREATER "4.7")
  set(USE_JUST_THREADS FALSE)
endif()

if(NOT USE_JUST_THREADS AND NOT JUST_THREAD_ROOT_DIR)
  unset(JustThread_LIBRARIES)
  return()
endif()
message("${HR}")

if(JUST_THREAD_ROOT_DIR)
  set(JUST_THREAD_ROOT_DIR ${JUST_THREAD_ROOT_DIR} CACHE PATH "Path to just::thread root directory" FORCE)
else()
  set(JUST_THREAD_ROOT_DIR
        "C:/Program Files/JustSoftwareSolutions/JustThread"
        "D:/Program Files/JustSoftwareSolutions/JustThread"
        "E:/Program Files/JustSoftwareSolutions/JustThread"
        "C:/Program Files (x86)/JustSoftwareSolutions/JustThread"
        "D:/Program Files (x86)/JustSoftwareSolutions/JustThread"
        "E:/Program Files (x86)/JustSoftwareSolutions/JustThread"
        "/usr")
endif()

if(MSVC)
  if(CMAKE_CL_64)
    set(JustThread_LIB_NAME "justthread_vc11x64_md")
    if(JUST_THREAD_DEADLOCK_CHECK)
      set(JustThread_LIB_NAME_DEBUG "justthread_check_vc11x64_mdd")
    else()
      set(JustThread_LIB_NAME_DEBUG "justthread_vc11x64_mdd")
    endif()
  else()
    set(JustThread_LIB_NAME "justthread_vc11_md")
    if(JUST_THREAD_DEADLOCK_CHECK)
      set(JustThread_LIB_NAME_DEBUG "justthread_check_vc11_mdd")
    else()
      set(JustThread_LIB_NAME_DEBUG "justthread_vc11_mdd")
    endif()
  endif()
else()
  set(JustThread_LIB_NAME "libjustthread.a")
  if(JUST_THREAD_DEADLOCK_CHECK)
    set(JustThread_LIB_NAME_DEBUG "libjustthread_check.a")
  else()
    set(JustThread_LIB_NAME_DEBUG "libjustthread.a")
  endif()
endif()

find_library(JustThread_LIBRARY NAMES ${JustThread_LIB_NAME} PATHS ${JUST_THREAD_ROOT_DIR} PATH_SUFFIXES lib  lib32 lib64 NO_DEFAULT_PATH)
find_library(JustThread_LIBRARY_DEBUG NAMES ${JustThread_LIB_NAME_DEBUG} PATHS ${JUST_THREAD_ROOT_DIR} PATH_SUFFIXES lib lib32 lib64 NO_DEFAULT_PATH)
find_path(JustThread_INCLUDE_DIR thread PATHS ${JUST_THREAD_ROOT_DIR} PATH_SUFFIXES include include/justthread  NO_DEFAULT_PATH)

if(NOT JustThread_LIBRARY)
  set(ERROR_MESSAGE "\nCould not find just::thread.  NO just::thread LIBRARY - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If just::thread is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DJUST_THREAD_ROOT_DIR=<Path to just::thread root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DJUST_THREAD_ROOT_DIR=\"C:\\Program Files\\JustSoftwareSolutions\\JustThread\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

if(NOT JustThread_LIBRARY_DEBUG)
  set(ERROR_MESSAGE "\nCould not find just::thread.  NO just::thread DEBUG LIBRARY - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If just::thread is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DJUST_THREAD_ROOT_DIR=<Path to just::thread root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DJUST_THREAD_ROOT_DIR=\"C:\\Program Files\\JustSoftwareSolutions\\JustThread\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

if(NOT JustThread_INCLUDE_DIR)
  set(ERROR_MESSAGE "\nCould not find just::thread.  NO thread HEADER - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If just::thread is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DJUST_THREAD_ROOT_DIR=<Path to just::thread root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DJUST_THREAD_ROOT_DIR=\"C:\\Program Files\\JustSoftwareSolutions\\JustThread\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

set(JustThread_FOUND 1 CACHE INTERNAL "" FORCE)

get_filename_component(JUST_THREAD_ROOT_DIR ${JustThread_LIBRARY} PATH)
set(JustThread_LIBRARY_DIR ${JUST_THREAD_ROOT_DIR} CACHE PATH "Path to just::thread library directory" FORCE)

include_directories(SYSTEM ${JustThread_INCLUDE_DIR})
if(CMAKE_INCLUDE_DIRECTORIES_BEFORE)
  set(INCLUDE_DIRS ${JustThread_INCLUDE_DIR} ${INCLUDE_DIRS})
else()
  set(INCLUDE_DIRS ${INCLUDE_DIRS} ${JustThread_INCLUDE_DIR})
endif()

set(JustThread_LIBRARIES optimized ${JustThread_LIBRARY} debug ${JustThread_LIBRARY_DEBUG})
if(UNIX AND NOT APPLE)
  set(JustThread_LIBRARIES ${JustThread_LIBRARIES} rt pthread)
  set(JustThread_LIBRARY ${JustThread_LIBRARY} rt pthread)
  set(JustThread_LIBRARY_DEBUG ${JustThread_LIBRARY_DEBUG} rt pthread)
endif()

message(STATUS "Found library ${JustThread_LIBRARY}")
message(STATUS "Found library ${JustThread_LIBRARY_DEBUG}")
