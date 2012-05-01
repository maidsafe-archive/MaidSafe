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


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
message("================================================================================\n")

project(common)

include(../cmake_modules/maidsafe_standard_setup.cmake)
add_subdirectory(../third_party_libs/cryptopp ../third_party_libs/cryptopp)
add_subdirectory(../third_party_libs/glog ../third_party_libs/glog)
# add_subdirectory(../third_party_libs/google-breakpad ../third_party_libs/google-breakpad)
add_subdirectory(../third_party_libs/googlemock ../third_party_libs/googlemock)
add_subdirectory(../third_party_libs/protobuf ../third_party_libs/protobuf)




###################################################################################################
# Third party library search                                                                      #
###################################################################################################
# unset(MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE CACHE)
# unset(CRYPTO_TEST_CMAKE CACHE)
# find_file(MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE install_prefix.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
# find_file(CRYPTO_TEST_CMAKE crypto_test.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
# if(NOT MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE OR NOT CRYPTO_TEST_CMAKE OR FORCE_THIRD_PARTY_BUILD)
#   if(NOT FORCE_THIRD_PARTY_BUILD)
#     set(FORCE_THIRD_PARTY_BUILD "ALL")
#   endif()
#   find_file(THIRD_PARTY_CMAKE CMakeLists.txt ${PROJECT_SOURCE_DIR}/../third_party_libs)
#   if(NOT THIRD_PARTY_CMAKE)
#     message(FATAL_ERROR "Third Party libs is broken.  Try a fresh checkout!")
#   endif()
#   if(${FORCE_THIRD_PARTY_BUILD} MATCHES "ALL")
#     message("-- Creating third party libs.  Building ALL targets.")
#   else()
#     message("-- Creating third party libs.  Building target ${FORCE_THIRD_PARTY_BUILD} only.")
#   endif()
#   execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory BuildThirdPartyLibs)
#   execute_process(COMMAND ${CMAKE_COMMAND} "-DBUILD_TARGET=${FORCE_THIRD_PARTY_BUILD}" ${THIRD_PARTY_CMAKE} -G ${CMAKE_GENERATOR} WORKING_DIRECTORY BuildThirdPartyLibs)# OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
#   unset(FORCE_THIRD_PARTY_BUILD CACHE)
#   find_file(MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE install_prefix.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
#   find_file(CRYPTO_TEST_CMAKE crypto_test.cmake ${PROJECT_SOURCE_DIR}/../third_party_libs)
#   if(NOT MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE )
#     message(FATAL_ERROR "Third Party libs is broken.  Try a fresh checkout.")
#   endif()
# endif()
# include(${MAIDSAFE_COMMON_INSTALL_PREFIX_CMAKE})
# set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/build ${PROJECT_SOURCE_DIR}/../cmake_modules)
# include(${MAIDSAFE_COMMON_INSTALL_PREFIX}/share/maidsafe/maidsafe_third_party.cmake)
# set(MaidSafeCommon_INCLUDE_DIR ${MAIDSAFE_COMMON_INSTALL_PREFIX}/include)



###################################################################################################
# Standard setup                                                                                  #
###################################################################################################
# include(../cmake_modules/maidsafe_standard_setup.cmake)
# Need to overwrite defaults for this project
# set(MaidSafeCommon_TOOLS_DIR ${PROJECT_SOURCE_DIR}/../tools)
# handle_versions(${PROJECT_SOURCE_DIR}/src/maidsafe/common/version.h)


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
set(PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB PROTO_FILES_COMMON RELATIVE ${PROTO_SOURCE_DIR} "${PROTO_SOURCE_DIR}/maidsafe/common/*.proto")
set(PROTO_FILES "${PROTO_FILES_COMMON}")
include(maidsafe_run_protoc)

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
ms_add_static_library(common ${ALL_FILES_COMMON_DIR})
ms_add_executable(TESTcommon Tests ${TEST_MAIN} "${ALL_FILES_COMMON_TESTS_DIR}")
ms_add_executable(TESTboost Tests ${TEST_MAIN} ${ALL_FILES_COMMON_BOOST_TESTS_DIR})

target_link_libraries(maidsafe_common cryptopp )#protobuf glog boost_date_time boost_filesystem boost_serialization boost_system boost_thread ${SYS_LIB})
target_link_libraries(TESTcommon maidsafe_common gtest)
target_link_libraries(TESTboost maidsafe_common glog boost_unit_test_framework gtest ${SYS_LIB})


###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
include(maidsafe_standard_flags)
ms_disable_warnings(${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.cc)


###################################################################################################
# Tests                                                                                           #
###################################################################################################
set(STYLE_TESTS ${ALL_FILES_COMMON_DIR} ${ALL_FILES_COMMON_TESTS_DIR})
list(REMOVE_ITEM STYLE_TESTS "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.cc")
list(REMOVE_ITEM STYLE_TESTS "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.h")
list(REMOVE_ITEM STYLE_TESTS "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.proto")
add_style_test(${STYLE_TESTS})
add_gtests(TESTcommon)
add_gtests(TESTboost)
include(${CRYPTO_TEST_CMAKE})

test_summary_output()

# Remove files from coverage stats
add_coverage_exclude(cryptopp/)
add_coverage_exclude(third_party_libs/)
add_coverage_exclude(utils\\\\.h)

# Remove third-party tests from MemCheck runs.
add_memcheck_ignore(boost.BEH_BOOST_bind_function_simple)
add_memcheck_ignore(boost.BEH_BOOST_bind_function_object1)
add_memcheck_ignore(boost.BEH_BOOST_bind_function_object2)
add_memcheck_ignore(boost.BEH_BOOST_bind_member_function_non_void)
add_memcheck_ignore(boost.BEH_BOOST_bind_member_function_void)
add_memcheck_ignore(boost.BEH_BOOST_nested_bind)
add_memcheck_ignore(boost.BEH_BOOST_smart_ptr_AtomicCount1)
add_memcheck_ignore(boost.BEH_BOOST_smart_ptr_AtomicCount2)
add_memcheck_ignore(boost.BEH_BOOST_smart_ptr_make_shared)
add_memcheck_ignore(boost.BEH_BOOST_formattest1)
add_memcheck_ignore(boost.BEH_BOOST_formattest2)
add_memcheck_ignore(CrypoppTest.BEH_FULL)
add_memcheck_ignore(CrypoppTest.BEH_AES)
add_memcheck_ignore(CrypoppTest.BEH_SHA)
add_memcheck_ignore(CrypoppTest.BEH_RSA_PSS)
add_memcheck_ignore(CrypoppTest.BEH_RSA_OAEP)
add_memcheck_ignore(CrypoppTest.BEH_RSA_PKCS1_1_5)


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

rename_outdated_built_exes()

file(STRINGS ${PROJECT_SOURCE_DIR}/src/maidsafe/common/version.h MAIDSAFE_COMMON_VERSION
       REGEX "VERSION [0-9]+$")
string(REGEX MATCH "[0-9]+$" MAIDSAFE_COMMON_VERSION ${MAIDSAFE_COMMON_VERSION})

ms_install_libs(maidsafe_common_static)
list(REMOVE_ITEM HEADER_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt.pb.h")
list(REMOVE_ITEM HEADER_FILES_COMMON_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/common/safe_encrypt_pb.h")
ms_install_headers(common ${HEADER_FILES_COMMON_DIR})

install(FILES ${MODULE_FILES} DESTINATION share/maidsafe/cmake_modules)
install(FILES ${TOOL_FILES} DESTINATION share/maidsafe/tools)
install(FILES ${EXPORT_BOOST_CMAKE} DESTINATION share/maidsafe)
ms_install_export()

find_file(CONINUOUS_SCRIPT_CMAKE maidsafe_dashboard.cmake PATHS ${CMAKE_MODULE_PATH})
if(CONINUOUS_SCRIPT_CMAKE)
  configure_file(${CONINUOUS_SCRIPT_CMAKE} ${CMAKE_BINARY_DIR}/maidsafe_dashboard.cmake @ONLY)
  install(FILES ${CMAKE_BINARY_DIR}/maidsafe_dashboard.cmake DESTINATION share/maidsafe/cmake_modules)
endif()
