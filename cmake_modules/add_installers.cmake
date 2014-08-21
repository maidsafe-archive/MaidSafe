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
#                                                                                                  #
#  Sets up installer targets for Win/Unix platforms.                                               #
#==================================================================================================#

# Installer Types
set(Types Farmer Dev Utilities DevDebug)

set(FarmerExeDepends vault vault_manager)
set(DevLibDepends maidsafe_api)
set(UtilitiesExeDepends local_network_controller)
set(DevDebugLibDepends maidsafe_api)

if(UNIX)
  set(FarmerName "maidsafe-farmer")
  set(DevName "maidsafe-dev")
  set(UtilitiesName "maidsafe-utilities")
  set(DevDebugName "maidsafe-dev-dbg")

  set(InstallerScriptName "installer_unix.cmake")
elseif(MSVC)
  set(FarmerName "Farmer")
  set(DevName "Dev")
  set(UtilitiesName "Utilities")
  set(DevDebugName "DevDebug")

  set(InstallerScriptName "installer_msvc.cmake")
else()
  message(STATUS "Installer targets not available in this platform")
  return()
endif()

# Create Custom Installer Targets
foreach(Type ${Types})
  # Custom char to pass an array to cmake installer script
  string(ASCII 236 CustomSeperator)

  unset(AllHeaders)
  foreach(LibDepend ${${Type}LibDepends})
    set(${Type}Libs "${${Type}Libs}${CustomSeperator}$<TARGET_FILE:${LibDepend}>")
    get_target_property(Headers ${LibDepend} INTERFACE_INCLUDE_DIRECTORIES)
    list(APPEND AllHeaders ${Headers})
    get_target_property(SystemHeaders ${LibDepend} INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
    if(SystemHeaders)
      list(APPEND AllHeaders ${SystemHeaders})
    endif()
  endforeach()
  if(AllHeaders)
    list(REMOVE_DUPLICATES AllHeaders)
    string(REPLACE ";" "${CustomSeperator}" AllHeaders "${AllHeaders}")
  endif()

  foreach(ExeDepend ${${Type}ExeDepends})
    set(${Type}Exes "${${Type}Exes}${CustomSeperator}$<TARGET_FILE:${ExeDepend}>")
  endforeach()

  add_custom_target(${${Type}Name}
                    COMMAND ${CMAKE_COMMAND}
                        -DCMAKE_BINARY_DIR="${CMAKE_BINARY_DIR}"
                        -DTargetName="${${Type}Name}"
                        -DTargetType=${Type}
                        -DTargetLibs="${${Type}Libs}"
                        -DTargetExes="${${Type}Exes}"
                        -DTargetHeaders="${AllHeaders}"
                        -DConfig=$<CONFIG>
                        -DCustomSeperator=${CustomSeperator}
                        -P "${CMAKE_SOURCE_DIR}/cmake_modules/${InstallerScriptName}"
                        -V)
endforeach()