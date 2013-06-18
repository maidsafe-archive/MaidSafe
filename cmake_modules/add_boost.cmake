#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2013 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#
#                                                                                                  #
#  Sets up Boost using ExternalProject_Add.                                                        #
#                                                                                                  #
#  Only the first 3 variables should require regular maintenance, i.e. BoostComponents,            #
#  BoostVersion and BoostSHA1.                                                                     #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    BoostSourceDir (required for subsequent include_directories calls) and per-library            #
#    variables defining the full path(s) to the release (and debug for MSVC) libraries, e.g.       #
#    BoostDateTimeLibs, BoostFilesystemLibs.                                                       #
#                                                                                                  #
#==================================================================================================#

set(BoostComponents
      chrono
      coroutine
      date_time
      filesystem
      iostreams
      program_options
      python
      regex
      system
      thread
      timer
      )
set(BoostVersion 1.54.0.beta.1)
set(BoostSHA1 54d862d4171ccaaa03e860f7a63143ca7f1c84b2)



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
foreach(BoostComponent ${BoostComponents})
  list(APPEND b2Args --with-${BoostComponent})
endforeach()

# Set up platform-specific b2 (bjam) command line arguments
if(MSVC)
  list(APPEND b2Args
              toolset=msvc
#              --layout=versioned
              define=_BIND_TO_CURRENT_MFC_VERSION=1
              define=_BIND_TO_CURRENT_CRT_VERSION=1
              )
  if(${TargetArchitecture} STREQUAL "x86_64")
    list(APPEND b2Args address-model=64)
  endif()
elseif(UNIX)
  list(APPEND b2Args variant=release cxxflags=-std=c++11 -sNO_BZIP2=1) # --layout=system)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    list(APPEND b2Args toolset=clang)
    if(HAVE_LIBC++)
      list(APPEND b2Args cxxflags=-stdlib=libc++ linkflags=-stdlib=libc++)
    endif()
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GCC")
    list(APPEND b2Args toolset=gcc)
  endif()
elseif(APPLE)
  list(APPEND b2Args toolset=clang cxxflags=-std=c++11 architecture=combined address-model=32_64)
endif()

# Create build folder name derived from version
string(REGEX REPLACE "beta\\.([0-9])$" "beta\\1" BoostFolderName ${BoostVersion})
string(REPLACE "." "_" BoostFolderName ${BoostFolderName})
set(BoostFolderName boost_${BoostFolderName})

# Set up build steps
include(ExternalProject)
ExternalProject_Add(
    boost
    PREFIX ${CMAKE_BINARY_DIR}/${BoostFolderName}
    URL http://sourceforge.net/projects/boost/files/boost/${BoostVersion}/${BoostFolderName}.tar.bz2/download
    URL_HASH SHA1=${BoostSHA1}
    TIMEOUT 600
    PATCH_COMMAND ${CMAKE_COMMAND} -E copy
        ${CMAKE_SOURCE_DIR}/src/third_party_libs/${BoostFolderName}/boost_variant_detail_move_patched.hpp
        <SOURCE_DIR>/boost/variant/detail/move.hpp
    CONFIGURE_COMMAND ${CMAKE_COMMAND} -E make_directory <SOURCE_DIR>/Build
    BUILD_COMMAND "${b2Args}"
    BUILD_IN_SOURCE ON
    INSTALL_COMMAND ""
    LOG_DOWNLOAD ON
    LOG_UPDATE ON
    LOG_CONFIGURE ON
    LOG_BUILD ON
    LOG_TEST ON
    LOG_INSTALL ON
    )

# Set extra step to build b2 (bjam)
if(MSVC)
  set(b2Bootstrap "bootstrap.bat")
else()
  set(b2Bootstrap "./bootstrap.sh")
endif()
ExternalProject_Add_Step(
    boost
    make_b2
    COMMAND ${b2Bootstrap}
    COMMENT "Building b2..."
    DEPENDEES download
    DEPENDERS configure
    WORKING_DIRECTORY <SOURCE_DIR>
    )

# Expose required variables (BoostSourceDir and library paths) to parent scope
ExternalProject_Get_Property(boost source_dir)
set(BoostSourceDir ${source_dir})
set(BoostSourceDir ${BoostSourceDir} PARENT_SCOPE)
include_directories(SYSTEM "${BoostSourceDir}")
foreach(Component ${BoostComponents})
  underscores_to_camel_case(${Component} CamelCaseComponent)
  if(MSVC)
    set(Boost${CamelCaseComponent}Libs debug ${BoostSourceDir}/stage/lib/libboost_${Component}-mt-gd.lib optimized ${BoostSourceDir}/stage/lib/libboost_${Component}-mt.lib)
  else()
    set(Boost${CamelCaseComponent}Libs ${BoostSourceDir}/stage/lib/libboost_${Component}-mt.a)
  endif()
  set(Boost${CamelCaseComponent}Libs ${Boost${CamelCaseComponent}Libs} PARENT_SCOPE)
endforeach()
set_target_properties(boost PROPERTIES LABELS Boost FOLDER "Third Party/Boost")



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
