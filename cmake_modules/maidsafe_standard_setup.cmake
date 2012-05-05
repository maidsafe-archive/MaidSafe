#==============================================================================#
#                                                                              #
#  Copyright (c) 2012 MaidSafe.net limited                                     #
#                                                                              #
#  The following source code is property of MaidSafe.net limited and is not    #
#  meant for external use.  The use of this code is governed by the license    #
#  file licence.txt found in the root directory of this project and also on    #
#  www.maidsafe.net.                                                           #
#                                                                              #
#  You are not free to copy, amend or otherwise use this source code without   #
#  the explicit written permission of the board of directors of MaidSafe.net.  #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Module used to create standard setup of each project                        #
#                                                                              #
#==============================================================================#


set(HR "================================================================================" PARENT_SCOPE)

string(REGEX REPLACE . "-" UNDERSCORE ${PROJECT_NAME})
message("${HR}\nConfiguring MaidSafe ${PROJECT_NAME} project\n--------------------${UNDERSCORE}---------\n")


if(CMAKE_SIZEOF_VOID_P EQUAL 4)
  set(MS_PROCESSOR_WIDTH 32-bit)
else()
  set(MS_PROCESSOR_WIDTH 64-bit)
endif()


if(MSVC)
  if(${MSVC_VERSION} LESS 1700)  # i.e for MSVC < Visual Studio 11
    message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of Visual Studio less than 11.")
  endif()
endif()


get_filename_component(MAIDSAFE_SOURCE_DIR ${PROJECT_SOURCE_DIR} PATH)
get_filename_component(MAIDSAFE_SOURCE_DIR ${MAIDSAFE_SOURCE_DIR} PATH)
set(CMAKE_MODULE_PATH ${maidsafe_SOURCE_DIR}/cmake_modules)
#string(TOLOWER ${CMAKE_CXX_COMPILER_ID} COMPILER)
#set(CMAKE_INSTALL_PREFIX ${MAIDSAFE_SOURCE_DIR}/installed_${COMPILER})
#file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_MESSAGE)


set(MAIDSAFE_TEST_TYPE_MESSAGE "Tests included: All")
if(NOT MAIDSAFE_TEST_TYPE)
  set(MAIDSAFE_TEST_TYPE "ALL" CACHE string "Choose the type of TEST, options are: ALL, BEH, FUNC" FORCE)
else()
  if(${MAIDSAFE_TEST_TYPE} MATCHES BEH)
    set(MAIDSAFE_TEST_TYPE_MESSAGE "Tests included: Behavioural")
  elseif(${MAIDSAFE_TEST_TYPE} MATCHES FUNC)
    set(MAIDSAFE_TEST_TYPE_MESSAGE "Tests included: Functional")
  else()
    set(MAIDSAFE_TEST_TYPE "ALL" CACHE string "Choose the type of TEST, options are: ALL BEH FUNC" FORCE)
  endif()
endif()

enable_testing()
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

# if(APPLE)
#   set(CMAKE_OSX_SYSROOT "/")
# endif()

set(CMAKE_DEBUG_POSTFIX -d)
set(CMAKE_RELWITHDEBINFO_POSTFIX -rwdi)
set(CMAKE_MINSIZEREL_POSTFIX -msr)

# include_directories(${MaidSafeCommon_INCLUDE_DIR}/maidsafe)
# if(UNIX)
#   set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
#   set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
# endif()

# if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
#   include_directories("/usr/local/include/c++/v1")
# endif()

# include_directories(SYSTEM ${MaidSafeCommon_INCLUDE_DIR} ${MaidSafeCommon_INCLUDE_DIR}/breakpad ${Boost_INCLUDE_DIR})

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")
include_directories("${MAIDSAFE_SOURCE_DIR}/src/third_party_libs/")  # for cryptopp
include_directories("${MAIDSAFE_SOURCE_DIR}/src/third_party_libs/protobuf/src")
include_directories("${MAIDSAFE_SOURCE_DIR}/src/third_party_libs/googlemock/gtest/include")
include_directories("${MAIDSAFE_SOURCE_DIR}/src/third_party_libs/googlemock/include")
include_directories("${MAIDSAFE_SOURCE_DIR}/src/third_party_libs/glog/src")
include_directories("${MAIDSAFE_SOURCE_DIR}/src/third_party_libs/boost")

include(maidsafe_utils)
include(maidsafe_run_protoc)

# Create CTestCustom.cmake to avoid inclusion of coverage results from test files, protocol buffer files and main.cc files
file(WRITE ${PROJECT_BINARY_DIR}/CTestCustom.cmake "\n")
add_coverage_exclude(\\\\.pb\\\\.)
add_coverage_exclude(tests/)
add_coverage_exclude(main\\\\.cc)

# Avoid running MemCheck on STYLE_CHECK tests
add_memcheck_ignore(STYLE_CHECK)


###################################################################################################
# Python library search                                                                           #
###################################################################################################
# unset(PYTHON_EXECUTABLE CACHE)
include(FindPythonInterp)
set(Python_ADDITIONAL_VERSIONS 3.9 3.8 3.7 3.6 3.5 3.4 3.3 3.2 3.1 3.0)
find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -V ERROR_VARIABLE PYTHON_VERSION ERROR_STRIP_TRAILING_WHITESPACE)
  string(REPLACE "Python " "" PYTHON_VERSION ${PYTHON_VERSION})
  message(STATUS "Found python executable v${PYTHON_VERSION} - style checking enabled.")
else()
  message(STATUS "Didn't find python executable: style checking disabled.")
endif()


###################################################################################################
# All other libraries search                                                                      #
###################################################################################################
if(UNIX)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib/i386-linux-gnu/ /usr/lib/x86_64-linux-gnu/ /usr/lib/)
  set(CMAKE_THREAD_PREFER_PTHREAD true)
  find_package(Threads REQUIRED)
  set(SYS_LIB ${CMAKE_THREAD_LIBS_INIT})
endif()

# include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR})
# set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${INCLUDE_DIRS})
# 
# link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})
# 
# message("${HR}")


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

include(CTest)
include(maidsafe_add_gtests)

set(CPACK_STRIP_FILES TRUE)
