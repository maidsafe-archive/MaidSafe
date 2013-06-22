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
#  Module used to locate Filesystem in Userspace (FUSE) lib and header.                            #
#                                                                                                  #
#  Settable variables to aid with finding CBFS are:                                                #
#    ADD_FUSE_INCLUDE_DIR                                                                          #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    Fuse_INCLUDE_DIR, Fuse_LIBRARY, Fuse_LIBRARY_DIR                                              #
#                                                                                                  #
#==================================================================================================#


unset(Fuse_INCLUDE_DIR CACHE)
unset(Fuse_LIBRARY CACHE)
unset(Fuse_LIBRARY_DIR CACHE)

if(WIN32)
  message(FATAL_ERROR "Invalid Library Search requested. Use maidsafe_find_cbfs.cmake for Windows")
endif()

if(APPLE)
  find_library(Fuse_LIBRARY libosxfuse_i64.dylib)
  find_path(Fuse_INCLUDE_DIR fuse.h PATHS "/usr/local/include/osxfuse" ${ADD_FUSE_INCLUDE_DIR} NO_DEFAULT_PATH)
  get_filename_component(Fuse_INCLUDE_DIR ${Fuse_INCLUDE_DIR} PATH)
  if(NOT Fuse_INCLUDE_DIR)
    set(ERROR_MESSAGE "\nCould not find include directory for OSXFUSE.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\n${ERROR_MESSAGE_CMAKE_PATH} -DADD_FUSE_INCLUDE_DIR=<Path to osxfuse include directory>")
    message(FATAL_ERROR ${ERROR_MESSAGE})
  endif()
else()
  find_library(Fuse_LIBRARY libfuse.so)
endif()

if(Fuse_LIBRARY)
  message(STATUS "Found library ${Fuse_LIBRARY}")
  get_filename_component(Fuse_LIBRARY_DIR ${Fuse_LIBRARY} PATH)
else()
  if(APPLE)
    set(ERROR_MESSAGE "\nCould not find library libosxfuse_i64.dylib.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\n${ERROR_MESSAGE_CMAKE_PATH} -DADD_LIBRARY_DIR=<Path to libosxfuse_i64.dylib directory>")
  else()
    set(ERROR_MESSAGE "\nCould not find library libfuse.so.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\n${ERROR_MESSAGE_CMAKE_PATH} -DADD_LIBRARY_DIR=<Path to libfuse directory>")
  endif()
  message(FATAL_ERROR {ERROR_MESSAGE})
endif()

if(APPLE)
  include_directories(SYSTEM ${Fuse_INCLUDE_DIR})
  if(CMAKE_INCLUDE_DIRECTORIES_BEFORE)
    set(INCLUDE_DIRS ${Fuse_INCLUDE_DIR} ${INCLUDE_DIRS})
  else()
    set(INCLUDE_DIRS ${INCLUDE_DIRS} ${Fuse_INCLUDE_DIR})
  endif()
endif()
