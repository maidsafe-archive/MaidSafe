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

################################################################################
# Pre-requirement: Use MS-Super Project                                        #
# Script Required Arguments: Build-Config (Debug / Release), CMAKE_GENERATOR   #
# Example ctest -S maidsafe_run_ci.cmake,Debug,"Visual Studio 11 Win64"        #
################################################################################
set(ScriptVersion 4)
set(CTEST_SOURCE_DIRECTORY ..)
set(CTEST_BINARY_DIRECTORY .)
include("${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake")


################################################################################
# Helper Functions                                                             #
################################################################################
function(script_argument_at Index Result)
  string(REPLACE "," ";" ScriptList ${CTEST_SCRIPT_ARG})
  list(LENGTH ScriptList ArgumentListLength)
  if(${Index} LESS ${ArgumentListLength} AND ${Index} GREATER -1)
    list(GET ScriptList ${Index} ReturnVal)
    set(${Result} ${ReturnVal} PARENT_SCOPE)
  endif()
endfunction()


################################################################################
# Variable(s) determined after running cmake                                   #
################################################################################
unset(DashboardModel CACHE)

script_argument_at(0 CTEST_CONFIGURATION_TYPE)
if(NOT "${CTEST_CONFIGURATION_TYPE}" MATCHES "^(Debug|Release)")
  message(FATAL_ERROR "Allowed arguments are Debug, Release \n eg. ctest -S ${CTEST_SCRIPT_NAME},Debug,\"Visual Studio 11 Win64\",Continuous")
endif()

script_argument_at(1 CTEST_CMAKE_GENERATOR)

# Select the model (Continuous, Experimental, Nightly, or Weekly).
script_argument_at(2 DashboardModel)
if(NOT ${DashboardModel} MATCHES Continuous
   AND NOT ${DashboardModel} MATCHES Experimental
   AND NOT ${DashboardModel} MATCHES Nightly
   AND NOT ${DashboardModel} MATCHES Weekly)
  set(DashboardModel Continuous)
endif()

find_program(HostnameCommand NAMES hostname)
execute_process(COMMAND ${HostnameCommand} OUTPUT_VARIABLE Hostname OUTPUT_STRIP_TRAILING_WHITESPACE)
set(CTEST_SITE "${Hostname}")

if(WIN32)
  if(CTEST_CMAKE_GENERATOR MATCHES "64$")
    set(MachineBuildType "x64")
  else()
    set(MachineBuildType "x86")
  endif()
endif()

message("Dashboard Model Selected:      ${DashboardModel}")
message("Build Configuration Selected:  ${CTEST_CONFIGURATION_TYPE}")
message("Hostname:                      ${Hostname}")
message("Make Generator:                ${CTEST_CMAKE_GENERATOR}")
if(WIN32)
  message("Build Type:                    ${MachineBuildType}")
endif()
message("================================================================================")


################################################################################
# Find programs & commands                                                     #
################################################################################
find_program(CTEST_CMAKE_COMMAND NAMES cmake)
if(NOT CTEST_CMAKE_COMMAND)
  SCRIPT_ARGUMENT_AT(2 CTEST_CMAKE_COMMAND)
  if(NOT CTEST_CMAKE_COMMAND)
    message(FATAL_ERROR "Couldn't find CMake executable. Provide Cmake Path as 3rd Script Argument")
  endif()
endif()

set(CMAKE_MODULE_PATH ${CTEST_SCRIPT_DIRECTORY})
include(maidsafe_find_git)
set(CTEST_UPDATE_COMMAND "${Git_EXECUTABLE}")

if(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "Make")
  set(CTEST_USE_LAUNCHERS 0)
elseif(NOT DEFINED CTEST_USE_LAUNCHERS)
  set(CTEST_USE_LAUNCHERS 1)
endif()


################################################################################
# Check current branch & update super project                                  #
################################################################################
message("Updating super project on 'next'")
#Update Super Project on next branch
execute_process(COMMAND ${Git_EXECUTABLE} checkout next
                WORKING_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/..
                RESULT_VARIABLE ResultVar
                OUTPUT_VARIABLE OutputVar
                ERROR_QUIET)
if(NOT ${ResultVar} EQUAL 0)
  message(FATAL_ERROR "Failed to switch to branch next in super project:\n\n${OutputVar}")
endif()
execute_process(COMMAND ${Git_EXECUTABLE} pull
                WORKING_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/..
                RESULT_VARIABLE ResultVar
                OUTPUT_VARIABLE OutputVar
                ERROR_QUIET)
if(NOT ${ResultVar} EQUAL 0)
  message(FATAL_ERROR "Failed to pull updates in super project:\n\n${OutputVar}")
endif()
message("================================================================================")


################################################################################
# Check current branch & update sub-projects                                   #
################################################################################
foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  if(${SubProject} STREQUAL "Vault")
    set(${SubProject}SourceDirectory ${CTEST_SCRIPT_DIRECTORY}/../src/pd)
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/pd)
  else()
    set(${SubProject}SourceDirectory ${CTEST_SCRIPT_DIRECTORY}/../src/${SubProject})
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/${SubProject})
  endif()
  if(NOT EXISTS ${${SubProject}SourceDirectory} OR NOT EXISTS ${${SubProject}BinaryDirectory})
    message(FATAL_ERROR "Unable to find ${SubProject} source (${${SubProject}SourceDirectory}) or binary (${${SubProject}BinaryDirectory}) directory.")
  endif()
endforeach()
message("All sub-project source & binary directories verified")

foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  execute_process(COMMAND ${Git_EXECUTABLE} status --short --branch
                  WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                  OUTPUT_VARIABLE OutputVar
                  RESULT_VARIABLE ResultVar)
  if(${ResultVar} EQUAL 0)
    string(REGEX REPLACE "\n.*" "" CurrentBranch ${OutputVar})
    string(REGEX REPLACE "[.][.][.].*" "" CurrentBranch ${CurrentBranch})
    string(REGEX REPLACE "## " "" CurrentBranch ${CurrentBranch})
    set(${SubProject}CurrentBranch ${CurrentBranch})
  else()
    set(${SubProject}CurrentBranch "unknown")
  endif()

  if(NOT ${SubProject}CurrentBranch STREQUAL "next")
    execute_process(COMMAND ${Git_EXECUTABLE} checkout next
                    WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar)
    if(NOT ${ResultVar} EQUAL 0)
      message(FATAL_ERROR "  Unable to switch branch to next:\n\n${OutputVar}")
    endif()
  endif()

  execute_process(COMMAND ${Git_EXECUTABLE} rev-parse --verify HEAD
                  WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                  RESULT_VARIABLE ResultVar
                  OUTPUT_VARIABLE OutputVar)
  if(${ResultVar} EQUAL 0)
    set(${SubProject}CurrentCommit ${OutputVar})
    string(REPLACE "\n" "" ${SubProject}CurrentCommit "${${SubProject}CurrentCommit}")
    execute_process(COMMAND ${Git_EXECUTABLE} log -1 --format="Hash: %H%nAuthor: %an%nCommitter: %cn%nCommit Message: %s"
                    WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar)
    if(${ResultVar} EQUAL 0)
      string(REPLACE "\"" "" OutputVar ${OutputVar})
      set(${SubProject}CurrentCommitLogMsg ${OutputVar})
    else()
      set(${SubProject}CurrentCommitLogMsg "N/A")
    endif()
  else()
    set(${SubProject}CurrentCommit "unknown")
  endif()

  execute_process(COMMAND ${Git_EXECUTABLE} pull
                  WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                  RESULT_VARIABLE ResultVar
                  OUTPUT_VARIABLE OutputVar
                  ERROR_QUIET)
  if(${ResultVar} EQUAL 0)
    execute_process(COMMAND ${Git_EXECUTABLE} rev-parse --verify HEAD
                    WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar)
    if(${ResultVar} EQUAL 0)
      set(${SubProject}NewCommit ${OutputVar})
      string(REPLACE "\n" "" ${SubProject}NewCommit "${${SubProject}NewCommit}")
      execute_process(COMMAND ${Git_EXECUTABLE} log -1 --format="Hash: %H%nAuthor: %an%nCommitter: %cn%nCommit Message: %s"
                      WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                      RESULT_VARIABLE ResultVar
                      OUTPUT_VARIABLE OutputVar)
      if(${ResultVar} EQUAL 0)
        string(REPLACE "\"" "" OutputVar ${OutputVar})
        set(${SubProject}NewCommitLogMsg ${OutputVar})
      else()
        set(${SubProject}NewCommitLogMsg "N/A")
      endif()
#       if(NOT ${SubProject}NewCommit STREQUAL ${SubProject}CurrentCommit)
#         SET_TEST_NEEDED_FOR_DEPENDANTS(${SubProject})
#       endif()
    else()
      message(FATAL_ERROR "Unable to retrieve commit ID. Aborting Process:\n\n${OutputVar}")
    endif()
  else()
    message(FATAL_ERROR "Unable to pull from next. Aborting Process:\n\n${OutputVar}")
  endif()
  string(SUBSTRING ${${SubProject}CurrentCommit} 0 7 OldCommit)
  string(SUBSTRING ${${SubProject}NewCommit} 0 7 NewCommit)
  message("-- ${SubProject} \t on '${CurrentBranch}'.  Old commit: ${OldCommit}  New commit: ${NewCommit}")
endforeach()
message("================================================================================")


################################################################################
# Build Project & Run tests if needed                                          #
################################################################################
ctest_start(${DashboardModel} TRACK ${DashboardModel})

foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
	message("Running CTest build & test for ${SubProject}")
# 	unset(CTEST_SOURCE_DIRECTORY CACHE)
# 	unset(CTEST_BINARY_DIRECTORY CACHE)
# 	unset(CTEST_BUILD_NAME CACHE)
# 	unset(CTEST_NOTES_FILES CACHE)
  set_property(GLOBAL PROPERTY SubProject ${SubProject})
  set_property(GLOBAL PROPERTY Label ${SubProject})

	set(CTEST_BUILD_NAME "${DashboardModel} Build - ${CTEST_CONFIGURATION_TYPE} ${MachineBuildType}, Script Version - ${ScriptVersion}")
# 	set(CTEST_SOURCE_DIRECTORY ${${SubProject}SourceDirectory})
# 	set(CTEST_BINARY_DIRECTORY ${${SubProject}BinaryDirectory})
  set(CTEST_BUILD_TARGET "All${SubProject}")

	ctest_read_custom_files(${CTEST_BINARY_DIRECTORY})
	if(WIN32)
	  ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}" SOURCE "${CTEST_SCRIPT_DIRECTORY}/.." OPTIONS "-DCI_BUILD_ENVIRONMENT=TRUE -DCLEAN_TEMP=ALWAYS")
	else()
	  ctest_configure(BUILD "${CTEST_BINARY_DIRECTORY}" SOURCE "${CTEST_SCRIPT_DIRECTORY}/..")
	endif()
	ctest_build(BUILD "${CTEST_BINARY_DIRECTORY}" RETURN_VALUE BuildResult)

  # runs only tests that have a LABELS property matching "${SubProject}"
  ctest_test(BUILD "${CTEST_BINARY_DIRECTORY}" INCLUDE_LABEL "${SubProject}")

 	unset(TagId CACHE)
	find_file(TagFile NAMES TAG PATHS ${CTEST_BINARY_DIRECTORY}/Testing NO_DEFAULT_PATH)
	file(READ ${TagFile} TagFileContents)
	string(REPLACE "\n" "" TagFileContents "${TagFileContents}")
	string(REGEX REPLACE "[A-Za-z]+" "" TagId "${TagFileContents}")

	# Modify XML files on x64 Windows
	if(WIN32)
	  if(${MachineBuildType} STREQUAL "x64")
	    set(XML_Files "Configure.xml" "Build.xml" "Test.xml")
	    foreach(XML_File ${XML_Files})
	      unset(ModFile CACHE)
	      unset(ModFileContents CACHE)
	      find_file(ModFile NAMES ${XML_File} PATHS ${CTEST_BINARY_DIRECTORY}/Testing/${TagId} NO_DEFAULT_PATH)
	      file(READ ${ModFile} ModFileContents)
	      string(REPLACE "OSPlatform=\"x86\"" "OSPlatform=\"${MachineBuildType}\"" ModFileContents "${ModFileContents}")
	      file(WRITE ${ModFile} "${ModFileContents}")
	    endforeach()
	  endif()
	endif()

	# Write git update details to file
	unset(ChangedFiles CACHE)
	execute_process(COMMAND ${Git_EXECUTABLE} diff --stat ${${SubProject}CurrentCommit} ${${SubProject}NewCommit}
            	    WORKING_DIRECTORY ${${SubProject}SourceDirectory}
            	    RESULT_VARIABLE ResultVar
            	    OUTPUT_VARIABLE ChangedFiles)
	if(ResultVar EQUAL 0)
	  file(WRITE "${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/GitDetails.txt" "Old Commit Details: \n${${SubProject}CurrentCommitLogMsg}
	      \nNew Commit: \n${${SubProject}NewCommitLogMsg}
	      \nFiles Changed:
	      \n${CHANGED_FILES}")
	  set(CTEST_NOTES_FILES "${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/GitDetails.txt")
	endif()
  ctest_submit(RETURN_VALUE ReturnVar)

	if(${ReturnVar} EQUAL 0)
	  message("CI Build Submitted - ${SubProject}")
	else()
	  message("CI Build Failed to Submit - ${SubProject}")
	endif()
	if(NOT ${BuildResult} EQUAL 0)
	  message("${SubProject} failed during build, exiting script")
	  break()
	endif()
endforeach()
