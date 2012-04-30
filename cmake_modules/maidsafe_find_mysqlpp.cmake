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
#  Module used to locate MySQL++ libs and headers.                             #
#                                                                              #
#  Settable variables to aid with finding MySQL++ are:                         #
#    MYSQLPP_LIB_DIR, MYSQLPP_INC_DIR and MYSQLPP_ROOT_DIR                     #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    Mysqlpp_INCLUDE_DIR, Mysqlpp_LIBRARY_DIR, Mysqlpp_LIBRARY and             #
#    Mysqlpp_FOUND                                                             #
#                                                                              #
#  For MSVC, Mysqlpp_LIBRARY_DIR_DEBUG is also set and cached.                 #
#                                                                              #
#==============================================================================#

INCLUDE(maidsafe_find_mysql)

IF(Mysql_FOUND)
  UNSET(WARNING_MESSAGE)
  UNSET(Mysqlpp_INCLUDE_DIR CACHE)
  UNSET(Mysqlpp_LIBRARY_DIR CACHE)
  UNSET(Mysqlpp_LIBRARY_DIR_DEBUG CACHE)
  UNSET(Mysqlpp_LIBRARY CACHE)
  UNSET(Mysqlpp_LIBRARY_DEBUG CACHE)
  UNSET(Mysqlpp_LIBRARY_RELEASE CACHE)
  UNSET(Mysqlpp_FOUND CACHE)

  IF(MYSQLPP_LIB_DIR)
    SET(MYSQLPP_LIB_DIR ${MYSQLPP_LIB_DIR} CACHE PATH "Path to MySQL++ libraries directory" FORCE)
  ENDIF()
  IF(MYSQLPP_INC_DIR)
    SET(MYSQLPP_INC_DIR ${MYSQLPP_INC_DIR} CACHE PATH "Path to MySQL++ include directory" FORCE)
  ENDIF()
  IF(MYSQLPP_ROOT_DIR)
    SET(MYSQLPP_ROOT_DIR ${MYSQLPP_ROOT_DIR} CACHE PATH "Path to MySQL++ root directory" FORCE)
  ENDIF()

  IF(MSVC)
    SET(MYSQLPP_LIBPATH_SUFFIX lib)
  ELSE()
    SET(MYSQLPP_LIBPATH_SUFFIX lib lib64)
  ENDIF()

  FIND_LIBRARY(Mysqlpp_LIBRARY_RELEASE NAMES mysqlpp PATHS ${MYSQLPP_LIB_DIR} ${MYSQLPP_ROOT_DIR} PATH_SUFFIXES ${MYSQLPP_LIBPATH_SUFFIX})
  IF(MSVC)
    FIND_LIBRARY(Mysqlpp_LIBRARY_DEBUG NAMES mysqlpp_d PATHS ${MYSQLPP_LIB_DIR} ${MYSQLPP_ROOT_DIR} PATH_SUFFIXES ${MYSQLPP_LIBPATH_SUFFIX})
  ENDIF()

  FIND_PATH(Mysqlpp_INCLUDE_DIR mysql++.h PATHS ${MYSQLPP_INC_DIR}/mysql++ ${MYSQLPP_INC_DIR} ${MYSQLPP_ROOT_DIR}/include)

  GET_FILENAME_COMPONENT(MYSQLPP_LIBRARY_DIR ${Mysqlpp_LIBRARY_RELEASE} PATH)
  SET(Mysqlpp_LIBRARY_DIR ${MYSQLPP_LIBRARY_DIR} CACHE PATH "Path to MySQL++ libraries directory" FORCE)

  IF(MSVC)
    GET_FILENAME_COMPONENT(MYSQLPP_LIBRARY_DIR_DEBUG ${Mysqlpp_LIBRARY_DEBUG} PATH)
    SET(Mysqlpp_LIBRARY_DIR_DEBUG ${MYSQLPP_LIBRARY_DIR_DEBUG} CACHE PATH "Path to MySQL++ debug libraries directory" FORCE)

    FIND_FILE(MYSQLPP_DLL mysqlpp.dll PATHS ${Mysqlpp_LIBRARY_DIR})
    FIND_FILE(MYSQLPP_DLL_DEBUG mysqlpp_d.dll PATHS ${Mysqlpp_LIBRARY_DIR_DEBUG})
    UNSET(COPIED_MYSQLPP_DLL_DEBUG CACHE)
	  IF(MYSQLPP_DLL AND MYSQLPP_DLL_DEBUG)
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQLPP_DLL_DEBUG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/)
      FIND_FILE(COPIED_MYSQLPP_DLL_DEBUG mysqlpp_d.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/)
      UNSET(COPIED_MYSQLPP_DLL_RELEASE CACHE)
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQLPP_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/)
      FIND_FILE(COPIED_MYSQLPP_DLL_RELEASE mysqlpp.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/)
      UNSET(COPIED_MYSQLPP_DLL_RELWITHDEBINFO CACHE)
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQLPP_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/)
      FIND_FILE(COPIED_MYSQLPP_DLL_RELWITHDEBINFO mysqlpp.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/)
      UNSET(COPIED_MYSQLPP_DLL_MINSIZEREL CACHE)
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQLPP_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/)
      FIND_FILE(COPIED_MYSQLPP_DLL_MINSIZEREL mysqlpp.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/)
	  ENDIF()

    IF(NOT MYSQLPP_DLL)
      SET(WARNING_MESSAGE TRUE)
      MESSAGE("-- Did not find MySQL++ DLL")
    ENDIF()

    IF(NOT MYSQLPP_DLL_DEBUG)
      SET(WARNING_MESSAGE TRUE)
      MESSAGE("-- Did not find MySQL++ Debug DLL")
    ENDIF()

    IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQLPP_DLL_DEBUG)
      SET(WARNING_MESSAGE TRUE)
      MESSAGE("-- Failed to copy MySQL++ Debug DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/")
    ENDIF()

    IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQLPP_DLL_RELEASE)
      SET(WARNING_MESSAGE TRUE)
      MESSAGE("-- Failed to copy MySQL++ DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
    ENDIF()

    IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQLPP_DLL_RELWITHDEBINFO)
      SET(WARNING_MESSAGE TRUE)
      MESSAGE("-- Failed to copy MySQL++ DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/")
    ENDIF()

    IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQLPP_DLL_MINSIZEREL)
      SET(WARNING_MESSAGE TRUE)
      MESSAGE("-- Failed to copy MySQL++ DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/")
    ENDIF()
  ENDIF()

  IF(NOT Mysqlpp_LIBRARY_RELEASE)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Did not find MySQL++ library")
  ELSE()
    MESSAGE("-- Found MySQL++ library")
    SET(Mysqlpp_LIBRARY ${Mysqlpp_LIBRARY_RELEASE} CACHE PATH "Path to MySql++ library" FORCE)
  ENDIF()

  IF(MSVC)
    IF(NOT Mysqlpp_LIBRARY_DEBUG)
      SET(WARNING_MESSAGE TRUE)
      MESSAGE("-- Did not find MySQL++ Debug library")
    ELSE()
      MESSAGE("-- Found MySQL++ Debug library")
      SET(Mysqlpp_LIBRARY debug ${Mysqlpp_LIBRARY_DEBUG} optimized ${Mysqlpp_LIBRARY} CACHE PATH "Path to MySql++ libraries" FORCE)
    ENDIF()
  ENDIF()

  IF(NOT Mysqlpp_INCLUDE_DIR)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Did not find MySQL++ library headers")
  ENDIF()

  IF(WARNING_MESSAGE)
    SET(WARNING_MESSAGE "   You can download it at http://tangentsoft.net/mysql++/\n")
    SET(WARNING_MESSAGE "${WARNING_MESSAGE}   If MySQL++ is already installed, run:\n")
    SET(WARNING_MESSAGE "${WARNING_MESSAGE}   ${ERROR_MESSAGE_CMAKE_PATH} -DMYSQLPP_LIB_DIR=<Path to MySQL++ lib directory> and/or")
    SET(WARNING_MESSAGE "${WARNING_MESSAGE}\n   ${ERROR_MESSAGE_CMAKE_PATH} -DMYSQLPP_INC_DIR=<Path to MySQL++ include directory> and/or")
    SET(WARNING_MESSAGE "${WARNING_MESSAGE}\n   ${ERROR_MESSAGE_CMAKE_PATH} -DMYSQLPP_ROOT_DIR=<Path to MySQL++ root directory>")
    MESSAGE("${WARNING_MESSAGE}")
    SET(Mysqlpp_FOUND FALSE CACHE INTERNAL "Found MySQL++ library and headers" FORCE)
    UNSET(Mysqlpp_INCLUDE_DIR CACHE)
    UNSET(Mysqlpp_LIBRARY_DIR CACHE)
    UNSET(Mysqlpp_LIBRARY_DIR_DEBUG CACHE)
    UNSET(Mysqlpp_LIBRARY CACHE)
    UNSET(Mysqlpp_LIBRARY_DEBUG CACHE)
    UNSET(Mysqlpp_LIBRARY_RELEASE CACHE)
  ELSE()
    SET(Mysqlpp_FOUND TRUE CACHE INTERNAL "Found MySQL++ library and headers" FORCE)
    INCLUDE_DIRECTORIES(SYSTEM ${Mysqlpp_INCLUDE_DIR})
    IF(CMAKE_INCLUDE_DIRECTORIES_BEFORE)
      SET(INCLUDE_DIRS ${Mysqlpp_INCLUDE_DIR} ${INCLUDE_DIRS})
    ELSE()
      SET(INCLUDE_DIRS ${INCLUDE_DIRS} ${Mysqlpp_INCLUDE_DIR})
    ENDIF()
  ENDIF()
ENDIF()
