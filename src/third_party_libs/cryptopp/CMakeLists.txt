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

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
#include(${PROJECT_SOURCE_DIR}/cmake_modules/maidsafe_standard_flags.cmake)
include(${PROJECT_SOURCE_DIR}/cmake_modules/maidsafe_standard_setup.cmake)

project(Cryptopp)
enable_testing()

 # enable libc++ if available
if(APPLE OR HAVE_LIBC++)
  set(LIBC++ "-stdlib=libc++")
endif()

set(CMAKE_DEBUG_POSTFIX _d)
set(CMAKE_RELWITHDEBINFO_POSTFIX _rwdi)
set(CMAKE_MINSIZEREL_POSTFIX _msr)

if(CMAKE_BUILD_TYPE MATCHES "Debug")
  set(TEST_POSTFIX ${CMAKE_DEBUG_POSTFIX})
elseif(CMAKE_BUILD_TYPE MATCHES "RelWithDebInfo")
  set(TEST_POSTFIX ${CMAKE_RELWITHDEBINFO_POSTFIX})
elseif(CMAKE_BUILD_TYPE MATCHES "MinSizeRel")
  set(TEST_POSTFIX ${CMAKE_MINSIZEREL_POSTFIX})
endif()

include(CTest)

set(CTEST_CUSTOM_MAXIMUM_PASSED_TEST_OUTPUT_SIZE 50000)
set(CTEST_CUSTOM_MAXIMUM_FAILED_TEST_OUTPUT_SIZE 50000)
set(CTEST_CONTINUOUS_DURATION 600)
set(CTEST_CONTINUOUS_MINIMUM_INTERVAL 10)
set(CTEST_START_WITH_EMPTY_BINARY_DIRECTORY true)

file(GLOB cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB cryptopp_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB TESTcryptopp_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")


if(CMAKE_CL_64)
  set(cryptopp_SOURCES ${cryptopp_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/x64dll.asm ${CMAKE_CURRENT_SOURCE_DIR}/x64masm.asm)
  add_custom_command(OUTPUT $(IntDir)x64dll.obj
                     COMMAND ml64.exe /c /nologo /Fo$(IntDir)x64dll.obj /Zi "${CMAKE_CURRENT_SOURCE_DIR}/x64dll.asm"
                     MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/x64dll.asm
                     VERBATIM)
  add_custom_command(OUTPUT $(IntDir)x64masm.obj
                     COMMAND ml64.exe /c /nologo /Fo$(IntDir)x64masm.obj /Zi "${CMAKE_CURRENT_SOURCE_DIR}/x64masm.asm"
                     MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/x64masm.asm
                     VERBATIM)
endif()

# Not used
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/eccrypto.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/eprecomp.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/cryptlib_bds.cpp")


# Set up test
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/bench.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/bench2.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/datatest.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dlltest.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/fipsalgt.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/regtest.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/test.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/validat1.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/validat2.cpp")
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/validat3.cpp")

set(cryptopp_TEST_SOURCES ${CMAKE_CURRENT_SOURCE_DIR}/bench.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/bench2.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/datatest.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/dlltest.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/fipsalgt.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/regtest.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/test.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/validat1.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/validat2.cpp
                          ${CMAKE_CURRENT_SOURCE_DIR}/validat3.cpp)

list(REMOVE_ITEM cryptopp_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/bench.h")
list(REMOVE_ITEM cryptopp_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/factory.h")
list(REMOVE_ITEM cryptopp_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/validate.h")

set(cryptopp_TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/bench.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/factory.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/validate.h)


add_executable(cryptest ${cryptopp_TEST_SOURCES} ${cryptopp_TEST_HEADERS})

add_executable(TESTcryptopp ${TESTcryptopp_DIR})

add_library(cryptopp STATIC ${cryptopp_SOURCES} ${cryptopp_HEADERS})

set_target_properties(cryptest cryptopp PROPERTIES FOLDER "Third Party/Crypto++")



add_custom_command(TARGET cryptest
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestData $<TARGET_FILE_DIR:cryptest>/TestData)
add_custom_command(TARGET cryptest
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestVectors $<TARGET_FILE_DIR:cryptest>/TestVectors)
add_custom_command(TARGET cryptest
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestData ${CMAKE_BINARY_DIR}/TestData)
add_custom_command(TARGET cryptest
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestVectors ${CMAKE_BINARY_DIR}/TestVectors)
# if(NOT MSVC)
#   set_target_properties(cryptest PROPERTIES
#                           DEBUG_OUTPUT_NAME cryptest${CMAKE_DEBUG_POSTFIX}
#                           RELWITHDEBINFO_OUTPUT_NAME cryptest${CMAKE_RELWITHDEBINFO_POSTFIX}
#                           MINSIZEREL_OUTPUT_NAME cryptest${CMAKE_MINSIZEREL_POSTFIX})
# 
# endif()

# install(TARGETS cryptopp ARCHIVE DESTINATION lib)

target_link_libraries(cryptest cryptopp)
target_link_libraries(TESTcryptopp cryptopp gtest)

if(UNIX)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-DCRYPTOPP_DISABLE_ASM -DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION)
    set_target_properties(cryptopp cryptest TESTcryptopp PROPERTIES
                            COMPILE_FLAGS "-Wno-tautological-compare -fPIC -Wno-c++11-narrowing -std=c++11 ${LIBC++}")
if (${HAVE_LIBC++})
    set_target_properties(cryptest TESTcryptopp PROPERTIES LINK_FLAGS "${LIBC++} -ldl")
endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
        set_target_properties(cryptopp cryptest PROPERTIES COMPILE_FLAGS "-O2 -fPIC -w -pipe -finline-functions -std=c++11")
        
    elseif(GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
        set_target_properties(cryptopp cryptest PROPERTIES COMPILE_FLAGS "-O2 -fPIC -w -pipe -finline-functions -std=c++0x")
    else()
      message(FATAL "Unsupported verion of GCC, minimum 4.6 required")
    endif()
  endif()
elseif(MSVC)
  set_target_properties(cryptopp PROPERTIES
                          COMPILE_FLAGS "/D_WINDOWS /DUSE_PRECOMPILED_HEADERS /DWIN32 /D_VC80_UPGRADE=0x0710 /EHsc /W3")
  set_target_properties(cryptest PROPERTIES
                          COMPILE_FLAGS "/D_CONSOLE /DWIN32 /D_VC80_UPGRADE=0x0710 /D_MBCS /EHsc /W3"
                          LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF /LTCG"
                          LINK_FLAGS_RELWITHDEBINFO "/LTCG"
                          LINK_FLAGS_MINSIZEREL "/LTCG")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Oi /Oy /GL /DNDEBUG /GF /MT /Gy /Zi")
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /Oi /D_DEBUG /MTd /Zi")
  list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dll.cpp") # this file doesn't use precompiled headers
  list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/iterhash.cpp") # this file doesn't use precompiled headers
  list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp") # this file is used to create precompiled headers
  set_source_files_properties(${cryptopp_SOURCES} PROPERTIES
                                COMPILE_FLAGS "/Yu\"pch.h\"")
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp PROPERTIES
                                COMPILE_FLAGS "/Yc\"pch.h\"")
  target_link_libraries(cryptest odbc32.lib odbccp32.lib Ws2_32.lib)
endif()

add_test(NAME CryptoppTest.BEH_FULL COMMAND $<TARGET_FILE:cryptest> v WORKING_DIRECTORY $<TARGET_FILE_DIR:cryptest>)
add_test(NAME CryptoppTest.BEH_AES COMMAND $<TARGET_FILE:cryptest> tv aes WORKING_DIRECTORY $<TARGET_FILE_DIR:cryptest>)
add_test(NAME CryptoppTest.BEH_SHA COMMAND $<TARGET_FILE:cryptest> tv sha WORKING_DIRECTORY $<TARGET_FILE_DIR:cryptest>)
add_test(NAME CryptoppTest.BEH_RSA_PSS COMMAND $<TARGET_FILE:cryptest> tv rsa_pss WORKING_DIRECTORY $<TARGET_FILE_DIR:cryptest>)
add_test(NAME CryptoppTest.BEH_RSA_OAEP COMMAND $<TARGET_FILE:cryptest> tv rsa_oaep WORKING_DIRECTORY $<TARGET_FILE_DIR:cryptest>)
add_test(NAME CryptoppTest.BEH_RSA_PKCS1_1_5 COMMAND $<TARGET_FILE:cryptest> tv rsa_pkcs1_1_5 WORKING_DIRECTORY $<TARGET_FILE_DIR:cryptest>)

set_property(TEST CryptoppTest.BEH_FULL PROPERTY LABELS Common common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_FULL PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_AES PROPERTY LABELS Common common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_AES PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_SHA PROPERTY LABELS Common common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_SHA PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_RSA_PSS PROPERTY LABELS Common common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_RSA_PSS PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_RSA_OAEP PROPERTY LABELS Common common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_RSA_OAEP PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_RSA_PKCS1_1_5 PROPERTY LABELS Common common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_RSA_PKCS1_1_5 PROPERTY TIMEOUT 60)
