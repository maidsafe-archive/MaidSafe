#==============================================================================#
#                                                                              #
#  Copyright (c) 2012 MaidSafe.net limited                                     #
#                                                                              #
#  The following source code is property of MaidSafe.net limited and is not    #
#  meant for external use.  The use of this code is governed by the license    #
#  file licence.txt found in the root directory of this project and also on    #
#  www.maidsafe.net.                                                           #
#                                                                              #
#  You are not free to copy, amend or otherwise use this source code without   #
#  the explicit written permission of the board of directors of MaidSafe.net.  #
#                                                                              #
#==============================================================================#


cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(maidsafe)

enable_testing()
include(CTest)
set(BUILD_SHARED_LIBS OFF)


set(SubModules
      common
      private
#      transport
      rudp
      routing
#       dht
#       encrypt
#       pki
#       passport
#       pd
#       drive
#       lifestuff
#       lifestuff-gui
      )

# Check the submodules are initialised
foreach(SubModule ${SubModules})
  if(NOT EXISTS "${CMAKE_SOURCE_DIR}/src/${SubModule}/.git")
    set(ErrorMsg "\nThe git submodule for ${SubModule} is not intialised.  ")
    set(ErrorMsg "${ErrorMsg}Please cd to ${CMAKE_SOURCE_DIR} and run:\n    ")
    set(ErrorMsg "${ErrorMsg}git submodule update --init --recursive\n")
    message(FATAL_ERROR ${ErrorMsg})
  endif()
endforeach()


set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
# set(MAIDSAFE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/${COMPILER}/installed" )
# set(MAIDSAFE_INCLUDE_DIR "${MAIDSAFE_INSTALL_PREFIX}/include")


add_subdirectory(src/third_party_libs)


# Build protoc if not already done
set(ProtocBuildDir ${CMAKE_BINARY_DIR}/build_protoc)
find_program(ProtocExe NAMES protoc PATHS ${ProtocBuildDir} PATH_SUFFIXES Release NO_DEFAULT_PATH)
if(NOT ProtocExe)
  message(STATUS "Building protoc...")
  execute_process(COMMAND
                    ${CMAKE_COMMAND} -E make_directory ${ProtocBuildDir}
                    OUTPUT_VARIABLE OutVar
                    RESULT_VARIABLE ResVar)
  execute_process(COMMAND
                    ${CMAKE_COMMAND} ${CMAKE_SOURCE_DIR}/src/third_party_libs/protobuf -G ${CMAKE_GENERATOR} -DCMAKE_BUILD_TYPE=Release
                    WORKING_DIRECTORY ${ProtocBuildDir}
                    OUTPUT_VARIABLE OutVar
                    RESULT_VARIABLE ResVar)
  set(ProtocOutVar "${OutVar}")
  execute_process(COMMAND ${CMAKE_COMMAND} --build . --config Release WORKING_DIRECTORY ${ProtocBuildDir} OUTPUT_VARIABLE OutVar RESULT_VARIABLE ResVar)
  set(ProtocOutVar "${ProtocOutVar}\n\n\n\n*********************\n\n\n\n${OutVar}")
  find_program(ProtocExe NAMES protoc PATHS ${ProtocBuildDir} PATH_SUFFIXES Release NO_DEFAULT_PATH)
  if(NOT ProtocExe)
    message(FATAL_ERROR "${ProtocOutVar}")
  endif()
endif()


# Add each sub-module
foreach(SubModule ${SubModules})
  add_subdirectory(src/${SubModule})
endforeach()

message("${HR}")


# When this target is built, it removes all .gcda files from the build directory and its subdirectories
if(UNIX)
  find_file(CLEAN_COVERAGE clean_coverage.cmake cmake_modules/)
  if(CLEAN_COVERAGE)
    add_custom_target(CleanCoverage COMMAND ${CMAKE_COMMAND} -DSEARCH_DIR=${CMAKE_BINARY_DIR} -P ${CLEAN_COVERAGE})
  endif()
endif()

cleanup_temp_dir()
