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
#  Module used to run Google Protocol Buffers compiler against .proto files if #
#  their contents have changed or if protobuf version has changed.  The path   #
#  to the compiler must be set in the variable Protoc_EXE.                     #
#                                                                              #
#==============================================================================#


# Function to generate CC and header files derived from proto files
FUNCTION(GENERATE_PROTO_FILES PROTO_FILE CACHE_NAME)
  FILE(STRINGS ${PROTO_SOURCE_DIR}/${PROTO_FILE} PROTO_STRING)
  UNSET(NEW_${ARGV1} CACHE)
  SET(NEW_${ARGV1} ${PROTO_STRING} CACHE STRING "Google Protocol Buffers - new file contents for ${ARGV1}")
  IF((FORCE_PROTOC_COMPILE) OR (NOT "${NEW_${ARGV1}}" STREQUAL "${${ARGV1}}"))
    GET_FILENAME_COMPONENT(PROTO_FILE_NAME ${PROTO_SOURCE_DIR}/${PROTO_FILE} NAME)
    EXECUTE_PROCESS(COMMAND ${Protoc_EXE}
                      --proto_path=${PROTO_SOURCE_DIR}
                      --cpp_out=${PROTO_SOURCE_DIR}
                      ${PROTO_SOURCE_DIR}/${PROTO_FILE}
                      RESULT_VARIABLE PROTO_RES
                      ERROR_VARIABLE PROTO_ERR)
    UNSET(${ARGV1} CACHE)
    IF(NOT ${PROTO_RES})
      MESSAGE("-- Generated files from ${PROTO_FILE_NAME}")
      SET(${ARGV1} ${PROTO_STRING} CACHE STRING "Google Protocol Buffers - file contents for ${PROTO_FILE}")
    ELSE()
      MESSAGE(FATAL_ERROR "Failed trying to generate files from ${PROTO_FILE_NAME}\n${PROTO_ERR}")
    ENDIF()
  ENDIF()
  UNSET(NEW_${ARGV1} CACHE)
ENDFUNCTION()


IF(NOT PROTO_FILES)
  MESSAGE("-- No proto files to be generated")
  RETURN()
ENDIF()

IF(NOT PROTO_SOURCE_DIR)
  MESSAGE(FATAL_ERROR "To generate proto files, PROTO_SOURCE_DIR must be set.")
ENDIF()

REMOVE_OLD_PROTO_FILES()

EXECUTE_PROCESS(COMMAND ${Protoc_EXE} "--version" OUTPUT_VARIABLE TMP_CURRENT_PROTOC_VERSION)
STRING(STRIP ${TMP_CURRENT_PROTOC_VERSION} CURRENT_PROTOC_VERSION)
IF(NOT PROTOC_VERSION STREQUAL CURRENT_PROTOC_VERSION)
  SET(PROTOC_VERSION ${CURRENT_PROTOC_VERSION} CACHE STATIC "Google Protocol Buffers - Current version" FORCE)
  SET(FORCE_PROTOC_COMPILE TRUE)
ENDIF()

FOREACH(PROTO_FILE ${PROTO_FILES})
  STRING(REGEX REPLACE "[\\/.:]" "_" PROTO_CACHE_NAME ${PROTO_FILE})
  GENERATE_PROTO_FILES(${PROTO_FILE} ${PROTO_CACHE_NAME})
ENDFOREACH()
