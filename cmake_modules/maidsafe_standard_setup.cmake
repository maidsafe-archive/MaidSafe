#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 maidsafe.net limited                                     #
#  All rights reserved.                                                        #
#                                                                              #
#  Redistribution and use in source and binary forms, with or without          #
#  modification, are permitted provided that the following conditions are met: #
#                                                                              #
#      * Redistributions of source code must retain the above copyright        #
#        notice, this list of conditions and the following disclaimer.         #
#      * Redistributions in binary form must reproduce the above copyright     #
#        notice, this list of conditions and the following disclaimer in the   #
#        documentation and/or other materials provided with the distribution.  #
#      * Neither the name of the maidsafe.net limited nor the names of its     #
#        contributors may be used to endorse or promote products derived from  #
#        this software without specific prior written permission.              #
#                                                                              #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  #
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
#  POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Written by maidsafe.net team                                                #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Module used to create standard setup of main CMakeLists.txt                 #
#                                                                              #
#==============================================================================#

if(WIN32)
  set(ERROR_MESSAGE_CMAKE_PATH "   cmake ..\\..")
else()
  set(ERROR_MESSAGE_CMAKE_PATH "cmake ../../..")
endif()

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

if(APPLE)
  set(CMAKE_OSX_SYSROOT "/")
endif()

if(NOT DEFINED ${KDEV})
  set(CMAKE_DEBUG_POSTFIX -d)
  set(CMAKE_RELWITHDEBINFO_POSTFIX -rwdi)
  set(CMAKE_MINSIZEREL_POSTFIX -msr)

  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    set(TEST_POSTFIX ${CMAKE_DEBUG_POSTFIX})
  elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
    set(TEST_POSTFIX ${CMAKE_RELWITHDEBINFO_POSTFIX})
  elseif(CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
    set(TEST_POSTFIX ${CMAKE_MINSIZEREL_POSTFIX})
  endif()
endif()

include_directories(${MaidSafeCommon_INCLUDE_DIR}/maidsafe)
if(UNIX)
  set(CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem ")
  set(CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem ")
endif()

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
  include_directories("/usr/local/include/c++/v1")
endif()

include_directories(SYSTEM ${MaidSafeCommon_INCLUDE_DIR} ${MaidSafeCommon_INCLUDE_DIR}/breakpad ${Boost_INCLUDE_DIR})

if(DEFINED ADD_LIBRARY_DIR)
  set(DEFAULT_LIBRARY_DIR ${DEFAULT_LIBRARY_DIR} ${ADD_LIBRARY_DIR})
  list(REMOVE_DUPLICATES DEFAULT_LIBRARY_DIR)
  set(DEFAULT_LIBRARY_DIR ${DEFAULT_LIBRARY_DIR} CACHE PATH "Path to libraries directories" FORCE)
endif()

set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} ${DEFAULT_LIBRARY_DIR})
include(maidsafe_utils)

# Create CTestCustom.cmake to avoid inclusion of coverage results from test files, protocol buffer files and main.cc files
file(WRITE ${PROJECT_BINARY_DIR}/CTestCustom.cmake "\n")
ADD_COVERAGE_EXCLUDE(\\\\.pb\\\\.)
ADD_COVERAGE_EXCLUDE(tests/)
ADD_COVERAGE_EXCLUDE(main\\\\.cc)

# Avoid running MemCheck on STYLE_CHECK tests
ADD_MEMCHECK_IGNORE(STYLE_CHECK)

include(maidsafe_run_protoc)


###################################################################################################
# Python library search                                                                           #
###################################################################################################
unset(PYTHON_EXECUTABLE CACHE)
include(FindPythonInterp)
set(Python_ADDITIONAL_VERSIONS 3.9 3.8 3.7 3.6 3.5 3.4 3.3 3.2 3.1 3.0)
find_package(PythonInterp)
if(PYTHONINTERP_FOUND)
  execute_process(COMMAND ${PYTHON_EXECUTABLE} -V ERROR_VARIABLE PYTHON_VERSION ERROR_STRIP_TRAILING_WHITESPACE)
  string(REPLACE "Python " "" PYTHON_VERSION ${PYTHON_VERSION})
  message("-- Found python executable v${PYTHON_VERSION} - style checking enabled.")
else()
  message("-- Didn't find python executable: style checking disabled.")
endif()


###################################################################################################
# All other libraries search                                                                      #
###################################################################################################
include(maidsafe_find_openmp)

if(UNIX)
  set(CMAKE_LIBRARY_PATH ${CMAKE_LIBRARY_PATH} /usr/lib/i386-linux-gnu/ /usr/lib/x86_64-linux-gnu/ /usr/lib/)
  set(CMAKE_THREAD_PREFER_PTHREAD true)
  find_package(Threads REQUIRED)
  set(SYS_LIB ${CMAKE_THREAD_LIBS_INIT})
elseif(WIN32)
  if(MSVC)
    set(SYS_LIB ws2_32 odbc32 odbccp32 WSock32 IPHlpApi)
  else()
    set(SYS_LIB advapi32 kernel32 ws2_32 iphlpapi mswsock)
  endif()
  foreach(library ${SYS_LIB})
    find_library(CURRENT_LIB ${library})
    if(CURRENT_LIB)
      message("-- Found library ${CURRENT_LIB}")
    else()
      set(ERROR_MESSAGE "\nCould not find library ${library}.")
      if(MSVC)
        set(ERROR_MESSAGE "${ERROR_MESSAGE}\nRun cmake from a Visual Studio Command Prompt.")
      else()
        set(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\n${ERROR_MESSAGE_CMAKE_PATH} -DADD_LIBRARY_DIR=<Path to ${library} directory>")
      endif()
      message(FATAL_ERROR "${ERROR_MESSAGE}")
    endif()
    unset(CURRENT_LIB CACHE)
  endforeach()
endif()

include_directories(BEFORE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR})
set(INCLUDE_DIRS ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR} ${INCLUDE_DIRS})

link_directories(${CMAKE_ARCHIVE_OUTPUT_DIRECTORY})

message("================================================================================")

if(MSVC)
  set(CMAKE_CXX_FLAGS "")
  set(CMAKE_CXX_FLAGS_INIT "")
  set(CMAKE_CXX_FLAGS_RELEASE "")
  set(CMAKE_CXX_FLAGS_DEBUG "")
endif()

# When this target is built, it removes all .gcda files from the build directory and its subdirectories
if(UNIX)
  find_file(CLEAN_COVERAGE clean_coverage.cmake ${CMAKE_MODULE_PATH})
  if(CLEAN_COVERAGE)
    add_custom_target(CleanCoverage COMMAND ${CMAKE_COMMAND} -DSEARCH_DIR=${CMAKE_BINARY_DIR} -P ${CLEAN_COVERAGE})
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

include(CTest)
include(maidsafe_add_gtests)

if(MAIDSAFE_COMMON_INSTALL_DIR)
  file(TO_CMAKE_PATH ${MAIDSAFE_COMMON_INSTALL_DIR} CMAKE_INSTALL_PREFIX)
endif()
if(INSTALL_PREFIX)
  file(TO_CMAKE_PATH ${INSTALL_PREFIX} INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}")
endif()
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_MESSAGE)

CLEANUP_TEMP_DIR()
set(CPACK_STRIP_FILES TRUE)