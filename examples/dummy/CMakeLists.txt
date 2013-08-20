#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or    #
#  later, and The General Public License (GPL), version 3. By contributing code to this project    #
#  You agree to the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in    #
#  the root directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also    #
#  available at:                                                                                   #
#                                                                                                  #
#    http://www.novinet.com/license                                                                #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, software distributed under the       #
#  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       #
#  either express or implied. See the License for the specific language governing permissions      #
#  and limitations under the License.                                                              #
#                                                                                                  #
#==================================================================================================#
#                                                                                                  #
#  This can be used as a starting point for writing a scratch project.  It sets up MaidSafe        #
#  variables, gives access to the MaidSafe C++ libraries and CMake utility functions and has a     #
#  small example executable which depends on MaidSafe-Common only.  It also gives access to        #
#  MaidSafe dependencies like boost, crypto++, gtest and gmock.  To include more MaidSafe          #
#  libraries, see the comment blocks below.                                                        #
#                                                                                                  #
#==================================================================================================#


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
get_filename_component(ProjectId "${CMAKE_SOURCE_DIR}" NAME)
string(REPLACE " " "_" ProjectId "${ProjectId}")
project(${ProjectId})

#==================================================================================================#
#  This imports much of the standard MaidSafe environment, and also brings all the MaidSafe        #
#  libraries into scope as CMake targets.                                                          #
#==================================================================================================#
find_file(MaidSafeExport NAMES maidsafe_export.cmake
                         PATHS ${MAIDSAFE_BINARY_DIR} ../build ../../build
                         NO_DEFAULT_PATH)

if(NOT MaidSafeExport)
  set(ErrorMessage "\n\nCan't find maidsafe_export.cmake in MAIDSAFE_BINARY_DIR.  ")
  set(ErrorMessage "${ErrorMessage}Currently MAIDSAFE_BINARY_DIR is set to ")
  set(ErrorMessage "${ErrorMessage}\"${MAIDSAFE_BINARY_DIR}\"  It must be set to the MaidSafe ")
  set(ErrorMessage "${ErrorMessage}super-project's build root.\nTo set it, run:\n")
  set(ErrorMessage "${ErrorMessage}    cmake . -DMAIDSAFE_BINARY_DIR=\"<path to build root>\"\n\n")
  message(FATAL_ERROR "${ErrorMessage}")
else()
  get_filename_component(MAIDSAFE_BINARY_DIR "${MaidSafeExport}" PATH CACHE)
endif()

include(${MaidSafeExport})


#==================================================================================================#
#  Standard setup stuff - probably doesn't need changed.                                           #
#==================================================================================================#
enable_testing()
include(CTest)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include_directories("${PROJECT_SOURCE_DIR}/include")
include_directories("${PROJECT_SOURCE_DIR}/src")

include(utils)
get_target_platform()
get_target_architecture()


#==================================================================================================#
#  Handle protocol buffer files.                                                                   #
#                                                                                                  #
#  Uncomment the first block if you have .proto files in your 'src' dir                            #
#  Also uncomment the second block if you have .proto files in a nested dir called 'src/nested'    #
#==================================================================================================#
include(add_protoc_command)
#set(ProtobufRootDir ${PROJECT_SOURCE_DIR}/src)
#set(ProtobufRelDir "")
#add_protoc_command(Root "${ProtobufRootDir}" "${ProtobufRelDir}")

#set(ProtobufRelDir "nested")
#add_protoc_command(Nested "${ProtobufRootDir}" "${ProtobufRelDir}")


#==================================================================================================#
#  Define libraries and executables.                                                               #
#==================================================================================================#
# Add all source and header files to 'add_executable' or 'add_library' calls
add_executable(${ProjectId}
               src/main.cc
               ${RootProtos}
               ${NestedProtos}
               )
# Specify dependencies.  MaidSafe libraries are prefixed 'maidsafe_'.  For details, see the contents
# of the ${MAIDSAFE_BINARY_DIR}/CMakeExports/<MaidSafe lib name>_export.cmake files.
target_link_libraries(${ProjectId} maidsafe_common)


#==================================================================================================#
#  Set compiler and linker flags.  Further flags can be appended as shown below for the Windows    #
#  and Linux versions of "don't treat warnings as errors" respectively.  For any conflicting       #
#  flags, the last one in the flags string will "win".                                             # 
#==================================================================================================#
include(standard_flags)
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /WX-")
#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-error")
