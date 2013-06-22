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

cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)

project(Cryptopp)

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)


file(GLOB cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB cryptopp_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB TESTcryptopp_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")


if(CMAKE_CL_64)
  enable_language(ASM_MASM)
  if(NOT CMAKE_ASM_MASM_COMPILER_WORKS)
    message(FATAL_ERROR "No assembler found.")
  endif()
  set(cryptopp_SOURCES ${cryptopp_SOURCES} ${CMAKE_CURRENT_SOURCE_DIR}/x64dll.asm ${CMAKE_CURRENT_SOURCE_DIR}/x64masm.asm $(IntDir)x64dll.obj $(IntDir)x64masm.obj)
  set_source_files_properties($(IntDir)x64dll.obj $(IntDir)x64masm.obj PROPERTIES GENERATED TRUE)
  add_custom_command(OUTPUT $(IntDir)x64dll.obj
                     COMMAND ${CMAKE_ASM_MASM_COMPILER} /c /nologo /Fo$(IntDir)x64dll.obj "${CMAKE_CURRENT_SOURCE_DIR}/x64dll.asm"
                     MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/x64dll.asm)
  add_custom_command(OUTPUT $(IntDir)x64masm.obj
                     COMMAND ${CMAKE_ASM_MASM_COMPILER} /c /nologo /Fo$(IntDir)x64masm.obj "${CMAKE_CURRENT_SOURCE_DIR}/x64masm.asm"
                     MAIN_DEPENDENCY ${CMAKE_CURRENT_SOURCE_DIR}/x64masm.asm)
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

target_link_libraries(cryptest cryptopp ${JustThread_LIBRARIES})
#target_link_libraries(TESTcryptopp cryptopp gtest ${JustThread_LIBRARIES})

if(UNIX)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if(HAVE_LIBC++)
      set(LibCXX "-stdlib=libc++")
      set_target_properties(cryptest PROPERTIES LINK_FLAGS "${LibCXX} -ldl")
    endif()
    add_definitions(-DCRYPTOPP_DISABLE_ASM -DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION)
    set_target_properties(cryptopp cryptest PROPERTIES
                            COMPILE_FLAGS "-w -Wno-tautological-compare -fPIC -Wno-c++11-narrowing -std=c++11 ${LibCXX}")
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set_target_properties(cryptopp cryptest PROPERTIES COMPILE_FLAGS "-O2 -fPIC -w -pipe -finline-functions -std=c++11")
  endif()
  if(JUST_THREAD_DEADLOCK_CHECK)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_JUST_THREAD_DEADLOCK_CHECK")
  endif()
elseif(MSVC)
  set_target_properties(cryptopp PROPERTIES
                          COMPILE_FLAGS "/D_WINDOWS /DUSE_PRECOMPILED_HEADERS /DWIN32 /D_VC80_UPGRADE=0x0710 /EHsc /W3 /wd4068"
                          STATIC_LIBRARY_FLAGS_RELEASE "/LTCG"
                          STATIC_LIBRARY_FLAGS_RELWITHDEBINFO "/LTCG")
  set_target_properties(cryptest PROPERTIES
                          COMPILE_FLAGS "/D_CONSOLE /DWIN32 /D_VC80_UPGRADE=0x0710 /D_MBCS /EHsc /W3"
                          LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF /LTCG"
                          LINK_FLAGS_RELWITHDEBINFO "/OPT:REF /OPT:ICF /LTCG /DEBUG"
                          LINK_FLAGS_MINSIZEREL "/LTCG")
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /Ob2 /Oi /Oy /GL /DNDEBUG /GF /MD /Gy")
  set(CMAKE_CXX_FLAGS_DEBUG "/Od /Oi /D_DEBUG /MDd /Zi")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  if(JUST_THREAD_DEADLOCK_CHECK)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D \"_JUST_THREAD_DEADLOCK_CHECK\"")
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
