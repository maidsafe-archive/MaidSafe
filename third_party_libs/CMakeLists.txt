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
#  Module used to build and install Third Party libs.                          #
#                                                                              #
#==============================================================================#


cmake_minimum_required(VERSION 2.8.4 FATAL_ERROR)

project(BuildThirdPartyLibs)

get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)

if(NOT BUILD_TARGET)
  set(BUILD_TARGET "ALL")
endif()
set(SEPARATOR "\n====================================================================================================\n\n")

get_filename_component(COMMON_ROOT_DIR ${CURRENT_DIR} PATH)
set(CMAKE_INSTALL_PREFIX ${COMMON_ROOT_DIR}/installed)
if(INSTALL_PREFIX)
  file(TO_CMAKE_PATH ${INSTALL_PREFIX} INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "${INSTALL_PREFIX}")
endif()
file(TO_NATIVE_PATH ${CMAKE_INSTALL_PREFIX} CMAKE_INSTALL_PREFIX_MESSAGE)
if(WIN32)
  set(LIB_PREFIX_RELEASE "${CMAKE_INSTALL_PREFIX}/lib/Release/")
  set(LIB_PREFIX_DEBUG "${CMAKE_INSTALL_PREFIX}/lib/Debug/")
else()
  set(LIB_PREFIX_RELEASE "${CMAKE_INSTALL_PREFIX}/lib/")
  set(LIB_PREFIX_DEBUG "${CMAKE_INSTALL_PREFIX}/lib/")
endif()
set(INC_DIR "${CMAKE_INSTALL_PREFIX}/include/")
set(BIN_DIR "${CMAKE_INSTALL_PREFIX}/bin/")
set(SHARE_DIR "${CMAKE_INSTALL_PREFIX}/share/maidsafe/")
set(EXPORT_FILE "${SHARE_DIR}maidsafe_third_party.cmake")

execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_PREFIX_RELEASE})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${LIB_PREFIX_DEBUG})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${INC_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BIN_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${SHARE_DIR})
file(WRITE ${EXPORT_FILE} "\n")

function(APPEND_TO_EXPORT_FILE LINE)
  file(APPEND ${EXPORT_FILE} "${LINE}\n")
endfunction()


###################################################################################################
# Google Logging library                                                                          #
###################################################################################################
if((${BUILD_TARGET} MATCHES "GLOG") OR (${BUILD_TARGET} MATCHES "ALL"))
  if(MSVC)
    message("-- Building Google Logging Debug libraries")
    message("     This may take a few minutes...")
    find_file(LIBGLOG_PROJECT NAMES libglog_static.vcxproj PATHS ${CURRENT_DIR}/src/glog/vsprojects/libglog_static NO_DEFAULT_PATH)
    if(CMAKE_CL_64)
      set(PLATFORM "Platform=x64")
    else()
      set(PLATFORM "Platform=Win32")
    endif()
    execute_process(COMMAND msbuild ${LIBGLOG_PROJECT} "/property:Configuration=Debug;${PLATFORM}" OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(GLOG_OUTVAR "${OUTVAR}")
    message("-- Building Google Logging Release libraries")
    message("     This may take a few minutes...")
    execute_process(COMMAND msbuild ${LIBGLOG_PROJECT} "/property:Configuration=Release;${PLATFORM}" OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(GLOG_OUTVAR "${GLOG_OUTVAR}${SEPARATOR}${OUTVAR}")
  else()
    unset(GLOG_CONFIGURE CACHE)
    set(GLOG_BUILD_DIR ${CURRENT_DIR}/build_glog)
    # Make a copy of glog to work with to avoid modified files being accidentally committed to repository
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CURRENT_DIR}/src/glog ${GLOG_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    find_file(GLOG_CONFIGURE configure PATHS ${GLOG_BUILD_DIR} NO_DEFAULT_PATH)
    if(NOT GLOG_CONFIGURE)
      message(FATAL_ERROR "${OUTVAR}\n${ERRVAR}\nCould not configure Google Logging.\n\n")
    endif()
    message("-- Configuring Google Logging library")
    message("     This may take a few minutes...")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      execute_process(COMMAND sh ${GLOG_CONFIGURE} --prefix=${GLOG_BUILD_DIR} --enable-shared=no "CXXFLAGS=-std=c++11 -stdlib=libc++" WORKING_DIRECTORY ${GLOG_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    else()
      execute_process(COMMAND sh ${GLOG_CONFIGURE} --prefix=${GLOG_BUILD_DIR} --enable-shared=no CXXFLAGS=-std=c++11 WORKING_DIRECTORY ${GLOG_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    endif()
    set(GLOG_OUTVAR "${OUTVAR}")
    message("-- Making Google Logging library")
    message("     This may take a few minutes...")
    execute_process(COMMAND chmod u+x configure install-sh WORKING_DIRECTORY ${GLOG_BUILD_DIR})
    execute_process(COMMAND make -i -C ${GLOG_BUILD_DIR} install-libLTLIBRARIES OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(GLOG_OUTVAR "${GLOG_OUTVAR}${SEPARATOR}${OUTVAR}")
  endif()
endif()

find_library(Glog_LIBRARY_RELEASE NAMES libglog.a libglog_static.lib
               PATHS ${CURRENT_DIR}/build_glog/lib ${CURRENT_DIR}/src/glog/vsprojects/libglog_static/Release ${CURRENT_DIR}/src/glog/vsprojects/libglog_static/x64/Release
               NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_library(Glog_LIBRARY_DEBUG NAMES libglog.a libglog_static.lib
               PATHS ${CURRENT_DIR}/build_glog/lib ${CURRENT_DIR}/src/glog/vsprojects/libglog_static/Debug ${CURRENT_DIR}/src/glog/vsprojects/libglog_static/x64/Debug
               NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
if(WIN32)
  find_path(Glog_INCLUDE_DIR glog/logging.h
              PATHS ${CURRENT_DIR}/src/glog/src/windows
              NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
else()
  find_path(Glog_INCLUDE_DIR glog/logging.h
              PATHS ${CURRENT_DIR}/build_glog/src
              NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
endif()

if(NOT Glog_LIBRARY_RELEASE OR NOT Glog_LIBRARY_DEBUG OR NOT Glog_INCLUDE_DIR)
  message(FATAL_ERROR "${GLOG_OUTVAR}")
endif()

message("-- Found Google Logging")
get_filename_component(Glog_LIBRARY_RELEASE_NAME ${Glog_LIBRARY_RELEASE} NAME)
get_filename_component(Glog_LIBRARY_DEBUG_NAME ${Glog_LIBRARY_DEBUG} NAME)
set(Glog_LIBRARY_RELEASE_INSTALLED ${LIB_PREFIX_RELEASE}${Glog_LIBRARY_RELEASE_NAME})
set(Glog_LIBRARY_DEBUG_INSTALLED ${LIB_PREFIX_DEBUG}${Glog_LIBRARY_DEBUG_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Glog_LIBRARY_RELEASE} ${LIB_PREFIX_RELEASE})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Glog_LIBRARY_DEBUG} ${LIB_PREFIX_DEBUG})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Glog_INCLUDE_DIR}/glog/logging.h ${INC_DIR}/glog/logging.h)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Glog_INCLUDE_DIR}/glog/log_severity.h ${INC_DIR}/glog/log_severity.h)
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Glog_INCLUDE_DIR}/glog/vlog_is_on.h ${INC_DIR}/glog/vlog_is_on.h)
message("-- Installed ${Glog_LIBRARY_RELEASE_INSTALLED}")
message("-- Installed ${Glog_LIBRARY_DEBUG_INSTALLED}")
file(GLOB_RECURSE HEADERS ${INC_DIR} "${INC_DIR}glog/*")
foreach(HEADER ${HEADERS})
  message("-- Installed ${HEADER}")
endforeach()

APPEND_TO_EXPORT_FILE("ADD_LIBRARY(glog_static STATIC IMPORTED)")
APPEND_TO_EXPORT_FILE("SET_TARGET_PROPERTIES(glog_static PROPERTIES")
APPEND_TO_EXPORT_FILE("                        IMPORTED_CONFIGURATIONS \"RELEASE;DEBUG\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_RELEASE \"${Glog_LIBRARY_RELEASE_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_DEBUG \"${Glog_LIBRARY_DEBUG_INSTALLED}\")\n")

###################################################################################################
# Google Protocol Buffers                                                                         #
###################################################################################################
if((${BUILD_TARGET} MATCHES "PROTOBUF") OR (${BUILD_TARGET} MATCHES "ALL"))
  if(MSVC)
    message("-- Building Google Protocol Buffers Debug libraries")
    message("     This may take a few minutes...")
    find_file(LIBPROTOBUF_PROJECT NAMES libprotobuf.vcxproj PATHS ${CURRENT_DIR}/src/protobuf/vsprojects NO_DEFAULT_PATH)
    find_file(LIBPROTOC_PROJECT NAMES libprotoc.vcxproj PATHS ${CURRENT_DIR}/src/protobuf/vsprojects NO_DEFAULT_PATH)
    find_file(PROTOC_PROJECT NAMES protoc.vcxproj PATHS ${CURRENT_DIR}/src/protobuf/vsprojects NO_DEFAULT_PATH)
    if(CMAKE_CL_64)
      set(PLATFORM "Platform=x64")
    else()
      set(PLATFORM "Platform=Win32")
    endif()
    # Build libprotobuf Debug
    execute_process(COMMAND msbuild ${LIBPROTOBUF_PROJECT} "/property:Configuration=Debug;${PLATFORM}" OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(PROTOBUF_OUTVAR "${OUTVAR}")
    message("-- Building Google Protocol Buffers Release libraries and compiler")
    message("     This may take a few minutes...")
    # Build libprotobuf Release
    execute_process(COMMAND msbuild ${LIBPROTOBUF_PROJECT} "/property:Configuration=Release;${PLATFORM}" OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(PROTOBUF_OUTVAR "${PROTOBUF_OUTVAR}${SEPARATOR}${OUTVAR}")
    # Build libprotoc Release
    execute_process(COMMAND msbuild ${LIBPROTOC_PROJECT} "/property:Configuration=Release;${PLATFORM}" OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(PROTOBUF_OUTVAR "${PROTOBUF_OUTVAR}${SEPARATOR}${OUTVAR}")
    # Build protoc Release
    execute_process(COMMAND msbuild ${PROTOC_PROJECT} "/property:Configuration=Release;${PLATFORM}" OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(PROTOBUF_OUTVAR "${PROTOBUF_OUTVAR}${SEPARATOR}${OUTVAR}")
    # Extract headers
    execute_process(COMMAND CMD /C CALL extract_includes.bat WORKING_DIRECTORY ${CURRENT_DIR}/src/protobuf/vsprojects OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(PROTOBUF_OUTVAR "${PROTOBUF_OUTVAR}${SEPARATOR}${OUTVAR}")
  else()
    unset(PROTOBUF_AUTOGEN CACHE)
    unset(PROTOBUF_CONFIGURE CACHE)
    set(PROTOBUF_BUILD_DIR ${CURRENT_DIR}/build_protobuf)
    # Make a copy of protobuf to work with to avoid modified files being accidentally committed to repository
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${CURRENT_DIR}/src/protobuf ${PROTOBUF_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    find_file(PROTOBUF_AUTOGEN autogen.sh PATHS ${PROTOBUF_BUILD_DIR} NO_DEFAULT_PATH)
    message("-- Generating configure file for Google Protocol Buffers")
    message("     This may take a few minutes...")
    execute_process(COMMAND sh ${PROTOBUF_AUTOGEN} WORKING_DIRECTORY ${PROTOBUF_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(PROTOBUF_OUTVAR "${OUTVAR}")
    find_file(PROTOBUF_CONFIGURE configure PATHS ${PROTOBUF_BUILD_DIR} NO_DEFAULT_PATH)
    if(NOT PROTOBUF_CONFIGURE)
      set(ERROR_MESSAGE "${OUTVAR}\n${ERRVAR}\nCould not create configure file for Google Protocol Buffers.\n\n")
      set(ERROR_MESSAGE "${ERROR_MESSAGE}*** This needs autoconf and libtool to complete successfully. ***\n")
      message(FATAL_ERROR "${ERROR_MESSAGE}")
    endif()
    message("-- Configuring Google Protocol Buffers")
    message("     This may take a few minutes...")
    if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
      execute_process(COMMAND sh ${PROTOBUF_CONFIGURE} --prefix=${PROTOBUF_BUILD_DIR} --enable-shared=no "CXXFLAGS=-std=c++11 -stdlib=libc++" WORKING_DIRECTORY ${PROTOBUF_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    else()
      execute_process(COMMAND sh ${PROTOBUF_CONFIGURE} --prefix=${PROTOBUF_BUILD_DIR} --enable-shared=no CXXFLAGS=-std=c++11 WORKING_DIRECTORY ${PROTOBUF_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    endif()
    set(PROTOBUF_OUTVAR "${PROTOBUF_OUTVAR}${SEPARATOR}${OUTVAR}")
    message("-- Making Google Protocol Buffers")
    message("     This may take a few minutes...")
    execute_process(COMMAND make install WORKING_DIRECTORY ${PROTOBUF_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR ERROR_VARIABLE ERRVAR)
    set(PROTOBUF_OUTVAR "${PROTOBUF_OUTVAR}${SEPARATOR}${OUTVAR}")
  endif()
endif()

find_library(Protobuf_LIBRARY_RELEASE NAMES libprotobuf.a libprotobuf.lib
               PATHS ${CURRENT_DIR}/build_protobuf/lib ${CURRENT_DIR}/src/protobuf/vsprojects/Release ${CURRENT_DIR}/src/protobuf/vsprojects/x64/Release
               NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_library(Protobuf_LIBRARY_DEBUG NAMES libprotobuf.a libprotobuf.lib
               PATHS ${CURRENT_DIR}/build_protobuf/lib ${CURRENT_DIR}/src/protobuf/vsprojects/Debug ${CURRENT_DIR}/src/protobuf/vsprojects/x64/Debug
               NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_program(Protoc_EXE NAMES protoc
               PATHS ${CURRENT_DIR}/build_protobuf/bin ${CURRENT_DIR}/src/protobuf/vsprojects/Release ${CURRENT_DIR}/src/protobuf/vsprojects/x64/Release
               NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_path(Protobuf_INCLUDE_DIR google/protobuf/service.h
            PATHS ${CURRENT_DIR}/build_protobuf/include ${CURRENT_DIR}/src/protobuf/vsprojects/include
            NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

if(NOT Protobuf_LIBRARY_RELEASE OR NOT Protobuf_LIBRARY_DEBUG OR NOT Protoc_EXE OR NOT Protobuf_INCLUDE_DIR)
  message(FATAL_ERROR "${PROTOBUF_OUTVAR}")
endif()

message("-- Found Google Protocol Buffers")
get_filename_component(Protobuf_LIBRARY_RELEASE_NAME ${Protobuf_LIBRARY_RELEASE} NAME)
get_filename_component(Protobuf_LIBRARY_DEBUG_NAME ${Protobuf_LIBRARY_DEBUG} NAME)
get_filename_component(Protoc_EXE_NAME ${Protoc_EXE} NAME)
set(Protobuf_LIBRARY_RELEASE_INSTALLED ${LIB_PREFIX_RELEASE}${Protobuf_LIBRARY_RELEASE_NAME})
set(Protobuf_LIBRARY_DEBUG_INSTALLED ${LIB_PREFIX_DEBUG}${Protobuf_LIBRARY_DEBUG_NAME})
set(Protoc_EXE_INSTALLED ${BIN_DIR}${Protoc_EXE_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Protobuf_LIBRARY_RELEASE} ${LIB_PREFIX_RELEASE})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Protobuf_LIBRARY_DEBUG} ${LIB_PREFIX_DEBUG})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Protoc_EXE} ${BIN_DIR})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${Protobuf_INCLUDE_DIR} ${INC_DIR})
message("-- Installed ${Protobuf_LIBRARY_RELEASE_INSTALLED}")
message("-- Installed ${Protobuf_LIBRARY_DEBUG_INSTALLED}")
message("-- Installed ${Protoc_EXE_INSTALLED}")
file(GLOB_RECURSE HEADERS ${INC_DIR} "${INC_DIR}google/*")
foreach(HEADER ${HEADERS})
  message("-- Installed ${HEADER}")
endforeach()

APPEND_TO_EXPORT_FILE("ADD_LIBRARY(protobuf_static STATIC IMPORTED)")
APPEND_TO_EXPORT_FILE("SET_TARGET_PROPERTIES(protobuf_static PROPERTIES")
APPEND_TO_EXPORT_FILE("                        IMPORTED_CONFIGURATIONS \"RELEASE;DEBUG\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_RELEASE \"${Protobuf_LIBRARY_RELEASE_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_DEBUG \"${Protobuf_LIBRARY_DEBUG_INSTALLED}\")\n")

APPEND_TO_EXPORT_FILE("SET(Protoc_EXE \"${Protoc_EXE_INSTALLED}\")\n")



###################################################################################################
# GoogleMock and GoogleTest                                                                       #
###################################################################################################
if((${BUILD_TARGET} MATCHES "GMOCK_GTEST") OR (${BUILD_TARGET} MATCHES "ALL"))
  message("-- Building GoogleMock and GoogleTest")
  message("     This may take a few minutes...")
  set(GMOCK_BUILD_DIR ${CURRENT_DIR}/build_googlemock)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${GMOCK_BUILD_DIR})
  if(MSVC)
    execute_process(COMMAND ${CMAKE_COMMAND} ../src/googlemock -G ${CMAKE_GENERATOR} WORKING_DIRECTORY ${GMOCK_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(GMOCK_GTEST_OUTVAR "${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug WORKING_DIRECTORY ${GMOCK_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(GMOCK_GTEST_OUTVAR "${GMOCK_GTEST_OUTVAR}${SEPARATOR}${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release WORKING_DIRECTORY ${GMOCK_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(GMOCK_GTEST_OUTVAR "${GMOCK_GTEST_OUTVAR}${SEPARATOR}${OUTVAR}")
  else()
    execute_process(COMMAND ${CMAKE_COMMAND} ../src/googlemock -G ${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=Release WORKING_DIRECTORY ${GMOCK_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(GMOCK_GTEST_OUTVAR "${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${GMOCK_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(GMOCK_GTEST_OUTVAR "${GMOCK_GTEST_OUTVAR}${SEPARATOR}${OUTVAR}")
  endif()
endif()

find_library(Gtest_LIBRARY_RELEASE NAMES gtest PATHS ${CURRENT_DIR}/build_googlemock/gtest
               PATH_SUFFIXES Release NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_library(Gtest_LIBRARY_DEBUG NAMES gtest PATHS ${CURRENT_DIR}/build_googlemock/gtest
               PATH_SUFFIXES Debug NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_path(Gtest_INCLUDE_DIR gtest/gtest.h PATHS ${CURRENT_DIR}/src/googlemock/gtest/include
            NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

find_library(Gmock_LIBRARY_RELEASE NAMES gmock PATHS ${CURRENT_DIR}/build_googlemock
               PATH_SUFFIXES Release NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_library(Gmock_LIBRARY_DEBUG NAMES gmock PATHS ${CURRENT_DIR}/build_googlemock
               PATH_SUFFIXES Debug NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_path(Gmock_INCLUDE_DIR gmock/gmock.h PATHS ${CURRENT_DIR}/src/googlemock/include
            NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

if(NOT Gtest_LIBRARY_RELEASE OR NOT Gtest_LIBRARY_DEBUG OR NOT Gtest_INCLUDE_DIR OR NOT Gmock_LIBRARY_RELEASE OR NOT Gmock_LIBRARY_DEBUG OR NOT Gmock_INCLUDE_DIR)
  message(FATAL_ERROR "${GMOCK_GTEST_OUTVAR}")
endif()

message("-- Found GoogleTest")
get_filename_component(Gtest_LIBRARY_RELEASE_NAME ${Gtest_LIBRARY_RELEASE} NAME)
get_filename_component(Gtest_LIBRARY_DEBUG_NAME ${Gtest_LIBRARY_DEBUG} NAME)
set(Gtest_LIBRARY_RELEASE_INSTALLED ${LIB_PREFIX_RELEASE}${Gtest_LIBRARY_RELEASE_NAME})
set(Gtest_LIBRARY_DEBUG_INSTALLED ${LIB_PREFIX_DEBUG}${Gtest_LIBRARY_DEBUG_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Gtest_LIBRARY_RELEASE} ${LIB_PREFIX_RELEASE})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Gtest_LIBRARY_DEBUG} ${LIB_PREFIX_DEBUG})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${Gtest_INCLUDE_DIR} ${INC_DIR})
message("-- Installed ${Gtest_LIBRARY_RELEASE_INSTALLED}")
message("-- Installed ${Gtest_LIBRARY_DEBUG_INSTALLED}")
file(GLOB_RECURSE HEADERS ${INC_DIR} "${INC_DIR}gtest/*")
foreach(HEADER ${HEADERS})
  message("-- Installed ${HEADER}")
endforeach()

APPEND_TO_EXPORT_FILE("ADD_LIBRARY(gtest_static STATIC IMPORTED)")
APPEND_TO_EXPORT_FILE("SET_TARGET_PROPERTIES(gtest_static PROPERTIES")
APPEND_TO_EXPORT_FILE("                        IMPORTED_CONFIGURATIONS \"RELEASE;DEBUG\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_RELEASE \"${Gtest_LIBRARY_RELEASE_INSTALLED}\"")
if(WIN32)
  APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_DEBUG \"${Gtest_LIBRARY_DEBUG_INSTALLED}\")")
else()
  APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_DEBUG \"${Gtest_LIBRARY_DEBUG_INSTALLED}\"")
  APPEND_TO_EXPORT_FILE("                        IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE \"pthread\"")
  APPEND_TO_EXPORT_FILE("                        IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG \"pthread\")\n")
endif()

message("-- Found GoogleMock")
get_filename_component(Gmock_LIBRARY_RELEASE_NAME ${Gmock_LIBRARY_RELEASE} NAME)
get_filename_component(Gmock_LIBRARY_DEBUG_NAME ${Gmock_LIBRARY_DEBUG} NAME)
set(Gmock_LIBRARY_RELEASE_INSTALLED ${LIB_PREFIX_RELEASE}${Gmock_LIBRARY_RELEASE_NAME})
set(Gmock_LIBRARY_DEBUG_INSTALLED ${LIB_PREFIX_DEBUG}${Gmock_LIBRARY_DEBUG_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Gmock_LIBRARY_RELEASE} ${LIB_PREFIX_RELEASE})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Gmock_LIBRARY_DEBUG} ${LIB_PREFIX_DEBUG})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${Gmock_INCLUDE_DIR} ${INC_DIR})
message("-- Installed ${Gmock_LIBRARY_RELEASE_INSTALLED}")
message("-- Installed ${Gmock_LIBRARY_DEBUG_INSTALLED}")
file(GLOB_RECURSE HEADERS ${INC_DIR} "${INC_DIR}gmock/*")
foreach(HEADER ${HEADERS})
  message("-- Installed ${HEADER}")
endforeach()

APPEND_TO_EXPORT_FILE("ADD_LIBRARY(gmock_static STATIC IMPORTED)")
APPEND_TO_EXPORT_FILE("SET_TARGET_PROPERTIES(gmock_static PROPERTIES")
APPEND_TO_EXPORT_FILE("                        IMPORTED_CONFIGURATIONS \"RELEASE;DEBUG\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_RELEASE \"${Gmock_LIBRARY_RELEASE_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_DEBUG \"${Gmock_LIBRARY_DEBUG_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE \"${Gtest_LIBRARY_RELEASE_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG \"${Gtest_LIBRARY_DEBUG_INSTALLED}\")\n")


###################################################################################################
# Cryptopp                                                                                        #
###################################################################################################
if((${BUILD_TARGET} MATCHES "CRYPTOPP") OR (${BUILD_TARGET} MATCHES "ALL"))
  message("-- Building Crypto++")
  message("     This may take a few minutes...")
  set(CRYPTOPP_BUILD_DIR ${CURRENT_DIR}/build_cryptopp)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
  if(MSVC)
    execute_process(COMMAND ${CMAKE_COMMAND} ../src/cryptopp -G ${CMAKE_GENERATOR} WORKING_DIRECTORY ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(CRYPTOPP_OUTVAR "${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug --target cryptopp_static WORKING_DIRECTORY ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(CRYPTOPP_OUTVAR "${CRYPTOPP_OUTVAR}${SEPARATOR}${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release WORKING_DIRECTORY ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(CRYPTOPP_OUTVAR "${CRYPTOPP_OUTVAR}${SEPARATOR}${OUTVAR}")
  else()
    execute_process(COMMAND ${CMAKE_COMMAND} ../src/cryptopp -G ${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=Debug WORKING_DIRECTORY ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(CRYPTOPP_OUTVAR "${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --target cryptopp_static WORKING_DIRECTORY ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(CRYPTOPP_OUTVAR "${CRYPTOPP_OUTVAR}${SEPARATOR}${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} ../src/cryptopp -G ${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=Release WORKING_DIRECTORY ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(CRYPTOPP_OUTVAR "${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${CRYPTOPP_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(CRYPTOPP_OUTVAR "${CRYPTOPP_OUTVAR}${SEPARATOR}${OUTVAR}")
  endif()
  file(GLOB CRYPTOPP_HEADERS ${CURRENT_DIR}/src/cryptopp/ "${CURRENT_DIR}/src/cryptopp/*.h")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${CRYPTOPP_BUILD_DIR}/include/cryptopp OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
  foreach(CRYPTOPP_HEADER ${CRYPTOPP_HEADERS})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CRYPTOPP_HEADER} ${CRYPTOPP_BUILD_DIR}/include/cryptopp/ OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
  endforeach()
endif()

find_library(Cryptopp_LIBRARY_RELEASE NAMES cryptopp PATHS ${CURRENT_DIR}/build_cryptopp
               PATH_SUFFIXES Release NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_library(Cryptopp_LIBRARY_DEBUG NAMES cryptopp_d cryptopp PATHS ${CURRENT_DIR}/build_cryptopp
               PATH_SUFFIXES Debug NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
if(WIN32)
  find_file(Cryptopp_PDB_DEBUG NAMES cryptopp_d.pdb PATHS ${CURRENT_DIR}/build_cryptopp
                 PATH_SUFFIXES Debug NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
endif()
find_path(Cryptopp_INCLUDE_DIR cryptopp/config.h PATHS ${CURRENT_DIR}/build_cryptopp/include
            NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

find_program(Cryptopp_TEST_EXE cryptest PATHS ${CURRENT_DIR}/build_cryptopp
               PATH_SUFFIXES Release NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

if(NOT Cryptopp_LIBRARY_RELEASE OR NOT Cryptopp_LIBRARY_DEBUG OR NOT Cryptopp_INCLUDE_DIR OR (NOT Cryptopp_TEST_EXE AND NOT (CMAKE_CXX_COMPILER_ID STREQUAL "Clang")))
  message(FATAL_ERROR "${CRYPTOPP_OUTVAR}")
endif()

message("-- Found Crypto++")
get_filename_component(Cryptopp_LIBRARY_RELEASE_NAME ${Cryptopp_LIBRARY_RELEASE} NAME)
get_filename_component(Cryptopp_LIBRARY_DEBUG_NAME ${Cryptopp_LIBRARY_DEBUG} NAME)
set(Cryptopp_LIBRARY_RELEASE_INSTALLED ${LIB_PREFIX_RELEASE}${Cryptopp_LIBRARY_RELEASE_NAME})
set(Cryptopp_LIBRARY_DEBUG_INSTALLED ${LIB_PREFIX_DEBUG}${Cryptopp_LIBRARY_DEBUG_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Cryptopp_LIBRARY_RELEASE} ${LIB_PREFIX_RELEASE})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Cryptopp_LIBRARY_DEBUG} ${LIB_PREFIX_DEBUG})
if(Cryptopp_PDB_DEBUG)
  execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Cryptopp_PDB_DEBUG} ${LIB_PREFIX_DEBUG})
endif()
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${Cryptopp_INCLUDE_DIR} ${INC_DIR})
message("-- Installed ${Cryptopp_LIBRARY_RELEASE_INSTALLED}")
message("-- Installed ${Cryptopp_LIBRARY_DEBUG_INSTALLED}")
file(GLOB_RECURSE HEADERS ${INC_DIR} "${INC_DIR}cryptopp/*")
foreach(HEADER ${HEADERS})
  message("-- Installed ${HEADER}")
endforeach()

APPEND_TO_EXPORT_FILE("ADD_LIBRARY(cryptopp_static STATIC IMPORTED)")
APPEND_TO_EXPORT_FILE("SET_TARGET_PROPERTIES(cryptopp_static PROPERTIES")
APPEND_TO_EXPORT_FILE("                        IMPORTED_CONFIGURATIONS \"RELEASE;DEBUG\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_RELEASE \"${Cryptopp_LIBRARY_RELEASE_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_DEBUG \"${Cryptopp_LIBRARY_DEBUG_INSTALLED}\")\n")

set(CRYPTO_TEST_FILE "${CURRENT_DIR}/crypto_test.cmake")
file(WRITE ${CRYPTO_TEST_FILE} "\n")
function(APPEND_TO_CRYPTO_TEST_FILE LINE)
  file(APPEND ${CRYPTO_TEST_FILE} "${LINE}\n")
endfunction()

APPEND_TO_CRYPTO_TEST_FILE("FILE(COPY \"${CURRENT_DIR}/src/cryptopp/TestData\" DESTINATION . FILES_MATCHING PATTERN \"*.dat\")")
APPEND_TO_CRYPTO_TEST_FILE("FILE(COPY \"${CURRENT_DIR}/src/cryptopp/TestVectors\" DESTINATION . FILES_MATCHING PATTERN \"*.txt\")")
APPEND_TO_CRYPTO_TEST_FILE("IF(NOT MEMORY_CHECK)")
APPEND_TO_CRYPTO_TEST_FILE("  ADD_TEST(CrypoppTest.BEH_FULL \"${Cryptopp_TEST_EXE}\" v)")
APPEND_TO_CRYPTO_TEST_FILE("  ADD_TEST(CrypoppTest.BEH_AES \"${Cryptopp_TEST_EXE}\" tv aes)")
APPEND_TO_CRYPTO_TEST_FILE("  ADD_TEST(CrypoppTest.BEH_SHA \"${Cryptopp_TEST_EXE}\" tv sha)")
APPEND_TO_CRYPTO_TEST_FILE("  ADD_TEST(CrypoppTest.BEH_RSA_PSS \"${Cryptopp_TEST_EXE}\" tv rsa_pss)")
APPEND_TO_CRYPTO_TEST_FILE("  ADD_TEST(CrypoppTest.BEH_RSA_OAEP \"${Cryptopp_TEST_EXE}\" tv rsa_oaep)")
APPEND_TO_CRYPTO_TEST_FILE("  ADD_TEST(CrypoppTest.BEH_RSA_PKCS1_1_5 \"${Cryptopp_TEST_EXE}\" tv rsa_pkcs1_1_5)")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_FULL PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_FULL PROPERTY TIMEOUT 60)")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_AES PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_AES PROPERTY TIMEOUT 60)")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_SHA PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_SHA PROPERTY TIMEOUT 60)")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PSS PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PSS PROPERTY TIMEOUT 60)")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_RSA_OAEP PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_RSA_OAEP PROPERTY TIMEOUT 60)")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PKCS1_1_5 PROPERTY LABELS Behavioural CryptoPP ${CMAKE_BUILD_TYPE})")
APPEND_TO_CRYPTO_TEST_FILE("  SET_PROPERTY(TEST CrypoppTest.BEH_RSA_PKCS1_1_5 PROPERTY TIMEOUT 60)")
APPEND_TO_CRYPTO_TEST_FILE("ENDIF()")


###################################################################################################
# Google Breakpad                                                                                 #
###################################################################################################

# Currently not supporting breakpad using OSX
if (FALSE)
if((${BUILD_TARGET} MATCHES "BREAKPAD") OR (${BUILD_TARGET} MATCHES "ALL"))
  message("-- Building Google Breakpad Client")
  message("     This may take a few minutes...")
  set(BREAKPAD_BUILD_DIR ${CURRENT_DIR}/build_breakpad)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BREAKPAD_BUILD_DIR})
  if(MSVC)
    execute_process(COMMAND ${CMAKE_COMMAND} ../src/google-breakpad -G ${CMAKE_GENERATOR} WORKING_DIRECTORY ${BREAKPAD_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(BREAKPAD_OUTVAR "${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Debug WORKING_DIRECTORY ${BREAKPAD_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(BREAKPAD_OUTVAR "${BREAKPAD_OUTVAR}${SEPARATOR}${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release WORKING_DIRECTORY ${BREAKPAD_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(BREAKPAD_OUTVAR "${BREAKPAD_OUTVAR}${SEPARATOR}${OUTVAR}")
  else()
    execute_process(COMMAND ${CMAKE_COMMAND} ../src/google-breakpad -G ${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=Release WORKING_DIRECTORY ${BREAKPAD_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(BREAKPAD_OUTVAR "${OUTVAR}")
    execute_process(COMMAND ${CMAKE_COMMAND} --build . WORKING_DIRECTORY ${BREAKPAD_BUILD_DIR} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
    set(BREAKPAD_OUTVAR "${BREAKPAD_OUTVAR}${SEPARATOR}${OUTVAR}")
  endif()
  if(WIN32)
    set(BREAKPAD_HEADERS
          client/windows/handler/exception_handler.h
          client/windows/common/ipc_protocol.h
          client/windows/crash_generation/crash_generation_client.h
          common/windows/string_utils-inl.h)
  elseif(UNIX)
    if(APPLE)
      set(BREAKPAD_HEADERS
            client/mac/handler/exception_handler.h
            client/mac/crash_generation/crash_generation_client.h
            common/mac/MachIPC.h)
    else()
      set(BREAKPAD_HEADERS
            client/linux/handler/exception_handler.h
            client/linux/android_ucontext.h
            client/linux/crash_generation/crash_generation_client.h
            client/linux/minidump_writer/minidump_writer.h)
    endif()
  endif()
  set(BREAKPAD_HEADERS ${BREAKPAD_HEADERS}
        google_breakpad/common/breakpad_types.h
        google_breakpad/common/minidump_cpu_amd64.h
        google_breakpad/common/minidump_cpu_arm.h
        google_breakpad/common/minidump_cpu_ppc.h
        google_breakpad/common/minidump_cpu_ppc64.h
        google_breakpad/common/minidump_cpu_sparc.h
        google_breakpad/common/minidump_cpu_x86.h
        google_breakpad/common/minidump_exception_linux.h
        google_breakpad/common/minidump_exception_mac.h
        google_breakpad/common/minidump_exception_solaris.h
        google_breakpad/common/minidump_exception_win32.h
        google_breakpad/common/minidump_format.h
        processor/scoped_ptr.h)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BREAKPAD_BUILD_DIR}/include/breakpad OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
  foreach(BREAKPAD_HEADER ${BREAKPAD_HEADERS})
    set(INSTALL_PATH ${BREAKPAD_BUILD_DIR}/include/breakpad/${BREAKPAD_HEADER})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${CURRENT_DIR}/src/google-breakpad/src/${BREAKPAD_HEADER} ${INSTALL_PATH} OUTPUT_VARIABLE OUTVAR RESULT_VARIABLE RESVAR)
  endforeach()
endif()

find_library(Breakpad_LIBRARY_RELEASE NAMES breakpad PATHS ${CURRENT_DIR}/build_breakpad
               PATH_SUFFIXES Release NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_library(Breakpad_LIBRARY_DEBUG NAMES breakpad_d breakpad PATHS ${CURRENT_DIR}/build_breakpad
               PATH_SUFFIXES Debug NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)
find_path(Breakpad_INCLUDE_DIR breakpad/google_breakpad/common/minidump_format.h PATHS ${CURRENT_DIR}/build_breakpad/include
            NO_CMAKE_PATH NO_SYSTEM_ENVIRONMENT_PATH NO_CMAKE_SYSTEM_PATH)

if(NOT Breakpad_LIBRARY_RELEASE OR NOT Breakpad_LIBRARY_DEBUG OR NOT Breakpad_INCLUDE_DIR)
  message(FATAL_ERROR "${BREAKPAD_OUTVAR}")
endif()

message("-- Found Google Breakpad Client")
get_filename_component(Breakpad_LIBRARY_RELEASE_NAME ${Breakpad_LIBRARY_RELEASE} NAME)
get_filename_component(Breakpad_LIBRARY_DEBUG_NAME ${Breakpad_LIBRARY_DEBUG} NAME)
set(Breakpad_LIBRARY_RELEASE_INSTALLED ${LIB_PREFIX_RELEASE}${Breakpad_LIBRARY_RELEASE_NAME})
set(Breakpad_LIBRARY_DEBUG_INSTALLED ${LIB_PREFIX_DEBUG}${Breakpad_LIBRARY_DEBUG_NAME})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Breakpad_LIBRARY_RELEASE} ${LIB_PREFIX_RELEASE})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy ${Breakpad_LIBRARY_DEBUG} ${LIB_PREFIX_DEBUG})
execute_process(COMMAND ${CMAKE_COMMAND} -E copy_directory ${Breakpad_INCLUDE_DIR} ${INC_DIR})
message("-- Installed ${Breakpad_LIBRARY_RELEASE_INSTALLED}")
message("-- Installed ${Breakpad_LIBRARY_DEBUG_INSTALLED}")
file(GLOB_RECURSE HEADERS ${INC_DIR} "${INC_DIR}breakpad/*")
foreach(HEADER ${HEADERS})
  message("-- Installed ${HEADER}")
endforeach()

APPEND_TO_EXPORT_FILE("ADD_LIBRARY(breakpad_static STATIC IMPORTED)")
APPEND_TO_EXPORT_FILE("SET_TARGET_PROPERTIES(breakpad_static PROPERTIES")
APPEND_TO_EXPORT_FILE("                        IMPORTED_CONFIGURATIONS \"RELEASE;DEBUG\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_RELEASE \"${Breakpad_LIBRARY_RELEASE_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LOCATION_DEBUG \"${Breakpad_LIBRARY_DEBUG_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE \"${Breakpad_LIBRARY_RELEASE_INSTALLED}\"")
APPEND_TO_EXPORT_FILE("                        IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG \"${Breakpad_LIBRARY_DEBUG_INSTALLED}\")\n")

endif()

unset(BUILD_TARGET CACHE)

file(WRITE ${CURRENT_DIR}/install_prefix.cmake "SET(MAIDSAFE_COMMON_INSTALL_PREFIX \"${CMAKE_INSTALL_PREFIX}\")")
