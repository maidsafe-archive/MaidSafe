#==============================================================================#
#                                                                              #
#  Copyright (c) 2012 MaidSafe.net limited                                     #
#                                                                              #
#  The following source code is property of MaidSafe.net limited and is not    #
#  meant for external use.  The use of this code is governed by the license    #
#  file licence.txt found in the root directory of this project and also on    #
#  www.maidsafe.net.                                                           #
#                                                                              #
#  You are not free to copy, amend or otherwise use this source code without   #
#  the explicit written permission of the board of directors of MaidSafe.net.  #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Module used to locate Callback File System (CBFS) lib and header.           #
#                                                                              #
#  Settable variables to aid with finding CBFS are:                            #
#    CBFS_ROOT_DIR                                                             #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    Cbfs_INCLUDE_DIR, Cbfs_LIBRARY_DIR, Cbfs_LIBRARY_DIR_DEBUG, Cbfs_LIBRARY, #
#    and Cbfs_LIBRARY_DEBUG                                                    #
#                                                                              #
#==============================================================================#


unset(Cbfs_INCLUDE_DIR CACHE)
unset(Cbfs_LIBRARY_DIR CACHE)
unset(Cbfs_LIBRARY_DIR_DEBUG CACHE)
unset(Cbfs_LIBRARY CACHE)
unset(Cbfs_LIBRARY_DEBUG CACHE)

if(CBFS_ROOT_DIR)
  set(CBFS_ROOT_DIR ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)
else()
  set(CBFS_ROOT_DIR "C:/Program Files/EldoS/Callback File System")
endif()

if(CMAKE_CL_64)
  set(CBFS_LIBPATH_SUFFIX "SourceCode/CallbackFS/CPP/x64/Release" "CPP/VC2008/64bit/static_runtime(MT)")
  set(CBFS_LIBPATH_SUFFIX_DEBUG "SourceCode/CallbackFS/CPP/x64/Debug" "CPP/VC2008/64bit/static_runtime(MT)")
  set(CBFS_INCPATH_SUFFIX "SourceCode/CallbackFS/CPP" "CPP/VC2008/64bit/static_runtime(MT)")
else()
  set(CBFS_LIBPATH_SUFFIX "SourceCode/CallbackFS/CPP/Release" "CPP/VC2008/32bit/static_runtime(MT)")
  set(CBFS_LIBPATH_SUFFIX_DEBUG "SourceCode/CallbackFS/CPP/Debug" "CPP/VC2008/32bit/static_runtime(MT)")
  set(CBFS_INCPATH_SUFFIX "SourceCode/CallbackFS/CPP" "CPP/VC2008/32bit/static_runtime(MT)")
endif()

find_library(Cbfs_LIBRARY NAMES cbfs PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_LIBPATH_SUFFIX} NO_DEFAULT_PATH)
find_library(Cbfs_LIBRARY_DEBUG NAMES cbfs PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_LIBPATH_SUFFIX_DEBUG} NO_DEFAULT_PATH)
find_path(Cbfs_INCLUDE_DIR CbFS.h PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_INCPATH_SUFFIX} NO_DEFAULT_PATH)

if(NOT Cbfs_LIBRARY)
  set(ERROR_MESSAGE "\nCould not find Callback File System.  NO CBFS LIBRARY - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Cbfs is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=<Path to Cbfs root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=\"C:\\Program Files\\EldoS\\Callback File System\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

if(NOT Cbfs_LIBRARY_DEBUG)
  set(ERROR_MESSAGE "\nCould not find Callback File System.  NO CBFS DEBUG LIBRARY - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Cbfs is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=<Path to Cbfs root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=\"C:\\Program Files\\EldoS\\Callback File System\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

if(NOT Cbfs_INCLUDE_DIR)
  set(ERROR_MESSAGE "\nCould not find Callback File System.  NO CBFS.H - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Cbfs is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=<Path to Cbfs root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=\"C:\\Program Files\\EldoS\\Callback File System\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

get_filename_component(CBFS_ROOT_DIR ${Cbfs_LIBRARY} PATH)
set(Cbfs_LIBRARY_DIR ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)
get_filename_component(CBFS_ROOT_DIR ${Cbfs_LIBRARY_DEBUG} PATH)
set(Cbfs_LIBRARY_DIR_DEBUG ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)

include_directories(SYSTEM ${Cbfs_INCLUDE_DIR})
if(CMAKE_INCLUDE_DIRECTORIES_BEFORE)
  set(INCLUDE_DIRS ${Cbfs_INCLUDE_DIR} ${INCLUDE_DIRS})
else()
  set(INCLUDE_DIRS ${INCLUDE_DIRS} ${Cbfs_INCLUDE_DIR})
endif()

message(STATUS "Found library ${Cbfs_LIBRARY}")
message(STATUS "Found library ${Cbfs_LIBRARY_DEBUG}")
