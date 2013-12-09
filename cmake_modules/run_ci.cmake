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
#  Module used to run CI on all submodules of MaidSafe/MaidSafe                                    #
#                                                                                                  #
#  Example usage: From MaidSafe build dir, run 'ctest -S CI_Continuous_Release.cmake'              #
#                                                                                                  #
#==================================================================================================#


set(ScriptVersion 12)
include(${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake)
include(${CTEST_SOURCE_DIRECTORY}/cmake_modules/ci_utils.cmake)

# Avoid non-ascii characters in tool output.
set(ENV{LC_ALL} C)

if(WIN32)
  if(CTEST_CMAKE_GENERATOR MATCHES "64$")
    set(MachineBuildType " x64")
  else()
    set(MachineBuildType " x86")
  endif()
endif()

set(CTEST_BUILD_NAME "${CTEST_CONFIGURATION_TYPE} ${TargetPlatform}${MachineBuildType}, ${CMAKE_CXX_COMPILER_ID} ${CMAKE_CXX_COMPILER_VERSION}, Script v${ScriptVersion}")
set(CTEST_UPDATE_COMMAND ${CTEST_GIT_COMMAND})
if(COVERAGE)
  set(CTEST_COVERAGE_COMMAND /usr/bin/gcov)
endif()
if(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "[Mm]ake")
  set(CTEST_USE_LAUNCHERS 0)
elseif(NOT DEFINED CTEST_USE_LAUNCHERS)
  set(CTEST_USE_LAUNCHERS 1)
endif()

message("Modules Selected:              ${CTEST_PROJECT_SUBPROJECTS}")
message("Dashboard Model Selected:      ${DashboardModel}")
message("Build Configuration Selected:  ${CTEST_CONFIGURATION_TYPE}")
message("Hostname:                      ${CTEST_SITE}")
message("Make Generator:                ${CTEST_CMAKE_GENERATOR}")
if(WIN32)
  message("Build Type:                    ${MachineBuildType}")
endif()
message("================================================================================")


#==================================================================================================#
# Set sub-projects' source and binary directories                                                  #
#==================================================================================================#
foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  string(TOLOWER ${SubProject} subproject)
  set(${SubProject}SourceDirectory ${CTEST_SOURCE_DIRECTORY}/src/${subproject})
  set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/${subproject})
  if(NOT EXISTS ${${SubProject}SourceDirectory} OR NOT EXISTS ${${SubProject}BinaryDirectory})
    message(FATAL_ERROR "Unable to find ${SubProject} source (${${SubProject}SourceDirectory}) or binary (${${SubProject}BinaryDirectory}) directory.")
  endif()
endforeach()


#==================================================================================================#
# Prepare for tests                                                                                #
#==================================================================================================#
set(RunAll ON)
if(DashboardModel STREQUAL "Experimental")
elseif(DashboardModel STREQUAL "Continuous")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  set(TestBranch next)
  message("Checking out super project to '${TestBranch}'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} ${TestBranch})
elseif(DashboardModel STREQUAL "Nightly")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  set(TestBranch next)
  message("Checking out super project to '${TestBranch}'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} ${TestBranch})
elseif(DashboardModel STREQUAL "Weekly")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  set(TestBranch next)
  message("Checking out super project to '${TestBranch}'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} ${TestBranch})
endif()


#==================================================================================================#
# Build project & run tests if needed                                                              #
#==================================================================================================#

set(StartTime ${CTEST_ELAPSED_TIME})
ctest_start(${DashboardModel} TRACK ${DashboardModel})
get_tag()

ctest_submit(FILES "${CTEST_SOURCE_DIRECTORY}/Project.xml")

if(DashboardModel STREQUAL "Experimental")
  set(UpdateSuperResult 0)
else()
  set(MaxUpdateAttempts 20)
  set(UpdateAttempts 0)
  set(UpdateSuperResult -1)
  while(NOT ${UpdateSuperResult} EQUAL 0 AND ${UpdateAttempts} LESS ${MaxUpdateAttempts})
    update_super_project()
  endwhile()
endif()
if(NOT ${UpdateSuperResult} EQUAL 0)
  message(FATAL_ERROR "Super project failed to update after ${UpdateAttempts} attempts.")
endif()


if(TestBranch)
  foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
    set_property(GLOBAL PROPERTY SubProject ${SubProject})
    set_property(GLOBAL PROPERTY Label ${SubProject})
    message("Checking out ${SubProject} project to '${TestBranch}'")
    checkout_to_branch(${${SubProject}SourceDirectory} ${TestBranch})
    if(DashboardModel STREQUAL "Experimental")
      set(UpdateResult 0)
      set(${SubProject}ShouldRun ON)
    else()
      set(UpdateAttempts 0)
      set(UpdateResult -1)
      while(NOT ${UpdateResult} EQUAL 0 AND ${UpdateAttempts} LESS ${MaxUpdateAttempts})
        update_sub_project(${SubProject})
      endwhile()
    endif()
    if(NOT ${UpdateResult} EQUAL 0)
      message(FATAL_ERROR "${SubProject} failed to update after ${UpdateAttempts} attempts.")
    endif()
  endforeach()
endif()

foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  set_property(GLOBAL PROPERTY SubProject ${SubProject})
  set_property(GLOBAL PROPERTY Label ${SubProject})
  list(LENGTH CTEST_PROJECT_SUBPROJECTS FinalSubProjectCount)
  math(EXPR FinalSubProjectIndex ${FinalSubProjectCount}-1)
  list(GET CTEST_PROJECT_SUBPROJECTS ${FinalSubProjectIndex} FinalSubProject)
  build_and_run(${SubProject} ${RunAll})
  if(BuildFailed)
    return()
  endif()
endforeach()

message("\n#################################### Iteration Complete ####################################\n")
