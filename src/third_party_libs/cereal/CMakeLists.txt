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


project(cereal)

add_library(cereal INTERFACE)
target_include_directories(cereal INTERFACE ${PROJECT_SOURCE_DIR}/include)

# For multi-config IDEs, add custom target to make cereal's files visible
if(CMAKE_CONFIGURATION_TYPES)
  file(GLOB_RECURSE GlobbedSrcs "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h" "${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")
  add_custom_target(CerealFiles
      COMMENT "NOTE: This is not a target which can be built; just a list of headers for Cereal."
      SOURCES ${GlobbedSrcs})
  set_target_properties(CerealFiles PROPERTIES FOLDER "Third Party/Cereal")
endif()
