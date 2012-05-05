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


set(SCRIPT_VERSION 15)
file(STRINGS "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}" INSTALLED_VERSION_INFO LIMIT_COUNT 1)
string(REPLACE " " ";" INSTALLED_VERSION_INFO ${INSTALLED_VERSION_INFO})
list(GET INSTALLED_VERSION_INFO 1 INSTALLED_VERSION)
string(REPLACE ")" "" INSTALLED_VERSION ${INSTALLED_VERSION})
if(NOT ${SCRIPT_VERSION} EQUAL ${INSTALLED_VERSION})
   message("New installed Version: ${INSTALLED_VERSION}")
   message("Current running version: ${SCRIPT_VERSION}")
   message(FATAL_ERROR "This script is updated. Start the script again !!!")
endif()
message("Starting Script version: ${SCRIPT_VERSION}")

###############################################################################
#Pre-requirement:                                                             #
# All modules present under one directory.                                    #
# All modules installed under MaidSafe-Common\installed                       #
# This Script run from MaidSafe-Common\installed\share\maidsafe\cmake_modules #
###############################################################################

###############################################################################
# List of Modules and Paths                                                   #
###############################################################################
#Append/comment modules name here in order of dependency starting with MAIDSAFE_COMMON
#Test will run for below modules
set(ALL_MODULE_LIST
    "MAIDSAFE_COMMON"
    "MAIDSAFE_PRIVATE"
    "MAIDSAFE_TRANSPORT"
#    "MAIDSAFE_BREAKPAD"
    "MAIDSAFE_ENCRYPT"
    "MAIDSAFE_DHT"
    "MAIDSAFE_PKI"
    "MAIDSAFE_PASSPORT"
    "MAIDSAFE_PD"
    "MAIDSAFE_DRIVE"
#    "FILE_BROWSER"
    "LIFESTUFF"
    "LIFESTUFF_GUI"
#    "SIGMOID_CORE"
    "SIGMOID_PRO"
#    "SIGMOID_ULTIMATE"
#    "DEDUPLICATOR_GAUGE"
    )

message("================================================================================")
message("Modules Selected For the test:")
foreach(EACH_MODULE ${ALL_MODULE_LIST})
  message("${EACH_MODULE}")
endforeach()
message("================================================================================")

###############################################################################
# Module configurations                                                       #
###############################################################################
#Module folder-name/path relative to TEST_ROOT_DIRECTORY
set(MAIDSAFE_COMMON "MaidSafe-Common/maidsafe_common_lib")
set(MAIDSAFE_PRIVATE "MaidSafe-Private")
set(MAIDSAFE_TRANSPORT "MaidSafe-Transport")
set(MAIDSAFE_BREAKPAD "MaidSafe-Breakpad")
set(MAIDSAFE_ENCRYPT "MaidSafe-Encrypt")
set(MAIDSAFE_DHT "MaidSafe-DHT")
set(MAIDSAFE_PKI "MaidSafe-PKI")
set(MAIDSAFE_PASSPORT "MaidSafe-Passport")
set(MAIDSAFE_PD "MaidSafe-PD")
set(FILE_BROWSER "File-Browser")
set(LIFESTUFF "LifeStuff")
set(LIFESTUFF_GUI "LifeStuff-GUI")
set(MAIDSAFE_DRIVE "MaidSafe-Drive")
set(SIGMOID_CORE "SigmoidCore")
set(SIGMOID_PRO "SigmoidPro")
set(SIGMOID_ULTIMATE "SigmoidUltimate")
set(DEDUPLICATOR_GAUGE "Deduplicator-Gauge")
#List the Module if needs to be installed
set(MAIDSAFE_COMMON_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_PRIVATE_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_TRANSPORT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_DHT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_ENCRYPT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_PKI_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_PASSPORT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_PD_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(FILE_BROWSER_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(LIFESTUFF_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
set(MAIDSAFE_DRIVE_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)

#List of dependent modules which will be tested for a given module
set(MAIDSAFE_COMMON_DEPENDANTS
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_TRANSPORT
    MAIDSAFE_BREAKPAD
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DHT
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_PD
    MAIDSAFE_DRIVE
    LIFESTUFF
    LIFESTUFF_GUI
    SIGMOID_CORE
    SIGMOID_PRO
    DEDUPLICATOR_GAUGE
    )
set(MAIDSAFE_PRIVATE_DEPENDANTS
    MAIDSAFE_PRIVATE
    MAIDSAFE_ENCRYPT
    MAIDSAFE_PD
    MAIDSAFE_DRIVE
    LIFESTUFF
    LIFESTUFF_GUI
    SIGMOID_PRO
    )
set(MAIDSAFE_TRANSPORT_DEPENDANTS
    MAIDSAFE_TRANSPORT
    MAIDSAFE_BREAKPAD
    MAIDSAFE_DHT
    MAIDSAFE_PD
    LIFESTUFF
    LIFESTUFF_GUI
    SIGMOID_PRO
    )
set(MAIDSAFE_ENCRYPT_DEPENDANTS
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DRIVE
    SIGMOID_CORE
    SIGMOID_PRO
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_DHT_DEPENDANTS
    MAIDSAFE_DHT
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_PD
    SIGMOID_PRO
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_PKI_DEPENDANTS
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_PD
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_PASSPORT_DEPENDANTS
    MAIDSAFE_PASSPORT
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(MAIDSAFE_PD_DEPENDANTS
    MAIDSAFE_PD
    SIGMOID_PRO
    LIFESTUFF
    LIFESTUFF_GUI
    )
set(FILE_BROWSER_DEPENDANTS
    FILE_BROWSER
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
set(MAIDSAFE_DRIVE_DEPENDANTS
    MAIDSAFE_DRIVE
    SIGMOID_CORE
    SIGMOID_PRO
    )
set(SIGMOID_CORE_DEPENDANTS
    SIGMOID_CORE
    )
set(SIGMOID_PRO_DEPENDANTS
    SIGMOID_PRO
    )
set(DEDUPLICATOR_GAUGE_DEPENDANTS
    DEDUPLICATOR_GAUGE
    )

# List of Dependencies per module
set(MAIDSAFE_COMMON_DEPENDS_ON
    )
set(MAIDSAFE_PRIVATE_DEPEND_ON
    MAIDSAFE_COMMON
    )
set(MAIDSAFE_TRANSPORT_DEPEND_ON
    MAIDSAFE_COMMON
    )
set(MAIDSAFE_BREAKPAD_DEPEND_ON
    MAIDSAFE_COMMON
    MAIDSAFE_TRANSPORT
    )
set(MAIDSAFE_ENCRYPT_DEPEND_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    )
set(MAIDSAFE_DHT_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_TRANSPORT
    )
set(MAIDSAFE_PKI_DEPENDS_ON
    MAIDSAFE_COMMON
    )
set(MAIDSAFE_PASSPORT_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PKI
    )
set(MAIDSAFE_PD_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_DHT
    MAIDSAFE_TRANSPORT
    MAIDSAFE_PKI
    )
set(FILE_BROWSER_DEPENDS_ON
    )
set(LIFESTUFF_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_ENCRYPT
    MAIDSAFE_TRANSPORT
    MAIDSAFE_DHT
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_DRIVE
    )
set(LIFESTUFF_GUI_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_ENCRYPT
    MAIDSAFE_TRANSPORT
    MAIDSAFE_DHT
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_DRIVE
    LIFESTUFF
    )
set(MAIDSAFE_DRIVE_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_ENCRYPT
    )
set(SIGMOID_CORE_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DRIVE
    )
set(SIGMOID_PRO_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DHT
    MAIDSAFE_TRANSPORT
    MAIDSAFE_DRIVE
    )
set(DEDUPLICATOR_GAUGE_DEPENDS_ON
    )

###############################################################################
# Variable(s) determined after running cmake                                  #
###############################################################################
set(CTEST_CMAKE_GENERATOR "@CMAKE_GENERATOR@")

# Modules supported for Makefile generators only.
#IF(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "Make")
#  LIST(REMOVE_ITEM ALL_MODULE_LIST "SIGMOID_CORE" "SIGMOID_PRO" "MAIDSAFE_DRIVE")
#ENDIF()

###############################################################################
# Test configurations                                                         #
###############################################################################
if(NOT "${CTEST_SCRIPT_ARG}" MATCHES "^(Nightly|NightlyDebug|NightlyRelease|NightlyMemCheck|NightlyDebugMemCheck|NightlyReleaseMemCheck|Experimental|ExperimentalDebug|ExperimentalRelease|ExperimentalMemCheck|ExperimentalDebugMemCheck|ExperimentalReleaseMemCheck|Continuous|ContinuousDebug|ContinuousRelease|ContinuousMemCheck|ContinuousDebugMemCheck|ContinuousReleaseMemCheck|Weekly|WeeklyRelease|WeeklyDebug)$")
  message(FATAL_ERROR "Allowed arguments are Nightly, Experimental, Continuous, Weekly,
                       NightlyDebug, NightlyRelease,NightlyMemCheck,NightlyDebugMemCheck,NightlyReleaseMemCheck,
                       ExperimentalDebug, ExperimentalRelease,ExperimentalMemCheck,ExperimentalDebugMemCheck,ExperimentalReleaseMemCheck,
                       ContinuousDebug, ContinuousRelease,ContinuousMemCheck,ContinuousDebugMemCheck,ContinuousReleaseMemCheck,
                       WeeklyRelease & WeeklyDebug
                       \n eg. ctest -S ${CTEST_SCRIPT_NAME},NightlyDebug")
endif()

# Select the model (Nightly, Experimental, Continuous & Weekly).
if(${CTEST_SCRIPT_ARG} MATCHES Continuous)
  set(DASHBOARD_MODEL Continuous)
elseif(${CTEST_SCRIPT_ARG} MATCHES Nightly)
  set(DASHBOARD_MODEL Nightly)
elseif(${CTEST_SCRIPT_ARG} MATCHES Experimental)
  set(DASHBOARD_MODEL Experimental)
elseif(${CTEST_SCRIPT_ARG} MATCHES Weekly)
  set(DASHBOARD_MODEL Weekly)
endif()

if(NOT DEFINED DASHBOARD_MODEL)
  set(DASHBOARD_MODEL Experimental)  #default to "Experimental"
endif()

if(${CTEST_SCRIPT_ARG} MATCHES Debug)
  set(CTEST_BUILD_CONFIGURATION "Debug")
elseif(${CTEST_SCRIPT_ARG} MATCHES Release)
  set(CTEST_BUILD_CONFIGURATION "Release")
endif()

if(NOT DEFINED CTEST_BUILD_CONFIGURATION)
  set(CTEST_BUILD_CONFIGURATION "Debug")  #default to "Debug"
endif()

if(${CTEST_SCRIPT_ARG} MATCHES MemCheck OR ${CTEST_SCRIPT_ARG} MATCHES Weekly)
  set(MEMORY_CHECK_NEEDED TRUE)
else()
  set(MEMORY_CHECK_NEEDED FALSE)
endif()

#SET(CTEST_BUILD_OPTIONS "-DWITH_SSH1=ON -WITH_SFTP=ON -DWITH_SERVER=ON -DWITH_ZLIB=ON -DWITH_PCAP=ON -DWITH_GCRYPT=OFF")
message("Dashboard Model Selected:      ${DASHBOARD_MODEL}")
message("Build Configuration Selected:  ${CTEST_BUILD_CONFIGURATION}")
message("================================================================================")

find_file(DART_CONFIG NAMES DartConfiguration.tcl
             PATHS "${CTEST_SCRIPT_DIRECTORY}/../../../../../MaidSafe-Common/maidsafe_common_lib/build"
             PATH_SUFFIXES Win_MSVC Linux/Debug Linux/Release Linux/RelWithDebInfo Linux/MinSizeRel OSX/Debug OSX/Release OSX/RelWithDebInfo OSX/MinSizeRel
             NO_DEFAULT_PATH)
file(STRINGS ${DART_CONFIG} DART_CONFIG_CONTENTS REGEX "BuildName: ")
string(REPLACE "BuildName: " "" CTEST_BUILD_NAME ${DART_CONFIG_CONTENTS})
#SET(CTEST_BUILD_NAME "${CTEST_BUILD_NAME} (${CTEST_BUILD_CONFIGURATION} v${SCRIPT_VERSION})")

###############################################################################
# Utility functions                                                           #
###############################################################################
function(SET_FORCE_TEST_FOR_ALL_MODULE VALUE)
  foreach(EACH_MODULE ${ALL_MODULE_LIST})
    set(${EACH_MODULE}_NEEDS_FORCE_TEST ${VALUE} PARENT_SCOPE)
  endforeach()
endfunction()

function(SET_FORCE_TEST_FOR_DEPENDANTS MODULE_NAME VALUE)
  foreach(EACH_MODULE ${${MODULE_NAME}_DEPENDANTS})
    set(${EACH_MODULE}_NEEDS_FORCE_TEST ${VALUE} PARENT_SCOPE)
  endforeach()
endfunction()

###############################################################################
# Finding Modules Paths & setting initial update status                       #
###############################################################################
## TEST_ROOT_DIRECTORY ##
set(MAIDSAFE_TEST_ROOT_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/../../../../../)

foreach(EACH_MODULE ${ALL_MODULE_LIST})
  set(${EACH_MODULE}_SOURCE_DIRECTORY ${MAIDSAFE_TEST_ROOT_DIRECTORY}/${${EACH_MODULE}})
  if(WIN32)
    set(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/Win_MSVC)
  elseif(APPLE)
    set(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/OSX/${CTEST_BUILD_CONFIGURATION})
  elseif(UNIX)
    set(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/Linux/${CTEST_BUILD_CONFIGURATION})
  endif()

  set(${EACH_MODULE}_UPDATED 0)
  SET_FORCE_TEST_FOR_DEPENDANTS(${EACH_MODULE} 0)

  if(EXISTS ${${EACH_MODULE}_SOURCE_DIRECTORY})
    message("Found ${EACH_MODULE} at "${${EACH_MODULE}_SOURCE_DIRECTORY})
  else()
    message(FATAL_ERROR "Unable to find ${EACH_MODULE} source directory: ${${EACH_MODULE}_SOURCE_DIRECTORY}")
  endif()

  if(EXISTS ${${EACH_MODULE}_BINARY_DIRECTORY})
    message("Found ${EACH_MODULE} Binary Directory at "${${EACH_MODULE}_BINARY_DIRECTORY})
  else()
    message(FATAL_ERROR "Unable to find ${EACH_MODULE} binary directory")
  endif()
endforeach()

message("================================================================================")

###############################################################################
#Finding hostname                                                             #
###############################################################################
find_program(HOSTNAME_CMD NAMES hostname)
EXEC_PROGRAM(${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE HOSTNAME)
set(CTEST_SITE "${HOSTNAME}")
message("Hostname: " ${HOSTNAME})

###############################################################################
# Finding Programs & Commands                                                 #
###############################################################################
find_program(CTEST_CMAKE_COMMAND NAMES cmake)
if(NOT CTEST_CMAKE_COMMAND)
  message(FATAL_ERROR "Couldn't find CMake executable. Specify path of CMake executable. \n e.g. -DCTEST_CMAKE_COMMAND=\"C:/Program Files/CMake 2.8/bin/cmake.exe\"")
endif()
message(STATUS "Found CMake")

set(CMAKE_MODULE_PATH ${CTEST_SCRIPT_DIRECTORY})
include(maidsafe_find_git)
set(CTEST_UPDATE_COMMAND "${Git_EXECUTABLE}")

if(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "Make")
  # Launchers work only with Makefile generators.
  set(CTEST_USE_LAUNCHERS 0)
elseif(NOT DEFINED CTEST_USE_LAUNCHERS)
  set(CTEST_USE_LAUNCHERS 1)
endif()

###############################################################################
# Configure CTest                                                             #
###############################################################################
set(BASIC_CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=${CTEST_BUILD_CONFIGURATION} -G \"${CTEST_CMAKE_GENERATOR}\"")
set(CTEST_CONFIGURATION_TYPE ${CTEST_BUILD_CONFIGURATION})

find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
if(CTEST_COVERAGE_COMMAND)
  set(COVERAGE_FLAG_ON "-DCOVERAGE=ON")
  set(COVERAGE_FLAG_OFF "-DCOVERAGE=OFF")
endif()

find_program(CTEST_MEMORYCHECK_COMMAND
  NAMES purify valgrind boundscheck
  PATHS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Rational Software\\Purify\\Setup;InstallFolder]"
  DOC "Path to the memory checking command, used for memory error detection."
  )
set(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --quiet --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=50 --verbose --demangle=yes")
if(CTEST_MEMORYCHECK_COMMAND)
  set(MEMORYCHECK_FLAG_ON "-DMEMORY_CHECK=ON")
  set(MEMORYCHECK_FLAG_OFF "-DMEMORY_CHECK=OFF")
  message(STATUS "Found MemCheck program")
elseif(MEMORY_CHECK_NEEDED)
  message(STATUS "Unable to find any memory check program. Will skip memory checks")
endif()

message("================================================================================")

# Avoid non-ascii characters in tool output.
set(ENV{LC_ALL} C)

###############################################################################
# CTest Utility Functions                                                     #
###############################################################################
function(CHECK_UPDATE_STATUS_FOR_MODULE MODULE_NAME)
  set(MODULE_SOURCE_DIRECTORY ${${MODULE_NAME}_SOURCE_DIRECTORY})
  set(MODULE_BINARY_DIRECTORY ${${MODULE_NAME}_BINARY_DIRECTORY})
  message("  Checking updates for " ${MODULE_NAME})
  set(CTEST_BUILD_NAME ${${MODULE_NAME}_BUILD_NAME})
  set(CTEST_SOURCE_DIRECTORY ${MODULE_SOURCE_DIRECTORY})
  set(CTEST_BINARY_DIRECTORY ${MODULE_BINARY_DIRECTORY})
  CTEST_START(${DASHBOARD_MODEL} TRACK "${DASHBOARD_MODEL}")
  CTEST_UPDATE(SOURCE ${MODULE_SOURCE_DIRECTORY} RETURN_VALUE UPDATED_COUNT)
  set(${MODULE_NAME}_UPDATED ${UPDATED_COUNT} PARENT_SCOPE)
endfunction()

function(RUN_TEST_ONCE MODULE_NAME)
  if(${${MODULE_NAME}_NEEDS_FORCE_TEST} EQUAL 0)
    return()
  endif()
  set(MODULE_SOURCE_DIRECTORY ${${MODULE_NAME}_SOURCE_DIRECTORY})
  set(MODULE_BINARY_DIRECTORY ${${MODULE_NAME}_BINARY_DIRECTORY})
  message("Running Test for " ${MODULE_NAME})
  set(CTEST_SOURCE_DIRECTORY ${MODULE_SOURCE_DIRECTORY})
  set(CTEST_BINARY_DIRECTORY ${MODULE_BINARY_DIRECTORY})

  set(CTEST_BUILD_NAME ${${MODULE_NAME}_BUILD_NAME})
  set(CTEST_CONFIGURE_COMMAND "${BASIC_CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\" ${COVERAGE_FLAG_ON} ${MEMORYCHECK_FLAG_OFF}")

  unset(CTEST_CUSTOM_COVERAGE_EXCLUDE)
  unset(CTEST_CUSTOM_MEMCHECK_IGNORE)

  CTEST_START(${DASHBOARD_MODEL} TRACK "${DASHBOARD_MODEL}")
  CTEST_READ_CUSTOM_FILES(${CTEST_BINARY_DIRECTORY})
  CTEST_UPDATE(RETURN_VALUE UPDATED_COUNT)
  set(${EACH_MODULE}_UPDATED ${UPDATED_COUNT} PARENT_SCOPE)
  if(${${EACH_MODULE}_UPDATED} GREATER 0)
    return()
  endif()

  CTEST_CONFIGURE(RETURN_VALUE RETURNED)
  if(NOT ${RETURNED} EQUAL 0)
    message("  CTEST_CONFIGURE failed ret: ${RETURNED}")
    message("  Will try again after re-installing dependencies")
    # Retry Configure after re-installing dependencies
    INSTALL_DEPENDENCIES(${MODULE_NAME})
    CTEST_CONFIGURE(RETURN_VALUE RETURNED)
    if(NOT ${RETURNED} EQUAL 0)
      message("  CTEST_CONFIGURE failed after re-installing dependencies. ret: ${RETURNED}")
      message("  ERROR : Skipping build for ${MODULE_NAME}")
      #Submitting configure failure results to cdash
      CTEST_SUBMIT()
      set(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
      set(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
      return()
    else()
      message("  Configured ${MODULE_NAME} after re-installing dependencies.")
    endif()
  else()
    message("  Configured ${MODULE_NAME}.")
  endif()

  #clean up module
  execute_process(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
      COMMAND ${CTEST_CMAKE_COMMAND} --build . --target clean --config ${CTEST_BUILD_CONFIGURATION}
      RESULT_VARIABLE ret_var
      OUTPUT_VARIABLE out_var
      )
  if(NOT ${ret_var} EQUAL 0)
    message("  Cleaning ${MODULE_NAME} returned ${ret_var}.Output:${out_var}")
    message("  ERROR : Skipping test for ${MODULE_NAME}")
    set(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
    set(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
    return()
  endif()
  message("  Cleaned ${MODULE_NAME}.")

  CTEST_BUILD(RETURN_VALUE RETURNED)
  if(NOT ${RETURNED} EQUAL 0)
    #Submitting build failure results to cdash
    CTEST_SUBMIT()
    message("  CTEST_BUILD failed ret: ${RETURNED}")

    set(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
    return()
  endif()
  message("  Built ${MODULE_NAME}.")

  CTEST_TEST()
  message("  Ran all tests for ${MODULE_NAME}.")

  CTEST_SUBMIT(PARTS Update Configure Build Test)

  if(MEMORY_CHECK_NEEDED AND CTEST_MEMORYCHECK_COMMAND)
    message("  Starting Memory check for ${MODULE_NAME}...")
    execute_process(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY} COMMAND ${CTEST_CMAKE_COMMAND} --build . --target clean --config ${CTEST_BUILD_CONFIGURATION})
    set(CTEST_CONFIGURE_COMMAND "${BASIC_CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\" ${COVERAGE_FLAG_OFF} ${MEMORYCHECK_FLAG_ON}")
    CTEST_CONFIGURE()
    CTEST_BUILD()
    CTEST_MEMCHECK()
    CTEST_SUBMIT(PARTS MemCheck)
    set(CTEST_CONFIGURE_COMMAND "${BASIC_CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\" ${MEMORYCHECK_FLAG_OFF}")
    CTEST_CONFIGURE()
    message("  Ran memory check for ${MODULE_NAME}.")
  elseif(MEMORY_CHECK_NEEDED)
    message("  ERROR : No memory check program found. Skipping Memory check for ${MODULE_NAME}.")
  endif()

  if(CTEST_COVERAGE_COMMAND)
    CTEST_COVERAGE()
    CTEST_SUBMIT(PARTS Coverage)
    message("  Ran coverage for ${MODULE_NAME}.")
  endif()

  #Installing if module needs installation
  if(${MODULE_NAME}_SHOULD_BE_INSTALLED_AFTER_UPDATE)
    execute_process(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
        COMMAND ${CTEST_CMAKE_COMMAND} --build . --config ${CTEST_BUILD_CONFIGURATION} --target install
        RESULT_VARIABLE ret_var
        OUTPUT_VARIABLE out_var
        )
    if(NOT ${ret_var} EQUAL 0)
      message("  Installing ${MODULE_NAME} returned ${ret_var} Output:${out_var}")
      message("  Skipping installation of ${MODULE_NAME}")
      set(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
      set(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
      return()
    else()
      message("  Installed ${MODULE_NAME}.")
    endif()
  endif()
  set(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
  set(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
endfunction()

function(INSTALL_DEPENDENCIES MODULE_NAME)
  foreach(EACH_MODULE ${${MODULE_NAME}_DEPENDS_ON})
    list(FIND ALL_MODULE_LIST ${EACH_MODULE} output)
    if(NOT ${output} EQUAL -1)
      set(MODULE_BINARY_DIRECTORY ${${EACH_MODULE}_BINARY_DIRECTORY})
      message("Installing " ${EACH_MODULE})
      if(${EACH_MODULE}_SHOULD_BE_INSTALLED_AFTER_UPDATE)
        execute_process(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
          COMMAND ${CTEST_CMAKE_COMMAND} --build . --config ${CTEST_BUILD_CONFIGURATION} --target install
          RESULT_VARIABLE ret_var
          OUTPUT_VARIABLE out_var
          )
        if(NOT ${ret_var} EQUAL 0)
          message("  ERROR : Installing ${EACH_MODULE} returned ${ret_var} Output:${out_var}")
        else()
          message("  Installed ${EACH_MODULE}.")
        endif()
      endif()
    endif()
  endforeach()
endfunction()

function(GET_CURRENT_BRANCH_FOR_ALL_MODULES)
  message("  Current Branch of Modules :")
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
      message("    ${EACH_MODULE} : ${CURRENT_BRANCH}")
      set(${EACH_MODULE}_CURRENT_BRANCH ${CURRENT_BRANCH} PARENT_SCOPE)
    elseif()
      set(${EACH_MODULE}_CURRENT_BRANCH unknown PARENT_SCOPE)
    endif()
  endforeach()
message("================================================================================")
endfunction()

function(SET_BUILD_NAME_FOR_ALL_MODULES)
  foreach(EACH_MODULE ${ALL_MODULE_LIST})
    set(${EACH_MODULE}_BUILD_NAME "${CTEST_BUILD_NAME} (${CTEST_BUILD_CONFIGURATION}-${${EACH_MODULE}_CURRENT_BRANCH} v${SCRIPT_VERSION})" PARENT_SCOPE)
  endforeach()
endfunction()
###############################################################################
# TEST                                                                        #
###############################################################################
GET_CURRENT_BRANCH_FOR_ALL_MODULES()
SET_BUILD_NAME_FOR_ALL_MODULES()

if("${DASHBOARD_MODEL}" STREQUAL "Continuous") ### DASHBOARD_MODEL Continuous
  while(1)
    message("Starting Continuous test now...")
    foreach(EACH_MODULE ${ALL_MODULE_LIST})
      CHECK_UPDATE_STATUS_FOR_MODULE(${EACH_MODULE})
      if(${${EACH_MODULE}_UPDATED} GREATER 0)
        message("${EACH_MODULE} has changed; Will run tests for module and its dependants...")
        SET_FORCE_TEST_FOR_DEPENDANTS(${EACH_MODULE} 1)
        break()
      endif()
      RUN_TEST_ONCE(${EACH_MODULE})
      if(${${EACH_MODULE}_UPDATED} GREATER 0)
        message("${EACH_MODULE} has changed; starting over again...")
        SET_FORCE_TEST_FOR_DEPENDANTS(${EACH_MODULE} 1)
        break()
      endif()
    endforeach()
    CTEST_SLEEP(60)
  endwhile()
elseif("${DASHBOARD_MODEL}" STREQUAL "Nightly") ### DASHBOARD_MODEL Nightly
  message("Starting Nightly test now...")
  foreach(EACH_MODULE ${ALL_MODULE_LIST})
    SET_FORCE_TEST_FOR_ALL_MODULE(${EACH_MODULE} 1)
    RUN_TEST_ONCE(${EACH_MODULE})
  endforeach()
  message("Nightly test completed. Exiting the script now...")
elseif("${DASHBOARD_MODEL}" STREQUAL "Experimental") ### DASHBOARD_MODEL Experimental
  message("Starting Experimental test now...")
  foreach(EACH_MODULE ${ALL_MODULE_LIST})
    SET_FORCE_TEST_FOR_ALL_MODULE(${EACH_MODULE} 1)
    RUN_TEST_ONCE(${EACH_MODULE})
  endforeach()
  message("Experimental test completed. Exiting the script now...")
elseif("${DASHBOARD_MODEL}" STREQUAL "Weekly") ### DASHBOARD_MODEL Weekly Memory Check
  message("Starting Weekly Memory Check test now...")
  foreach(EACH_MODULE ${ALL_MODULE_LIST})
    CHECK_UPDATE_STATUS_FOR_MODULE(${EACH_MODULE})
  endforeach()
  message("Updated all Modules. Will start tests now...")
  foreach(EACH_MODULE ${ALL_MODULE_LIST})
    SET_FORCE_TEST_FOR_ALL_MODULE(${EACH_MODULE} 1)
    RUN_TEST_ONCE(${EACH_MODULE})
  endforeach()
  message("Weekly memory check test completed. Exiting the script now...")
endif()
