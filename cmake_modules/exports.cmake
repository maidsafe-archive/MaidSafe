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
#  Create a file called maidsafe_export.cmake in the build root that may be included by outside    #
#  projects to import all available MaidSafe targets.                                              #
#                                                                                                  #
#==================================================================================================#


set(ExportFile ${CMAKE_BINARY_DIR}/maidsafe_export.cmake)
file(WRITE ${ExportFile} "# Path to MaidSafe super-project root\n")
file(APPEND ${ExportFile} "set(maidsafe_SOURCE_DIR \"${CMAKE_SOURCE_DIR}\")\n\n")
file(APPEND ${ExportFile} "# Add module path to CMAKE_MODULE_PATH\n")
file(APPEND ${ExportFile} "set(CMAKE_MODULE_PATH \"${maidsafe_SOURCE_DIR}/cmake_modules\")\n\n")
file(APPEND ${ExportFile} "# Handle inclusion of just::thread\n")
file(APPEND ${ExportFile} "set(USE_JUST_THREADS ${USE_JUST_THREADS})\n")
file(APPEND ${ExportFile} "include(maidsafe_find_just_thread)\n\n")
file(APPEND ${ExportFile} "# Handle libc++ variables\n")
file(APPEND ${ExportFile} "set(HAVE_LIBC++ ${HAVE_LIBC++})\n")
file(APPEND ${ExportFile} "set(HAVE_LIBC++ABI ${HAVE_LIBC++ABI})\n\n")
file(APPEND ${ExportFile} "# Set postfixes\n")
file(APPEND ${ExportFile} "set(CMAKE_DEBUG_POSTFIX -d)\n")
file(APPEND ${ExportFile} "set(CMAKE_RELWITHDEBINFO_POSTFIX -rwdi)\n")
file(APPEND ${ExportFile} "set(CMAKE_MINSIZEREL_POSTFIX -msr)\n")
file(APPEND ${ExportFile} "set(CMAKE_RELEASENOINLINE_POSTFIX -rni)\n\n")
if(Cbfs_FOUND)
  file(APPEND ${ExportFile} "# Set Callback Filesystem GUIDs\n")
  file(APPEND ${ExportFile} "set(CbfsGUIDTesting \"${CbfsGUIDTesting}\")\n")
  file(APPEND ${ExportFile} "set(CbfsGUIDLocal \"${CbfsGUIDLocal}\")\n")
  file(APPEND ${ExportFile} "set(CbfsGUIDNetwork \"${CbfsGUIDNetwork}\")\n\n")
endif()

if(ANDROID_BUILD)
  file(APPEND ${ExportFile} "set(ANDROID_BUILD \"${ANDROID_BUILD}\")\n")
  file(APPEND ${ExportFile} "set(AndroidApiLevel \"${AndroidApiLevel}\")\n")
  file(APPEND ${ExportFile} "set(ANDROID_NDK_TOOLCHAIN_ROOT \"${ANDROID_NDK_TOOLCHAIN_ROOT}\")\n")
  file(APPEND ${ExportFile} "set(CMAKE_C_COMPILER \"${CMAKE_C_COMPILER}\")\n")
  file(APPEND ${ExportFile} "set(CMAKE_CXX_COMPILER \"${CMAKE_CXX_COMPILER}\")\n")
  file(APPEND ${ExportFile} "macro(ms_android_setup_flags)\n")
  file(APPEND ${ExportFile} "  set(CMAKE_C_FLAGS \"${CMAKE_C_FLAGS}\")\n")
  file(APPEND ${ExportFile} "  set(CMAKE_CXX_FLAGS \"${CMAKE_CXX_FLAGS}\")\n")
  file(APPEND ${ExportFile} "  set(CMAKE_EXE_LINKER_FLAGS \"${CMAKE_EXE_LINKER_FLAGS}\")\n")
  file(APPEND ${ExportFile} "  set(CMAKE_SHARED_LINKER_FLAGS \"${CMAKE_SHARED_LINKER_FLAGS}\")\n")
  file(APPEND ${ExportFile} "endmacro()\n\n")
endif()

#set(AllTargets ${AllStaticLibs} local_drive drive sqlite cryptopp gmock gtest)
set(AllTargets ${AllStaticLibs} sqlite cryptopp gmock gtest)
if(NOT CMAKE_VERSION VERSION_LESS "3.0")
  list(APPEND AllTargets asio cereal)
endif()
if(Cbfs_FOUND)
  list(APPEND AllTargets cbfs_driver)
endif()

set(AutoGeneratedExportFile ${CMAKE_BINARY_DIR}/maidsafe_export_auto_generated.cmake)
export(TARGETS ${AllTargets} FILE ${AutoGeneratedExportFile})
file(APPEND ${ExportFile} "# Include file generated by 'export' command\n")
file(APPEND ${ExportFile} "include(\"${AutoGeneratedExportFile}\")\n\n")

# Workaround for CMake bug - once fixed remove this 'foreach' loop.
file(APPEND ${ExportFile} "\n\n\n\n\n")
foreach(Target ${AllTargets})
  get_target_property(SystemIncludes ${Target} INTERFACE_SYSTEM_INCLUDE_DIRECTORIES)
  if(SystemIncludes)
    file(APPEND ${ExportFile} "# Add INTERFACE_SYSTEM_INCLUDE_DIRECTORIES for target ${Target}\n")
    file(APPEND ${ExportFile} "set_target_properties(${Target} PROPERTIES\n")
    file(APPEND ${ExportFile} "  INTERFACE_SYSTEM_INCLUDE_DIRECTORIES \"${SystemIncludes}\"\n")
    file(APPEND ${ExportFile} "  )\n\n")
  endif()
endforeach()

# Workaround for CMake bug - once fixed, add ${AllBoostLibs} to 'export(TARGETS...)'
# command above and remove this 'foreach' loop.
file(APPEND ${ExportFile} "\n\n\n\n\n")
foreach(BoostLib ${AllBoostLibs})
  if(MSVC)
    get_target_property(LocationDebug ${BoostLib} IMPORTED_LOCATION_DEBUG)
    get_target_property(LocationRelease ${BoostLib} IMPORTED_LOCATION_RELEASE)
    get_target_property(LocationMinSizeRel ${BoostLib} IMPORTED_LOCATION_MINSIZEREL)
    get_target_property(LocationRelWithDebInfo ${BoostLib} IMPORTED_LOCATION_RELWITHDEBINFO)
  else()
    get_target_property(Location ${BoostLib} IMPORTED_LOCATION)
  endif()
  file(APPEND ${ExportFile} "# Create imported target ${BoostLib}\n")
  file(APPEND ${ExportFile} "add_library(${BoostLib} STATIC IMPORTED)\n\n")
  if(MSVC)
    file(APPEND ${ExportFile} "# Import target \"${BoostLib}\" for configuration \"Debug\"\n")
    file(APPEND ${ExportFile} "set_property(TARGET ${BoostLib} APPEND PROPERTY IMPORTED_CONFIGURATIONS DEBUG)\n")
    file(APPEND ${ExportFile} "set_target_properties(${BoostLib} PROPERTIES\n")
    file(APPEND ${ExportFile} "  IMPORTED_LOCATION_DEBUG \"${LocationDebug}\"\n")
    file(APPEND ${ExportFile} "  )\n\n")
    file(APPEND ${ExportFile} "# Import target \"${BoostLib}\" for configuration \"Release\"\n")
    file(APPEND ${ExportFile} "set_property(TARGET ${BoostLib} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)\n")
    file(APPEND ${ExportFile} "set_target_properties(${BoostLib} PROPERTIES\n")
    file(APPEND ${ExportFile} "  IMPORTED_LOCATION_RELEASE \"${LocationRelease}\"\n")
    file(APPEND ${ExportFile} "  )\n\n")
    file(APPEND ${ExportFile} "# Import target \"${BoostLib}\" for configuration \"MinSizeRel\"\n")
    file(APPEND ${ExportFile} "set_property(TARGET ${BoostLib} APPEND PROPERTY IMPORTED_CONFIGURATIONS MINSIZEREL)\n")
    file(APPEND ${ExportFile} "set_target_properties(${BoostLib} PROPERTIES\n")
    file(APPEND ${ExportFile} "  IMPORTED_LOCATION_MINSIZEREL \"${LocationMinSizeRel}\"\n")
    file(APPEND ${ExportFile} "  )\n\n")
    file(APPEND ${ExportFile} "# Import target \"${BoostLib}\" for configuration \"RelWithDebInfo\"\n")
    file(APPEND ${ExportFile} "set_property(TARGET ${BoostLib} APPEND PROPERTY IMPORTED_CONFIGURATIONS RELWITHDEBINFO)\n")
    file(APPEND ${ExportFile} "set_target_properties(${BoostLib} PROPERTIES\n")
    file(APPEND ${ExportFile} "  IMPORTED_LOCATION_RELWITHDEBINFO \"${LocationRelWithDebInfo}\"\n")
    file(APPEND ${ExportFile} "  )\n\n")
  else()
    file(APPEND ${ExportFile} "# Import target \"${BoostLib}\" for all configurations\n")
    file(APPEND ${ExportFile} "set_target_properties(${BoostLib} PROPERTIES\n")
    file(APPEND ${ExportFile} "  IMPORTED_LOCATION \"${Location}\"\n")
    file(APPEND ${ExportFile} "  )\n\n")
  endif()
endforeach()
