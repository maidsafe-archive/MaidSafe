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

project(Cryptopp)
#include(${PROJECT_SOURCE_DIR}/cmake_modules/maidsafe_standard_flags.cmake)
include(${CMAKE_SOURCE_DIR}/cmake_modules/maidsafe_standard_setup.cmake)


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

set(CurrentCamelCaseProjectName ${CamelCaseProjectName})
set(CamelCaseProjectName Common)
ms_add_executable(cryptest "Tests/Common" ${cryptopp_TEST_SOURCES} ${cryptopp_TEST_HEADERS})
set(CamelCaseProjectName ${CurrentCamelCaseProjectName})

#add_executable(TESTcryptopp ${TESTcryptopp_DIR})

add_library(cryptopp STATIC ${cryptopp_SOURCES} ${cryptopp_HEADERS})
set_target_properties(cryptopp PROPERTIES FOLDER "Third Party/Crypto++")


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

target_link_libraries(cryptest cryptopp)
#target_link_libraries(TESTcryptopp cryptopp gtest)
if(JustThread_FOUND)
  target_link_libraries(cryptest optimized ${JustThread_LIBRARY} debug ${JustThread_LIBRARY_DEBUG})
endif()

if(UNIX)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(APPLE OR HAVE_LIBC++)
      set(LibCXX "-stdlib=libc++")
      set_target_properties(cryptest PROPERTIES LINK_FLAGS "${LibCXX} -ldl")
    endif()
    add_definitions(-DCRYPTOPP_DISABLE_ASM -DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION)
    set_target_properties(cryptopp cryptest PROPERTIES
                            COMPILE_FLAGS "-w -Wno-tautological-compare -fPIC -Wno-c++11-narrowing -std=c++11 ${LibCXX}")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if (GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
      set_target_properties(cryptopp cryptest PROPERTIES COMPILE_FLAGS "-O2 -fPIC -w -pipe -finline-functions -std=c++11")
    elseif(GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
      set_target_properties(cryptopp cryptest PROPERTIES COMPILE_FLAGS "-O2 -fPIC -w -pipe -finline-functions -std=c++0x")
    else()
      message(FATAL_ERROR "Unsupported verion of GCC, minimum 4.6 required")
    endif()
  endif()
  if(JUST_THREAD_DEADLOCK_CHECK)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_JUST_THREAD_DEADLOCK_CHECK")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} -D_JUST_THREAD_DEADLOCK_CHECK")
  endif()
elseif(MSVC)
  set_target_properties(cryptopp PROPERTIES
                          COMPILE_FLAGS "/D_WINDOWS /DUSE_PRECOMPILED_HEADERS /DWIN32 /D_VC80_UPGRADE=0x0710 /EHsc /W3")
  set_target_properties(cryptest PROPERTIES
                          COMPILE_FLAGS "/D_CONSOLE /DWIN32 /D_VC80_UPGRADE=0x0710 /D_MBCS /EHsc /W3"
                          LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF /LTCG"
                          LINK_FLAGS_RELWITHDEBINFO "/LTCG"
                          LINK_FLAGS_MINSIZEREL "/LTCG")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Oi /Oy /GL /DNDEBUG /GF /MD /Gy /Zi")
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /Oi /D_DEBUG /MDd /Zi")
  if(JUST_THREAD_DEADLOCK_CHECK)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D \"_JUST_THREAD_DEADLOCK_CHECK\"")
    set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELWITHDEBINFO} /D \"_JUST_THREAD_DEADLOCK_CHECK\"")
  endif()
  # Reduce compiler's heap allocation from 1000% of default to 500% to avoid errors when compiling precompiled headers
  string(REPLACE "Zm1000" "Zm500" CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})
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

set_property(TEST CryptoppTest.BEH_FULL PROPERTY LABELS Common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_FULL PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_AES PROPERTY LABELS Common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_AES PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_SHA PROPERTY LABELS Common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_SHA PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_RSA_PSS PROPERTY LABELS Common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_RSA_PSS PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_RSA_OAEP PROPERTY LABELS Common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_RSA_OAEP PROPERTY TIMEOUT 60)
set_property(TEST CryptoppTest.BEH_RSA_PKCS1_1_5 PROPERTY LABELS Common Behavioural CryptoPP ${CMAKE_BUILD_TYPE})
set_property(TEST CryptoppTest.BEH_RSA_PKCS1_1_5 PROPERTY TIMEOUT 60)
