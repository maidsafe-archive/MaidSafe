#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
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


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.10 FATAL_ERROR)
get_filename_component(ProjectId "${CMAKE_SOURCE_DIR}" NAME)
string(REPLACE " " "_" ProjectId "${ProjectId}")
project(${ProjectId})


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

include(utils)
get_target_platform()
get_target_architecture()

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

#==================================================================================================#
#  Define libraries and executables.                                                               #
#==================================================================================================#
add_library(surefile STATIC ${ProjectAllFiles})
target_link_libraries(surefile maidsafe_drive)
set_target_properties(surefile PROPERTIES FOLDER "Library")

add_executable(surefile_demo ${ProjectDemo})
target_link_libraries(surefile_demo surefile)
set_target_properties(surefile_demo PROPERTIES FOLDER "Executables")

add_executable(TESTsurefile ${TestFiles})
target_link_libraries(TESTsurefile surefile)
set_target_properties(TESTsurefile PROPERTIES FOLDER "Tests")


#==================================================================================================#
#  Set compiler and linker flags.                                                                  #
#==================================================================================================#
include(standard_flags)
if(Cbfs_FOUND)
  add_definitions(-DCBFS_KEY=${Cbfs_KEY} -DHAVE_CBFS)
endif()
