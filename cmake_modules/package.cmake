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
#  Generates a package from all exported targets.                                                  #
#                                                                                                  #
#==================================================================================================#

#Removing the previous CPackConfig.cmake & CPackSourceConfig.cmake files
find_file(CPACK_CONFIG_CMAKE_FILES NAMES CPackConfig.cmake CPackSourceConfig.cmake PATHS ${CMAKE_BINARY_DIR})
file(REMOVE ${CPACK_CONFIG_CMAKE_FILES})

if(UNIX AND NOT APPLE)
elseif(APPLE)
elseif(WIN32)
  if(MSVC)
    if(CMAKE_CL_64)
      set(VC_RUNTIME_DIR "$ENV{VCInstallDir}/redist/x64/Microsoft.VC120.CRT")
    else()
      set(VC_RUNTIME_DIR "$ENV{VCInstallDir}/redist/x86/Microsoft.VC120.CRT")
    endif()
    find_file(MSVCP120 NAMES msvcp120.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
    find_file(MSVCR120 NAMES msvcr120.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
    find_file(VCCORLIB120 NAMES vccorlib120.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
    if(NOT MSVCP120)
      set(ERROR_MESSAGE "\nCould not find library msvcp120.dll.\nRun cmake from a Visual Studio Command Prompt.")
      message(FATAL_ERROR "${ERROR_MESSAGE}")
    endif()
  endif()

  install(FILES ${OpenMP_DLL} DESTINATION bin)
  install(FILES ${MSVCP120} DESTINATION bin)
  install(FILES ${MSVCR120} DESTINATION bin)
  install(FILES ${VCCORLIB120} DESTINATION bin)
endif()

set(CPACK_GENERATOR ZIP)
set(CPACK_PACKAGE_NAME "MaidSafe")
set(CPACK_PACKAGE_VERSION_MAJOR ${ApplicationVersionMajor})
set(CPACK_PACKAGE_VERSION_MINOR ${ApplicationVersionMinor})
set(CPACK_PACKAGE_VERSION_PATCH ${ApplicationVersionPatch})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${TargetPlatform}_${TargetArchitecture}")
set(CPACK_PACKAGE_RELOCATABLE ON)
set(CPACK_STRIP_FILES ON)
