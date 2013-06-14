#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2012 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#


cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
project(maidsafe)

enable_testing()
set(BUILD_SHARED_LIBS OFF)
set(HR "================================================================================")


set(SubModules
      common
      private
      rudp
      routing
      encrypt
      passport
      nfs
      vault
      drive
      lifestuff)
# Set all submodules' source dir before calling add_subdirectory to avoid
# having to run CMake twice in a clean build tree.
foreach(SubModule ${SubModules})
  set(${SubModule}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/${SubModule})
endforeach()

if(EXISTS ${CMAKE_SOURCE_DIR}/src/lifestuff-ui-qt)
  list(APPEND SubModules lifestuff-ui-qt)
endif()

# Check the submodules are initialised
foreach(SubModule ${SubModules})
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/src/${SubModule}/.git")
    set(ErrorMsg "\nThe git submodule for ${SubModule} is not intialised.  ")
    set(ErrorMsg "${ErrorMsg}Please cd to ${CMAKE_SOURCE_DIR} and run:\n    ")
    set(ErrorMsg "${ErrorMsg}git submodule update --init --recursive\n")
    message(FATAL_ERROR ${ErrorMsg})
  endif()
endforeach()

set(ApplicationVersionMajor 0)
set(ApplicationVersionMinor 1)
set(ApplicationVersionPatch 002)

# Testing environment if ApplicationVersionMinor is odd
math(EXPR MaidsafeTesting ${ApplicationVersionMinor}%2)


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})

# Include helper functions, check compiler version, and get target platform details
include(cmake_modules/utils.cmake)
check_compiler()
get_target_platform()
get_target_architecture()

# Find just::thread
include(cmake_modules/maidsafe_find_just_thread.cmake)

# Coverage:When this target is built, it removes all .gcda files from the build directory and its subdirectories
if(UNIX)
  find_file(CleanCoverageFile clean_coverage.cmake cmake_modules)
  add_custom_target(CleanCoverage COMMAND ${CMAKE_COMMAND} -DSearchDir=${CMAKE_BINARY_DIR} -P ${CleanCoverageFile})
  set(MaidsafeCoverage ${COVERAGE})
endif()

# Python interpreter and library search
if(NOT PYTHON_EXECUTABLE OR NOT PYTHONLIBS_FOUND)
  message("${HR}")
endif()
set(Python_ADDITIONAL_VERSIONS 3.9 3.8 3.7 3.6 3.5 3.4 3.3 3.2 3.1 3.0)
find_package(PythonInterp REQUIRED)
if(NOT PYTHONINTERP_FOUND)
  message(FATAL_ERROR "Didn't find python executable.")
endif()
if(WIN32)
  get_filename_component(PythonPath ${PYTHON_EXECUTABLE} PATH)
  get_filename_component(PythonLibName ${PythonPath} NAME)
  string(TOLOWER "${PythonLibName}" PythonLibName)
  find_library(PythonLibrary NAMES ${PythonLibName} PATHS ${PythonPath}/libs NO_DEFAULT_PATH)
  find_path(PythonIncludeDir NAMES Python.h PATHS ${PythonPath}/include NO_DEFAULT_PATH)
endif()
find_package(PythonLibs REQUIRED)


add_subdirectory(src/third_party_libs)


# Build protoc if not already done
set(ProtocBuildDir ${CMAKE_BINARY_DIR}/build_protoc)
find_program(ProtocExe NAMES protoc PATHS ${ProtocBuildDir} PATH_SUFFIXES Release NO_DEFAULT_PATH)
if(NOT ProtocExe)
  message("${HR}")
  message(STATUS "Building protoc.  Will take a minute or so...")
  execute_process(COMMAND
                    ${CMAKE_COMMAND} -E make_directory ${ProtocBuildDir}
                    OUTPUT_VARIABLE OutVar
                    RESULT_VARIABLE ResVar)
  if(NOT MSVC)
    set(ProtocBuildType "-DCMAKE_BUILD_TYPE=Release")
  endif()
  execute_process(COMMAND
                    ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/third_party_libs/protobuf -G ${CMAKE_GENERATOR} ${ProtocBuildType}
                    WORKING_DIRECTORY ${ProtocBuildDir}
                    OUTPUT_VARIABLE OutVar
                    RESULT_VARIABLE ResVar)
  set(ProtocOutVar "${OutVar}")
  execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release WORKING_DIRECTORY ${ProtocBuildDir} OUTPUT_VARIABLE OutVar RESULT_VARIABLE ResVar)
  set(ProtocOutVar "${ProtocOutVar}\n\n\n\n*********************\n\n\n\n${OutVar}")
  find_program(ProtocExe NAMES protoc PATHS ${ProtocBuildDir} PATH_SUFFIXES Release NO_DEFAULT_PATH)
  if(NOT ProtocExe)
    message(FATAL_ERROR "${ProtocOutVar}")
  endif()
endif()


# Add each sub-module
foreach(SubModule ${SubModules})
  add_subdirectory(src/${SubModule})
endforeach()

if(EXISTS ${CMAKE_SOURCE_DIR}/src/sandbox)
  message("${HR}\nConfiguring Sandbox projects\n----------------------------")
  file(GLOB AllSandboxDirs "${maidsafe_SOURCE_DIR}/src/sandbox/*")
  list(REMOVE_ITEM AllSandboxDirs "${maidsafe_SOURCE_DIR}/src/sandbox/default"
                                  "${maidsafe_SOURCE_DIR}/src/sandbox/.git"
                                  "${maidsafe_SOURCE_DIR}/src/sandbox/.gitignore"
                                  "${maidsafe_SOURCE_DIR}/src/sandbox/README"
                                  "${maidsafe_SOURCE_DIR}/src/sandbox/standard_sandbox_setup.cmake")
  foreach(SandboxDir ${AllSandboxDirs})
    get_filename_component(DirName ${SandboxDir} NAME)
    message(STATUS "${DirName}")
    add_subdirectory(${SandboxDir})
  endforeach()
endif()

message("${HR}")

message(STATUS "To include all tests,                    cmake . -DMAIDSAFE_TEST_TYPE=ALL")
message(STATUS "To include behavioural tests,            cmake . -DMAIDSAFE_TEST_TYPE=BEH")
message(STATUS "To include functional tests,             cmake . -DMAIDSAFE_TEST_TYPE=FUNC")
message("${HR}")


tidy_ctest_custom()


# create hooks
# message("Running setup for hooks")
# if(UNIX)
#   execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/hooks/linux/scripts/hooks.py)
# else()
#   execute_process(COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/tools/hooks/windows/scripts/dist/hooks.exe)
# endif()
# message("${HR}")

cleanup_temp_dir()
rename_outdated_built_exes()

unset(MAKECOMMAND CACHE)
include(CTest)

setup_ci_scripts()

unset(QA_BUILD CACHE)

if(MSVC)
  set(RootFiles ${CMAKE_SOURCE_DIR}/CI.cmake.in
                ${CMAKE_SOURCE_DIR}/CMakeLists.txt
                ${CMAKE_SOURCE_DIR}/CTestConfig.cmake
                ${CMAKE_SOURCE_DIR}/Project.xml)
  source_group("root" FILES ${RootFiles})

  source_group("root\\src\\third_party_libs" FILES ${CMAKE_SOURCE_DIR}/src/third_party_libs/CMakeLists.txt)

  file(GLOB ModuleFiles ${CMAKE_SOURCE_DIR}/cmake_modules/*)
  source_group("root\\cmake_modules" FILES ${ModuleFiles})

  add_custom_target(CMakeFiles SOURCES ${RootFiles} src/third_party_libs/CMakeLists.txt ${ModuleFiles})
endif()
