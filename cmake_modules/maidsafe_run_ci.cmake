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
#  Script Purpose: Run CI on All Submodules of MaidSafe/MaidSafe               #
#==============================================================================#

###############################################################################
# Pre-requirement: Use MS-Super Project (master branch)                       #
# Script Required Arguments: Build-Config (Debug / Release), CMAKE_GENERATOR  #
# Example ctest -S maidsafe_run_ci.cmake,Debug,"Visual Studio 11 Win64"       #
###############################################################################

set(SCRIPT_VERSION 1)
file(READ "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}" INSTALLED_VERSION_INFO)
STRING(REGEX MATCH "SCRIPT_VERSION [0-9]+" INSTALLED_VERSION ${INSTALLED_VERSION_INFO})
STRING(REGEX REPLACE "SCRIPT_VERSION " "" INSTALLED_VERSION ${INSTALLED_VERSION})
if(NOT ${SCRIPT_VERSION} EQUAL ${INSTALLED_VERSION})
   message("New installed Version: ${INSTALLED_VERSION}")
   message("Current running version: ${SCRIPT_VERSION}")
   message(FATAL_ERROR "This script is updated. Start the script again !!!")
endif()

###############################################################################
# List of Modules and Paths                                                   #
###############################################################################

set(ALL_MODULE_LIST
    "MAIDSAFE_COMMON"
    "MAIDSAFE_PRIVATE"
    "MAIDSAFE_RUDP"
    "MAIDSAFE_ROUTING"
    "MAIDSAFE_ENCRYPT"
    "MAIDSAFE_DRIVE"
    "MAIDSAFE_PD"
    "MAIDSAFE_PASSPORT"
    "LIFESTUFF"
    "LIFESTUFF_GUI"
    )
message("================================================================================")

###############################################################################
# Module configurations                                                       #
###############################################################################

#sub-module folder-name
set(MAIDSAFE_COMMON "common")
set(MAIDSAFE_PRIVATE "private")
set(MAIDSAFE_RUDP "rudp")
set(MAIDSAFE_ROUTING "routing")
set(MAIDSAFE_ENCRYPT "encrypt")
set(MAIDSAFE_DRIVE "drive")
set(MAIDSAFE_PD "pd")
set(MAIDSAFE_PASSPORT "passport")
set(LIFESTUFF "lifestuff")
set(LIFESTUFF_GUI "lifestuff-gui")

#List of dependants for each sub-module
set(MAIDSAFE_COMMON_DEPENDANTS
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_RUDP
    MAIDSAFE_ROUTING
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DRIVE
    MAIDSAFE_PD
    MAIDSAFE_PASSPORT
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_PRIVATE_DEPENDANTS
    MAIDSAFE_PRIVATE
    MAIDSAFE_ROUTING
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DRIVE
    MAIDSAFE_PD
    MAIDSAFE_PASSPORT
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_RUDP_DEPENDANTS
    MAIDSAFE_RUDP
    MAIDSAFE_ROUTING
    MAIDSAFE_PD
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_ROUTING_DEPENDANTS
    MAIDSAFE_ROUTING
    MAIDSAFE_PD
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_ENCRYPT_DEPENDANTS
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DRIVE
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_DRIVE_DEPENDANTS
    MAIDSAFE_DRIVE
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_PD_DEPENDANTS
    MAIDSAFE_PD
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_PASSPORT_DEPENDANTS
    MAIDSAFE_PASSPORT
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(LIFESTUFF_DEPENDANTS
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(LIFESTUFF_GUI_DEPENDANTS
    LIFESTUFF_GUI
    )

###############################################################################
# Helper Functions                                                            #
###############################################################################

function(SET_TEST_NEEDED_FOR_DEPENDANTS MODULE_NAME)
  foreach(EACH_MODULE ${${MODULE_NAME}_DEPENDANTS})
    if(NOT ${EACH_MODULE}_NEEDS_TESTED)
      set(${EACH_MODULE}_NEEDS_TESTED 1 PARENT_SCOPE)
      message("    Flagging ${${EACH_MODULE}} to be Tested in this Cycle")
    endif()
  endforeach()
endfunction()

function(SCRIPT_ARGUMENT_AT INDEX RESULT)
  string(REPLACE "," ";" SCRIPT_LIST ${CTEST_SCRIPT_ARG})
  list(LENGTH SCRIPT_LIST ARGUMENT_LIST_LENGTH)
  if(${INDEX} LESS ${ARGUMENT_LIST_LENGTH} AND ${INDEX} GREATER -1)
    list(GET SCRIPT_LIST ${INDEX} RETURN_VAL)
    set(${RESULT} ${RETURN_VAL} PARENT_SCOPE)
  endif()
endfunction()

###############################################################################
# Variable(s) determined after running cmake                                  #
###############################################################################
unset(RESULT_GENERATOR CACHE)
unset(RESULT_CONFIG CACHE)
unset(RESULT_CMAKE CACHE)

SCRIPT_ARGUMENT_AT(1 RESULT_GENERATOR)
set(CTEST_CMAKE_GENERATOR ${RESULT_GENERATOR})

SCRIPT_ARGUMENT_AT(0 RESULT_CONFIG)
if(NOT "${RESULT_CONFIG}" MATCHES "^(Debug|Release)")
  message(FATAL_ERROR "Allowed arguments are Debug, Release \n eg. ctest -S ${CTEST_SCRIPT_NAME},Debug,\"Visual Studio 11 Win64\"")
endif()
if(${RESULT_CONFIG} STREQUAL "Debug")
  set(CTEST_CONFIGURATION_TYPE "Debug")
else()
  set(CTEST_CONFIGURATION_TYPE "Release")
endif()
set(DASHBOARD_MODEL "Continuous")
set(MEMORY_CHECK_NEEDED FALSE)

find_program(HOSTNAME_CMD NAMES hostname)
EXEC_PROGRAM(${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE HOSTNAME)
set(CTEST_SITE "${HOSTNAME}")

if(WIN32)
  if(CTEST_CMAKE_GENERATOR MATCHES "64$")
    set(MACHINE_BUILD_TYPE "x64")
  else()
    set(MACHINE_BUILD_TYPE "x86")
  endif()
endif()

message("Dashboard Model Selected:      ${DASHBOARD_MODEL}")
message("Build Configuration Selected:  ${CTEST_CONFIGURATION_TYPE}")
message("Hostname:                      ${HOSTNAME}")
message("Make Generator:                ${CTEST_CMAKE_GENERATOR}")
if(WIN32)
  message("Build Type:                    ${MACHINE_BUILD_TYPE}")
endif()
message("================================================================================")

###############################################################################
# Finding Programs & Commands                                                 #
###############################################################################
find_program(CTEST_CMAKE_COMMAND NAMES cmake)
if(NOT CTEST_CMAKE_COMMAND)
  SCRIPT_ARGUMENT_AT(2 RESULT_CMAKE)
  if(RESULT_CMAKE)
    set(CTEST_CMAKE_COMMAND ${RESULT_CMAKE})
  else()
    message(FATAL_ERROR "Couldn't find CMake executable. Provide Cmake Path as 3rd Script Argument")
  endif()
endif()
message(STATUS "Found CMake")

set(CMAKE_MODULE_PATH ${CTEST_SCRIPT_DIRECTORY})
include(maidsafe_find_git)
set(CTEST_UPDATE_COMMAND "${Git_EXECUTABLE}")

if(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "Make")
  set(CTEST_USE_LAUNCHERS 0)
elseif(NOT DEFINED CTEST_USE_LAUNCHERS)
  set(CTEST_USE_LAUNCHERS 1)
endif()

###############################################################################
# Check Current branch & Update Super Project                                 #
###############################################################################
message("Updating Super Project on master branch")
#Update Super Project on master branch
execute_process(WORKING_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/..
    COMMAND ${Git_EXECUTABLE} checkout master
    RESULT_VARIABLE ret_var
    OUTPUT_VARIABLE out_var
    )
if(NOT ${ret_var} EQUAL 0)
  message(FATAL_ERROR "Failed to Switch to branch master in Super Project.")
endif()
execute_process(WORKING_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/..
    COMMAND ${Git_EXECUTABLE} pull
    RESULT_VARIABLE ret_var
    OUTPUT_VARIABLE out_var
    )
if(NOT ${ret_var} EQUAL 0)
  message(FATAL_ERROR "Failed to Pull Updates in Super Project.")
endif()
message("================================================================================")

###############################################################################
# Check Current branch & Update Sub Projects                                  #
###############################################################################

foreach(EACH_MODULE ${ALL_MODULE_LIST})
  set(${EACH_MODULE}_SOURCE_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/../src/${${EACH_MODULE}})
  set(${EACH_MODULE}_BINARY_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/src/${${EACH_MODULE}})

  if(NOT EXISTS ${${EACH_MODULE}_SOURCE_DIRECTORY})
    message(FATAL_ERROR "Unable to find ${EACH_MODULE} source directory: ${${EACH_MODULE}_SOURCE_DIRECTORY}")
  endif()

  if(NOT EXISTS ${${EACH_MODULE}_BINARY_DIRECTORY})
    message(FATAL_ERROR "Unable to find ${EACH_MODULE} binary directory")
  endif()
endforeach()
message("All Sub-Project Sources & Binary Directories Verified")
message("Verifying Current branches of Sub-Projects:")
foreach(EACH_MODULE ${ALL_MODULE_LIST})
  execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
      COMMAND ${Git_EXECUTABLE} status --short --branch
      RESULT_VARIABLE ret_var
      OUTPUT_VARIABLE out_var
      )
  if(${ret_var} EQUAL 0)
    string(REGEX REPLACE "\n.*" "" CURRENT_BRANCH ${out_var})
    string(REGEX REPLACE "[.][.][.].*" "" CURRENT_BRANCH ${CURRENT_BRANCH})
    string(REGEX REPLACE "## " "" CURRENT_BRANCH ${CURRENT_BRANCH})
    message("  ${${EACH_MODULE}} : ${CURRENT_BRANCH}")
    set(${EACH_MODULE}_CURRENT_BRANCH ${CURRENT_BRANCH})
  else()
    set(${EACH_MODULE}_CURRENT_BRANCH "unknown")
  endif()

  if(NOT ${EACH_MODULE}_CURRENT_BRANCH STREQUAL "next")
    execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
        COMMAND ${Git_EXECUTABLE} checkout next
        RESULT_VARIABLE ret_var
        OUTPUT_VARIABLE out_var
        )
    if(NOT ${ret_var} EQUAL 0)
      message(FATAL_ERROR "  Unable to switch branch to next")
    endif()
  endif()

  execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
      COMMAND ${Git_EXECUTABLE} rev-parse --verify HEAD
      RESULT_VARIABLE ret_var
      OUTPUT_VARIABLE out_var
      )
  if(${ret_var} EQUAL 0)
    set(${EACH_MODULE}_CURRENT_COMMIT ${out_var})
    string(REPLACE "\n" "" ${EACH_MODULE}_CURRENT_COMMIT "${${EACH_MODULE}_CURRENT_COMMIT}")
    execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
        COMMAND ${Git_EXECUTABLE} log -1 --format="Hash: %H%nAuthor: %an%nCommitter: %cn%nCommit Message: %s"
        RESULT_VARIABLE ret_var
        OUTPUT_VARIABLE out_var
        )
    if(${ret_var} EQUAL 0)
      string(REPLACE "\"" "" out_var ${out_var})
      set(${EACH_MODULE}_CURRENT_COMMIT_LOG_MSG ${out_var})
    else()
      set(${EACH_MODULE}_CURRENT_COMMIT_LOG_MSG "N/A")
    endif()
  else()
    set(${EACH_MODULE}_CURRENT_COMMIT "unknown")
  endif()

  execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
      COMMAND ${Git_EXECUTABLE} pull
      RESULT_VARIABLE ret_var
      OUTPUT_VARIABLE out_var
      )
  if(${ret_var} EQUAL 0)
    execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
      COMMAND ${Git_EXECUTABLE} rev-parse --verify HEAD
      RESULT_VARIABLE ret_var
      OUTPUT_VARIABLE out_var
      )
    if(${ret_var} EQUAL 0)
      set(${EACH_MODULE}_NEW_COMMIT ${out_var})
      string(REPLACE "\n" "" ${EACH_MODULE}_NEW_COMMIT "${${EACH_MODULE}_NEW_COMMIT}")
      execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
          COMMAND ${Git_EXECUTABLE} log -1 --format="Hash: %H%nAuthor: %an%nCommitter: %cn%nCommit Message: %s"
          RESULT_VARIABLE ret_var
          OUTPUT_VARIABLE out_var
          )
      if(${ret_var} EQUAL 0)
        string(REPLACE "\"" "" out_var ${out_var})
        set(${EACH_MODULE}_NEW_COMMIT_LOG_MSG ${out_var})
      else()
        set(${EACH_MODULE}_NEW_COMMIT_LOG_MSG "N/A")
      endif()
      if(NOT ${EACH_MODULE}_NEW_COMMIT STREQUAL ${EACH_MODULE}_CURRENT_COMMIT)
        SET_TEST_NEEDED_FOR_DEPENDANTS(${EACH_MODULE})
      endif()
    else()
      message(FATAL_ERROR "Unable to retrieve commit id. Aborting Process")
    endif()
  else()
    message(FATAL_ERROR "Unable to pull from next. Aborting Process")
  endif()
endforeach()
message("================================================================================")

###############################################################################
# Build Project & Run tests if needed                                         #
###############################################################################

foreach(EACH_MODULE ${ALL_MODULE_LIST})
  if(${EACH_MODULE}_NEEDS_TESTED)
    message("Running CTest Build & Test for - ${EACH_MODULE}")
    unset(MODULE_SOURCE_DIRECTORY CACHE)
    unset(MODULE_BINARY_DIRECTORY CACHE)
    unset(CTEST_SOURCE_DIRECTORY CACHE)
    unset(CTEST_BINARY_DIRECTORY CACHE)
    unset(CTEST_BUILD_NAME CACHE)
    unset(CTEST_CUSTOM_COVERAGE_EXCLUDE)
    unset(CTEST_CUSTOM_MEMCHECK_IGNORE)
    unset(CTEST_NOTES_FILES CACHE)

    set(CTEST_BUILD_NAME "CI Build - ${CTEST_CONFIGURATION_TYPE} ${MACHINE_BUILD_TYPE} , Script Version - ${SCRIPT_VERSION}")
    set(MODULE_SOURCE_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY})
    set(MODULE_BINARY_DIRECTORY ${${EACH_MODULE}_BINARY_DIRECTORY})
    set(CTEST_SOURCE_DIRECTORY ${MODULE_SOURCE_DIRECTORY})
    set(CTEST_BINARY_DIRECTORY ${MODULE_BINARY_DIRECTORY})

    CTEST_START(${DASHBOARD_MODEL} TRACK ${DASHBOARD_MODEL})
    CTEST_READ_CUSTOM_FILES(${CTEST_BINARY_DIRECTORY})
    if (WIN32)
      CTEST_CONFIGURE(BUILD "${CMAKE_CURRENT_BINARY_DIR}" SOURCE "${CTEST_SCRIPT_DIRECTORY}/.." OPTIONS "-DCI_BUILD_ENVIRONMENT=TRUE -DCLEAN_TEMP=ALWAYS")
    else()
      CTEST_CONFIGURE(BUILD "${CMAKE_CURRENT_BINARY_DIR}" SOURCE "${CTEST_SCRIPT_DIRECTORY}/..")
    endif()
    CTEST_BUILD(RETURN_VALUE build_result)
    CTEST_TEST()

    unset(TagId CACHE)
    find_file(TagFile NAMES TAG PATHS ${CTEST_BINARY_DIRECTORY}/Testing NO_DEFAULT_PATH)
    file(READ ${TagFile} TagFileContents)
    string(REPLACE "\n" "" TagFileContents "${TagFileContents}")
    string(REGEX REPLACE "[A-Za-z]+" "" TagId "${TagFileContents}")

    #Modify XML Files on Windows for Machine Build-Type x64
    if(WIN32)
      if(${MACHINE_BUILD_TYPE} STREQUAL "x64")
        set(XML_FILES
                "Configure.xml"
                "Build.xml"
                "Test.xml"
                )
        foreach(XML_FILE ${XML_FILES})
          unset(ModFile CACHE)
          unset(ModFileContents CACHE)
          find_file(ModFile NAMES ${XML_FILE} PATHS ${CTEST_BINARY_DIRECTORY}/Testing/${TagId} NO_DEFAULT_PATH)
          file(READ ${ModFile} ModFileContents)
          string(REPLACE "OSPlatform=\"x86\"" "OSPlatform=\"${MACHINE_BUILD_TYPE}\"" ModFileContents "${ModFileContents}")
          file(WRITE ${ModFile} "${ModFileContents}")
        endforeach()
      endif()
    endif()

    # Write Git-Update Details To File
    unset(CHANGED_FILES CACHE)
    execute_process(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
        COMMAND ${Git_EXECUTABLE} diff --stat ${${EACH_MODULE}_CURRENT_COMMIT} ${${EACH_MODULE}_NEW_COMMIT}
        RESULT_VARIABLE ret_var
        OUTPUT_VARIABLE CHANGED_FILES
        )
    if(ret_var EQUAL 0)
      file(WRITE "${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/GitDetails.txt" "Old Commit Details: \n${${EACH_MODULE}_CURRENT_COMMIT_LOG_MSG}
          \nNew Commit: \n${${EACH_MODULE}_NEW_COMMIT_LOG_MSG}
          \nFiles Changed:
          \n${CHANGED_FILES}")
      set(CTEST_NOTES_FILES "${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/GitDetails.txt")
      CTEST_SUBMIT(PARTS Configure Build Test Notes RETURN_VALUE result)
    else()
      CTEST_SUBMIT(PARTS Configure Build Test RETURN_VALUE result)
    endif()
    if(${result} EQUAL 0)
      message("CI Build Submitted - ${EACH_MODULE}")
    else()
      message("CI Build Failed to Submit - ${EACH_MODULE}")
    endif()
    if(NOT ${build_result} EQUAL 0)
      message("${EACH_MODULE} failed during build, exiting script")
      break()
    endif()
  endif()
endforeach()
