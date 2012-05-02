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
project(Cryptopp)
enable_testing()
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

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

add_library(cryptopp STATIC ${cryptopp_SOURCES} ${cryptopp_HEADERS})
set_target_properties(cryptopp PROPERTIES OUTPUT_NAME cryptopp)

if(NOT MSVC)
  set_target_properties(cryptest PROPERTIES
                          DEBUG_OUTPUT_NAME cryptest${CMAKE_DEBUG_POSTFIX}
                          RELWITHDEBINFO_OUTPUT_NAME cryptest${CMAKE_RELWITHDEBINFO_POSTFIX}
                          MINSIZEREL_OUTPUT_NAME cryptest${CMAKE_MINSIZEREL_POSTFIX})

endif()

install(TARGETS cryptopp ARCHIVE DESTINATION lib)

if(NOT WIN32)
  add_definitions(-DNDEBUG)
endif()


target_link_libraries(cryptest cryptopp)

if(APPLE)
#  add_definitions(-DCRYPTOPP_DISABLE_ASM -DCRYPTOPP_DISABLE_AESNI)
  set_target_properties(cryptopp PROPERTIES COMPILE_FLAGS "-O2 -w -finline-functions")
endif()

if(UNIX)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-DCRYPTOPP_DISABLE_ASM)
    set_target_properties(cryptopp PROPERTIES
                            COMPILE_FLAGS "-O2 -DNDEBUG -g -DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION  -std=c++11 -stdlib=libc++ -Wno-tautological-compare -pipe -pthread -c ")
    set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/wake.cpp PROPERTIES COMPILE_FLAGS "-Wno-c++11-narrowing")
    set_target_properties(cryptest PROPERTIES
                            COMPILE_FLAGS "-O2 -DNDEBUG -g -DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION -std=c++11 -stdlib=libc++ -Wno-tautological-compare -pipe -pthread -c ")
  else()
    set_target_properties(cryptopp PROPERTIES COMPILE_FLAGS "-O2  -DNDEBUG -fPIC -w -pipe -finline-functions -std=c++11")
    set_target_properties(cryptest PROPERTIES COMPILE_FLAGS "-O2  -DNDEBUG -fPIC -w -pipe -finline-functions -std=c++11")
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

ADD_TEST(CrypoppTest.BEH_FULL cryptest v)
ADD_TEST(CrypoppTest.BEH_AES cryptest tv aes)
ADD_TEST(CrypoppTest.BEH_SHA cryptest tv sha)
ADD_TEST(CrypoppTest.BEH_RSA_PSS cryptest tv rsa_pss)
ADD_TEST(CrypoppTest.BEH_RSA_OAEP cryptest tv rsa_oaep)
ADD_TEST(CrypoppTest.BEH_RSA_PKCS1_1_5 cryptest tv rsa_pkcs1_1_5)

SET_PROPERTY(TEST CrypoppTest.BEH_FULL PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
SET_PROPERTY(TEST CrypoppTest.BEH_FULL PROPERTY TIMEOUT 60)
SET_PROPERTY(TEST CrypoppTest.BEH_AES PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
SET_PROPERTY(TEST CrypoppTest.BEH_AES PROPERTY TIMEOUT 60)
SET_PROPERTY(TEST CrypoppTest.BEH_SHA PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
SET_PROPERTY(TEST CrypoppTest.BEH_SHA PROPERTY TIMEOUT 60)
SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PSS PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PSS PROPERTY TIMEOUT 60)
SET_PROPERTY(TEST CrypoppTest.BEH_RSA_OAEP PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
SET_PROPERTY(TEST CrypoppTest.BEH_RSA_OAEP PROPERTY TIMEOUT 60)
SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PKCS1_1_5 PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PKCS1_1_5 PROPERTY TIMEOUT 60)



