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
#  To generate a developer package, run:                                                           #
#    cpack -G ZIP  (on Windows) or                                                                 #
#    cpack -G TGZ                                                                                  #
#                                                                                                  #
#==================================================================================================#


if(CPack_CMake_INCLUDED)
  message(FATAL_ERROR "\n\nCPack.cmake has already been included.  It *must* be included only at the end of this file.\n\n")
endif()

# Removing the previous CPackConfig.cmake & CPackSourceConfig.cmake files
find_file(CPACK_CONFIG_CMAKE_FILES NAMES CPackConfig.cmake CPackSourceConfig.cmake PATHS ${CMAKE_BINARY_DIR})
file(REMOVE ${CPACK_CONFIG_CMAKE_FILES})

if(WIN32)
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
    install(DIRECTORY ${CMAKE_BINARY_DIR}/Debug/ COMPONENT Apps CONFIGURATIONS Debug DESTINATION bin/debug FILES_MATCHING PATTERN "*.pdb")
  endif()

  install(FILES ${MSVCP120} ${MSVCR120} ${VCCORLIB120} COMPONENT Apps CONFIGURATIONS Debug DESTINATION bin/debug)
  install(FILES ${MSVCP120} ${MSVCR120} ${VCCORLIB120} COMPONENT Apps CONFIGURATIONS Release DESTINATION bin)
endif()
install(FILES ${CMAKE_SOURCE_DIR}/cmake_modules/find_maidsafe.cmake COMPONENT Development DESTINATION .)


set(CPACK_PACKAGE_NAME "MaidSafe")
set(CPACK_PACKAGE_VENDOR "MaidSafe.net")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SAFE network installer")
set(CPACK_PACKAGE_VERSION_MAJOR ${ApplicationVersionMajor})
set(CPACK_PACKAGE_VERSION_MINOR ${ApplicationVersionMinor})
set(CPACK_PACKAGE_VERSION_PATCH ${ApplicationVersionPatch})
set(CPACK_PACKAGE_VERSION "${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
if(WIN32 OR APPLE)
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "MaidSafe")
else()
  set(CPACK_PACKAGE_INSTALL_DIRECTORY "maidsafe")
endif()
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${TargetPlatform}_${TargetArchitecture}")
set(CPACK_PACKAGE_RELOCATABLE ON)
set(CPACK_STRIP_FILES ON)
set(CPACK_ARCHIVE_COMPONENT_INSTALL ON)

# Specify the full list of components
set(CPACK_COMPONENTS_ALL Development Apps Tools Tests Benchmarks)

# Libraries and Headers
set(CPACK_COMPONENT_DEVELOPMENT_DISPLAY_NAME "Libraries and Headers")
set(CPACK_COMPONENT_DEVELOPMENT_DESCRIPTION "C++ libraries and headers.")

# Apps
set(CPACK_COMPONENT_APPS_DISPLAY_NAME "Apps")
set(CPACK_COMPONENT_APPS_DESCRIPTION "Production apps.")
set(CPACK_COMPONENT_APPS_GROUP "Runtime")
set(CPACK_COMPONENT_APPS_DISABLED OFF)

# Tools
set(CPACK_COMPONENT_TOOLS_DISPLAY_NAME "Tools")
set(CPACK_COMPONENT_TOOLS_DESCRIPTION "Suite of tools providing utility functions.")
set(CPACK_COMPONENT_TOOLS_GROUP "Runtime")
set(CPACK_COMPONENT_TOOLS_DISABLED ON)

# Tests
set(CPACK_COMPONENT_TESTS_DISPLAY_NAME "Tests")
set(CPACK_COMPONENT_TESTS_DESCRIPTION "Suite comprising several executables which run tests.")
set(CPACK_COMPONENT_TESTS_GROUP "Runtime")
set(CPACK_COMPONENT_TESTS_DISABLED ON)

# Benchmarks
set(CPACK_COMPONENT_BENCHMARKS_DISPLAY_NAME "Benchmarks")
set(CPACK_COMPONENT_BENCHMARKS_DESCRIPTION "Suite comprising several executables which run benchmarks.")
set(CPACK_COMPONENT_BENCHMARKS_GROUP "Runtime")
set(CPACK_COMPONENT_BENCHMARKS_DISABLED ON)

set(CPACK_COMPONENT_GROUP_RUNTIME_DESCRIPTION "All executables/apps")
set(CPACK_COMPONENT_GROUP_RUNTIME_EXPANDED ON)

# Including CPack must be done *after* all the CPack varaibles are set, or they will have no effect. 
include(CPack)
