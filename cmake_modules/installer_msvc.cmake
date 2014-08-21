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

if(NOT "${Config}" STREQUAL Release)
  if(${TargetType} STREQUAL DevDebug)
    message(FATAL_ERROR "Debug Installers are currently disabled in Windows")
  endif()

  message(FATAL_ERROR "Invalid Build Config. Installers are only supported in Release config")
endif()

message("TargetName - ${TargetName}")

separate_arguments(TargetLibs WINDOWS_COMMAND "${TargetLibs}")
foreach(Lib ${TargetLibs})
  message("TargetLibs - ${Lib}")
endforeach()

separate_arguments(TargetHeaders WINDOWS_COMMAND "${TargetHeaders}")
foreach(Header ${TargetHeaders})
  message("Header - ${Header}")
endforeach()

separate_arguments(TargetExes WINDOWS_COMMAND "${TargetExes}")
foreach(Exe ${TargetExes})
  message("TargetExes - ${Exe}")
endforeach()
