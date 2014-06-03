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
#                                                                                                  #
#  Module used to locate Filesystem in Userspace (FUSE) lib and header.                            #
#                                                                                                  #
#  Settable variables to aid with finding FUSE are:                                                #
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
  if(NOT Fuse_INCLUDE_DIR)
    set(ERROR_MESSAGE "\nCould not find include directory for OSXFUSE.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\ncmake . -DADD_FUSE_INCLUDE_DIR=<Path to osxfuse include directory>")
    message(FATAL_ERROR ${ERROR_MESSAGE})
  endif()
elseif(BSD)
  # FreeBSD 10 and later has fuse built in as standard, but headers may not be installed
  find_library(Fuse_LIBRARY libfuse.so)
  find_path(Fuse_INCLUDE_DIR fuse/fuse.h PATHS "/usr/local/include" ${ADD_FUSE_INCLUDE_DIR} NO_DEFAULT_PATH)
  if(NOT Fuse_INCLUDE_DIR)
    set(ERROR_MESSAGE "\nCould not find include directory for FUSE.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}  Try 'pkg install fusefs-libs'")
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
    set(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\ncmake . -DADD_LIBRARY_DIR=<Path to libosxfuse_i64.dylib directory>")
  else()
    set(ERROR_MESSAGE "\nCould not find library libfuse.so.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\ncmake . -DADD_LIBRARY_DIR=<Path to libfuse directory>")
  endif()
  message(FATAL_ERROR ${ERROR_MESSAGE})
endif()
