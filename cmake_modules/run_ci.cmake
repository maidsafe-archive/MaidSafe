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
#  Module used to run CI on all submodules of MaidSafe/MaidSafe                                    #
#                                                                                                  #
#  Example usage: ctest -S CI_Continuous_Release.cmake                                             #
#                                                                                                  #
#==================================================================================================#


set(ScriptVersion 7)
include(${CTEST_SOURCE_DIRECTORY}/CTestConfig.cmake)
include(${CTEST_SOURCE_DIRECTORY}/cmake_modules/ci_utils.cmake)

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


#==================================================================================================#
# Set sub-projects' source and binary directories                                                  #
#==================================================================================================#
foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
  if(${SubProject} STREQUAL "LifestuffUiQt")
    set(${SubProject}SourceDirectory ${CTEST_SOURCE_DIRECTORY}/src/lifestuff_ui_qt)
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/lifestuff_ui_qt)
  else()
    string(TOLOWER ${SubProject} subproject)
    set(${SubProject}SourceDirectory ${CTEST_SOURCE_DIRECTORY}/src/${subproject})
    set(${SubProject}BinaryDirectory ${CMAKE_CURRENT_BINARY_DIR}/src/${subproject})
  endif()
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
  message("Checking out super project to 'next'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} next)
elseif(DashboardModel STREQUAL "Nightly")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  message("Checking out super project to 'next'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} next)
elseif(DashboardModel STREQUAL "Weekly")
  ctest_empty_binary_directory(${CTEST_BINARY_DIRECTORY})
  message("Checking out super project to 'next'")
  checkout_to_branch(${CTEST_SOURCE_DIRECTORY} next)
endif()


#==================================================================================================#
# Build project & run tests if needed                                                              #
#==================================================================================================#
while(${CTEST_ELAPSED_TIME} LESS 72000)
  set(StartTime ${CTEST_ELAPSED_TIME})
  ctest_start(${DashboardModel} TRACK ${DashboardModel})
  get_tag()

  ctest_submit(FILES "${CTEST_SOURCE_DIRECTORY}/Project.xml")
  update_super_project()
  
  foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
    set_property(GLOBAL PROPERTY SubProject ${SubProject})
    set_property(GLOBAL PROPERTY Label ${SubProject})
    update_sub_project(${SubProject})
  endforeach()

  foreach(SubProject ${CTEST_PROJECT_SUBPROJECTS})
    set_property(GLOBAL PROPERTY SubProject ${SubProject})
    set_property(GLOBAL PROPERTY Label ${SubProject})
    build_and_run(${SubProject} ${RunAll})
    if(BuildFailed)
      return()
    endif()
  endforeach()

  if(NOT DashboardModel STREQUAL "Continuous")
    return()
  endif()
  ctest_sleep(${StartTime} 300 ${CTEST_ELAPSED_TIME})
endwhile()
