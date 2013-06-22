#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2013 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#


set(ProtocBuildDir ${CMAKE_BINARY_DIR}/build_protoc)
find_program(ProtocExe NAMES protoc PATHS ${ProtocBuildDir} PATH_SUFFIXES Release NO_DEFAULT_PATH)
if(NOT ProtocExe)
  message("${HR}")
  message(STATUS "Building protoc.  Will take a minute or so...")
  execute_process(COMMAND
                    ${CMAKE_COMMAND} -E make_directory ${ProtocBuildDir}
                    OUTPUT_VARIABLE OutVar
                    RESULT_VARIABLE ResVar)
  if(NOT MSVC)
    set(ProtocBuildType "-DCMAKE_BUILD_TYPE=Release")
  endif()
  execute_process(COMMAND
                    ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/third_party_libs/protobuf -G ${CMAKE_GENERATOR} ${ProtocBuildType}
                    WORKING_DIRECTORY ${ProtocBuildDir}
                    OUTPUT_VARIABLE OutVar
                    RESULT_VARIABLE ResVar)
  set(ProtocOutVar "${OutVar}")
  execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release WORKING_DIRECTORY ${ProtocBuildDir} OUTPUT_VARIABLE OutVar RESULT_VARIABLE ResVar)
  set(ProtocOutVar "${ProtocOutVar}\n\n\n\n*********************\n\n\n\n${OutVar}")
  find_program(ProtocExe NAMES protoc PATHS ${ProtocBuildDir} PATH_SUFFIXES Release NO_DEFAULT_PATH)
  if(NOT ProtocExe)
    message(FATAL_ERROR "${ProtocOutVar}")
  endif()
endif()
