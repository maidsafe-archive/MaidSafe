#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 maidsafe.net limited                                     #
#  All rights reserved.                                                        #
#                                                                              #
#  Redistribution and use in source and binary forms, with or without          #
#  modification, are permitted provided that the following conditions are met: #
#                                                                              #
#      * Redistributions of source code must retain the above copyright        #
#        notice, this list of conditions and the following disclaimer.         #
#      * Redistributions in binary form must reproduce the above copyright     #
#        notice, this list of conditions and the following disclaimer in the   #
#        documentation and/or other materials provided with the distribution.  #
#      * Neither the name of the maidsafe.net limited nor the names of its     #
#        contributors may be used to endorse or promote products derived from  #
#        this software without specific prior written permission.              #
#                                                                              #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  #
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
#  POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Written by maidsafe.net team                                                #
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


UNSET(Cbfs_INCLUDE_DIR CACHE)
UNSET(Cbfs_LIBRARY_DIR CACHE)
UNSET(Cbfs_LIBRARY_DIR_DEBUG CACHE)
UNSET(Cbfs_LIBRARY CACHE)
UNSET(Cbfs_LIBRARY_DEBUG CACHE)

IF(CBFS_ROOT_DIR)
  SET(CBFS_ROOT_DIR ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)
ELSE()
  SET(CBFS_ROOT_DIR "C:/Program Files/EldoS/Callback File System")
ENDIF()

IF(CMAKE_CL_64)
  SET(CBFS_LIBPATH_SUFFIX "SourceCode/CallbackFS/CPP/x64/Release" "CPP/VC2008/64bit/static_runtime(MT)")
  SET(CBFS_LIBPATH_SUFFIX_DEBUG "SourceCode/CallbackFS/CPP/x64/Debug" "CPP/VC2008/64bit/static_runtime(MT)")
  SET(CBFS_INCPATH_SUFFIX "SourceCode/CallbackFS/CPP" "CPP/VC2008/64bit/static_runtime(MT)")
ELSE()
  SET(CBFS_LIBPATH_SUFFIX "SourceCode/CallbackFS/CPP/Release" "CPP/VC2008/32bit/static_runtime(MT)")
  SET(CBFS_LIBPATH_SUFFIX_DEBUG "SourceCode/CallbackFS/CPP/Debug" "CPP/VC2008/32bit/static_runtime(MT)")
  SET(CBFS_INCPATH_SUFFIX "SourceCode/CallbackFS/CPP" "CPP/VC2008/32bit/static_runtime(MT)")
ENDIF()

FIND_LIBRARY(Cbfs_LIBRARY NAMES cbfs PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_LIBPATH_SUFFIX} NO_DEFAULT_PATH)
FIND_LIBRARY(Cbfs_LIBRARY_DEBUG NAMES cbfs PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_LIBPATH_SUFFIX_DEBUG} NO_DEFAULT_PATH)
FIND_PATH(Cbfs_INCLUDE_DIR CbFS.h PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_INCPATH_SUFFIX} NO_DEFAULT_PATH)

IF(NOT Cbfs_LIBRARY)
  SET(ERROR_MESSAGE "\nCould not find Callback File System.  NO CBFS LIBRARY - ")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If Cbfs is already installed, run:\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=<Path to Cbfs root directory>\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=\"C:\\Program Files\\EldoS\\Callback File System\"\n\n")
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF()

IF(NOT Cbfs_LIBRARY_DEBUG)
  SET(ERROR_MESSAGE "\nCould not find Callback File System.  NO CBFS DEBUG LIBRARY - ")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If Cbfs is already installed, run:\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=<Path to Cbfs root directory>\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=\"C:\\Program Files\\EldoS\\Callback File System\"\n\n")
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF()

IF(NOT Cbfs_INCLUDE_DIR)
  SET(ERROR_MESSAGE "\nCould not find Callback File System.  NO CBFS.H - ")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If Cbfs is already installed, run:\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=<Path to Cbfs root directory>\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=\"C:\\Program Files\\EldoS\\Callback File System\"\n\n")
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF()

GET_FILENAME_COMPONENT(CBFS_ROOT_DIR ${Cbfs_LIBRARY} PATH)
SET(Cbfs_LIBRARY_DIR ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)
GET_FILENAME_COMPONENT(CBFS_ROOT_DIR ${Cbfs_LIBRARY_DEBUG} PATH)
SET(Cbfs_LIBRARY_DIR_DEBUG ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)

INCLUDE_DIRECTORIES(SYSTEM ${Cbfs_INCLUDE_DIR})
IF(CMAKE_INCLUDE_DIRECTORIES_BEFORE)
  SET(INCLUDE_DIRS ${Cbfs_INCLUDE_DIR} ${INCLUDE_DIRS})
ELSE()
  SET(INCLUDE_DIRS ${INCLUDE_DIRS} ${Cbfs_INCLUDE_DIR})
ENDIF()

MESSAGE("-- Found library ${Cbfs_LIBRARY}")
MESSAGE("-- Found library ${Cbfs_LIBRARY_DEBUG}")
