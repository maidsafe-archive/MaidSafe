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


ms_check_compiler()
ms_underscores_to_camel_case(${PROJECT_NAME} CamelCaseProjectName)


if(NOT PROJECT_NAME STREQUAL "Cryptopp" AND
   NOT PROJECT_NAME STREQUAL "sqlite" AND
   NOT PROJECT_NAME STREQUAL "launcher_ui")
  ms_get_branch_and_commit(Branch Commit)
  set(Msg "Configuring MaidSafe ${CamelCaseProjectName} project on ${Branch} at commit ${Commit}")
  string(REGEX REPLACE . "-" Underscore ${Msg})
  message("${HR}\n${Msg}\n${Underscore}")
endif()


set(CMAKE_MODULE_PATH ${maidsafe_SOURCE_DIR}/cmake_modules)


if(INCLUDE_TESTS)
  set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: All.  ")
  if(NOT MAIDSAFE_TEST_TYPE)
    set(MAIDSAFE_TEST_TYPE "ALL" CACHE string "Choose the type of TEST, options are: ALL, BEH, FUNC, UNIT, NETWORK" FORCE)
  elseif(MAIDSAFE_TEST_TYPE STREQUAL "BEH")
    set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: Behavioural.  ")
  elseif(MAIDSAFE_TEST_TYPE STREQUAL "FUNC")
    set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: Functional.  ")
  elseif(MAIDSAFE_TEST_TYPE STREQUAL "NETWORK")
    set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: Network.  ")
  elseif(MAIDSAFE_TEST_TYPE STREQUAL "UNIT")
    set(MAIDSAFE_TEST_TYPE_MESSAGE "GTests included: Behavioural and Functional.  ")
  else()
    set(MAIDSAFE_TEST_TYPE "ALL" CACHE string "Choose the type of TEST, options are: ALL, BEH, FUNC, UNIT, NETWORK" FORCE)
  endif()
  enable_testing()
endif()


set_property(GLOBAL PROPERTY USE_FOLDERS ON)


if(UNIX)
  set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
  set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif()


set(CMAKE_INCLUDE_DIRECTORIES_PROJECT_BEFORE ON)


include(check_licenses)
include(utils)
ms_check_licenses()
ms_set_postfixes()


# All other libraries search
if(UNIX)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib/i386-linux-gnu/ /usr/lib/x86_64-linux-gnu/ /usr/lib/)
  set(CMAKE_THREAD_PREFER_PTHREAD true)
  find_package(Threads REQUIRED)
  if(NOT APPLE)
    if(NOT ANDROID_BUILD)
      set(RtLibrary rt)
    endif()
    set(SYS_LIB ${CMAKE_THREAD_LIBS_INIT} ${RtLibrary})
  endif()
endif()


if(INCLUDE_TESTS)
  # Avoid running MemCheck on Style Check tests
  ms_add_memcheck_ignore(${CamelCaseProjectName}StyleCheck)

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
  if(MEMORY_CHECK)
    ms_set_global_test_timeout_factor(20)
  endif()

  if(UNIX)
    unset(MEMORYCHECK_SUPPRESSIONS_FILE CACHE)
    find_file(MEMORYCHECK_SUPPRESSIONS_FILE NAMES MemCheck.supp PATHS ${PROJECT_SOURCE_DIR} DOC "File that contains suppressions for the memory checker")
    set(MEMORYCHECK_COMMAND_OPTIONS "--tool=memcheck --quiet --verbose --trace-children=yes --demangle=yes --num-callers=50 --show-below-main=yes --leak-check=full --show-reachable=yes --track-origins=yes --gen-suppressions=all")
  endif()

  unset(MAKECOMMAND CACHE)
  include(CTest)
  include(add_gtests)
endif()


set(CPACK_STRIP_FILES TRUE)
