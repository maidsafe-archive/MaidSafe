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
#                                                                                                  #
#==================================================================================================#



if(NOT CMAKE_CONFIGURATION_TYPES AND NOT CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Installers unavailable with ${CMAKE_BUILD_TYPE} configuration")
  return()
endif()

# Installer Types
if(INCLUDE_TESTS)
  set(Types Farmer Dev Utilities)
else()
# Temporarily disable Utilities target while it only contains tests
# Remove this exclusion when Utilities includes tools as well as tests
  set(Types Farmer Dev)
endif()

include(monolithic_lib)

set(FarmerExeDepends vault vault_manager)
set(DevLibDepends maidsafe)
set(UtilitiesExeDepends test_common
                        test_routing
                        test_routing_api
                        test_drive
                        test_passport
                        test_nfs
                        test_encrypt
                        test_filesystem
                        test_api)
set(DevDebugLibDepends ${DevLibDepends})

if(UNIX)
  set(FarmerName "maidsafe-farmer")
  set(DevName "maidsafe-dev")
  set(UtilitiesName "maidsafe-utilities")
  set(DevDebugName "maidsafe-dev-dbg")

  set(InstallerScriptName "installer_unix.cmake")
elseif(MSVC)
  set(FarmerName "FarmerInstaller")
  set(DevName "DevInstaller")
  set(UtilitiesName "UtilitiesInstaller")

  set(InstallerScriptName "installer_msvc.cmake")
else()
  message(STATUS "Installer targets not available in this platform")
  return()
endif()

function(safe_path InPath OutPath)
  set(${OutPath} "\\\"${InPath}\\\"" PARENT_SCOPE)
endfunction()

# Create Custom Installer Targets
foreach(Type ${Types})
  foreach(ExeDepend ${${Type}ExeDepends})
    if(TARGET ${ExeDepend})
      safe_path($<TARGET_FILE:${ExeDepend}> SafePath)
      list(APPEND ${Type}Exes ${SafePath})
    endif()
  endforeach()

  add_custom_target(${${Type}Name}
                    COMMAND ${CMAKE_COMMAND}
                        -DSUPER_PROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}"
                        -DSUPER_PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
                        -DCMAKE_CL_64="${CMAKE_CL_64}"
                        -DVersion="${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}"
                        -DTargetName="${${Type}Name}"
                        -DTargetType=${Type}
                        $<$<OR:$<STREQUAL:${Type},Dev>,$<STREQUAL:${Type},DevDebug>>:-DTargetLibs=$<TARGET_FILE:maidsafe>>
                        -DMonolithicIncludes="${MonolithicIncludes}"
                        -DTargetExes="${${Type}Exes}"
                        -DBoostSourceDir="${BoostSourceDir}"
                        -DConfig=$<CONFIGURATION>
                        -DCMAKE_SYSTEM_VERSION="${CMAKE_SYSTEM_VERSION}"
                        -P "${CMAKE_SOURCE_DIR}/cmake_modules/${InstallerScriptName}"
                        -V)
  set_target_properties(${${Type}Name} PROPERTIES FOLDER "MaidSafe/Installers")
endforeach()
