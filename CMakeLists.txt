#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
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


cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)

# Prefer out-of-source builds (i.e. the build root being outside of the source root), but allow for
# building in a folder immediately inside the source root (not one of the existing source folders).
set(SourceDir ${CMAKE_SOURCE_DIR})
foreach(SpecialChar "^" "$" "." "[" "]" "*" "+" "?" "|" "(" ")")
  string(REPLACE "${SpecialChar}" "\\${SpecialChar}" SourceDir "${SourceDir}")
endforeach()
if(CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR OR
   CMAKE_BINARY_DIR MATCHES "${SourceDir}/cmake_modules" OR
   CMAKE_BINARY_DIR MATCHES "${SourceDir}/src" OR
   CMAKE_BINARY_DIR MATCHES "${SourceDir}/tools")
  set(Msg "\n\nThis project doesn't allow \"in-source builds\".\nYou must run CMake from a folder outside")
  set(Msg "${Msg} the root of this project.\nFor more details about configuring the project see\n")
  if(WIN32)
    set(Msg "${Msg}https://github.com/maidsafe/MaidSafe/wiki/Build-Instructions-for-Windows")
  elseif(APPLE)
    set(Msg "${Msg}https://github.com/maidsafe/MaidSafe/wiki/Build-Instructions-for-OS-X")
  else()
    set(Msg "${Msg}https://github.com/maidsafe/MaidSafe/wiki/Build-Instructions-for-Linux")
  endif()
  message(WARNING "${Msg}\n\n")
  message(STATUS "Cancelling CMake and cleaning up source tree...")
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${CMAKE_BINARY_DIR}/CMakeFiles")
  # This next call should cause CMake to crash.  We should remove this dirty hack if CMake becomes
  # able to be cancelled in a clean way (i.e. doesn't leave behind files/folders).
  math(EXPR Crash 0/0)
  message(FATAL_ERROR "CMake should have crashed - this is a failsafe in case the call used to trigger the crash gets fixed.")
endif()


# Setup android build if required
if(ANDROID_BUILD)
  include("${CMAKE_SOURCE_DIR}/cmake_modules/android_setup.cmake")
endif()


project(maidsafe)


if(ANDROID_BUILD)
  ms_android_setup_flags()
endif()


if(CMAKE_CONFIGURATION_TYPES)
  set(CMAKE_CONFIGURATION_TYPES ${CMAKE_CONFIGURATION_TYPES} ReleaseNoInline CACHE INTERNAL "" FORCE)
else()
  if(NOT CMAKE_BUILD_TYPE)
    message(STATUS "No build type given.  Setting and caching CMAKE_BUILD_TYPE to Debug.")
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Type of build; options are: Debug, Release, RelWithDebInfo, or MinSizeRel." FORCE)
  else()
    message(STATUS "Build type set to ${CMAKE_BUILD_TYPE}")
  endif()
endif()


set(CMAKE_EXPORT_COMPILE_COMMANDS ON) # works for Unix MakeFiles (also ninja)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_ERROR_DEPRECATED ON)
set(CMAKE_ERROR_ON_ABSOLUTE_INSTALL_DESTINATION ON)
set(EXPORT_COMPILE_COMMANDS ON)
set(HR "================================================================================")


set(MaidsafeGeneratedSourcesDir ${CMAKE_BINARY_DIR}/GeneratedSources)
set(SubModules
      common
      passport
      crux
      routing
      encrypt
      #nfs
      vault
      vault_manager
      #drive
      api
      launcher)
# Set all submodules' source dir before calling add_subdirectory to avoid
# having to run CMake twice in a clean build tree.
foreach(SubModule ${SubModules})
  set(${SubModule}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/${SubModule})
endforeach()


# Check the submodules are initialised
foreach(SubModule ${SubModules})
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/src/${SubModule}/.git")
    set(ErrorMsg "\nThe git submodule for ${SubModule} is not intialised.  ")
    set(ErrorMsg "${ErrorMsg}Please cd to ${CMAKE_SOURCE_DIR} and run:\n    ")
    set(ErrorMsg "${ErrorMsg}git submodule update --init --recursive\n")
    message(FATAL_ERROR ${ErrorMsg})
  endif()
endforeach()


set(CMAKE_MODULE_PATH ${maidsafe_SOURCE_DIR}/cmake_modules)
include(version)


# Force all libraries and exes to be built to the top-level build dir.
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})


# Include helper functions, check compiler version, and get target platform details
include(utils)
ms_extra_platforms()
ms_check_compiler()
ms_get_target_platform()
ms_get_target_architecture()
unset(AllStaticLibs CACHE)


# Add build types for the Address, Memory, Thread and Undefined Behaviour Sanitizers
include(find_sanitize_blacklist)
include(find_asan)
include(find_msan)
include(find_tsan)
include(find_ubsan)
include(find_stack_protector_strong)


include(maidsafe_find_just_thread)
include(maidsafe_find_python)
include(maidsafe_find_git)


option(INCLUDE_TESTS "Whether to include test targets or not" ON)
if(APPLE)
  option(HAVE_LIBC++ "If ON, '-stdlib=libc++' is added to the compile and link flags" ON)
else()
  option(HAVE_LIBC++ "If ON, '-stdlib=libc++' is added to the compile and link flags" OFF)
  option(HAVE_LIBC++ABI "If ON, '-lc++abi' is added to the link flags" OFF)
endif()
# Don't use 'option' for logging so we can tell if the user has explicitly set LOGGING or not.
if(LOGGING)
  set(USE_LOGGING ON)
elseif(DEFINED LOGGING)
  set(DONT_USE_LOGGING ON)
endif()
option(VLOGGING "Whether to use Visualiser logging or not" OFF)
option(USE_BOOST_CACHE "If ON, try to download, extract and build boost to a single cache directory for all MaidSafe clones" OFF)
option(BOOST_DISABLE_ASSERTS "If ON, all BOOST_ASSERTs and BOOST_VERIFYs are disabled" OFF)
option(USE_JUST_THREADS "If ON, try to find and use the just::thread library (Windows or gcc 4.7 only)" OFF)
include(standard_flags)


if(INCLUDE_TESTS)
  include(CTest)

  # Adds a target 'CleanCoverage' on Unix.  When this target is built, it removes
  # all .gcda files from the build directory and its subdirectories
  if(UNIX)
    find_file(CleanCoverageFile clean_coverage.cmake cmake_modules)
    add_custom_target(CleanCoverage COMMAND ${CMAKE_COMMAND} -DSearchDir=${CMAKE_BINARY_DIR} -P ${CleanCoverageFile})
    option(COVERAGE "If ON, targets will be built with coverage flags included." OFF)
  endif()


  # 'SkipReturnCodeValue' is used as the exit code when running disabled GTests.
  # It must be between 0 and 255 inclusive.  (Bash returns 127 if a command is not found).
  set(SkipReturnCodeValue 127)


  # Prepare CTestCustom.cmake
  set(CTestCustomFile "${CMAKE_BINARY_DIR}/CTestCustom.cmake")
  file(WRITE ${CTestCustomFile} "")
  ms_add_coverage_exclude(\\\\.pb\\\\.)
  ms_add_coverage_exclude(tests/)
  ms_add_coverage_exclude(boost/)
  ms_add_coverage_exclude(src/third_party_libs/)
  ms_add_coverage_exclude(GeneratedProtoFiles/)
  ms_add_coverage_exclude(main\\\\.cc)
endif()


if(BOOTSTRAP)
  message(STATUS "BOOTSTRAP set as '${BOOTSTRAP}'.")
else()
  message(STATUS "BOOTSTRAP not set.")
endif()


# Add third party libs and each sub-module
if(CMAKE_VERSION VERSION_LESS "3.0")
  include_directories("${CMAKE_SOURCE_DIR}/src/third_party_libs/header_only")
endif()
add_subdirectory(src/third_party_libs)
foreach(SubModule ${SubModules})
  add_subdirectory(src/${SubModule})
endforeach()
include(exports)

message("${HR}")


# Tidy CTestCustom.cmake and apply pre- and post-test commands
if(INCLUDE_TESTS)
  file(STRINGS ${CTestCustomFile} CTestCustomContents)
  list(REMOVE_DUPLICATES CTestCustomContents)
  list(SORT CTestCustomContents)
  string(REPLACE ";" "\n" CTestCustomContents "${CTestCustomContents}")
  file(WRITE ${CTestCustomFile} "${CTestCustomContents}\n")
endif()


ms_cleanup_temp_dir()
ms_rename_outdated_built_exes()
message(STATUS "To see values of cached CMake variables, run 'cmake . -N -LH'")


unset(MAKECOMMAND CACHE)
ms_setup_ci_scripts()
unset(QA_BUILD CACHE)


# Convenience feature - adds all CMake related files to folder in VS IDE
if(MSVC OR XCODE)
  set(RootFiles ${CMAKE_SOURCE_DIR}/CI.cmake.in
                ${CMAKE_SOURCE_DIR}/CMakeLists.txt
                ${CMAKE_SOURCE_DIR}/CTestConfig.cmake
                ${CMAKE_SOURCE_DIR}/Project.xml)
  source_group("root" FILES ${RootFiles})

  source_group("root\\src\\third_party_libs" FILES ${CMAKE_SOURCE_DIR}/src/third_party_libs/CMakeLists.txt)

  file(GLOB ModuleFiles ${CMAKE_SOURCE_DIR}/cmake_modules/*)
  source_group("root\\cmake_modules" FILES ${ModuleFiles})

  add_custom_target(CMakeFiles SOURCES ${RootFiles} src/third_party_libs/CMakeLists.txt ${ModuleFiles})

  foreach(ConfigType ${CMAKE_CONFIGURATION_TYPES})
    file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/${ConfigType})
  endforeach()
endif()


include(add_installers)
