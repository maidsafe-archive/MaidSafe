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
  message(FATAL_ERROR "Invalid build configuration.  ${TargetName} is only available for Release builds.")
endif()

if("${TargetType}" STREQUAL Dev)
  message("Building Monolithic target in Debug mode")
  execute_process(COMMAND ${CMAKE_COMMAND} --build ${SUPER_PROJECT_BINARY_DIR} --target maidsafe --config Debug -- /M:7
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT "${ResVar}" EQUAL 0)
    message(FATAL_ERROR "Failed - ${OutVar}")
  endif()
endif()

find_program(AdvancedInstaller NAMES AdvancedInstaller.com PATHS "C:/Program Files (x86)/Caphyon/Advanced Installer 11.4.1/bin/x86")
if(NOT AdvancedInstaller)
  message(FATAL_ERROR "Failed to find AdvancedInstaller.")
endif()

set(InstallerDir "${SUPER_PROJECT_BINARY_DIR}/Release")
set(InstallerConfigFile "${InstallerDir}/${TargetType}.aic")

if(CMAKE_CL_64)
  set(PACKAGE_TYPE x64)
else()
  set(PACKAGE_TYPE x86)
endif()

if(${CMAKE_SYSTEM_VERSION} STREQUAL 6.1)
  set(UDSystemVersion Win7)
elseif(${CMAKE_SYSTEM_VERSION} STREQUAL 6.2)
  set(UDSystemVersion Win8)
else()
  message(FATAL_ERROR "Operating system version not found.")
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${InstallerDir}")
file(COPY "${SUPER_PROJECT_SOURCE_DIR}/tools/installers/win/${TargetType}.aip" DESTINATION "${InstallerDir}")

file(WRITE ${InstallerConfigFile} ";aic\n")
file(APPEND ${InstallerConfigFile} "SetVersion ${Version}\n")
file(APPEND ${InstallerConfigFile} "SetPackageType ${PACKAGE_TYPE}\n")
file(APPEND ${InstallerConfigFile} "SetProperty UDInstallerType=\"${TargetType}\"\n")
file(APPEND ${InstallerConfigFile} "SetProperty UDPackageType=\"${PACKAGE_TYPE}\"\n")
file(APPEND ${InstallerConfigFile} "SetProperty UDSystemVersion=\"${UDSystemVersion}\"\n")

file(TO_NATIVE_PATH ${SUPER_PROJECT_SOURCE_DIR} SuperProjectSourceFolder)
file(APPEND ${InstallerConfigFile} "NewPathVariable -name SuperProjectSourceFolder -value \"${SuperProjectSourceFolder}\" -valuetype Folder\n")

file(TO_NATIVE_PATH ${SUPER_PROJECT_BINARY_DIR} SuperProjectBinaryFolder)
file(APPEND ${InstallerConfigFile} "NewPathVariable -name SuperProjectBinaryFolder -value \"${SuperProjectBinaryFolder}\" -valuetype Folder\n")

file(TO_NATIVE_PATH ${InstallerDir} InstallerDirWinPath)
file(APPEND ${InstallerConfigFile} "SetOutputLocation -buildname DefaultBuild -path \"${InstallerDirWinPath}\"\n")
file(APPEND ${InstallerConfigFile} "Save\nRebuild")

file(TO_NATIVE_PATH ${InstallerConfigFile} InstallerConfigFileWinPath)

message("Creating installer package. This may take a while...")
execute_process(COMMAND "${AdvancedInstaller}" /execute ${TargetType}.aip ${InstallerConfigFileWinPath}
                WORKING_DIRECTORY "${InstallerDir}"
                RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
if(NOT "${ResVar}" EQUAL 0)
  message(FATAL_ERROR "Failed - ${OutVar}")
endif()

file(REMOVE "${InstallerConfigFile}" "${InstallerDir}/${TargetType}.aip")
file(REMOVE_RECURSE "${InstallerDir}/${TargetType}-cache")
message("Success")
