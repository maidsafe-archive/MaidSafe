#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or    #
#  later, and The General Public License (GPL), version 3. By contributing code to this project    #
#  You agree to the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in    #
#  the root directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also    #
#  available at:                                                                                   #
#                                                                                                  #
#    http://www.novinet.com/license                                                                #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, software distributed under the       #
#  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       #
#  either express or implied. See the License for the specific language governing permissions      #
#  and limitations under the License.                                                              #
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
if(MSVC)
  set(ErrorFormat msvs)
else()
  set(ErrorFormat gcc)
endif()
file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/temp_proto_files)
foreach(ProtoFile ${ProtoFiles})
  execute_process(COMMAND ${ProtocExe}
                    --proto_path=${ProtoSrcDir}
                    --cpp_out=${CMAKE_BINARY_DIR}/temp_proto_files
                    --error_format=${ErrorFormat}
                    ${ProtoSrcDir}/${ProtoFile}
                    RESULT_VARIABLE ProtocResult
                    ERROR_VARIABLE ProtocError)
  if(ProtocResult EQUAL 0)
    # Copy newly-generated files to source tree if different from existing ones
    get_filename_component(ProtoFileNameWe ${ProtoFile} NAME_WE)
    get_filename_component(ProtoFileRelPath ${ProtoFile} PATH)
    set(TempCC ${CMAKE_BINARY_DIR}/temp_proto_files/${ProtoFileRelPath}/${ProtoFileNameWe}.pb.cc)
    set(TempH ${CMAKE_BINARY_DIR}/temp_proto_files/${ProtoFileRelPath}/${ProtoFileNameWe}.pb.h)
    set(SourceCC ${ProtoSrcDir}/${ProtoFileRelPath}/${ProtoFileNameWe}.pb.cc)
    set(SourceH ${ProtoSrcDir}/${ProtoFileRelPath}/${ProtoFileNameWe}.pb.h)
    execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TempCC} ${SourceCC}
                    RESULT_VARIABLE ComparisonResultCC ERROR_VARIABLE ComparisonErrors)
    execute_process(COMMAND ${CMAKE_COMMAND} -E compare_files ${TempH} ${SourceH}
                    RESULT_VARIABLE ComparisonResultH ERROR_VARIABLE ComparisonErrors)
    if(NOT ComparisonResultCC EQUAL 0 OR NOT ComparisonResultH EQUAL 0)
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${TempCC} ${SourceCC})
      execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${TempH} ${SourceH})
      message(STATUS "Generated files from ${ProtoFileNameWe}.proto")
    else()
      message(STATUS "Checked files from ${ProtoFileNameWe}.proto -- up to date")
    endif()
  else()
    message(FATAL_ERROR "Failed trying to generate files from ${ProtoFile}\n${ProtocError}")
  endif()
endforeach()
