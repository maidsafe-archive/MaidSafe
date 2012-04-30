#==============================================================================#
#                                                                              #
# Copyright [2011] maidsafe.net limited                                        #
#                                                                              #
# Description:  See below.                                                     #
# Version:      1.0                                                            #
# Created:      2011-03-20-23.22.31                                            #
# Revision:     none                                                           #
# Company:      maidsafe.net limited                                           #
#                                                                              #
# The following source code is property of maidsafe.net limited and is not     #
# meant for external use.  The use of this code is governed by the license     #
# file LICENSE.TXT found in the root of this directory and also on             #
# www.maidsafe.net.                                                            #
#                                                                              #
# You are not free to copy, amend or otherwise use this source code without    #
# the explicit written permission of the board of directors of maidsafe.net.   #
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
