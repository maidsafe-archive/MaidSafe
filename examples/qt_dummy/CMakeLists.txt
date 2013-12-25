#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
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
#  This can be used as a starting point for writing a Qt UI project for MaidSafe libraries.        #
#  It sets up MaidSafe variables, gives access to the MaidSafe C++ libraries and CMake utility     #
#  functions and has a small UI example executable.  It also gives access to MaidSafe dependencies #
#  like boost, crypto++, gtest and gmock.  To include more MaidSafe libraries,                     #
#  see the comment blocks below.                                                                   #
#                                                                                                  #
#==================================================================================================#


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.11 FATAL_ERROR)
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

include(utils)
ms_get_target_platform()
ms_get_target_architecture()


#==================================================================================================#
# Qt library search                                                                                #
#==================================================================================================#
set(Qt5Required ON)
include(maidsafe_find_qt5)


#==================================================================================================#
#  Handle protocol buffer files.                                                                   #
#                                                                                                  #
#  Uncomment the first block if you have .proto files in your 'src' dir                            #
#  Also uncomment the second block if you have .proto files in a nested dir called 'src/nested'    #
#==================================================================================================#
include(add_protoc_command)
#set(ProtobufRootDir ${PROJECT_SOURCE_DIR}/src)
#set(ProtobufRelDir "")
#ms_add_protoc_command(Root "${ProtobufRootDir}" "${ProtobufRelDir}")

#set(ProtobufRelDir "nested")
#ms_add_protoc_command(Nested "${ProtobufRootDir}" "${ProtobufRelDir}")


#==================================================================================================#
# Set up UI files                                                                                  #
#==================================================================================================#
# Add main.cc and ui helpers
file(GLOB SOURCE_FILES_HELPERS "src/*.*")

# Add controllers
file(GLOB SOURCE_FILES_CONTROLLERS "src/controllers/*.*")

# Add .ui(widget) files
file(GLOB SOURCE_FILES_WIDGETS "src/widgets/*.ui")
QT5_WRAP_UI(WRAPPED_SOURCE_FILES_WIDGETS ${SOURCE_FILES_WIDGETS})

# Add .qrc files
file(GLOB SOURCE_FILES_RESOURCES "src/resources/*.qrc")
QT5_ADD_RESOURCES(WRAPPED_SOURCE_FILES_RESOURCES ${SOURCE_FILES_RESOURCES})


#==================================================================================================#
#  Define libraries and executables.                                                               #
#==================================================================================================#
# Add all source and header files to 'add_executable' or 'add_library' calls
add_executable(${ProjectId}
               ${SOURCE_FILES_HELPERS}
               ${SOURCE_FILES_CONTROLLERS}
               ${SOURCE_FILES_WIDGETS}
               ${WRAPPED_SOURCE_FILES_WIDGETS}
               ${WRAPPED_SOURCE_FILES_RESOURCES}
               ${RootProtos}
               ${NestedProtos}
               )

target_include_directories(${ProjectId} PUBLIC "${PROJECT_SOURCE_DIR}/include" PRIVATE "${PROJECT_SOURCE_DIR}/src")

# Specify dependencies.  MaidSafe libraries are prefixed 'maidsafe_'.  For details, see the contents
# of the ${MAIDSAFE_BINARY_DIR}/CMakeExports/<MaidSafe lib name>_export.cmake files.
target_link_libraries(${ProjectId} maidsafe_common Qt5::Core Qt5::Widgets)

if(WIN32)
  set_target_properties(${ProjectId} PROPERTIES WIN32_EXECUTABLE TRUE)
endif()

#==================================================================================================#
#  Set compiler and linker flags.  Further flags can be appended as shown below for the Windows    #
#  and Linux versions of "don't treat warnings as errors" respectively.  For any conflicting       #
#  flags, the last one in the flags string will "win".                                             #
#==================================================================================================#
include(standard_flags)
#target_compile_options(${ProjectId} PRIVATE $<$<BOOL:${MSVC}>:/WX-> $<$<BOOL:${UNIX}>:-Wno-error>)

target_compile_definitions(${ProjectId} PRIVATE $<$<CONFIG:Release>:QT_NO_DEBUG_OUTPUT QT_NO_WARNING_OUTPUT QT_NO_DEBUG_STREAM>)
