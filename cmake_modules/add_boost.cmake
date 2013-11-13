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
#  Sets up Boost using ExternalProject_Add.                                                        #
#                                                                                                  #
#  Only the first 2 variables should require regular maintenance, i.e. BoostVersion & BoostSHA1.   #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    BoostSourceDir (required for subsequent include_directories calls) and per-library            #
#    variables defining the full path(s) to the release (and debug for MSVC) libraries, e.g.       #
#    BoostDateTimeLibs, BoostFilesystemLibs.                                                       #
#                                                                                                  #
#==================================================================================================#

set(BoostVersion 1.55.0)
set(BoostSHA1 cef9a0cc7084b1d639e06cd3bc34e4251524c840)



# Create build folder name derived from version
string(REGEX REPLACE "beta\\.([0-9])$" "beta\\1" BoostFolderName ${BoostVersion})
string(REPLACE "." "_" BoostFolderName ${BoostFolderName})
set(BoostFolderName boost_${BoostFolderName})

get_temp_dir()
if(NOT IS_DIRECTORY "${TempDir}")
  set(TempDir ${CMAKE_BINARY_DIR})
endif()
set(BoostDownloadFolder "${BoostFolderName}_${CMAKE_CXX_COMPILER_ID}_${CMAKE_CXX_COMPILER_VERSION}")
string(REPLACE "." "_" BoostDownloadFolder ${BoostDownloadFolder})
set(BoostDownloadFolder ${TempDir}/${BoostDownloadFolder})

# Download boost if required
if(NOT EXISTS "${TempDir}/${BoostFolderName}.tar.bz2")
  message(STATUS "Downloading boost ${BoostVersion} to ${TempDir}")
endif()
file(DOWNLOAD http://sourceforge.net/projects/boost/files/boost/${BoostVersion}/${BoostFolderName}.tar.bz2/download
     ${TempDir}/${BoostFolderName}.tar.bz2
     INACTIVITY_TIMEOUT 60
     TIMEOUT 600
     STATUS Status
     SHOW_PROGRESS
     EXPECTED_HASH SHA1=${BoostSHA1}
     )

# Extract boost if required
string(FIND "${Status}" "returning early" Found)
if(Found LESS 0 OR NOT IS_DIRECTORY "${BoostDownloadFolder}")
  message(STATUS "Extracting boost ${BoostVersion} to ${BoostDownloadFolder}")
  file(MAKE_DIRECTORY "${BoostDownloadFolder}")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${TempDir}/${BoostFolderName}.tar.bz2
                  WORKING_DIRECTORY ${BoostDownloadFolder}
                  RESULT_VARIABLE Result
                  )
  if(NOT Result EQUAL 0)
    message(FATAL_ERROR "Failed extracting boost ${BoostVersion} to ${BoostDownloadFolder}")
  endif()
endif()

# Get the path to the extracted folder
file(GLOB BoostSourceDir "${BoostDownloadFolder}/*")
list(LENGTH BoostSourceDir n)
if(NOT n EQUAL 1 OR NOT IS_DIRECTORY "${BoostSourceDir}")
  message(FATAL_ERROR "Failed extracting boost ${BoostVersion} to ${BoostDownloadFolder}")
endif()

# Build b2 (bjam) if required
unset(b2Path CACHE)
find_program(b2Path NAMES b2 PATHS ${BoostSourceDir} NO_DEFAULT_PATH)
if(NOT b2Path)
  message(STATUS "Building b2 (bjam)")
  if(MSVC)
    set(b2Bootstrap "bootstrap.bat")
  else()
    set(b2Bootstrap "./bootstrap.sh")
  endif()
  execute_process(COMMAND ${b2Bootstrap} WORKING_DIRECTORY ${BoostSourceDir}
                  RESULT_VARIABLE Result OUTPUT_VARIABLE Output)
  if(NOT Result EQUAL 0)
    message(FATAL_ERROR "Failed running ${b2Bootstrap}:\n${Output}\n")
  endif()
endif()
execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${BoostSourceDir}/Build)

# Expose BoostSourceDir to parent scope
set(BoostSourceDir ${BoostSourceDir} PARENT_SCOPE)

# Set up general b2 (bjam) command line arguments
set(b2Args <SOURCE_DIR>/b2
           link=static
           threading=multi
           runtime-link=shared
           --layout=tagged
           --build-dir=Build
           stage
           -d+2
           )

# Set up platform-specific b2 (bjam) command line arguments
if(MSVC)
  list(APPEND b2Args
              toolset=msvc
              define=_BIND_TO_CURRENT_MFC_VERSION=1
              define=_BIND_TO_CURRENT_CRT_VERSION=1
              )
  if(${TargetArchitecture} STREQUAL "x86_64")
    list(APPEND b2Args address-model=64)
  endif()
elseif(UNIX)
  list(APPEND b2Args variant=release cxxflags=-fPIC cxxflags=-std=c++11 -sNO_BZIP2=1)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    list(APPEND b2Args toolset=clang)
    if(HAVE_LIBC++)
      list(APPEND b2Args cxxflags=-stdlib=libc++ linkflags=-stdlib=libc++)
    endif()
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    list(APPEND b2Args toolset=gcc)
  endif()
elseif(APPLE)
  list(APPEND b2Args toolset=clang cxxflags=-fPIC cxxflags=-std=c++11 architecture=combined address-model=32_64)
endif()

# Get list of components
execute_process(COMMAND ./b2 --show-libraries WORKING_DIRECTORY ${BoostSourceDir}
                ERROR_QUIET OUTPUT_VARIABLE Output)
string(REGEX REPLACE "(^[^:]+:|[- ])" "" BoostComponents "${Output}")
string(REGEX REPLACE "\n" ";" BoostComponents "${BoostComponents}")

# Build each required component
include(ExternalProject)
foreach(Component ${BoostComponents})
  ExternalProject_Add(
      boost_${Component}
      PREFIX ${CMAKE_BINARY_DIR}/${BoostFolderName}
      SOURCE_DIR ${BoostSourceDir}
      BINARY_DIR ${BoostSourceDir}
      CONFIGURE_COMMAND ""
      BUILD_COMMAND "${b2Args}" --with-${Component}
      INSTALL_COMMAND ""
      LOG_BUILD ON
      )
  underscores_to_camel_case(${Component} CamelCaseComponent)
  add_library(Boost${CamelCaseComponent} STATIC IMPORTED GLOBAL)
  if(MSVC)
    set_target_properties(Boost${CamelCaseComponent} PROPERTIES
                          IMPORTED_LOCATION_DEBUG ${BoostSourceDir}/stage/lib/libboost_${Component}-mt-gd.lib
                          IMPORTED_LOCATION_MINSIZEREL ${BoostSourceDir}/stage/lib/libboost_${Component}-mt.lib
                          IMPORTED_LOCATION_RELEASE ${BoostSourceDir}/stage/lib/libboost_${Component}-mt.lib
                          IMPORTED_LOCATION_RELWITHDEBINFO ${BoostSourceDir}/stage/lib/libboost_${Component}-mt.lib)
  else()
    set_target_properties(Boost${CamelCaseComponent} PROPERTIES
                          IMPORTED_LOCATION ${BoostSourceDir}/stage/lib/libboost_${Component}-mt.a)
  endif()
  set_target_properties(boost_${Component} Boost${CamelCaseComponent} PROPERTIES
                        LABELS Boost FOLDER "Third Party/Boost")
  add_dependencies(Boost${CamelCaseComponent} boost_${Component})
  set(Boost${CamelCaseComponent}Libs Boost${CamelCaseComponent})
  set(Boost${CamelCaseComponent}Libs ${Boost${CamelCaseComponent}Libs} PARENT_SCOPE)
endforeach()



# Set up download step for the currently-unofficial Boost.Process
ExternalProject_Add(
    boost_process
    PREFIX ${CMAKE_BINARY_DIR}/boost_process
    URL http://www.highscore.de/boost/process0.5/process.zip
    URL_HASH SHA1=281e8575e3593797c94f0230e40c2f0dc49923aa
    TIMEOUT 30
    CONFIGURE_COMMAND ""
    BUILD_COMMAND ""
    BUILD_IN_SOURCE ON
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_TEST ON
    LOG_INSTALL ON
    )

# Copy the folders/files to the main boost source dir 
ExternalProject_Add_Step(
    boost_process
    copy_boost_process_dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/boost/process ${BoostSourceDir}/boost/process
    COMMENT "Copying Boost.Process boost dir..."
    DEPENDEES download
    DEPENDERS configure
    )
ExternalProject_Add_Step(
    boost_process
    copy_boost_process_hpp
    COMMAND ${CMAKE_COMMAND} -E copy <SOURCE_DIR>/boost/process.hpp ${BoostSourceDir}/boost
    COMMENT "Copying Boost.Process header..."
    DEPENDEES download
    DEPENDERS configure
    )
ExternalProject_Add_Step(
    boost_process
    copy_libs_process_dir
    COMMAND ${CMAKE_COMMAND} -E copy_directory <SOURCE_DIR>/libs/process ${BoostSourceDir}/libs/process
    COMMENT "Copying Boost.Process libs dir..."
    DEPENDEES download
    DEPENDERS configure
    )
set_target_properties(boost_process PROPERTIES LABELS Boost FOLDER "Third Party/Boost")
add_dependencies(boost_process boost)
