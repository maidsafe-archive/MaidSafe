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
set(ScriptVersion 6)
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
set(CTEST_UPDATE_COMMAND ${CTEST_GIT_COMMAND})

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
# Helper functions                                                             #
################################################################################
function(checkout_to_branch SourceDir Branch)
  execute_process(COMMAND ${CTEST_GIT_COMMAND} checkout ${Branch}
                  WORKING_DIRECTORY ${SourceDir}
                  RESULT_VARIABLE ResultVar
                  OUTPUT_VARIABLE OutputVar
                  ERROR_VARIABLE ErrorVar)
  if(NOT ${ResultVar} EQUAL 0)
    message(FATAL_ERROR "Failed to switch to '${Branch}' in ${SourceDir}:\n\n${ErrorVar}")
  endif()
endfunction()

function(get_git_log SourceDir CommitHash CommitMessage)
  execute_process(COMMAND ${CTEST_GIT_COMMAND} rev-parse --verify HEAD
                  WORKING_DIRECTORY ${${SourceDir}}
                  RESULT_VARIABLE ResultVar
                  OUTPUT_VARIABLE ${CommitHash}
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(${ResultVar} EQUAL 0)
    execute_process(COMMAND ${CTEST_GIT_COMMAND} log -1 --format="Hash: %H%nAuthor: %an%nCommitter: %cn%nCommit Message: %s"
                    WORKING_DIRECTORY ${${SourceDir}}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE ${CommitMessage})
    if(${ResultVar} EQUAL 0)
      string(REPLACE "\"" "" ${CommitMessage} ${${CommitMessage}})
    else()
      set(${CommitMessage} "N/A")
    endif()
  else()
    set(${${CommitHash}} "unknown")
  endif()
  set(${CommitHash} ${${CommitHash}} PARENT_SCOPE)
  set(${CommitMessage} ${${CommitMessage}} PARENT_SCOPE)
endfunction()

function(update_super_project)
  if(DashboardModel STREQUAL "Continuous")
    get_git_log(CTEST_SOURCE_DIRECTORY SuperCurrentCommit SuperCurrentCommitLogMsg)
    execute_process(COMMAND ${CTEST_GIT_COMMAND} pull
                    WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar
                    ERROR_VARIABLE ErrorVar)
    if(NOT ${ResultVar} EQUAL 0)
      message(FATAL_ERROR "Failed to pull super project:\n\n${ErrorVar}")
    endif()
    get_git_log(CTEST_SOURCE_DIRECTORY SuperNewCommit SuperNewCommitLogMsg)
    if(NOT ${SuperNewCommit} STREQUAL ${SuperCurrentCommit})
      set(ForceRun ON PARENT_SCOPE)
    endif()
  elseif(DashboardModel STREQUAL "Nightly" OR DashboardModel STREQUAL "Weekly")
    message("Updating super project")
    ctest_update(RETURN_VALUE UpdatedCount)
    if(UpdatedCount LESS 0)
      message(FATAL_ERROR "Failed to update the super project.")
    endif()
  endif()
endfunction()

macro(update_sub_project SubProject)
  if(DashboardModel STREQUAL "Continuous")
    message("Updating ${SubProject}")
    get_git_log(${SubProject}SourceDirectory ${SubProject}CurrentCommit ${SubProject}CurrentCommitLogMsg)
    ctest_update(SOURCE ${${SubProject}SourceDirectory} RETURN_VALUE UpdatedCount)
    if(UpdatedCount LESS 0)
      message(FATAL_ERROR "Failed to update ${SubProject}.")
    elseif(UpdatedCount GREATER 0)
      set(${SubProject}ShouldRun ON)
    else()
                                                                           # Look at project's dependencies
      set(${SubProject}ShouldRun OFF)
    endif()
    get_git_log(${SubProject}SourceDirectory ${SubProject}NewCommit ${SubProject}NewCommitLogMsg)
  else()
    set(${SubProject}ShouldRun ON)
  endif()
endmacro()

function(build_and_run SubProject ForceRun)
  if(NOT ${SubProject}ShouldRun AND NOT ${ForceRun})
    return()
  endif()

  message("Building ${SubProject}")
  ctest_configure(OPTIONS "${ExtraConfigureArgs}")
  ctest_read_custom_files(${CMAKE_CURRENT_BINARY_DIR})
	ctest_build(RETURN_VALUE BuildResult)

  # runs only tests that have a LABELS property matching "${SubProject}"
  message("Testing ${SubProject}")
  ctest_test(INCLUDE_LABEL "${SubProject}")

  if(NOT DashboardModel STREQUAL "Experimental")
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
  	      \n${ChangedFiles}")
  	  set(CTEST_NOTES_FILES "${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/GitDetails.txt")
  	endif()
  endif()
  
  ctest_submit(RETURN_VALUE ReturnVar)

	if(${ReturnVar} EQUAL 0)
	  message("Submitted results for ${SubProject}\n-----------------------------------")
	else()
	  message("Failed to submit results for ${SubProject}\n-------------------------------------------")
	endif()
	if(NOT ${BuildResult} EQUAL 0)
	  message("${SubProject} failed during build, exiting script")
    if(WIN32)
      # TODO(Viv) Check OS Version
      execute_process(COMMAND cmd /c "ci_build_reporter.py win7 ${MachineBuildType} fail ${SubProject} ${${SubProject}NewCommitLogAuthor}"
                      WORKING_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/tools"
                      RESULT_VARIABLE ResultVar
                      OUTPUT_VARIABLE OutputVar)
    else()
      # Need Linux Execute Script Command with argument detections
    endif()
	  break()
  elseif(${SubProject} STREQUAL "LifestuffGui")
    if(WIN32)
      # TODO(Viv) Check OS Version
      execute_process(COMMAND cmd /c "ci_build_reporter.py win7 ${MachineBuildType} ok ${SubProject} ${${SubProject}NewCommitLogAuthor}"
                      WORKING_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/tools"
                      RESULT_VARIABLE ResultVar
                      OUTPUT_VARIABLE OutputVar)                      
    else()
      # Need Linux Execute Script Command with argument detections
    endif()
	endif()
endfunction()


################################################################################
# Set sub-projects' source and binary directories                              #
################################################################################
foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  if(${SubProject} STREQUAL "Vault")
    set(${SubProject}SourceDirectory ${CTEST_SOURCE_DIRECTORY}/src/pd)
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/pd)
  else()
    string(TOLOWER ${SubProject} subproject)
    set(${SubProject}SourceDirectory ${CTEST_SOURCE_DIRECTORY}/src/${subproject})
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/${subproject})
  endif()
  if(NOT EXISTS ${${SubProject}SourceDirectory} OR NOT EXISTS ${${SubProject}BinaryDirectory})
    message(FATAL_ERROR "Unable to find ${SubProject} source (${${SubProject}SourceDirectory}) or binary (${${SubProject}BinaryDirectory}) directory.")
  endif()
endforeach()


################################################################################
# Prepare for tests                                                            #
################################################################################
set(ForceRun OFF)
if(DashboardModel STREQUAL "Experimental")
elseif(DashboardModel STREQUAL "Continuous")
#  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  message("Checking out super project to 'next'")
                                                                                                              checkout_to_branch(${CTEST_SOURCE_DIRECTORY} ci_script)
  execute_process(COMMAND ${CTEST_GIT_COMMAND} pull
                  WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
                  RESULT_VARIABLE ResultVar
                  OUTPUT_VARIABLE OutputVar
                  ERROR_VARIABLE ErrorVar)
  if(NOT ${ResultVar} EQUAL 0)
    message(FATAL_ERROR "Failed to pull super project:\n\n${ErrorVar}")
  endif()
elseif(DashboardModel STREQUAL "Nightly")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  message("Checking out super project to 'next'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} next)
elseif(DashboardModel STREQUAL "Weekly")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  message("Checking out super project to 'next'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} next)
endif()
message("================================================================================")


################################################################################
# Build project & run tests if needed                                          #
################################################################################
while(${CTEST_ELAPSED_TIME} LESS 72000)
  ctest_start(${DashboardModel} TRACK ${DashboardModel})
  update_super_project()
  ctest_submit(FILES "${CTEST_SOURCE_DIRECTORY}/Project.xml")

  foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
    set_property(GLOBAL PROPERTY SubProject ${SubProject})
    set_property(GLOBAL PROPERTY Label ${SubProject})
    set(CTEST_BUILD_TARGET "All${SubProject}")
    update_sub_project(${SubProject})
  endforeach()

  foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
    build_and_run(${SubProject} ${ForceRun})
  endforeach()

  if(NOT DashboardModel STREQUAL "Continuous")
    return()
  endif()
endwhile()