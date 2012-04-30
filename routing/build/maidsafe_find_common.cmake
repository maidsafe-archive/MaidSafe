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
#  Module used to locate MaidSafe-Common tools, cmake modules and the          #
#    maidsafe_common libs and headers.                                         #
#                                                                              #
#  Settable variables to aid with finding MaidSafe-Common are:                 #
#    MAIDSAFE_COMMON_INSTALL_DIR                                               #
#                                                                              #
#  If found, a target named maidsafe_common_static is imported.                #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    MaidSafeCommon_INCLUDE_DIR, MaidSafeCommon_MODULES_DIR,                   #
#    MaidSafeCommon_TOOLS_DIR and MAIDSAFE_COMMON_VERSION.                     #
#                                                                              #
#==============================================================================#

UNSET(MaidSafeCommon_MODULES_DIR CACHE)

IF(NOT MAIDSAFE_COMMON_INSTALL_DIR)
  IF(DEFAULT_THIRD_PARTY_ROOT)
    SET(MAIDSAFE_COMMON_INSTALL_DIR ${DEFAULT_THIRD_PARTY_ROOT})
  ELSE()
    GET_FILENAME_COMPONENT(MAIDSAFE_COMMON_INSTALL_DIR ${PROJECT_SOURCE_DIR} PATH)
    SET(MAIDSAFE_COMMON_INSTALL_DIR ${MAIDSAFE_COMMON_INSTALL_DIR}/MaidSafe-Common/installed)
  ENDIF()
ENDIF()

FIND_PATH(MaidSafeCommon_MODULES_DIR maidsafe_run_protoc.cmake
            PATHS ${MAIDSAFE_COMMON_INSTALL_DIR}
            PATH_SUFFIXES share/maidsafe/cmake_modules
            NO_SYSTEM_ENVIRONMENT_PATH
            NO_CMAKE_SYSTEM_PATH)

IF(NOT MaidSafeCommon_MODULES_DIR)
  SET(ERROR_MESSAGE "\nCould not find MaidSafe Common - ${MaidSafeCommon_MODULES_DIR}.\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}You can clone it at git@github.com:maidsafe/MaidSafe-Common.git\n\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If MaidSafe Common is already installed, run:")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}\n${ERROR_MESSAGE_CMAKE_PATH} -DMAIDSAFE_COMMON_INSTALL_DIR=<Path to MaidSafe Common install directory>\n\n")
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF()

SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${MaidSafeCommon_MODULES_DIR})
INCLUDE(maidsafe_initialise_common)
