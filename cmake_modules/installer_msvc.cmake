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
  if("${TargetType}" STREQUAL DevDebug)
    message(FATAL_ERROR "Debug installers are currently disabled in Windows")
  endif()
  message(FATAL_ERROR "Invalid build configuration.  ${TargetName} is only availale for Release builds.")
endif()

separate_arguments(TargetLibs WINDOWS_COMMAND "${TargetLibs}")
message("${TargetLibs}")

separate_arguments(TargetHeaders WINDOWS_COMMAND "${TargetHeaders}")
message("Headers - ${TargetHeaders}")

set(InstallerDir "${SUPER_PROJECT_BINARY_DIR}/Release")
# file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/common/include/maidsafe" DESTINATION "${InstallerDir}")

message(FATAL_ERROR "Done--------------")

# TODO: find AdvancedInstaller in a better way like CBFS. Also check if available in ENV-PATH
set(AdvancedInstallerPath "C:/Program Files (x86)/Caphyon/Advanced Installer 11.4.1/bin/x86/AdvancedInstaller.com")
set(InstallerDir "${SUPER_PROJECT_BINARY_DIR}/Release/Installer")
set(InstallerConfigFile "${InstallerDir}/${TargetType}.aic")

if(CMAKE_CL_64)
  set(PACKAGE_TYPE x64)
else()
  set(PACKAGE_TYPE x86)
endif()

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${InstallerDir}")

file(WRITE ${InstallerConfigFile} ";aic\n")
file(APPEND ${InstallerConfigFile} "SetVersion ${Version}\n")
file(APPEND ${InstallerConfigFile} "SetPackageType ${PACKAGE_TYPE}\n")
file(APPEND ${InstallerConfigFile} "SetProperty UDInstallerType=\"${TargetType}\"\n")
file(APPEND ${InstallerConfigFile} "SetProperty UDPackageType=\"${PACKAGE_TYPE}\"\n")

file(TO_NATIVE_PATH ${SUPER_PROJECT_SOURCE_DIR} SuperProjectSourceFolder)
file(APPEND ${InstallerConfigFile} "NewPathVariable -name SuperProjectSourceFolder -value \"${SuperProjectSourceFolder}\" -valuetype Folder\n")

file(TO_NATIVE_PATH ${SUPER_PROJECT_BINARY_DIR}/Debug BinariesFolder)
file(APPEND ${InstallerConfigFile} "NewPathVariable -name BinariesFolder -value \"${BinariesFolder}\" -valuetype Folder\n")

file(TO_NATIVE_PATH ${InstallerDir} InstallerDirWinPath)
file(APPEND ${InstallerConfigFile} "SetOutputLocation -buildname DefaultBuild -path \"${InstallerDirWinPath}\"\n")
file(APPEND ${InstallerConfigFile} "Save\nRebuild")

file(COPY "${SUPER_PROJECT_SOURCE_DIR}/tools/installers/win/${TargetType}.aip" DESTINATION "${InstallerDir}")
file(TO_NATIVE_PATH ${InstallerConfigFile} InstallerConfigFileWinPath)

execute_process(COMMAND "${AdvancedInstallerPath}" /execute ${TargetType}.aip ${InstallerConfigFileWinPath}
                WORKING_DIRECTORY "${InstallerDir}"
                RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
if(NOT "${ResVar}" EQUAL 0)
  message(FATAL_ERROR "Failed - ${OutVar}")
endif()

message("Success")
