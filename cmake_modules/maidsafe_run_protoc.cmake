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
#  Module used to run Google Protocol Buffers compiler against .proto files if #
#  their contents have changed or if protobuf version has changed.             #
#                                                                              #
#==============================================================================#


# Function to generate CC and header files derived from proto files
function(generate_proto_files PROTO_FILE CACHE_NAME)
  file(STRINGS ${PROTO_SOURCE_DIR}/${PROTO_FILE} PROTO_STRING)
  unset(NEW_${ARGV1} CACHE)
  set(NEW_${ARGV1} ${PROTO_STRING} CACHE string "Google Protocol Buffers - new file contents for ${ARGV1}")
  if((FORCE_PROTOC_COMPILE) OR (NOT "${NEW_${ARGV1}}" STREQUAL "${${ARGV1}}"))
    set(RAN_PROTOC TRUE PARENT_SCOPE)
    get_filename_component(PROTO_FILE_NAME ${PROTO_SOURCE_DIR}/${PROTO_FILE} NAME)
    execute_process(COMMAND ${ProtocExe}
                      --proto_path=${PROTO_SOURCE_DIR}
                      --cpp_out=${PROTO_SOURCE_DIR}
                      ${PROTO_SOURCE_DIR}/${PROTO_FILE}
                      RESULT_VARIABLE PROTO_RES
                      ERROR_VARIABLE PROTO_ERR)
    unset(${ARGV1} CACHE)
    if(NOT ${PROTO_RES})
      message(STATUS "Generated files from ${PROTO_FILE_NAME}")
      set(${ARGV1} ${PROTO_STRING} CACHE string "Google Protocol Buffers - file contents for ${PROTO_FILE}")
    else()
      message(FATAL_ERROR "Failed trying to generate files from ${PROTO_FILE_NAME}\n${PROTO_ERR}")
    endif()
  endif()
  unset(NEW_${ARGV1} CACHE)
endfunction()


set(RAN_PROTOC FALSE PARENT_SCOPE)

if(NOT PROTO_FILES)
  message(STATUS "No proto files to be generated")
  return()
endif()

if(NOT PROTO_SOURCE_DIR)
  message(FATAL_ERROR "To generate proto files, PROTO_SOURCE_DIR must be set.")
endif()

remove_old_proto_files()

execute_process(COMMAND ${ProtocExe} "--version" OUTPUT_VARIABLE TMP_CURRENT_PROTOC_VERSION)
string(STRIP ${TMP_CURRENT_PROTOC_VERSION} CURRENT_PROTOC_VERSION)
if(NOT PROTOC_VERSION STREQUAL CURRENT_PROTOC_VERSION)
  set(PROTOC_VERSION ${CURRENT_PROTOC_VERSION} CACHE STATIC "Google Protocol Buffers - Current version" FORCE)
  set(FORCE_PROTOC_COMPILE TRUE)
endif()

foreach(PROTO_FILE ${PROTO_FILES})
  string(REGEX REPLACE "[\\/.:]" "_" PROTO_CACHE_NAME ${PROTO_FILE})
  generate_proto_files(${PROTO_FILE} ${PROTO_CACHE_NAME})
endforeach()
