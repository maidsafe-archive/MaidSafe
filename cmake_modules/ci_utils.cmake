#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
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
#  Helper module used in running CI on all submodules of MaidSafe/MaidSafe                         #
#                                                                                                  #
#==================================================================================================#


set(SelfGitFetchCheck ON)
set(FetchFailed OFF)

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

# Calls git fetch via execute_process to check error code
# Current workaround to the ctest_update = 0 regardless of git fetch cmd failure
# Relate CMake Bugs: http://www.cmake.org/Bug/view.php?id=8262 && http://www.cmake.org/Bug/view.php?id=8277
function(check_fetch_failure)
  execute_process(COMMAND ${CTEST_GIT_COMMAND} fetch --dry-run
        WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
        RESULT_VARIABLE FetchResult
        OUTPUT_VARIABLE OutputVar
        ERROR_VARIABLE ErrorVar)
  if(FetchResult EQUAL 0)
    set(FetchFailed OFF PARENT_SCOPE)
  else()
    set(FetchFailed ON PARENT_SCOPE)
  endif()
endfunction()

function(update_super_project)
  math(EXPR UpdateAttempts ${UpdateAttempts}+1)
  set(UpdateAttempts ${UpdateAttempts} PARENT_SCOPE)
  message("Updating super project, attempt ${UpdateAttempts} of ${MaxUpdateAttempts}")
  if(DashboardModel STREQUAL "Continuous")
    get_git_log(CTEST_SOURCE_DIRECTORY SuperCurrentCommit SuperCurrentCommitLogMsg SuperCurrentCommitLogAuthor)
    execute_process(COMMAND ${CTEST_GIT_COMMAND} pull
                    WORKING_DIRECTORY ${CTEST_SOURCE_DIRECTORY}
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar
                    ERROR_VARIABLE ErrorVar)
    set(UpdateSuperResult ${ResultVar} PARENT_SCOPE)
    if(NOT ${ResultVar} EQUAL 0)
      message(WARNING "Super project failed to update.  Sleeping for 30 seconds.")
      ctest_sleep(30)
      return()
      #message(WARNING "Failed to pull super project:\n\n${ErrorVar}")
    endif()
    get_git_log(CTEST_SOURCE_DIRECTORY SuperNewCommit SuperNewCommitLogMsg SuperNewCommitLogAuthor)
    if(${SuperNewCommit} STREQUAL ${SuperCurrentCommit})
      set(RunAll OFF PARENT_SCOPE)
    endif()
  elseif(DashboardModel STREQUAL "Nightly" OR DashboardModel STREQUAL "Weekly")
    message("Updating super project")
    ctest_update(RETURN_VALUE UpdatedCount)
    if(UpdatedCount LESS 0)
      set(UpdateSuperResult ${UpdatedCount} PARENT_SCOPE)
      message(WARNING "Super project failed to update.  Sleeping for 30 seconds.")
      ctest_sleep(30)
      return()
    elseif(UpdatedCount EQUAL 0)
      if(SelfGitFetchCheck)
        check_fetch_failure()
        if(FetchFailed)
          set(UpdateSuperResult -99 PARENT_SCOPE)
          message(WARNING "Super project failed to update.  Sleeping for 30 seconds.")
          ctest_sleep(30)
          return()
        else()
          set(UpdateSuperResult 0 PARENT_SCOPE)
        endif()
      endif()
      #message(FATAL_ERROR "Failed to update the super project.")
    endif()
    ctest_submit()
    set(UpdateSuperResult 0 PARENT_SCOPE)
  endif()
endfunction()


macro(update_sub_project SubProject)
  math(EXPR UpdateAttempts ${UpdateAttempts}+1)
  message("Updating ${SubProject}, attempt ${UpdateAttempts} of ${MaxUpdateAttempts}")
  ctest_update(SOURCE ${${SubProject}SourceDirectory} RETURN_VALUE UpdatedCount)
  if(DashboardModel STREQUAL "Continuous")
    get_git_log(${SubProject}SourceDirectory ${SubProject}CurrentCommit ${SubProject}CurrentCommitLogMsg ${SubProject}CurrentCommitLogAuthor)
    if(UpdatedCount LESS 0)
      set(UpdateResult ${UpdatedCount})
      message(WARNING "${SubProject} failed to update.  Sleeping for 30 seconds.")
      ctest_sleep(30)
      set(${SubProject}ShouldRun OFF)
      unset(${SubProject}UpdateXmlName)
    elseif(UpdatedCount GREATER 0)
      set(UpdateResult 0)
      set(${SubProject}ShouldRun ON)
      # temporarily rename the Update.xml so it can be found and submitted along with the other subproject's xml files after testing
      set(${SubProject}UpdateXmlName ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/Update-${SubProject}.xml)
      file(RENAME ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/Update.xml ${${SubProject}UpdateXmlName})
    else()
                                                                           # Look at project's dependencies
      set(${SubProject}ShouldRun OFF)
      unset(${SubProject}UpdateXmlName)
      if(SelfGitFetchCheck)
        check_fetch_failure()
        if(FetchFailed)
          set(UpdateResult -99)
          message(WARNING "${SubProject} failed to update.  Sleeping for 30 seconds.")
          ctest_sleep(30)
        else()
          set(UpdateResult 0)
        endif()
      endif()
    endif()
    get_git_log(${SubProject}SourceDirectory ${SubProject}NewCommit ${SubProject}NewCommitLogMsg ${SubProject}NewCommitLogAuthor)
  elseif(DashboardModel STREQUAL "Nightly" OR DashboardModel STREQUAL "Weekly")
    set(${SubProject}ShouldRun ON)
    if(SelfGitFetchCheck)
      check_fetch_failure()
      if(FetchFailed)
        set(UpdateResult -99)
        message(WARNING "${SubProject} failed to update.  Sleeping for 30 seconds.")
        ctest_sleep(30)
      else()
        set(UpdateResult 0)
      endif()
    endif()
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


function(report_build_result Result)
  if(${Result} STREQUAL "false")
    message(WARNING "\n#################################### ${SubProject} failed during build ####################################\n")
  endif()
#   execute_process(COMMAND ${CTEST_PYTHON_EXECUTABLE} ci_build_reporter.py "${MachineType}" "k${MachineBuildType}" "${Result}" "${SubProject}" "${${SubProject}NewCommitLogAuthor}"
#                   WORKING_DIRECTORY "${CTEST_SOURCE_DIRECTORY}/tools"
#                   RESULT_VARIABLE ResultVar
#                   OUTPUT_VARIABLE OutputVar)
#   if(NOT ${ResultVar} EQUAL 0)
#     message(WARNING "${SubProject} failed running \"${CTEST_PYTHON_EXECUTABLE} ci_build_reporter.py \"${MachineType}\" \"k${MachineBuildType}\" \"${Result}\" \"${SubProject}\" \"${${SubProject}NewCommitLogAuthor}\"\"\n\n${OutputVar}")
#   endif()
endfunction()


function(build_and_run SubProject RunAll)
  # if(NOT ${SubProject} STREQUAL "Client")
  #   message("Temporary skip of All${SubProject}")
  #   return()
  # endif()
  if(NOT ${SubProject}ShouldRun AND NOT RunAll)
    message("Not building or running tests in ${SubProject}")
    if(${SubProject} STREQUAL ${FinalSubProject})
      report_build_result("true")
    endif()
    return()
  endif()

  # add coverage flags
  if(DashboardModel STREQUAL "Experimental" OR DashboardModel STREQUAL "Continuous" AND NOT WIN32)
    set(ExtraConfigureArgs "${ExtraConfigureArgs};-DCOVERAGE=ON")
    set(CTEST_COVERAGE_COMMAND /usr/bin/gcov)
  endif()
  ctest_configure(OPTIONS "${ExtraConfigureArgs}")
  ctest_read_custom_files(${CMAKE_CURRENT_BINARY_DIR})

  # build
  message("Building ${SubProject}")
  set(CTEST_BUILD_TARGET "All${SubProject}")
  if(PathOfMake)
    set(CTEST_BUILD_COMMAND "${PathOfMake} -k ${CTEST_BUILD_TARGET}")
  endif()
  ctest_build(RETURN_VALUE BuildResult)
  if(BuildResult EQUAL 0)
    set(RecurringBuildFailureCount 0)
  else()
    if(${SubProject}RecurringBuildFailureCount)
      math(EXPR RecurringBuildFailureCount ${${SubProject}RecurringBuildFailureCount}+1)
    else()
      set(RecurringBuildFailureCount 1)
    endif()
    message(WARNING "Recurring build failure count of ${RecurringBuildFailureCount} for ${SubProject}.")
    if(SelfGitFetchCheck)
      check_fetch_failure()
    endif()
  endif()
  set(${SubProject}RecurringBuildFailureCount ${RecurringBuildFailureCount} PARENT_SCOPE)

  if(RecurringBuildFailureCount EQUAL 0)
    # runs only tests that have a LABELS property matching "${SubProject}"
    message("Testing ${SubProject}")
    ctest_test(INCLUDE_LABEL "${SubProject}")
  endif()

  ctest_coverage()

  # teardown network with python script if it's Client
  if(${SubProject} STREQUAL "Client")
    execute_process(COMMAND ${CTEST_SOURCE_DIRECTORY}/tools/client_killer.py
                    RESULT_VARIABLE TEARDOWN_RESULT
                    WORKING_DIRECTORY ${CTEST_BINARY_DIRECTORY})
  endif()

  fix_xml_files_platform_entries_for_x64()
  write_git_update_details_to_file()

  # Rename Update-${SubProject}.xml back to Update.xml before submitting
  if(${SubProject}UpdateXmlName)
    file(RENAME ${${SubProject}UpdateXmlName} ${CTEST_BINARY_DIRECTORY}/Testing/${TagId}/Update.xml)
  endif()

  # Avoid clogging dashboard with repeated build failures
  if(RecurringBuildFailureCount LESS 3)
    ctest_submit(RETURN_VALUE ReturnVar)
  endif()

  if(${ReturnVar} EQUAL 0)
    message("Submitted results for ${SubProject}\n-----------------------------------")
  else()
    message(WARNING "Failed to submit results for ${SubProject}\n-------------------------------------------")
  endif()

  if(NOT ${BuildResult} EQUAL 0)
    report_build_result("false")
    message(FATAL_ERROR "Build of ${SubProject} has resulted in a fatal error. Reported to Dash.")
  elseif(FinalSubmodule)
    report_build_result("true")
  endif()
endfunction()
