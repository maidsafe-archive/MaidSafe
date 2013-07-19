#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or    #
#  later, and The General Public License (GPL), version 3. By contributing code to this project    #
#  You agree to the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in    #
#  the root directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also    #
#  available at:                                                                                   #
#                                                                                                  #
#    http://www.novinet.com/license                                                                #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, software distributed under the       #
#  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       #
#  either express or implied. See the License for the specific language governing permissions      #
#  and limitations under the License.                                                              #
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
      passport
      rudp
      routing
      encrypt
      nfs
      vault
      drive
      lifestuff)
# Set all submodules' source dir before calling add_subdirectory to avoid
# having to run CMake twice in a clean build tree.
foreach(SubModule ${SubModules})
  set(${SubModule}_SOURCE_DIR ${CMAKE_SOURCE_DIR}/src/${SubModule})
endforeach()

if(EXISTS ${CMAKE_SOURCE_DIR}/src/lifestuff_ui_qt)
  list(APPEND SubModules lifestuff_ui_qt)
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


set(CMAKE_MODULE_PATH ${maidsafe_SOURCE_DIR}/cmake_modules)
include(version)


# Enable auto-linking qtmain for Windows
cmake_policy(SET CMP0020 NEW)


# Force all libraries and exes to be built to the top-level build dir.
set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR})


# Include helper functions, check compiler version, and get target platform details
include(utils)
check_compiler()
get_target_platform()
get_target_architecture()


# Adds a target 'CleanCoverage' on Unix.  When this target is built, it removes
# all .gcda files from the build directory and its subdirectories
if(UNIX)
  find_file(CleanCoverageFile clean_coverage.cmake cmake_modules)
  add_custom_target(CleanCoverage COMMAND ${CMAKE_COMMAND} -DSearchDir=${CMAKE_BINARY_DIR} -P ${CleanCoverageFile})
  option(COVERAGE "If ON, targets will be built with coverage flags included." OFF)
  set(MaidsafeCoverage ${COVERAGE})
endif()


include(maidsafe_find_just_thread)
include(maidsafe_find_python)
include(maidsafe_find_git)
include(maidsafe_find_openmp)

if(APPLE)
  option(HAVE_LIBC++ "If ON, '-stdlib=libc++' is added to the compile and link flags" ON)

else()
  option(HAVE_LIBC++ "If ON, '-stdlib=libc++' is added to the compile and link flags" OFF)
  option(HAVE_LIBC++ABI "If ON, '-lc++abi' is added to the link flags" OFF)
endif()
option(USE_JUST_THREADS "If ON, try to find and use the just::thread library (Windows or gcc 4.7 only)" OFF)


add_subdirectory(src/third_party_libs)


# Add each sub-module
foreach(SubModule ${SubModules})
  add_subdirectory(src/${SubModule})
endforeach()

if(EXISTS ${CMAKE_SOURCE_DIR}/src/sandbox)
  message("${HR}\nConfiguring Sandbox projects\n----------------------------")
  file(GLOB AllSandboxDirs "${maidsafe_SOURCE_DIR}/src/sandbox/*")
  list(REMOVE_ITEM AllSandboxDirs "${maidsafe_SOURCE_DIR}/src/sandbox/.git"
                                  "${maidsafe_SOURCE_DIR}/src/sandbox/.gitignore"
                                  "${maidsafe_SOURCE_DIR}/src/sandbox/README")
  foreach(SandboxDir ${AllSandboxDirs})
    get_filename_component(DirName ${SandboxDir} NAME)
    message(STATUS "${DirName}")
    add_subdirectory(${SandboxDir})
  endforeach()
endif()

message("${HR}")

if(UNIX)
  if(MaidsafeCoverage)
    message(STATUS "Coverage ON")
  else()
    message(STATUS "Coverage OFF.  To enable, re-run CMake with -DCOVERAGE=ON")
  endif()
endif()

message(STATUS "To include all tests,                    cmake . -DMAIDSAFE_TEST_TYPE=ALL")
message(STATUS "To include only behavioural tests,       cmake . -DMAIDSAFE_TEST_TYPE=BEH")
message(STATUS "To include only functional tests,        cmake . -DMAIDSAFE_TEST_TYPE=FUNC")
message("${HR}")


tidy_ctest_custom()
cleanup_temp_dir()
rename_outdated_built_exes()


unset(MAKECOMMAND CACHE)
include(CTest)
setup_ci_scripts()
unset(QA_BUILD CACHE)


# Convenience feature - adds all CMake related files to folder in VS IDE
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
