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


cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)
project(Cryptopp)

include(${CMAKE_SOURCE_DIR}/cmake_modules/standard_setup.cmake)


file(GLOB cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
file(GLOB cryptopp_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
file(GLOB TESTcryptopp_DIR "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")


if(CMAKE_CL_64)
  enable_language(ASM_MASM)
  if(NOT CMAKE_ASM_MASM_COMPILER_WORKS)
    # This could happen if the CMakeCache.txt was deleted; try finding the ASM_MASM compiler again.
    include(CMakeDetermineASM_MASMCompiler)
    include(CMakeTestASM_MASMCompiler)
    if(NOT CMAKE_ASM_MASM_COMPILER_WORKS)
      message(FATAL_ERROR "No assembler found.")
    endif()
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
list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/eccrypto.cpp"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/eprecomp.cpp"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/cryptlib_bds.cpp"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/algebra.cpp"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/polynomi.cpp"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/simple.cpp"
                                  "${CMAKE_CURRENT_SOURCE_DIR}/strciphr.cpp")
if(NOT MSVC)
  list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp"
                                    "${CMAKE_CURRENT_SOURCE_DIR}/winpipes.cpp")
endif()

# Set up test
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
set(cryptopp_TEST_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/bench.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/factory.h
                          ${CMAKE_CURRENT_SOURCE_DIR}/validate.h)

list(REMOVE_ITEM cryptopp_SOURCES ${cryptopp_TEST_SOURCES})
list(REMOVE_ITEM cryptopp_HEADERS ${cryptopp_TEST_HEADERS})

set(CurrentCamelCaseProjectName ${CamelCaseProjectName})
set(CamelCaseProjectName Common)
ms_add_executable(cryptest "." ${cryptopp_TEST_SOURCES} ${cryptopp_TEST_HEADERS})
set(CamelCaseProjectName ${CurrentCamelCaseProjectName})

#add_executable(TESTcryptopp ${TESTcryptopp_DIR})

add_library(cryptopp STATIC ${cryptopp_SOURCES} ${cryptopp_HEADERS})
if(TARGET check_sanitizer_blacklist)
  add_dependencies(cryptopp check_sanitizer_blacklist)
endif()

ms_target_include_system_dirs(cryptopp INTERFACE ${PROJECT_SOURCE_DIR}/.. PRIVATE ${PROJECT_SOURCE_DIR})
if(JustThread_FOUND)
  ms_target_include_system_dirs(cryptopp PUBLIC ${JustThread_INCLUDE_DIR})
endif()
set_target_properties(cryptopp cryptest PROPERTIES FOLDER "Third Party/Crypto++")


add_custom_command(TARGET cryptest
                     POST_BUILD
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestData $<TARGET_FILE_DIR:cryptest>/TestData
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestVectors $<TARGET_FILE_DIR:cryptest>/TestVectors
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestData ${CMAKE_BINARY_DIR}/TestData
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestVectors ${CMAKE_BINARY_DIR}/TestVectors
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestData ${CMAKE_CURRENT_BINARY_DIR}/../TestData
                     COMMAND ${CMAKE_COMMAND} ARGS -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/TestVectors ${CMAKE_CURRENT_BINARY_DIR}/../TestVectors)

target_link_libraries(cryptest cryptopp ${JustThread_LIBRARIES})
#target_link_libraries(TESTcryptopp cryptopp gtest ${JustThread_LIBRARIES})

set(AllStaticLibsForCurrentProject cryptopp)
set(AllExesForCurrentProject cryptest)
include(../../../cmake_modules/standard_flags.cmake)

target_compile_definitions(cryptopp
  PUBLIC
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:CRYPTOPP_DISABLE_ASM CRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION>
    $<$<AND:$<BOOL:${JUST_THREAD_DEADLOCK_CHECK}>,$<CONFIG:Debug>>:_JUST_THREAD_DEADLOCK_CHECK>
  PRIVATE
    $<$<BOOL:${MSVC}>:_WINDOWS USE_PRECOMPILED_HEADERS WIN32 _VC80_UPGRADE=0x0710>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(cryptopp
  PUBLIC
    $<$<BOOL:${MSVC}>:
        $<$<CONFIG:Release>:/O2 /GL /MD>
        $<$<CONFIG:Debug>:/Od /MDd /Zi> # /Oi
        $<$<CONFIG:MinSizeRel>:/MD>
        $<$<CONFIG:RelWithDebInfo>:/O2 /GL /MD /Zi>
    >
    $<$<BOOL:${UNIX}>:-std=c++11 ${LibCXX}>
  PRIVATE
    $<$<BOOL:${MSVC}>:/EHsc /W3 /wd4068 /wd4996>
    $<$<BOOL:${UNIX}>:
        -w -fPIC -O2
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-tautological-compare -Wno-c++11-narrowing>
        $<$<CXX_COMPILER_ID:GNU>:-pipe -finline-functions>
    >
)

target_compile_definitions(cryptest
  PRIVATE
    $<$<BOOL:${MSVC}>:WIN32 _VC80_UPGRADE=0x0710 _MBCS $<$<CONFIG:Debug>:_DEBUG>>
    $<$<CONFIG:Release>:NDEBUG>
)

target_compile_options(cryptest
  PRIVATE
    $<$<BOOL:${MSVC}>:/EHsc /W3>
    $<$<BOOL:${UNIX}>:
        -w -fPIC
        $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:-Wno-tautological-compare -Wno-c++11-narrowing>
        $<$<CXX_COMPILER_ID:GNU>:-O2 -pipe -finline-functions>
    >
)

if(UNIX)
  if(${CMAKE_CXX_COMPILER_ID} MATCHES "^(Apple)?Clang$")
    if(HAVE_LIBC++)
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LibCXX} -ldl")
    endif()
    if(HAVE_LIBC++ABI)
      set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LibCXXAbi}")
    endif()
  endif()
elseif(MSVC)
  list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/dll.cpp") # this file doesn't use precompiled headers
  list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/iterhash.cpp") # this file doesn't use precompiled headers
  list(REMOVE_ITEM cryptopp_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp") # this file is used to create precompiled headers
  set_source_files_properties(${cryptopp_SOURCES} PROPERTIES COMPILE_FLAGS "/Yu\"pch.h\"")
  set_source_files_properties(${CMAKE_CURRENT_SOURCE_DIR}/pch.cpp PROPERTIES COMPILE_FLAGS "/Yc\"pch.h\"")
  target_link_libraries(cryptest odbc32.lib odbccp32.lib Ws2_32.lib)
endif()


#==================================================================================================#
# Package                                                                                          #
#==================================================================================================#
install(TARGETS cryptopp COMPONENT Development CONFIGURATIONS Debug Release ARCHIVE DESTINATION lib)
install(FILES ${cryptopp_HEADERS} COMPONENT Development DESTINATION include/maidsafe/third_party_libs/cryptopp)

install(TARGETS cryptest COMPONENT Tests CONFIGURATIONS Debug RUNTIME DESTINATION bin/debug)
install(TARGETS cryptest COMPONENT Tests CONFIGURATIONS Release RUNTIME DESTINATION bin)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/TestData ${PROJECT_SOURCE_DIR}/TestVectors COMPONENT Tests CONFIGURATIONS Debug DESTINATION bin/debug)
install(DIRECTORY ${PROJECT_SOURCE_DIR}/TestData ${PROJECT_SOURCE_DIR}/TestVectors COMPONENT Tests CONFIGURATIONS Release DESTINATION bin)
