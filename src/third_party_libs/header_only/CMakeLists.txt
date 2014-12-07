#==================================================================================================#
#                                                                                                  #
#  Copyright 2014 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.maidsafe.net/licenses                                                            #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
#                                                                                                  #
#==================================================================================================#


file(GLOB Libs RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/*")
foreach(Lib ${Libs})
  if(IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/${Lib}")
    if(NOT CMAKE_VERSION VERSION_LESS "3.0")
      add_library(${Lib} INTERFACE)
      target_include_directories(${Lib} INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}")
    endif()

    # For multi-config IDEs, add custom target to make library's files visible
    if(CMAKE_CONFIGURATION_TYPES)
      file(GLOB_RECURSE GlobbedSrcs "${CMAKE_CURRENT_SOURCE_DIR}/${Lib}/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/${Lib}/*.hpp" "${CMAKE_CURRENT_SOURCE_DIR}/${Lib}/*.ipp")
      ms_underscores_to_camel_case(${Lib} CamelCaseLib)
      add_custom_target(${CamelCaseLib}Files
          COMMENT "NOTE: This is not a target which can be built; just a list of headers for ${CamelCaseLib}."
          SOURCES ${GlobbedSrcs})
      set_target_properties(${CamelCaseLib}Files PROPERTIES FOLDER "Third Party/${CamelCaseLib}")
    endif()
  endif()
endforeach()

if(NOT CMAKE_VERSION VERSION_LESS "3.0")
  target_link_libraries(asio INTERFACE ${BoostCoroutineLibs} ${BoostContextLibs} ${BoostSystemLibs})
endif()
