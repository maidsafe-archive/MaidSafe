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
#  Module used to run Google Protocol Buffers compiler against .proto files in a given source      #
#  directory.                                                                                      #
#                                                                                                  #
#  The function searches the source directory for .proto files and adds a custom command to        #
#  generate the .pb.cc and pb.h files to an output directory in the build tree.  Any CMake target  #
#  which includes these files will cause them to be generated if they don't currently exist.       #
#                                                                                                  #
#  The output directory is passed to 'include_directories' to add it to the includes search path.  #
#                                                                                                  #
#  The function's signature contains the following variables:                                      #
#    * BaseName -         name of variable which forms prefix of output variables.                 #
#    * ProtoRootDir -     full path to a directory.  This is passed as the first --proto_path arg  #
#                         argument and so defines the "missing" part of the relative path used in  #
#                         the generated pb.ccs' #include statements.                               #
#    * ProtoRelativeDir - path relative to ${ProtoRootDir} which will be searched for .proto       #
#                         files.  May contain 0 .proto files.                                      #
#                                                                                                  #
#  Any extra variables passed are assumed to be full paths to import directories, and are passed   #
#  as --proto_path arguments to protoc in the order in which they are passed.                      #
#                                                                                                  #
#  The function exports the following variables to the parent namespace:                           #
#    * ${BaseName}ProtoSources - list of full path of all generated .pb.cc files.                  #
#    * ${BaseName}ProtoHeaders - list of full path of all generated .pb.h files.                   #
#    * ${BaseName}Protos -       list of full path to all .proto files in the source directory     #
#                                along with contents of '${BaseName}ProtoSources' and              #
#                                '${BaseName}ProtoHeaders'                                         #
#                                                                                                  #
#==================================================================================================#


function(add_protoc_command BaseName ProtoRootDir ProtoRelativeDir)
  set(${BaseName}ProtoSources "" PARENT_SCOPE)
  set(${BaseName}ProtoHeaders "" PARENT_SCOPE)
  set(${BaseName}Protos "" PARENT_SCOPE)
  set(GeneratedProtoRootDir ${CMAKE_BINARY_DIR}/GeneratedProtoFiles)
  include_directories(${GeneratedProtoRootDir})

  # Get list of .proto files
  file(GLOB ProtoFiles RELATIVE ${ProtoRootDir} ${ProtoRootDir}/${ProtoRelativeDir}/*.proto)
  
  # Search for and remove old generated .pb.cc and .pb.h files in the output dir
  file(GLOB ExistingPbFiles
       RELATIVE ${GeneratedProtoRootDir}
       ${GeneratedProtoRootDir}/${ProtoRelativeDir}/*.pb.*)
  list(LENGTH ExistingPbFiles ExistingPbFilesCount)
  string(REGEX REPLACE "([^;]*)\\.proto" "\\1.pb.cc;\\1.pb.h" GeneratedFiles "${ProtoFiles}")
  if(ExistingPbFilesCount)
    list(REMOVE_ITEM ExistingPbFiles ${GeneratedFiles})
    foreach(ExistingPbFile ${ExistingPbFiles})
      file(REMOVE ${GeneratedProtoRootDir}/${ExistingPbFile})
      message(STATUS "Removed ${ExistingPbFile}")
    endforeach()
  endif()
  
  if(NOT ProtoFiles)
    return()
  endif()

  # Set up protoc arguments
  set(ProtocArgs "--proto_path=${ProtoRootDir}")
  set(ProtoImportDirs ${ARGN})
  foreach(ProtoImportDir ${ProtoImportDirs})
    list(APPEND ProtocArgs "--proto_path=${ProtoImportDir}")
  endforeach()
  list(REMOVE_DUPLICATES ProtocArgs)
  list(APPEND ProtocArgs "--cpp_out=${GeneratedProtoRootDir}")
  if(MSVC)
    list(APPEND ProtocArgs "--error_format=msvs")
  else()
    list(APPEND ProtocArgs "--error_format=gcc")
  endif()

  # Add custom command to generate CC and header files
  unset(GeneratedProtoFiles)
  unset(GeneratedProtoSources)
  unset(GeneratedProtoHeaders)
  file(MAKE_DIRECTORY ${GeneratedProtoRootDir})
  foreach(ProtoFile ${ProtoFiles})
    get_filename_component(ProtoFileNameWe ${ProtoFile} NAME_WE)
    set(GeneratedSource ${GeneratedProtoRootDir}/${ProtoRelativeDir}/${ProtoFileNameWe}.pb.cc)
    set(GeneratedHeader ${GeneratedProtoRootDir}/${ProtoRelativeDir}/${ProtoFileNameWe}.pb.h)
    list(APPEND GeneratedProtoSources "${GeneratedSource}")
    list(APPEND GeneratedProtoHeaders "${GeneratedHeader}")
    list(APPEND Protos "${ProtoRootDir}/${ProtoFile}")
    add_custom_command(OUTPUT ${GeneratedSource} ${GeneratedHeader}
                       COMMAND $<TARGET_FILE:protoc> ${ProtocArgs} ${ProtoRootDir}/${ProtoFile}
                       DEPENDS protoc ${ProtoRootDir}/${ProtoFile}
                       COMMENT "Generated files from ${ProtoFileNameWe}.proto"
                       VERBATIM)
  endforeach()

  set_source_files_properties(${GeneratedProtoSources} ${GeneratedProtoHeaders} PROPERTIES GENERATED TRUE)
  if(MSVC)
    set_source_files_properties(${GeneratedProtoSources} PROPERTIES COMPILE_FLAGS "/W0")
  else()
    set_source_files_properties(${GeneratedProtoSources} PROPERTIES COMPILE_FLAGS "-w")
  endif()
  set(${BaseName}ProtoSources ${GeneratedProtoSources} PARENT_SCOPE)
  set(${BaseName}ProtoHeaders ${GeneratedProtoHeaders} PARENT_SCOPE)
  set(${BaseName}Protos ${Protos} ${GeneratedProtoSources} ${GeneratedProtoHeaders} PARENT_SCOPE)
endfunction()
