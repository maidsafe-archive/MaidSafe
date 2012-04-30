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

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(Cryptopp)
enable_testing()

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

add_library(cryptopp_static STATIC ${cryptopp_SOURCES} ${cryptopp_HEADERS})
set_target_properties(cryptopp_static PROPERTIES OUTPUT_NAME cryptopp)

if(NOT MSVC)
  set_target_properties(cryptest PROPERTIES
                          DEBUG_OUTPUT_NAME cryptest${CMAKE_DEBUG_POSTFIX}
                          RELWITHDEBINFO_OUTPUT_NAME cryptest${CMAKE_RELWITHDEBINFO_POSTFIX}
                          MINSIZEREL_OUTPUT_NAME cryptest${CMAKE_MINSIZEREL_POSTFIX})

endif()

install(TARGETS cryptopp_static ARCHIVE DESTINATION lib)

if(NOT WIN32)
  add_definitions(-DNDEBUG)
endif()


target_link_libraries(cryptest cryptopp_static)

if(APPLE)
#  add_definitions(-DCRYPTOPP_DISABLE_ASM -DCRYPTOPP_DISABLE_AESNI)
#  set_target_properties(cryptopp_static PROPERTIES COMPILE_FLAGS "-O2 -fPIC -w -finline-functions")
  set_target_properties(cryptopp_static PROPERTIES COMPILE_FLAGS "-O2 -w -finline-functions")
endif()

if(UNIX)
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    include_directories("/usr/local/include/c++/v1") 
    add_definitions(-DCRYPTOPP_DISABLE_ASM)
    set_target_properties(cryptopp_static PROPERTIES
                            COMPILE_FLAGS "-Wno-tautological-compare -pipe -std=c++11 -stdlib=libc++ -Wno-c++11-narrowing")
    set_target_properties(cryptest PROPERTIES
                            COMPILE_FLAGS "-O2 -w -finline-functions -Wno-tautological-compare -pipe -stdlib=libc++ -Wno-c++11-narrowing"
                            LINK_FLAGS_RELEASE "-stdlib=libc++"
                            LINK_FLAGS_DEBUG "-stdlib=libc++")
  else()
    set_target_properties(cryptopp_static PROPERTIES COMPILE_FLAGS "-O2 -fPIC -w -pipe -finline-functions -std=c++11")
  endif()
elseif(MSVC)
  set_target_properties(cryptopp_static PROPERTIES
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

