#==================================================================================================#
#                                                                                                  #
#  Copyright 2014 MaidSafe.net limited                                                             #
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
#                                                                                                  #
#  Sets up monolithic target for Win/Unix platforms.                                               #
#                                                                                                  #
#==================================================================================================#


set(DevLibDepends maidsafe_common
                  maidsafe_passport
                  maidsafe_crux
                  maidsafe_routing
                  #maidsafe_encrypt
                  #maidsafe_api
                  #maidsafe_nfs_core
                  #maidsafe_nfs_client
                  #maidsafe_launcher
                  ${AllBoostLibs}
                  cryptopp
                  sqlite)
list(REMOVE_ITEM DevLibDepends BoostContext BoostPython BoostGraphParallel BoostMath BoostMpi BoostRegex BoostSerialization BoostTest)

set(SourceFile "${MaidsafeGeneratedSourcesDir}/monolithic.cc")
ms_set_postfixes()
add_library(maidsafe STATIC "${SourceFile}")
set_target_properties(maidsafe PROPERTIES FOLDER "MaidSafe/Libraries/Monolithic")

set(MonolithicIncludes "${CMAKE_BINARY_DIR}/MonolithicIncludes")

set(HeadersHelper "${CMAKE_BINARY_DIR}/headers_helper.cmake")
configure_file("${CMAKE_SOURCE_DIR}/cmake_modules/headers_helper.cmake.in" "${HeadersHelper}" @ONLY)
if(CMAKE_BUILD_TYPE STREQUAL "Release" OR CMAKE_CONFIGURATION_TYPES)
  add_custom_command(TARGET maidsafe POST_BUILD COMMAND ${CMAKE_COMMAND} -P "${HeadersHelper}")
endif()

foreach(Lib ${DevLibDepends})
  if(MSVC)
    if(POLICY CMP0026)
      cmake_policy(PUSH)
      cmake_policy(SET CMP0026 OLD)
    endif()
    foreach(Config ${CMAKE_CONFIGURATION_TYPES})
      string(TOUPPER ${Config} ConfigUppercase)
      get_target_property(LibLocation${Config} ${Lib} LOCATION_${ConfigUppercase})
      list(APPEND LibLocations${Config} "${LibLocation${Config}}")
    endforeach()
    if(POLICY CMP0026)
      cmake_policy(POP)
    endif()
  else()
    list(APPEND LibLocations "$<TARGET_FILE:${Lib}>")
  endif()
endforeach()

target_link_libraries(maidsafe PUBLIC ${DevLibDepends})

add_custom_command(OUTPUT "${SourceFile}"
    COMMAND ${CMAKE_COMMAND} -E make_directory "${MaidsafeGeneratedSourcesDir}"
    COMMAND ${CMAKE_COMMAND} -E touch "${SourceFile}"
    DEPENDS ${DevLibDepends})

if(MSVC)
  foreach(Config ${CMAKE_CONFIGURATION_TYPES})
    list(REMOVE_DUPLICATES LibLocations${Config})
    # Turn list of libs to space-separated string for passing to linker
    string(REPLACE ";" "\" \"" Flags${Config} "${LibLocations${Config}}")
    string(TOUPPER ${Config} ConfigUppercase)
    set_target_properties(maidsafe PROPERTIES STATIC_LIBRARY_FLAGS_${ConfigUppercase} "\"${Flags${Config}}\"")
  endforeach()
elseif(APPLE)
  find_program(LibTool NAMES libtool)
  if(NOT LibTool)
    message(FATAL_ERROR "Failed to find libtool.")
  endif()
  add_custom_command(TARGET maidsafe POST_BUILD
      COMMAND rm $<TARGET_FILE:maidsafe>
      COMMAND ${LibTool} -static -o $<TARGET_FILE:maidsafe> ${LibLocations})
else()
  add_custom_command(TARGET maidsafe POST_BUILD
      COMMAND rm $<TARGET_FILE:maidsafe>
      COMMAND ${CMAKE_COMMAND}
      -DSUPER_PROJECT_BINARY_DIR="${CMAKE_BINARY_DIR}"
      -DSUPER_PROJECT_SOURCE_DIR="${CMAKE_SOURCE_DIR}"
      -DTARGET_LOCATION="$<TARGET_FILE:maidsafe>"
      -DSTATIC_LIBS="${LibLocations}"
      -DLIB="${Lib}"
      -DCMAKE_AR="${CMAKE_AR}"
      -DConfig=$<CONFIGURATION>
      -P "${CMAKE_SOURCE_DIR}/cmake_modules/make_unix_static_lib.cmake"
      -V)
endif()
