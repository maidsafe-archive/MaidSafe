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
#  Module used to locate MaidSafe-Common tools, and the                        #
#    maidsafe_common libs and headers.                                         #
#                                                                              #
#  Settable variables to aid with finding MaidSafe-Common are:                 #
#    MAIDSAFE_COMMON_INSTALL_DIR                                               #
#                                                                              #
#  If found, a target named maidsafe_common_static is imported.                #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    MaidSafeCommon_INCLUDE_DIR, MaidSafeCommon_TOOLS_DIR and                  #
#    MAIDSAFE_COMMON_VERSION.                                                  #
#                                                                              #
#==============================================================================#


FUNCTION(ADD_MAIDSAFE_LIBRARY MAIDSAFE_LIBRARY_NAME)
  STRING(TOLOWER ${MAIDSAFE_LIBRARY_NAME} LIBRARY_NAME)
  STRING(REGEX REPLACE "-" "_" LIBRARY_NAME ${LIBRARY_NAME})
  FIND_FILE(IMPORT_CMAKE ${LIBRARY_NAME}.cmake PATHS ${MAIDSAFE_COMMON_INSTALL_DIR}/share/maidsafe)
  IF(NOT IMPORT_CMAKE)
    MESSAGE(FATAL_ERROR "\n\nThis project needs to have ${MAIDSAFE_LIBRARY_NAME} installed.\n\n")
  ENDIF()
  INCLUDE(${IMPORT_CMAKE})
  UNSET(IMPORT_CMAKE CACHE)

  MESSAGE("-- Found ${MAIDSAFE_LIBRARY_NAME} library:")
  GET_TARGET_PROPERTY(LIB_NAME_RELEASE ${LIBRARY_NAME}_static LOCATION_RELEASE)
  GET_TARGET_PROPERTY(LIB_NAME_DEBUG ${LIBRARY_NAME}_static LOCATION_DEBUG)
  GET_TARGET_PROPERTY(LIB_NAME_RELWITHDEBINFO ${LIBRARY_NAME}_static LOCATION_RELWITHDEBINFO)
  GET_TARGET_PROPERTY(LIB_NAME_MINSIZEREL ${LIBRARY_NAME}_static LOCATION_MINSIZEREL)
  IF(NOT LIB_NAME_RELEASE STREQUAL "")
    GET_FILENAME_COMPONENT(LIB_NAME_RELEASE ${LIB_NAME_RELEASE} NAME)
    MESSAGE("--   using ${LIB_NAME_RELEASE} for Release builds.")
  ENDIF()
  IF(NOT LIB_NAME_DEBUG STREQUAL "")
    GET_FILENAME_COMPONENT(LIB_NAME_DEBUG ${LIB_NAME_DEBUG} NAME)
    MESSAGE("--   using ${LIB_NAME_DEBUG} for Debug builds.")
  ENDIF()
  IF(NOT LIB_NAME_RELWITHDEBINFO STREQUAL "")
    GET_FILENAME_COMPONENT(LIB_NAME_RELWITHDEBINFO ${LIB_NAME_RELWITHDEBINFO} NAME)
    MESSAGE("--   using ${LIB_NAME_RELWITHDEBINFO} for RelWithDebInfo builds.")
  ENDIF()
  IF(NOT LIB_NAME_MINSIZEREL STREQUAL "")
    GET_FILENAME_COMPONENT(LIB_NAME_MINSIZEREL ${LIB_NAME_MINSIZEREL} NAME)
    MESSAGE("--   using ${LIB_NAME_MINSIZEREL} for MinSizeRel builds.")
  ENDIF()
  
  # Try for <lib name>_VERSION
  STRING(TOUPPER ${LIBRARY_NAME} VERSION_NAME)
  SET(VERSION_NAME ${VERSION_NAME}_VERSION)
  IF(NOT ${${VERSION_NAME}} STRGREATER 0)
    # Try for MAIDSAFE_<lib name>_VERSION
    SET(MAIDSAFE_VERSION_NAME MAIDSAFE_${VERSION_NAME})
    IF(NOT ${${MAIDSAFE_VERSION_NAME}} STRGREATER 0)
      # Try for SIGMOID_<lib name>_VERSION
      SET(SIGMOID_VERSION_NAME SIGMOID_${VERSION_NAME})
      IF(NOT ${${SIGMOID_VERSION_NAME}} STRGREATER 0)
        SET(${VERSION_NAME} "NOT FOUND")
      ELSE()
        SET(VERSION_NAME ${SIGMOID_VERSION_NAME})
      ENDIF()
    ELSE()
      SET(VERSION_NAME ${MAIDSAFE_VERSION_NAME})
    ENDIF()
  ENDIF()
  SET(${VERSION_NAME} ${${VERSION_NAME}} PARENT_SCOPE)
ENDFUNCTION()

UNSET(MaidSafeCommon_INCLUDE_DIR CACHE)
UNSET(MaidSafeCommon_TOOLS_DIR CACHE)

IF(NOT MAIDSAFE_COMMON_INSTALL_DIR)
  IF(DEFAULT_THIRD_PARTY_ROOT)
    SET(MAIDSAFE_COMMON_INSTALL_DIR ${DEFAULT_THIRD_PARTY_ROOT})
  ELSE()
    GET_FILENAME_COMPONENT(MAIDSAFE_COMMON_INSTALL_DIR ${PROJECT_SOURCE_DIR} PATH)
    SET(MAIDSAFE_COMMON_INSTALL_DIR ${MAIDSAFE_COMMON_INSTALL_DIR}/MaidSafe-Common/installed)
  ENDIF()
ENDIF()

SET(MAIDSAFE_PATH_SUFFIX share/maidsafe)
FIND_FILE(MAIDSAFE_THIRD_PARTY_CMAKE maidsafe_third_party.cmake PATHS ${MAIDSAFE_COMMON_INSTALL_DIR} PATH_SUFFIXES ${MAIDSAFE_PATH_SUFFIX} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
FIND_FILE(BOOST_LIBS_CMAKE boost_libs.cmake PATHS ${MAIDSAFE_COMMON_INSTALL_DIR} PATH_SUFFIXES ${MAIDSAFE_PATH_SUFFIX} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
FIND_FILE(MAIDSAFE_COMMON_CMAKE maidsafe_common.cmake PATHS ${MAIDSAFE_COMMON_INSTALL_DIR} PATH_SUFFIXES ${MAIDSAFE_PATH_SUFFIX} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
IF(MAIDSAFE_THIRD_PARTY_CMAKE AND BOOST_LIBS_CMAKE AND MAIDSAFE_COMMON_CMAKE)
  INCLUDE(${MAIDSAFE_THIRD_PARTY_CMAKE})
  INCLUDE(${BOOST_LIBS_CMAKE})
  INCLUDE(${MAIDSAFE_COMMON_CMAKE})
ENDIF()

SET(MAIDSAFE_PATH_SUFFIX include)
FIND_PATH(MaidSafeCommon_INCLUDE_DIR maidsafe/common/version.h PATHS ${MAIDSAFE_COMMON_INC_DIR} ${MAIDSAFE_COMMON_INSTALL_DIR} PATH_SUFFIXES ${MAIDSAFE_PATH_SUFFIX} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

SET(MAIDSAFE_PATH_SUFFIX share/maidsafe/tools)
FIND_PATH(MaidSafeCommon_TOOLS_DIR cpplint.py PATHS ${MAIDSAFE_COMMON_INSTALL_DIR} PATH_SUFFIXES ${MAIDSAFE_PATH_SUFFIX} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

SET(MAIDSAFE_PATH_SUFFIX ../../..)
FIND_PATH(DEFAULT_THIRD_PARTY_ROOT README PATHS ${MAIDSAFE_COMMON_INSTALL_DIR} PATH_SUFFIXES ${MAIDSAFE_PATH_SUFFIX} NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

SET(ERROR_MESSAGE)
IF(NOT MaidSafeCommon_INCLUDE_DIR)
  SET(ERROR_MESSAGE "\nCould not find MaidSafe Common - ${MaidSafeCommon_INCLUDE_DIR}.\n")
ENDIF()
IF(NOT MaidSafeCommon_TOOLS_DIR)
  SET(ERROR_MESSAGE "\nCould not find MaidSafe Common - ${MaidSafeCommon_TOOLS_DIR}.\n")
ENDIF()
IF(ERROR_MESSAGE)
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}You can clone it at git@github.com:maidsafe/MaidSafe-Common.git\n\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If MaidSafe Common is already installed, run:")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}\n${ERROR_MESSAGE_CMAKE_PATH} -DMAIDSAFE_COMMON_INSTALL_DIR=<Path to MaidSafe Common install directory>\n\n")
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF()

MESSAGE("-- Found MaidSafe-Common library:")
GET_TARGET_PROPERTY(LIB_NAME_RELEASE maidsafe_common_static LOCATION_RELEASE)
GET_TARGET_PROPERTY(LIB_NAME_DEBUG maidsafe_common_static LOCATION_DEBUG)
GET_TARGET_PROPERTY(LIB_NAME_RELWITHDEBINFO maidsafe_common_static LOCATION_RELWITHDEBINFO)
GET_TARGET_PROPERTY(LIB_NAME_MINSIZEREL maidsafe_common_static LOCATION_MINSIZEREL)
IF(NOT LIB_NAME_RELEASE STREQUAL "")
  GET_FILENAME_COMPONENT(LIB_NAME_RELEASE ${LIB_NAME_RELEASE} NAME)
  MESSAGE("--   using ${LIB_NAME_RELEASE} for Release builds.")
ENDIF()
IF(NOT LIB_NAME_DEBUG STREQUAL "")
  GET_FILENAME_COMPONENT(LIB_NAME_DEBUG ${LIB_NAME_DEBUG} NAME)
  MESSAGE("--   using ${LIB_NAME_DEBUG} for Debug builds.")
ENDIF()
IF(NOT LIB_NAME_RELWITHDEBINFO STREQUAL "")
  GET_FILENAME_COMPONENT(LIB_NAME_RELWITHDEBINFO ${LIB_NAME_RELWITHDEBINFO} NAME)
  MESSAGE("--   using ${LIB_NAME_RELWITHDEBINFO} for RelWithDebInfo builds.")
ENDIF()
IF(NOT LIB_NAME_MINSIZEREL STREQUAL "")
  GET_FILENAME_COMPONENT(LIB_NAME_MINSIZEREL ${LIB_NAME_MINSIZEREL} NAME)
  MESSAGE("--   using ${LIB_NAME_MINSIZEREL} for MinSizeRel builds.")
ENDIF()
