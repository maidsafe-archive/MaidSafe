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
#  Module used to locate MySQL libs and headers.                               #
#                                                                              #
#  Settable variables to aid with finding MySQL are:                           #
#    MYSQL_LIB_DIR, MYSQL_INC_DIR and MYSQL_ROOT_DIR                           #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    Mysql_INCLUDE_DIR, Mysql_LIBRARY_DIR, Mysql_LIBRARY and Mysql_FOUND.      #
#                                                                              #
#  For MSVC, Mysql_LIBRARY_DIR_DEBUG is also set and cached.                   #
#                                                                              #
#==============================================================================#

UNSET(WARNING_MESSAGE)
UNSET(Mysql_INCLUDE_DIR CACHE)
UNSET(Mysql_LIBRARY_DIR CACHE)
UNSET(Mysql_LIBRARY_DIR_DEBUG CACHE)
UNSET(Mysql_LIBRARY CACHE)
UNSET(Mysql_LIBRARY_DEBUG CACHE)
UNSET(Mysql_LIBRARY_RELEASE CACHE)
UNSET(Mysql_FOUND CACHE)

IF(MYSQL_LIB_DIR)
  SET(MYSQL_LIB_DIR ${MYSQL_LIB_DIR} CACHE PATH "Path to MySQL libraries directory" FORCE)
ENDIF()
IF(MYSQL_INC_DIR)
  SET(MYSQL_INC_DIR ${MYSQL_INC_DIR} CACHE PATH "Path to MySQL include directory" FORCE)
ENDIF()
IF(MYSQL_ROOT_DIR)
  SET(MYSQL_ROOT_DIR ${MYSQL_ROOT_DIR} CACHE PATH "Path to MySQL root directory" FORCE)
ENDIF()

FIND_LIBRARY(Mysql_LIBRARY_RELEASE NAMES mysqlclient libmysql PATHS ${MYSQL_LIB_DIR} ${MYSQL_ROOT_DIR} "${MYSQL_ROOT_DIR}/MySQL Server 5.1" "C:/Program Files/MySQL/MySQL Server 5.1" PATH_SUFFIXES lib lib64 lib/opt)
IF(MSVC)
  FIND_LIBRARY(Mysql_LIBRARY_DEBUG NAMES libmysql PATHS ${MYSQL_ROOT_DIR} "${MYSQL_ROOT_DIR}/MySQL Server 5.1" "C:/Program Files/MySQL/MySQL Server 5.1" PATH_SUFFIXES lib/debug)
ENDIF()

FIND_PATH(Mysql_INCLUDE_DIR mysql.h PATHS ${MYSQL_INC_DIR}/mysql ${MYSQL_INC_DIR} ${MYSQL_ROOT_DIR} "${MYSQL_ROOT_DIR}/MySQL Server 5.1" "C:/Program Files/MySQL/MySQL Server 5.1" PATH_SUFFIXES include)

GET_FILENAME_COMPONENT(MYSQL_LIBRARY_DIR ${Mysql_LIBRARY_RELEASE} PATH)
SET(Mysql_LIBRARY_DIR ${MYSQL_LIBRARY_DIR} CACHE PATH "Path to MySQL release libraries directory" FORCE)
IF(MSVC)
  GET_FILENAME_COMPONENT(MYSQL_LIBRARY_DIR_DEBUG ${Mysql_LIBRARY_DEBUG} PATH)
  SET(Mysql_LIBRARY_DIR_DEBUG ${MYSQL_LIBRARY_DIR_DEBUG} CACHE PATH "Path to MySQL debug libraries directory" FORCE)

  FIND_FILE(MYSQL_DLL libmysql.dll PATHS ${Mysql_LIBRARY_DIR})
  FIND_FILE(MYSQL_DLL_DEBUG libmysql.dll PATHS ${Mysql_LIBRARY_DIR_DEBUG})
  IF(MYSQL_DLL AND MYSQL_DLL_DEBUG)
    UNSET(COPIED_MYSQL_DLL_DEBUG CACHE)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQL_DLL_DEBUG} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/)
    FIND_FILE(COPIED_MYSQL_DLL_DEBUG libmysql.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/)
    UNSET(COPIED_MYSQL_DLL_RELEASE CACHE)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQL_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/)
    FIND_FILE(COPIED_MYSQL_DLL_RELEASE libmysql.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/)
    UNSET(COPIED_MYSQL_DLL_RELWITHDEBINFO CACHE)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQL_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/)
    FIND_FILE(COPIED_MYSQL_DLL_RELWITHDEBINFO libmysql.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/)
    UNSET(COPIED_MYSQL_DLL_MINSIZEREL CACHE)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/)
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -E copy ${MYSQL_DLL} ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/)
    FIND_FILE(COPIED_MYSQL_DLL_MINSIZEREL libmysql.dll PATHS ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/)
  ENDIF()

  IF(NOT MYSQL_DLL)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Did not find MySQL DLL")
  ENDIF()

  IF(NOT MYSQL_DLL_DEBUG)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Did not find MySQL Debug DLL")
  ENDIF()

  IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQL_DLL_DEBUG)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Failed to copy MySQL Debug DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Debug/")
  ENDIF()

  IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQL_DLL_RELEASE)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Failed to copy MySQL DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/Release/")
  ENDIF()

  IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQL_DLL_RELWITHDEBINFO)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Failed to copy MySQL DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/RelWithDebInfo/")
  ENDIF()

  IF(NOT WARNING_MESSAGE AND NOT COPIED_MYSQL_DLL_MINSIZEREL)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Failed to copy MySQL DLL to ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/MinSizeRel/")
  ENDIF()
ENDIF()

IF(NOT Mysql_LIBRARY_RELEASE)
  SET(WARNING_MESSAGE TRUE)
  MESSAGE("-- Did not find MySQL Client library")
ELSE()
  MESSAGE("-- Found MySQL Client library")
  SET(Mysql_LIBRARY ${Mysql_LIBRARY_RELEASE} CACHE PATH "Path to MySql library" FORCE)
ENDIF()

IF(MSVC)
  IF(NOT Mysql_LIBRARY_DEBUG)
    SET(WARNING_MESSAGE TRUE)
    MESSAGE("-- Did not find MySQL Debug library")
  ELSE()
    MESSAGE("-- Found MySQL Debug library")
    SET(Mysql_LIBRARY debug ${Mysql_LIBRARY_DEBUG} optimized ${Mysql_LIBRARY} CACHE PATH "Path to MySql libraries" FORCE)
  ENDIF()
ENDIF()

IF(NOT Mysql_INCLUDE_DIR)
  SET(WARNING_MESSAGE TRUE)
  MESSAGE("-- Did not find MySQL library headers")
ENDIF()

IF(WARNING_MESSAGE)
  SET(WARNING_MESSAGE "   You can download the MySQL Community Server at http://www.mysql.com/downloads/mysql\n")
  SET(WARNING_MESSAGE "${WARNING_MESSAGE}   If MySQL is already installed, run:\n")
  SET(WARNING_MESSAGE "${WARNING_MESSAGE}   ${ERROR_MESSAGE_CMAKE_PATH} -DMYSQL_LIB_DIR=<Path to MySQL lib directory> and/or")
  SET(WARNING_MESSAGE "${WARNING_MESSAGE}\n   ${ERROR_MESSAGE_CMAKE_PATH} -DMYSQL_INC_DIR=<Path to MySQL include directory> and/or")
  SET(WARNING_MESSAGE "${WARNING_MESSAGE}\n   ${ERROR_MESSAGE_CMAKE_PATH} -DMYSQL_ROOT_DIR=<Path to MySQL root directory>")
  MESSAGE("${WARNING_MESSAGE}")
  SET(Mysql_FOUND FALSE CACHE INTERNAL "Found MySQL library and headers" FORCE)
  UNSET(Mysql_INCLUDE_DIR CACHE)
  UNSET(Mysql_LIBRARY_DIR CACHE)
  UNSET(Mysql_LIBRARY_DIR_DEBUG CACHE)
  UNSET(Mysql_LIBRARY CACHE)
  UNSET(Mysql_LIBRARY_DEBUG CACHE)
  UNSET(Mysql_LIBRARY_RELEASE CACHE)
ELSE()
  SET(Mysql_FOUND TRUE CACHE INTERNAL "Found MySQL library and headers" FORCE)
  INCLUDE_DIRECTORIES(SYSTEM ${Mysql_INCLUDE_DIR})
  IF(CMAKE_INCLUDE_DIRECTORIES_BEFORE)
    SET(INCLUDE_DIRS ${Mysql_INCLUDE_DIR} ${INCLUDE_DIRS})
  ELSE()
    SET(INCLUDE_DIRS ${INCLUDE_DIRS} ${Cbfs_INCLUDE_DIR})
  ENDIF()
ENDIF()
