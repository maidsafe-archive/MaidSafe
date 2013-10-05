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
#                                                                                                  #
#  Module used to create standard setup of each project                                            #
#                                                                                                  #
#==================================================================================================#


# Disable auto-linking qtmain for Windows.  This policy can be set to NEW (and the corresponding
# explicit linking of qtmain can be removed) when the minimum CMake version becomes 2.8.11.
if(POLICY CMP0020)
  cmake_policy(SET CMP0020 OLD)
endif()

check_compiler()
underscores_to_camel_case(${PROJECT_NAME} CamelCaseProjectName)

string(REGEX REPLACE . "-" UNDERSCORE ${CamelCaseProjectName})
if(NOT PROJECT_NAME STREQUAL Cryptopp AND NOT PROJECT_NAME STREQUAL leveldb AND NOT PROJECT_NAME STREQUAL network_viewer)
  message("${HR}\nConfiguring MaidSafe ${CamelCaseProjectName} project\n--------------------${UNDERSCORE}---------")
endif()

set(CMAKE_MODULE_PATH ${maidsafe_SOURCE_DIR}/cmake_modules)


set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: All")
if(NOT MAIDSAFE_TEST_TYPE)
  set(MAIDSAFE_TEST_TYPE "ALL" CACHE string "Choose the type of TEST, options are: ALL, BEH, FUNC" FORCE)
else()
  if(${MAIDSAFE_TEST_TYPE} MATCHES BEH)
    set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: Behavioural")
  elseif(${MAIDSAFE_TEST_TYPE} MATCHES FUNC)
    set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: Functional")
  else()
    set(MAIDSAFE_TEST_TYPE "ALL" CACHE string "Choose the type of TEST, options are: ALL BEH FUNC" FORCE)
  endif()
endif()


enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)


set(CMAKE_DEBUG_POSTFIX -d)
set(CMAKE_RELWITHDEBINFO_POSTFIX -rwdi)
set(CMAKE_MINSIZEREL_POSTFIX -msr)

if(UNIX)
  set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
  set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif()


set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)
include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories(SYSTEM "${BoostSourceDir}")
include_directories(SYSTEM "${CatchSourceDir}")
include_directories(SYSTEM "${maidsafe_SOURCE_DIR}/src/third_party_libs")  # for cryptopp
include_directories(SYSTEM "${maidsafe_SOURCE_DIR}/src/third_party_libs/protobuf/src")
include_directories(SYSTEM "${maidsafe_SOURCE_DIR}/src/third_party_libs/googlemock/gtest/include")
include_directories(SYSTEM "${maidsafe_SOURCE_DIR}/src/third_party_libs/googlemock/include")
include_directories(SYSTEM "${maidsafe_SOURCE_DIR}/src/third_party_libs/leveldb/include")

include(check_licenses)
include(utils)
include(add_catch_tests)
check_licenses()


# Create CTestCustom.cmake to avoid inclusion of coverage results from test files, protocol buffer files and main.cc files
file(WRITE ${PROJECT_BINARY_DIR}/CTestCustom.cmake "\n")
add_coverage_exclude(\\\\.pb\\\\.)
add_coverage_exclude(tests/)
add_coverage_exclude(main\\\\.cc)


# Avoid running MemCheck on Style Check tests
add_memcheck_ignore(${CamelCaseProjectName}StyleCheck)


# All other libraries search
if(UNIX)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib/i386-linux-gnu/ /usr/lib/x86_64-linux-gnu/ /usr/lib/)
  set(CMAKE_THREAD_PREFER_PTHREAD true)
  find_package(Threads REQUIRED)
  if(NOT APPLE)
    set(SYS_LIB ${CMAKE_THREAD_LIBS_INIT} rt)
  endif()
endif()


set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 50000)
set(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 50000)
set(CTEST_CONTINUOUS_DURATION 600)
set(CTEST_CONTINUOUS_MINIMUM_INTERVAL 10)
set(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY true)

if(NOT DEFINED MEMORY_CHECK)
  if($ENV{MEMORY_CHECK})
    set(MEMORY_CHECK ON)
  endif()
endif()

if(UNIX)
  unset(MEMORYCHECK_SUPPRESSIONS_FILE CACHE)
  find_file(MEMORYCHECK_SUPPRESSIONS_FILE NAMES MemCheck.supp PATHS ${PROJECT_SOURCE_DIR} DOC "File that contains suppressions for the memory checker")
  set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --quiet --verbose --trace-children=yes --demangle=yes --num-callers=50 --show-below-main=yes --leak-check=full --show-reachable=yes --track-origins=yes --gen-suppressions=all")
endif()

unset(MAKECOMMAND CACHE)
include(CTest)
include(add_gtests)

set(CPACK_STRIP_FILES TRUE)
