#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 MaidSafe.net limited                                     #
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


set(MS_PROJECT_NAME maidsafe_common)

cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)
message("================================================================================\n")

get_filename_component(CMAKE_BUILD_TYPE_DIR ${CMAKE_BINARY_DIR} NAME)

# Variable MSVC is not set until after "PROJECT" command below
if(CMAKE_BUILD_TYPE_DIR MATCHES "Win_MSVC")
  set(CMAKE_BUILD_TYPE_DIR Release)
elseif(CMAKE_BUILD_TYPE_DIR MATCHES "kdev")
  set(CMAKE_BUILD_TYPE_DIR Debug)
  set(KDEV 1)
endif()

if(NOT ${CMAKE_C_COMPILER_WORKS})
  message(FATAL_ERROR "No generator previously specified.\nTo see a full list of generators, run:\n\tcmake --help\n")
endif()

# This "PROJECT" command resets CMAKE_BUILD_TYPE, hence it's copied from CMAKE_BUILD_TYPE_DIR which is set above.
if((CMAKE_BUILD_TYPE_DIR MATCHES "Release") OR (DEFINED ${KDEV}))
  project(${MS_PROJECT_NAME})
else()
  string(TOLOWER ${CMAKE_BUILD_TYPE_DIR} MS_PROJECT_NAME_APPENDIX)
  project(${MS_PROJECT_NAME}_${MS_PROJECT_NAME_APPENDIX})
endif()

set(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_DIR} CACHE INTERNAL "Except for MSVC, the build type is set by running cmake from the appropriate build subdirectory e.g. for a Debug build, \"cd build/Linux/Debug && cmake ../../..\"." FORCE)

if(MSVC)
  if(${MSVC_VERSION} LESS 1700)  # i.e for MSVC < Visual Studio 11.0
    message(FATAL_ERROR "In order to use C++11 features, this library cannot be built using a version of Visual Studio less than 11.0 (i.e. VS 2011)")
  endif()
endif()


###################################################################################################
# Third party library search                                                                      #
###################################################################################################
unset(MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE CACHE)
unset(CRYPTO_TEST_CMAKE CACHE)
find_file(MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE install_prefix.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
find_file(CRYPTO_TEST_CMAKE crypto_test.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
if(NOT MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE OR NOT CRYPTO_TEST_CMAKE OR FORCE_THIRD_PARTY_BUILD)
  if(NOT FORCE_THIRD_PARTY_BUILD)
    set(FORCE_THIRD_PARTY_BUILD "ALL")
  endif()
  find_file(THIRD_PARTY_CMAKE CMakeLists.txt ${PROJECT_SOURCE_DIR}/../third_party_libs)
  if(NOT THIRD_PARTY_CMAKE)
    message(FATAL_ERROR "Third Party libs is broken.  Try a fresh checkout!")
  endif()
  if(${FORCE_THIRD_PARTY_BUILD} MATCHES "ALL")
    message("-- Creating third party libs.  Building ALL targets.")
  else()
    message("-- Creating third party libs.  Building target ${FORCE_THIRD_PARTY_BUILD} only.")
  endif()
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory BuildThirdPartyLibs)
  execute_process(COMMAND ${CMAKE_COMMAND} "-DBUILD_TARGET=${FORCE_THIRD_PARTY_BUILD}" ${THIRD_PARTY_CMAKE} -G ${CMAKE_GENERATOR} WORKING_DIRECTORY BuildThirdPartyLibs)# OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
  unset(FORCE_THIRD_PARTY_BUILD CACHE)
  find_file(MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE install_prefix.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
  find_file(CRYPTO_TEST_CMAKE crypto_test.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
  if(NOT MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE )
    message(FATAL_ERROR "Third Party libs is broken.  Try a fresh checkout.")
  endif()
endif()
include(${MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE})
set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build ${PROJECT_SOURCE_DIR}/../cmake_modules)
include(${MAIDSAFE_COMMON_INSTALL_PREFIX}/share/maidsafe/maidsafe_third_party.cmake)
set(MaidSafeCommon_INCLUDE_DIR ${MAIDSAFE_COMMON_INSTALL_PREFIX}/include)

set(PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB PROTO_FILES_COMMON RELATIVE ${PROTO_SOURCE_DIR} "${PROTO_SOURCE_DIR}/maidsafe/common/*.proto")
set(PROTO_FILES "${PROTO_FILES_COMMON}")


###################################################################################################
# Standard setup                                                                                  #
###################################################################################################
include(maidsafe_standard_setup)
# Need to overwrite defaults for this project
set(MaidSafeCommon_TOOLS_DIR ${PROJECT_SOURCE_DIR}/../tools)
HANDLE_VERSIONS(${PROJECT_SOURCE_DIR}/src/maidsafe/common/version.h)


###################################################################################################
# Boost libraries search                                                                          #
###################################################################################################
set(REQUIRED_BOOST_COMPONENTS filesystem system thread date_time unit_test_framework)
include(maidsafe_find_boost)
set(EXPORT_BOOST_CMAKE ${CMAKE_BINARY_DIR}/boost_libs.cmake)
file(WRITE ${EXPORT_BOOST_CMAKE} "${EXPORT_BOOST}")
file(APPEND ${EXPORT_BOOST_CMAKE} "\nSET(Boost_LIBRARY_DIRS \"${Boost_LIBRARY_DIRS}\")\n")
file(APPEND ${EXPORT_BOOST_CMAKE} "SET(Boost_INCLUDE_DIR \"${Boost_INCLUDE_DIR}\")\n")
include(${EXPORT_BOOST_CMAKE})
message("================================================================================")


###################################################################################################
# Set up all files as GLOBs                                                                       #
###################################################################################################
file(GLOB SOURCE_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/*.cc")
file(GLOB HEADER_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/*.h")
file(GLOB PROTO_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/*.proto")
file(GLOB SOURCE_PROTO_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/*.pb.cc")
file(GLOB HEADER_PROTO_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/*.pb.h")
file(GLOB MS_HEADER_PROTO_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/*_pb.h")

if(APPLE)
  # Currently not supporting breakpad using OSX, so remove breakpad header and source
  list(REMOVE_ITEM SOURCE_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/breakpad.cc")
  list(REMOVE_ITEM HEADER_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/breakpad.h")
endif()
set(ALL_FILES_COMMON_DIR ${SOURCE_FILES_COMMON_DIR} ${HEADER_FILES_COMMON_DIR} ${PROTO_FILES_COMMON_DIR})
list(REMOVE_ITEM SOURCE_FILES_COMMON_DIR ${SOURCE_PROTO_FILES_COMMON_DIR})
list(REMOVE_ITEM HEADER_FILES_COMMON_DIR ${HEADER_PROTO_FILES_COMMON_DIR} ${MS_HEADER_PROTO_FILES_COMMON_DIR})
source_group("common Source Files" FILES ${SOURCE_FILES_COMMON_DIR})
source_group("common Header Files" FILES ${HEADER_FILES_COMMON_DIR})
source_group("Common Proto Files" FILES ${PROTO_FILES_COMMON_DIR} ${SOURCE_PROTO_FILES_COMMON_DIR} ${HEADER_PROTO_FILES_COMMON_DIR} ${MS_HEADER_PROTO_FILES_COMMON_DIR})

set(TEST_MAIN "${PROJECT_SOURCE_DIR}/src/maidsafe/common/tests/main.cc")

file(GLOB SOURCE_FILES_COMMON_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/tests/*.cc")
file(GLOB HEADER_FILES_COMMON_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/tests/*.h")
set(ALL_FILES_COMMON_TESTS_DIR ${SOURCE_FILES_COMMON_TESTS_DIR} ${HEADER_FILES_COMMON_TESTS_DIR})
source_group("common tests Source Files" FILES ${SOURCE_FILES_COMMON_TESTS_DIR})
source_group("common tests Header Files" FILES ${HEADER_FILES_COMMON_TESTS_DIR})

file(GLOB SOURCE_FILES_COMMON_BOOST_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/tests/boost/*.cc")
file(GLOB HEADER_FILES_COMMON_BOOST_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/tests/boost/*.h")
set(ALL_FILES_COMMON_BOOST_TESTS_DIR ${SOURCE_FILES_COMMON_BOOST_TESTS_DIR} ${HEADER_FILES_COMMON_BOOST_TESTS_DIR})
source_group("common boost tests Source Files" FILES ${SOURCE_FILES_COMMON_BOOST_TESTS_DIR})
source_group("common boost tests Header Files" FILES ${HEADER_FILES_COMMON_BOOST_TESTS_DIR})

file(GLOB MODULE_FILES "${PROJECT_SOURCE_DIR}/../cmake_modules/*")
file(GLOB TOOL_FILES "${PROJECT_SOURCE_DIR}/../tools/*")


###################################################################################################
# Define MaidSafe libraries and executables                                                       #
###################################################################################################
MS_ADD_STATIC_LIBRARY(${MS_PROJECT_NAME} ${ALL_FILES_COMMON_DIR})
MS_ADD_EXECUTABLE(TESTcommon Tests ${TEST_MAIN} "${ALL_FILES_COMMON_TESTS_DIR}")
MS_ADD_EXECUTABLE(TESTboost Tests ${TEST_MAIN} ${ALL_FILES_COMMON_BOOST_TESTS_DIR})

target_link_libraries(maidsafe_common_static cryptopp_static protobuf_static glog_static boost_date_time boost_filesystem boost_serialization boost_system boost_thread ${SYS_LIB})
target_link_libraries(TESTcommon maidsafe_common_static gtest_static)
target_link_libraries(TESTboost maidsafe_common_static glog_static boost_unit_test_framework gtest_static ${SYS_LIB})


###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
include(maidsafe_standard_flags)
MS_DISABLE_WARNINGS(${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.cc)


###################################################################################################
# Tests                                                                                           #
###################################################################################################
set(STYLE_TESTS ${ALL_FILES_COMMON_DIR} ${ALL_FILES_COMMON_TESTS_DIR})
list(REMOVE_ITEM STYLE_TESTS "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.cc")
list(REMOVE_ITEM STYLE_TESTS "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.h")
list(REMOVE_ITEM STYLE_TESTS "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.proto")
ADD_STYLE_TEST(${STYLE_TESTS})
ADD_GTESTS(TESTcommon)
ADD_GTESTS(TESTboost)
include(${CRYPTO_TEST_CMAKE})

TEST_SUMMARY_OUTPUT()

# Remove files from coverage stats
ADD_COVERAGE_EXCLUDE(cryptopp/)
ADD_COVERAGE_EXCLUDE(third_party_libs/)
ADD_COVERAGE_EXCLUDE(utils\\\\.h)

# Remove third-party tests from MemCheck runs.
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_bind_function_simple)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_bind_function_object1)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_bind_function_object2)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_bind_member_function_non_void)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_bind_member_function_void)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_nested_bind)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_smart_ptr_AtomicCount1)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_smart_ptr_AtomicCount2)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_smart_ptr_make_shared)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_formattest1)
ADD_MEMCHECK_IGNORE(boost.BEH_BOOST_formattest2)
ADD_MEMCHECK_IGNORE(CrypoppTest.BEH_FULL)
ADD_MEMCHECK_IGNORE(CrypoppTest.BEH_AES)
ADD_MEMCHECK_IGNORE(CrypoppTest.BEH_SHA)
ADD_MEMCHECK_IGNORE(CrypoppTest.BEH_RSA_PSS)
ADD_MEMCHECK_IGNORE(CrypoppTest.BEH_RSA_OAEP)
ADD_MEMCHECK_IGNORE(CrypoppTest.BEH_RSA_PKCS1_1_5)


###################################################################################################
# Install details                                                                                 #
###################################################################################################
get_filename_component(COMMON_ROOT_DIR ${PROJECT_SOURCE_DIR} PATH)
set(CMAKE_INSTALL_PREFIX ${COMMON_ROOT_DIR}/installed)
if(INSTALL_PREFIX)
  file(TO_CMAKE_PATH ${INSTALL_PREFIX} INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}")
endif()
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_MESSAGE)

message("\nThe libraries, headers, cmake modules and tools will be installed to:\n")
message("    \"${CMAKE_INSTALL_PREFIX_MESSAGE}\"\n\n")
message("To include this project in any other MaidSafe project, use:\n")
message("    -DMAIDSAFE_COMMON_INSTALL_DIR=\"${CMAKE_INSTALL_PREFIX_MESSAGE}\"\n\n")
message("To force the rebuild of the third party libs, use one of:\n")
message(" ${ERROR_MESSAGE_CMAKE_PATH} -DFORCE_THIRD_PARTY_BUILD=ALL")
message(" ${ERROR_MESSAGE_CMAKE_PATH} -DFORCE_THIRD_PARTY_BUILD=GMOCK_GTEST")
message(" ${ERROR_MESSAGE_CMAKE_PATH} -DFORCE_THIRD_PARTY_BUILD=CRYPTOPP")
message(" ${ERROR_MESSAGE_CMAKE_PATH} -DFORCE_THIRD_PARTY_BUILD=PROTOBUF")
message(" ${ERROR_MESSAGE_CMAKE_PATH} -DFORCE_THIRD_PARTY_BUILD=GLOG")
message(" ${ERROR_MESSAGE_CMAKE_PATH} -DFORCE_THIRD_PARTY_BUILD=BREAKPAD\n\n")
message("To build and install this project now, run:\n")
if(MSVC)
  message("    cmake --build . --config Release --target install")
  message("    cmake --build . --config Debug --target install")
else()
  message("    cmake --build . --target install")
endif()
message("\n\n================================================================================"\n)

RENAME_OUTDATED_BUILT_EXES()

file(STRINGS ${PROJECT_SOURCE_DIR}/src/maidsafe/common/version.h MAIDSAFE_COMMON_VERSION
       REGEX "VERSION [0-9]+$")
string(REGEX MATCH "[0-9]+$" MAIDSAFE_COMMON_VERSION ${MAIDSAFE_COMMON_VERSION})

MS_INSTALL_LIBS(maidsafe_common_static)
list(REMOVE_ITEM HEADER_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.h")
list(REMOVE_ITEM HEADER_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt_pb.h")
MS_INSTALL_HEADERS(common ${HEADER_FILES_COMMON_DIR})

install(FILES ${MODULE_FILES} DESTINATION share/maidsafe/cmake_modules)
install(FILES ${TOOL_FILES} DESTINATION share/maidsafe/tools)
install(FILES ${EXPORT_BOOST_CMAKE} DESTINATION share/maidsafe)
MS_INSTALL_EXPORT()

find_file(CONINUOUS_SCRIPT_CMAKE maidsafe_dashboard.cmake PATHS ${CMAKE_MODULE_PATH})
if(CONINUOUS_SCRIPT_CMAKE)
  configure_file(${CONINUOUS_SCRIPT_CMAKE} ${CMAKE_BINARY_DIR}/maidsafe_dashboard.cmake @ONLY)
  install(FILES ${CMAKE_BINARY_DIR}/maidsafe_dashboard.cmake DESTINATION share/maidsafe/cmake_modules)
endif()