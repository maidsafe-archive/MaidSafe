#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2012 MaidSafe.net limited                                                         #
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
#  Helper module used in running CI on all submodules of MaidSafe/MaidSafe                         #
#                                                                                                  #
#==================================================================================================#


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


macro(get_tag)
  file(STRINGS ${CTEST_BINARY_DIRECTORY}/Testing/TAG TagId LIMIT_COUNT 1)
endmacro()


function(get_git_log SourceDir CommitHash CommitMessage Author)
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
      string(REGEX MATCH "Author: ([^\n]+)" FoundVar ${${CommitMessage}})
      set(${Author} ${CMAKE_MATCH_1})
    else()
      set(${CommitMessage} "N/A")
      set(${Author} "N/A")
    endif()
  else()
    set(${${CommitHash}} "unknown")
  endif()
  set(${CommitHash} ${${CommitHash}} PARENT_SCOPE)
  set(${CommitMessage} ${${CommitMessage}} PARENT_SCOPE)
  set(${Author} ${${Author}} PARENT_SCOPE)
endfunction()


function(update_super_project)
  if(DashboardModel STREQUAL "Continuous")
    get_git_log(CTEST_SOURCE_DIRECTORY SuperCurrentCommit SuperCurrentCommitLogMsg SuperCurrentCommitLogAuthor)
    execute_process(COMMAND ${CTEST_GIT_COMMAND} pull
                    WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar
                    ERROR_VARIABLE ErrorVar)
    if(NOT ${ResultVar} EQUAL 0)
      message(FATAL_ERROR "Failed to pull super project:\n\n${ErrorVar}")
    endif()
    get_git_log(CTEST_SOURCE_DIRECTORY SuperNewCommit SuperNewCommitLogMsg SuperNewCommitLogAuthor)
    if(${SuperNewCommit} STREQUAL ${SuperCurrentCommit})
      set(RunAll OFF PARENT_SCOPE)
    endif()
  elseif(DashboardModel STREQUAL "Nightly" OR DashboardModel STREQUAL "Weekly")
    message("Updating super project")
    ctest_update(RETURN_VALUE UpdatedCount)
    if(UpdatedCount LESS 0)
      message(FATAL_ERROR "Failed to update the super project.")
    endif()
    ctest_submit()
  endif()
endfunction()


macro(update_sub_project SubProject)
  if(DashboardModel STREQUAL "Continuous")
    message("Updating ${SubProject}")
    get_git_log(${SubProject}SourceDirectory ${SubProject}CurrentCommit ${SubProject}CurrentCommitLogMsg ${SubProject}CurrentCommitLogAuthor)
    ctest_update(SOURCE ${${SubProject}SourceDirectory} RETURN_VALUE UpdatedCount)
    if(UpdatedCount LESS 0)
      message(FATAL_ERROR "Failed to update ${SubProject}.")
    elseif(UpdatedCount GREATER 0)
      set(${SubProject}ShouldRun ON)
      # temporarily rename the Update.xml so it can be found and submitted along with the other subproject's xml files after testing
      set(${SubProject}UpdateXmlName ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/Update-${SubProject}.xml)
      file(RENAME ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/Update.xml ${${SubProject}UpdateXmlName})
    else()
                                                                           # Look at project's dependencies
      set(${SubProject}ShouldRun OFF)
      unset(${SubProject}UpdateXmlName)
    endif()
    get_git_log(${SubProject}SourceDirectory ${SubProject}NewCommit ${SubProject}NewCommitLogMsg ${SubProject}NewCommitLogAuthor)
  else()
    set(${SubProject}ShouldRun ON)
  endif()
endmacro()


function(fix_xml_files_platform_entries_for_x64)
  if(NOT DashboardModel STREQUAL "Experimental" AND WIN32 AND MachineBuildType STREQUAL "x64")
    foreach(XML_File "Configure.xml" "Build.xml" "Test.xml")
      file(READ ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/${XML_File} ModFileContents)
      string(REPLACE "OSPlatform=\"x86\"" "OSPlatform=\"x64\"" ModFileContents "${ModFileContents}")
      file(WRITE ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/${XML_File} "${ModFileContents}")
    endforeach()
  endif()
endfunction()


function(write_git_update_details_to_file)
  if(NOT DashboardModel STREQUAL "Experimental")
    execute_process(COMMAND ${CTEST_GIT_COMMAND} diff --stat ${${SubProject}CurrentCommit} ${${SubProject}NewCommit}
                    WORKING_DIRECTORY ${${SubProject}SourceDirectory}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE ChangedFiles)
    if(ResultVar EQUAL 0)
      file(WRITE "${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/GitDetails.txt" "Old Commit Details: \n${${SubProject}CurrentCommitLogMsg}
          \nNew Commit: \n${${SubProject}NewCommitLogMsg}
          \nFiles Changed:
          \n${ChangedFiles}")
      set(CTEST_NOTES_FILES "${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/GitDetails.txt" PARENT_SCOPE)
    endif()
  endif()
endfunction()


function(handle_failed_build)
  message("${SubProject} failed during build, exiting script")
  if(WIN32)
    # TODO(Viv) Check OS Version
    execute_process(COMMAND cmd /c "ci_build_reporter.py win8 ${MachineBuildType} fail ${SubProject} ${${SubProject}NewCommitLogAuthor}"
                    WORKING_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/tools"
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar)
  else()
    # Need Linux Execute Script Command with argument detections
  endif()
endfunction()


function(build_and_run SubProject RunAll)
  # if(NOT ${SubProject} STREQUAL "Lifestuff")
  #   message("Temporary skip of All${SubProject}")
  #   return()
  # endif()
  if(NOT ${SubProject}ShouldRun AND NOT RunAll)
    message("Not building or running tests in ${SubProject}")
    return()
  endif()

  message("Building ${SubProject}")
  set(CTEST_BUILD_TARGET "All${SubProject}")
  # add coverage flags
  if(DashboardModel STREQUAL "Experimental" AND NOT WIN32)
    set(ExtraConfigureArgs "${ExtraConfigureArgs};-DCOVERAGE=ON")
  endif()
  ctest_configure(OPTIONS "${ExtraConfigureArgs}")
  ctest_read_custom_files(${CMAKE_CURRENT_BINARY_DIR})
  ctest_build(RETURN_VALUE BuildResult)

  # teardown network with python script if it's Lifestuff
  #if(${SubProject} STREQUAL "Lifestuff")
  #  execute_process(COMMAND ${CTEST_SOURCE_DIRECTORY}/tools/lifestuff_killer.py
  #                  WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY})
  #endif()

  # set up network with python script if it's Lifestuff
  if(${SubProject} STREQUAL "Lifestuff")
    #message("--------------------------------------------: python ${CTEST_SOURCE_DIRECTORY}/tools/py_function.py")
    #execute_process(COMMAND python ${CTEST_SOURCE_DIRECTORY}/tools/py_function.py
    #                WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY})
    #                #RESULT_VARIABLE SetupResult)
    #                #OUTPUT_VARIABLE SetupOutput)
    #message("++++++++++++++++++++++++++++++++++++++++++++: ${SetupOutput}")
    #if(SetupResult EQUAL 0)
    #  message(FATAL_ERROR "Error running set up")
    #endif()
  endif()

  # runs only tests that have a LABELS property matching "${SubProject}"
  message("Testing ${SubProject}")
  ctest_test(INCLUDE_LABEL "${SubProject}")

  # teardown network with python script if it's Lifestuff
  if(${SubProject} STREQUAL "Lifestuff")
    execute_process(COMMAND ${CTEST_SOURCE_DIRECTORY}/tools/lifestuff_killer.py
                    RESULT_VARIABLE TEARDOWN_RESULT
                    WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY})
  endif()

  fix_xml_files_platform_entries_for_x64()
  write_git_update_details_to_file()

  # Rename Update-${SubProject}.xml back to Update.xml before submitting
  if(${SubProject}UpdateXmlName)
    file(RENAME ${${SubProject}UpdateXmlName} ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/Update.xml)
  endif()

  ctest_submit(RETURN_VALUE ReturnVar)

  if(${ReturnVar} EQUAL 0)
    message("Submitted results for ${SubProject}\n-----------------------------------")
  else()
    message("Failed to submit results for ${SubProject}\n-------------------------------------------")
  endif()

  if(NOT ${BuildResult} EQUAL 0)
    handle_failed_build()
    set(BuildFailed TRUE PARENT_SCOPE)
    break()
  elseif(${SubProject} STREQUAL "LifestuffUiQt")
    if(WIN32)
      # TODO(Viv) Check OS Version
      execute_process(COMMAND cmd /c "ci_build_reporter.py win8 ${MachineBuildType} ok ${SubProject} ${${SubProject}NewCommitLogAuthor}"
                      WORKING_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/tools"
                      RESULT_VARIABLE ResultVar
                      OUTPUT_VARIABLE OutputVar)
    else()
      # Need Linux Execute Script Command with argument detections
    endif()
  endif()
endfunction()
