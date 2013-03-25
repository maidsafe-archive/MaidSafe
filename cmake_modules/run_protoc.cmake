#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2012 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#
#                                                                                                  #
#  Module used to run Google Protocol Buffers compiler against .proto files and copy to source     #
#  tree if their contents have changed.                                                            #
#                                                                                                  #
#==================================================================================================#


set(ProtoSrcDir ${PROJECT_SOURCE_DIR}/src)
if(NOT EXISTS ${ProtoSrcDir}/maidsafe)
  return()
endif()

file(GLOB_RECURSE ProtoFiles RELATIVE ${ProtoSrcDir} *.proto)

# Search for and remove old generated .pb.cc and .pb.h files in the source tree
file(GLOB_RECURSE ExistingPbFiles RELATIVE ${ProtoSrcDir} *.pb.*)
list(LENGTH ExistingPbFiles ExistingPbFilesCount)
string(REGEX REPLACE "([^;]*)\\.proto" "\\1.pb.cc;\\1.pb.h" GeneratedFiles "${ProtoFiles}")
if(ExistingPbFilesCount)
  list(REMOVE_ITEM ExistingPbFiles ${GeneratedFiles})
  foreach(ExistingPbFile ${ExistingPbFiles})
    file(REMOVE ${ProtoSrcDir}/${ExistingPbFile})
    message(STATUS "Removed ${ExistingPbFile}")
  endforeach()
endif()

if(NOT ProtoFiles)
  message(STATUS "No proto files to be generated.")
  return()
endif()

# Generate CC and header files derived from proto files into temp dir
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp_proto_files)
foreach(ProtoFile ${ProtoFiles})
  execute_process(COMMAND ${ProtocExe}
                    --proto_path=${ProtoSrcDir}
                    --cpp_out=${CMAKE_BINARY_DIR}/temp_proto_files
                    ${ProtoSrcDir}/${ProtoFile}
                    RESULT_VARIABLE ProtocResult
                    ERROR_VARIABLE ProtocError)
  if(NOT ProtocResult)
    get_filename_component(ProtoFileName ${ProtoFile} NAME)
    message(STATUS "Generated files from ${ProtoFileName}")
  else()
    message(FATAL_ERROR "Failed trying to generate files from ${ProtoFile}\n${ProtocError}")
  endif()
endforeach()

# Copy newly-generated files to source tree if different from existing ones
set(CMAKE_DISABLE_SOURCE_CHANGES OFF)
foreach(GeneratedFile ${GeneratedFiles})
  configure_file(${CMAKE_BINARY_DIR}/temp_proto_files/${GeneratedFile} ${ProtoSrcDir}/${GeneratedFile} COPYONLY)
endforeach()
set(CMAKE_DISABLE_SOURCE_CHANGES ON)
