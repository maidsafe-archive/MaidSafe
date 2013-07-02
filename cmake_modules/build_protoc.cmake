#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
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
