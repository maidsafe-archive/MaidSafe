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
# Example ctest -S CI_Continuous_Release.cmake                                 #
################################################################################
set(ScriptVersion 4)
include("${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake")

# Avoid non-ascii characters in tool output.
set(ENV{LC_ALL} C)

if(WIN32)
  if(CTEST_CMAKE_GENERATOR MATCHES "64$")
    set(MachineBuildType "x64")
  else()
    set(MachineBuildType "x86")
  endif()
endif()

set(CTEST_BUILD_NAME "${DashboardModel} Build - ${CTEST_CONFIGURATION_TYPE} ${MachineBuildType}, Script Version - ${ScriptVersion}")

if(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "[Mm]ake")
  set(CTEST_USE_LAUNCHERS 0)
elseif(NOT DEFINED CTEST_USE_LAUNCHERS)
  set(CTEST_USE_LAUNCHERS 1)
endif()

message("Dashboard Model Selected:      ${DashboardModel}")
message("Build Configuration Selected:  ${CTEST_CONFIGURATION_TYPE}")
message("Hostname:                      ${CTEST_SITE}")
message("Make Generator:                ${CTEST_CMAKE_GENERATOR}")
if(WIN32)
  message("Build Type:                    ${MachineBuildType}")
endif()
message("================================================================================")


################################################################################
# Check current branch & update super project                                  #
################################################################################
message("Updating super project on 'next'")
#Update Super Project on next branch
execute_process(COMMAND ${CTEST_GIT_COMMAND} checkout next
                WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
                RESULT_VARIABLE ResultVar
                OUTPUT_VARIABLE OutputVar
                ERROR_QUIET)
if(NOT ${ResultVar} EQUAL 0)
  message(FATAL_ERROR "Failed to switch to branch next in super project:\n\n${OutputVar}")
endif()
execute_process(COMMAND ${CTEST_GIT_COMMAND} pull
                WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
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
    set(${SubProject}SourceDirectory ${CTEST_SOURCE_DIRECTORY}/src/pd)
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/pd)
  else()
    set(${SubProject}SourceDirectory ${CTEST_SOURCE_DIRECTORY}/src/${SubProject})
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/${SubProject})
  endif()
  if(NOT EXISTS ${${SubProject}SourceDirectory} OR NOT EXISTS ${${SubProject}BinaryDirectory})
    message(FATAL_ERROR "Unable to find ${SubProject} source (${${SubProject}SourceDirectory}) or binary (${${SubProject}BinaryDirectory}) directory.")
  endif()
endforeach()
message("All sub-project source & binary directories verified")

foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  execute_process(COMMAND ${CTEST_GIT_COMMAND} status --short --branch
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
    execute_process(COMMAND ${CTEST_GIT_COMMAND} checkout next
                    WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar)
    if(NOT ${ResultVar} EQUAL 0)
      message(FATAL_ERROR "  Unable to switch branch to next:\n\n${OutputVar}")
    endif()
  endif()

  execute_process(COMMAND ${CTEST_GIT_COMMAND} rev-parse --verify HEAD
                  WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                  RESULT_VARIABLE ResultVar
                  OUTPUT_VARIABLE OutputVar)
  if(${ResultVar} EQUAL 0)
    set(${SubProject}CurrentCommit ${OutputVar})
    string(REPLACE "\n" "" ${SubProject}CurrentCommit "${${SubProject}CurrentCommit}")
    execute_process(COMMAND ${CTEST_GIT_COMMAND} log -1 --format="Hash: %H%nAuthor: %an%nCommitter: %cn%nCommit Message: %s"
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

  execute_process(COMMAND ${CTEST_GIT_COMMAND} pull
                  WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                  RESULT_VARIABLE ResultVar
                  OUTPUT_VARIABLE OutputVar
                  ERROR_QUIET)
  if(${ResultVar} EQUAL 0)
    execute_process(COMMAND ${CTEST_GIT_COMMAND} rev-parse --verify HEAD
                    WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar)
    if(${ResultVar} EQUAL 0)
      set(${SubProject}NewCommit ${OutputVar})
      string(REPLACE "\n" "" ${SubProject}NewCommit "${${SubProject}NewCommit}")
      execute_process(COMMAND ${CTEST_GIT_COMMAND} log -1 --format="Hash: %H%nAuthor: %an%nCommitter: %cn%nCommit Message: %s"
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
ctest_submit(FILES "${CTEST_SOURCE_DIRECTORY}/Project.xml")

foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  message("Running CTest build & test for ${SubProject}")
  set_property(GLOBAL PROPERTY SubProject ${SubProject})
  set_property(GLOBAL PROPERTY Label ${SubProject})
  set(CTEST_BUILD_TARGET "All${SubProject}")

  ctest_configure(OPTIONS "${ExtraConfigureArgs}")
  ctest_read_custom_files()
	ctest_build(RETURN_VALUE BuildResult)

  # runs only tests that have a LABELS property matching "${SubProject}"
  ctest_test(INCLUDE_LABEL "${SubProject}")

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
	execute_process(COMMAND ${CTEST_GIT_COMMAND} diff --stat ${${SubProject}CurrentCommit} ${${SubProject}NewCommit}
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
