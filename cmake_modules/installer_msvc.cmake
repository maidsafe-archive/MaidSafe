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
  if(NOT "${TargetType}" STREQUAL DevDebug)
    message(FATAL_ERROR "Debug installers are currently disabled in Windows")
  endif()
  message(FATAL_ERROR "Invalid build configuration.  ${TargetName} is only availale for Release builds.")
endif()

if (${TargetType} STREQUAL Farmer)
  set(AdvancedInstallerPath "C:/Program Files (x86)/Caphyon/Advanced Installer 11.4.1/bin/x86/AdvancedInstaller.com")
  set(InstallerDir "${SUPER_PROJECT_BINARY_DIR}/Release/Installer")

  if(CMAKE_CL_64)
    set(PACKAGE_TYPE x64)
  else()
    set(PACKAGE_TYPE x86)
  endif()

  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${InstallerDir}")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/tools/installers/win/farmer.aip" DESTINATION "${InstallerDir}")

  ### TODO: All execute process commands to be bundled as a single command via .aic file

  ## Set Properties
  execute_process(COMMAND "${AdvancedInstallerPath}" /edit farmer.aip /SetProperty UDInstallerType=${TargetType}
                  WORKING_DIRECTORY "${InstallerDir}"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT "${ResVar}" EQUAL 0)
    message(FATAL_ERROR "Failed ${OutVar}")
  endif()

  execute_process(COMMAND "${AdvancedInstallerPath}" /edit farmer.aip /SetProperty UDPackageType=${PACKAGE_TYPE}
                  WORKING_DIRECTORY "${InstallerDir}"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT "${ResVar}" EQUAL 0)
    message(FATAL_ERROR "Failed ${OutVar}")
  endif()

  ## Set Version
  execute_process(COMMAND "${AdvancedInstallerPath}" /edit farmer.aip /SetVersion ${Version}
                  WORKING_DIRECTORY "${InstallerDir}"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT "${ResVar}" EQUAL 0)
    message(FATAL_ERROR "Failed ${OutVar}")
  endif()

  ## Set Package Type
  execute_process(COMMAND "${AdvancedInstallerPath}" /edit farmer.aip /SetPackageType ${PACKAGE_TYPE}
                  WORKING_DIRECTORY "${InstallerDir}"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT "${ResVar}" EQUAL 0)
    message(FATAL_ERROR "Failed ${OutVar}")
  endif()

  ## Add Files - TODO: Needs Updated to use custom Path variable from Release folder
  separate_arguments(TargetExes WINDOWS_COMMAND "${TargetExes}")
  foreach(Exe ${TargetExes})
    file(TO_NATIVE_PATH ${Exe} Exe)
    execute_process(COMMAND "${AdvancedInstallerPath}" /edit farmer.aip /AddFile APPDIR ${Exe}
                    WORKING_DIRECTORY "${InstallerDir}"
                    RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
    if(NOT "${ResVar}" EQUAL 0)
      message(FATAL_ERROR "Failed ${OutVar}")
    endif()
  endforeach()

  ## Set Installer Output Location
  file(TO_NATIVE_PATH ${InstallerDir} InstallerDirWinPath)
  execute_process(COMMAND "${AdvancedInstallerPath}" /edit farmer.aip /SetOutputLocation -buildname DefaultBuild -path ${InstallerDirWinPath}
                  WORKING_DIRECTORY "${InstallerDir}"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT "${ResVar}" EQUAL 0)
    message(FATAL_ERROR "Failed ${OutVar}")
  endif()

  ## Rebuild Installer to build regardless of changes
  execute_process(COMMAND "${AdvancedInstallerPath}" /rebuild farmer.aip
                  WORKING_DIRECTORY "${InstallerDir}"
                  RESULT_VARIABLE ResVar OUTPUT_VARIABLE OutVar)
  if(NOT "${ResVar}" EQUAL 0)
    message(FATAL_ERROR "Failed ${OutVar}")
  endif()

  message("Done")
endif()

# message("TargetType - ${TargetType}")
# message("TargetName - ${TargetName}")
# message("Version - ${Version}")
# message("Config - ${Config}")
# message("CMAKE_CL_64 - ${CMAKE_CL_64}")
# message("BoostSourceDir - ${BoostSourceDir}")

# message("SUPER_PROJECT_BINARY_DIR - ${SUPER_PROJECT_BINARY_DIR}")
# message("SUPER_PROJECT_SOURCE_DIR - ${SUPER_PROJECT_SOURCE_DIR}")

# separate_arguments(TargetLibs WINDOWS_COMMAND "${TargetLibs}")
# foreach(Lib ${TargetLibs})
  # message("TargetLibs - ${Lib}")
# endforeach()

# separate_arguments(TargetHeaders WINDOWS_COMMAND "${TargetHeaders}")
# foreach(Header ${TargetHeaders})
  # message("TargetHeaders - ${Header}")
# endforeach()

# separate_arguments(TargetExes WINDOWS_COMMAND "${TargetExes}")
# foreach(Exe ${TargetExes})
  # message("TargetExes - ${Exe}")
# endforeach()
