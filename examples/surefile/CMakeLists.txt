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


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
get_filename_component(ProjectId "${CMAKE_SOURCE_DIR}" NAME)
string(REPLACE " " "_" ProjectId "${ProjectId}")
project(${ProjectId}_lib)


#==================================================================================================#
#  Import standard environment and bring the MaidSafe libraries into scope as CMake targets.       #
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
#  Standard setup stuff.                                                                           #
#==================================================================================================#
enable_testing()
include(CTest)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include(check_licenses)
include(utils)
check_licenses()
get_target_platform()
get_target_architecture()

include(maidsafe_find_openmp)

if(WIN32)
  set(CBFS_KEY "${MAIDSAFE_BINARY_DIR}/MaidSafe-Drive-Private/eldos_licence_key.txt")
  include(maidsafe_find_cbfs)
endif()


#==================================================================================================#
#  Set up files.                                                                                   #
#==================================================================================================#
set(ProjectApiDir ${PROJECT_SOURCE_DIR}/include/maidsafe/${ProjectId})
set(ProjectSourcesDir ${PROJECT_SOURCE_DIR}/src/maidsafe/${ProjectId})
set(ProjectDemo ${ProjectSourcesDir}/${ProjectId}_demo.cc)
glob_dir(${ProjectId} ${ProjectSourcesDir} ${ProjectId})

file(GLOB ProjectApiFiles "${ProjectApiDir}/*.*")
file(GLOB ProjectSourceFiles "${ProjectSourcesDir}/*.*")

set(ProjectAllFiles ${${ProjectId}AllFiles})
list(REMOVE_ITEM ProjectAllFiles ${ProjectDemo})

set(TestFiles ${PROJECT_SOURCE_DIR}/src/maidsafe/${ProjectId}/tests/surefile_test.cc)

include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_SOURCE_DIR}/src)


#==================================================================================================#
#  Define libraries and executables.                                                               #
#==================================================================================================#
add_library(maidsafe_surefile STATIC ${ProjectAllFiles})
target_link_libraries(maidsafe_surefile maidsafe_drive)
set_target_properties(maidsafe_surefile PROPERTIES FOLDER "Library")

add_executable(surefile_demo ${ProjectDemo})
target_link_libraries(surefile_demo maidsafe_surefile)
set_target_properties(surefile_demo PROPERTIES FOLDER "Executables")

add_executable(TESTsurefile ${TestFiles})
target_link_libraries(TESTsurefile maidsafe_surefile)
set_target_properties(TESTsurefile PROPERTIES FOLDER "Tests")


#==================================================================================================#
#  Export detail                                                                                   #
#==================================================================================================#
set(ExportFile ${CMAKE_BINARY_DIR}/surefile_export.cmake)
file(APPEND ${ExportFile} "# Include paths\n")
file(APPEND ${ExportFile} "include_directories(\"${PROJECT_SOURCE_DIR}/include\")\n")
export(TARGETS maidsafe_surefile APPEND FILE ${ExportFile})


#==================================================================================================#
#  Set compiler and linker flags.                                                                  #
#==================================================================================================#
include(standard_flags)
remove_definitions(-DAPPLICATION_NAME=LifeStuff
                   -DAPPLICATION_VERSION_MAJOR=${ApplicationVersionMajor}
                   -DAPPLICATION_VERSION_MINOR=${ApplicationVersionMinor}
                   -DAPPLICATION_VERSION_PATCH=${ApplicationVersionPatch})
add_definitions(-DAPPLICATION_NAME=SureFile
                -DAPPLICATION_VERSION_MAJOR=0
                -DAPPLICATION_VERSION_MINOR=5
                -DAPPLICATION_VERSION_PATCH=000)
if(Cbfs_FOUND)
  add_definitions(-DCBFS_KEY=${Cbfs_KEY} -DHAVE_CBFS)
endif()
