SET(SCRIPT_VERSION 15)
FILE(STRINGS "${CTEST_SCRIPT_DIRECTORY}/${CTEST_SCRIPT_NAME}" INSTALLED_VERSION_INFO LIMIT_COUNT 1)
STRING(REPLACE " " ";" INSTALLED_VERSION_INFO ${INSTALLED_VERSION_INFO})
LIST(GET INSTALLED_VERSION_INFO 1 INSTALLED_VERSION)
STRING(REPLACE ")" "" INSTALLED_VERSION ${INSTALLED_VERSION})
IF(NOT ${SCRIPT_VERSION} EQUAL ${INSTALLED_VERSION})
   MESSAGE("New installed Version: ${INSTALLED_VERSION}")
   MESSAGE("Current running version: ${SCRIPT_VERSION}")
   MESSAGE(FATAL_ERROR "This script is updated. Start the script again !!!")
ENDIF()
MESSAGE("Starting Script version: ${SCRIPT_VERSION}")

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
SET(ALL_MODULE_LIST
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

MESSAGE("================================================================================")
MESSAGE("Modules Selected For the test:")
FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
  MESSAGE("${EACH_MODULE}")
ENDFOREACH()
MESSAGE("================================================================================")

###############################################################################
# Module configurations                                                       #
###############################################################################
#Module folder-name/path relative to TEST_ROOT_DIRECTORY
SET(MAIDSAFE_COMMON "MaidSafe-Common/maidsafe_common_lib")
SET(MAIDSAFE_PRIVATE "MaidSafe-Private")
SET(MAIDSAFE_TRANSPORT "MaidSafe-Transport")
SET(MAIDSAFE_BREAKPAD "MaidSafe-Breakpad")
SET(MAIDSAFE_ENCRYPT "MaidSafe-Encrypt")
SET(MAIDSAFE_DHT "MaidSafe-DHT")
SET(MAIDSAFE_PKI "MaidSafe-PKI")
SET(MAIDSAFE_PASSPORT "MaidSafe-Passport")
SET(MAIDSAFE_PD "MaidSafe-PD")
SET(FILE_BROWSER "File-Browser")
SET(LIFESTUFF "LifeStuff")
SET(LIFESTUFF_GUI "LifeStuff-GUI")
SET(MAIDSAFE_DRIVE "MaidSafe-Drive")
SET(SIGMOID_CORE "SigmoidCore")
SET(SIGMOID_PRO "SigmoidPro")
SET(SIGMOID_ULTIMATE "SigmoidUltimate")
SET(DEDUPLICATOR_GAUGE "Deduplicator-Gauge")
#List the Module if needs to be installed
SET(MAIDSAFE_COMMON_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_PRIVATE_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_TRANSPORT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_DHT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_ENCRYPT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_PKI_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_PASSPORT_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_PD_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(FILE_BROWSER_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(LIFESTUFF_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)
SET(MAIDSAFE_DRIVE_SHOULD_BE_INSTALLED_AFTER_UPDATE 1)

#List of dependent modules which will be tested for a given module
SET(MAIDSAFE_COMMON_DEPENDANTS
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
SET(MAIDSAFE_PRIVATE_DEPENDANTS
    MAIDSAFE_PRIVATE
    MAIDSAFE_ENCRYPT
    MAIDSAFE_PD
    MAIDSAFE_DRIVE
    LIFESTUFF
    LIFESTUFF_GUI
    SIGMOID_PRO
    )
SET(MAIDSAFE_TRANSPORT_DEPENDANTS
    MAIDSAFE_TRANSPORT
    MAIDSAFE_BREAKPAD
    MAIDSAFE_DHT
    MAIDSAFE_PD
    LIFESTUFF
    LIFESTUFF_GUI
    SIGMOID_PRO
    )
SET(MAIDSAFE_ENCRYPT_DEPENDANTS
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DRIVE
    SIGMOID_CORE
    SIGMOID_PRO
    LIFESTUFF
    LIFESTUFF_GUI
    )
SET(MAIDSAFE_DHT_DEPENDANTS
    MAIDSAFE_DHT
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_PD
    SIGMOID_PRO
    LIFESTUFF
    LIFESTUFF_GUI
    )
SET(MAIDSAFE_PKI_DEPENDANTS
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_PD
    LIFESTUFF
    LIFESTUFF_GUI
    )
SET(MAIDSAFE_PASSPORT_DEPENDANTS
    MAIDSAFE_PASSPORT
    LIFESTUFF
    LIFESTUFF_GUI
    )
SET(MAIDSAFE_PD_DEPENDANTS
    MAIDSAFE_PD
    SIGMOID_PRO
    LIFESTUFF
    LIFESTUFF_GUI
    )
SET(FILE_BROWSER_DEPENDANTS
    FILE_BROWSER
    LIFESTUFF
    LIFESTUFF_GUI
    )
SET(LIFESTUFF_DEPENDANTS
    LIFESTUFF
    LIFESTUFF_GUI
    )
SET(LIFESTUFF_GUI_DEPENDANTS
    LIFESTUFF_GUI
    )
SET(MAIDSAFE_DRIVE_DEPENDANTS
    MAIDSAFE_DRIVE
    SIGMOID_CORE
    SIGMOID_PRO
    )
SET(SIGMOID_CORE_DEPENDANTS
    SIGMOID_CORE
    )
SET(SIGMOID_PRO_DEPENDANTS
    SIGMOID_PRO
    )
SET(DEDUPLICATOR_GAUGE_DEPENDANTS
    DEDUPLICATOR_GAUGE
    )

# List of Dependencies per module
SET(MAIDSAFE_COMMON_DEPENDS_ON
    )
SET(MAIDSAFE_PRIVATE_DEPEND_ON
    MAIDSAFE_COMMON
    )
SET(MAIDSAFE_TRANSPORT_DEPEND_ON
    MAIDSAFE_COMMON
    )
SET(MAIDSAFE_BREAKPAD_DEPEND_ON
    MAIDSAFE_COMMON
    MAIDSAFE_TRANSPORT
    )
SET(MAIDSAFE_ENCRYPT_DEPEND_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    )
SET(MAIDSAFE_DHT_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_TRANSPORT
    )
SET(MAIDSAFE_PKI_DEPENDS_ON
    MAIDSAFE_COMMON
    )
SET(MAIDSAFE_PASSPORT_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PKI
    )
SET(MAIDSAFE_PD_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_DHT
    MAIDSAFE_TRANSPORT
    MAIDSAFE_PKI
    )
SET(FILE_BROWSER_DEPENDS_ON
    )
SET(LIFESTUFF_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_ENCRYPT
    MAIDSAFE_TRANSPORT
    MAIDSAFE_DHT
    MAIDSAFE_PKI
    MAIDSAFE_PASSPORT
    MAIDSAFE_DRIVE
    )
SET(LIFESTUFF_GUI_DEPENDS_ON
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
SET(MAIDSAFE_DRIVE_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_PRIVATE
    MAIDSAFE_ENCRYPT
    )
SET(SIGMOID_CORE_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DRIVE
    )
SET(SIGMOID_PRO_DEPENDS_ON
    MAIDSAFE_COMMON
    MAIDSAFE_ENCRYPT
    MAIDSAFE_DHT
    MAIDSAFE_TRANSPORT
    MAIDSAFE_DRIVE
    )
SET(DEDUPLICATOR_GAUGE_DEPENDS_ON
    )

###############################################################################
# Variable(s) determined after running cmake                                  #
###############################################################################
SET(CTEST_CMAKE_GENERATOR "@CMAKE_GENERATOR@")

# Modules supported for Makefile generators only.
#IF(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "Make")
#  LIST(REMOVE_ITEM ALL_MODULE_LIST "SIGMOID_CORE" "SIGMOID_PRO" "MAIDSAFE_DRIVE")
#ENDIF()

###############################################################################
# Test configurations                                                         #
###############################################################################
IF(NOT "${CTEST_SCRIPT_ARG}" MATCHES "^(Nightly|NightlyDebug|NightlyRelease|NightlyMemCheck|NightlyDebugMemCheck|NightlyReleaseMemCheck|Experimental|ExperimentalDebug|ExperimentalRelease|ExperimentalMemCheck|ExperimentalDebugMemCheck|ExperimentalReleaseMemCheck|Continuous|ContinuousDebug|ContinuousRelease|ContinuousMemCheck|ContinuousDebugMemCheck|ContinuousReleaseMemCheck|Weekly|WeeklyRelease|WeeklyDebug)$")
  MESSAGE(FATAL_ERROR "Allowed arguments are Nightly, Experimental, Continuous, Weekly,
                       NightlyDebug, NightlyRelease,NightlyMemCheck,NightlyDebugMemCheck,NightlyReleaseMemCheck,
                       ExperimentalDebug, ExperimentalRelease,ExperimentalMemCheck,ExperimentalDebugMemCheck,ExperimentalReleaseMemCheck,
                       ContinuousDebug, ContinuousRelease,ContinuousMemCheck,ContinuousDebugMemCheck,ContinuousReleaseMemCheck,
                       WeeklyRelease & WeeklyDebug
                       \n eg. ctest -S ${CTEST_SCRIPT_NAME},NightlyDebug")
ENDIF()

# Select the model (Nightly, Experimental, Continuous & Weekly).
IF(${CTEST_SCRIPT_ARG} MATCHES Continuous)
  SET(DASHBOARD_MODEL Continuous)
ELSEIF(${CTEST_SCRIPT_ARG} MATCHES Nightly)
  SET(DASHBOARD_MODEL Nightly)
ELSEIF(${CTEST_SCRIPT_ARG} MATCHES Experimental)
  SET(DASHBOARD_MODEL Experimental)
ELSEIF(${CTEST_SCRIPT_ARG} MATCHES Weekly)
  SET(DASHBOARD_MODEL Weekly)
ENDIF()

IF(NOT DEFINED DASHBOARD_MODEL)
  SET(DASHBOARD_MODEL Experimental)  #default to "Experimental"
ENDIF()

IF(${CTEST_SCRIPT_ARG} MATCHES Debug)
  SET(CTEST_BUILD_CONFIGURATION "Debug")
ELSEIF(${CTEST_SCRIPT_ARG} MATCHES Release)
  SET(CTEST_BUILD_CONFIGURATION "Release")
ENDIF()

IF(NOT DEFINED CTEST_BUILD_CONFIGURATION)
  SET(CTEST_BUILD_CONFIGURATION "Debug")  #default to "Debug"
ENDIF()

IF(${CTEST_SCRIPT_ARG} MATCHES MemCheck OR ${CTEST_SCRIPT_ARG} MATCHES Weekly)
  SET(MEMORY_CHECK_NEEDED TRUE)
ELSE()
  SET(MEMORY_CHECK_NEEDED FALSE)
ENDIF()

#SET(CTEST_BUILD_OPTIONS "-DWITH_SSH1=ON -WITH_SFTP=ON -DWITH_SERVER=ON -DWITH_ZLIB=ON -DWITH_PCAP=ON -DWITH_GCRYPT=OFF")
MESSAGE("Dashboard Model Selected:      ${DASHBOARD_MODEL}")
MESSAGE("Build Configuration Selected:  ${CTEST_BUILD_CONFIGURATION}")
MESSAGE("================================================================================")

FIND_FILE(DART_CONFIG NAMES DartConfiguration.tcl
             PATHS "${CTEST_SCRIPT_DIRECTORY}/../../../../../MaidSafe-Common/maidsafe_common_lib/build"
             PATH_SUFFIXES Win_MSVC Linux/Debug Linux/Release Linux/RelWithDebInfo Linux/MinSizeRel OSX/Debug OSX/Release OSX/RelWithDebInfo OSX/MinSizeRel
             NO_DEFAULT_PATH)
FILE(STRINGS ${DART_CONFIG} DART_CONFIG_CONTENTS REGEX "BuildName: ")
STRING(REPLACE "BuildName: " "" CTEST_BUILD_NAME ${DART_CONFIG_CONTENTS})
#SET(CTEST_BUILD_NAME "${CTEST_BUILD_NAME} (${CTEST_BUILD_CONFIGURATION} v${SCRIPT_VERSION})")

###############################################################################
# Utility functions                                                           #
###############################################################################
FUNCTION(SET_FORCE_TEST_FOR_ALL_MODULE VALUE)
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    SET(${EACH_MODULE}_NEEDS_FORCE_TEST ${VALUE} PARENT_SCOPE)
  ENDFOREACH()
ENDFUNCTION()

FUNCTION(SET_FORCE_TEST_FOR_DEPENDANTS MODULE_NAME VALUE)
  FOREACH(EACH_MODULE ${${MODULE_NAME}_DEPENDANTS})
    SET(${EACH_MODULE}_NEEDS_FORCE_TEST ${VALUE} PARENT_SCOPE)
  ENDFOREACH()
ENDFUNCTION()

###############################################################################
# Finding Modules Paths & setting initial update status                       #
###############################################################################
## TEST_ROOT_DIRECTORY ##
SET(MAIDSAFE_TEST_ROOT_DIRECTORY ${CTEST_SCRIPT_DIRECTORY}/../../../../../)

FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
  SET(${EACH_MODULE}_SOURCE_DIRECTORY ${MAIDSAFE_TEST_ROOT_DIRECTORY}/${${EACH_MODULE}})
  IF(WIN32)
    SET(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/Win_MSVC)
  ELSEIF(APPLE)
    SET(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/OSX/${CTEST_BUILD_CONFIGURATION})
  ELSEIF(UNIX)
    SET(${EACH_MODULE}_BINARY_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}/build/Linux/${CTEST_BUILD_CONFIGURATION})
  ENDIF()

  SET(${EACH_MODULE}_UPDATED 0)
  SET_FORCE_TEST_FOR_DEPENDANTS(${EACH_MODULE} 0)

  IF(EXISTS ${${EACH_MODULE}_SOURCE_DIRECTORY})
    MESSAGE("Found ${EACH_MODULE} at "${${EACH_MODULE}_SOURCE_DIRECTORY})
  ELSE()
    MESSAGE(FATAL_ERROR "Unable to find ${EACH_MODULE} source directory: ${${EACH_MODULE}_SOURCE_DIRECTORY}")
  ENDIF()

  IF(EXISTS ${${EACH_MODULE}_BINARY_DIRECTORY})
    MESSAGE("Found ${EACH_MODULE} Binary Directory at "${${EACH_MODULE}_BINARY_DIRECTORY})
  ELSE()
    MESSAGE(FATAL_ERROR "Unable to find ${EACH_MODULE} binary directory")
  ENDIF()
ENDFOREACH()

MESSAGE("================================================================================")

###############################################################################
#Finding hostname                                                             #
###############################################################################
FIND_PROGRAM(HOSTNAME_CMD NAMES hostname)
EXEC_PROGRAM(${HOSTNAME_CMD} ARGS OUTPUT_VARIABLE HOSTNAME)
SET(CTEST_SITE "${HOSTNAME}")
MESSAGE("Hostname: " ${HOSTNAME})

###############################################################################
# Finding Programs & Commands                                                 #
###############################################################################
FIND_PROGRAM(CTEST_CMAKE_COMMAND NAMES cmake)
IF(NOT CTEST_CMAKE_COMMAND)
  MESSAGE(FATAL_ERROR "Couldn't find CMake executable. Specify path of CMake executable. \n e.g. -DCTEST_CMAKE_COMMAND=\"C:/Program Files/CMake 2.8/bin/cmake.exe\"")
ENDIF()
MESSAGE("-- Found CMake")

SET(CMAKE_MODULE_PATH ${CTEST_SCRIPT_DIRECTORY})
INCLUDE(maidsafe_find_git)
SET(CTEST_UPDATE_COMMAND "${Git_EXECUTABLE}")

IF(NOT "${CTEST_CMAKE_GENERATOR}" MATCHES "Make")
  # Launchers work only with Makefile generators.
  SET(CTEST_USE_LAUNCHERS 0)
ELSEIF(NOT DEFINED CTEST_USE_LAUNCHERS)
  SET(CTEST_USE_LAUNCHERS 1)
ENDIF()

###############################################################################
# Configure CTest                                                             #
###############################################################################
SET(BASIC_CTEST_CONFIGURE_COMMAND "${CMAKE_COMMAND} -DCMAKE_BUILD_TYPE:STRING=${CTEST_BUILD_CONFIGURATION} -G \"${CTEST_CMAKE_GENERATOR}\"")
SET(CTEST_CONFIGURATION_TYPE ${CTEST_BUILD_CONFIGURATION})

FIND_PROGRAM(CTEST_COVERAGE_COMMAND NAMES gcov)
IF(CTEST_COVERAGE_COMMAND)
  SET(COVERAGE_FLAG_ON "-DCOVERAGE=ON")
  SET(COVERAGE_FLAG_OFF "-DCOVERAGE=OFF")
ENDIF()

FIND_PROGRAM(CTEST_MEMORYCHECK_COMMAND
  NAMES purify valgrind boundscheck
  PATHS
  "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Rational Software\\Purify\\Setup;InstallFolder]"
  DOC "Path to the memory checking command, used for memory error detection."
  )
SET(CTEST_MEMORYCHECK_COMMAND_OPTIONS "--trace-children=yes --quiet --tool=memcheck --leak-check=yes --show-reachable=yes --num-callers=50 --verbose --demangle=yes")
IF(CTEST_MEMORYCHECK_COMMAND)
  SET(MEMORYCHECK_FLAG_ON "-DMEMORY_CHECK=ON")
  SET(MEMORYCHECK_FLAG_OFF "-DMEMORY_CHECK=OFF")
  MESSAGE("-- Found MemCheck program")
ELSEIF(MEMORY_CHECK_NEEDED)
  MESSAGE("-- Unable to find any memory check program. Will skip memory checks")
ENDIF()

MESSAGE("================================================================================")

# Avoid non-ascii characters in tool output.
SET(ENV{LC_ALL} C)

###############################################################################
# CTest Utility Functions                                                     #
###############################################################################
FUNCTION(CHECK_UPDATE_STATUS_FOR_MODULE MODULE_NAME)
  SET(MODULE_SOURCE_DIRECTORY ${${MODULE_NAME}_SOURCE_DIRECTORY})
  SET(MODULE_BINARY_DIRECTORY ${${MODULE_NAME}_BINARY_DIRECTORY})
  MESSAGE("  Checking updates for " ${MODULE_NAME})
  SET(CTEST_BUILD_NAME ${${MODULE_NAME}_BUILD_NAME})
  SET(CTEST_SOURCE_DIRECTORY ${MODULE_SOURCE_DIRECTORY})
  SET(CTEST_BINARY_DIRECTORY ${MODULE_BINARY_DIRECTORY})
  CTEST_START(${DASHBOARD_MODEL} TRACK "${DASHBOARD_MODEL}")
  CTEST_UPDATE(SOURCE ${MODULE_SOURCE_DIRECTORY} RETURN_VALUE UPDATED_COUNT)
  SET(${MODULE_NAME}_UPDATED ${UPDATED_COUNT} PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(RUN_TEST_ONCE MODULE_NAME)
  IF(${${MODULE_NAME}_NEEDS_FORCE_TEST} EQUAL 0)
    RETURN()
  ENDIF()
  SET(MODULE_SOURCE_DIRECTORY ${${MODULE_NAME}_SOURCE_DIRECTORY})
  SET(MODULE_BINARY_DIRECTORY ${${MODULE_NAME}_BINARY_DIRECTORY})
  MESSAGE("Running Test for " ${MODULE_NAME})
  SET(CTEST_SOURCE_DIRECTORY ${MODULE_SOURCE_DIRECTORY})
  SET(CTEST_BINARY_DIRECTORY ${MODULE_BINARY_DIRECTORY})

  SET(CTEST_BUILD_NAME ${${MODULE_NAME}_BUILD_NAME})
  SET(CTEST_CONFIGURE_COMMAND "${BASIC_CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\" ${COVERAGE_FLAG_ON} ${MEMORYCHECK_FLAG_OFF}")

  UNSET(CTEST_CUSTOM_COVERAGE_EXCLUDE)
  UNSET(CTEST_CUSTOM_MEMCHECK_IGNORE)

  CTEST_START(${DASHBOARD_MODEL} TRACK "${DASHBOARD_MODEL}")
  CTEST_READ_CUSTOM_FILES(${CTEST_BINARY_DIRECTORY})
  CTEST_UPDATE(RETURN_VALUE UPDATED_COUNT)
  SET(${EACH_MODULE}_UPDATED ${UPDATED_COUNT} PARENT_SCOPE)
  IF(${${EACH_MODULE}_UPDATED} GREATER 0)
    RETURN()
  ENDIF()

  CTEST_CONFIGURE(RETURN_VALUE RETURNED)
  IF(NOT ${RETURNED} EQUAL 0)
    MESSAGE("  CTEST_CONFIGURE failed ret: ${RETURNED}")
    MESSAGE("  Will try again after re-installing dependencies")
    # Retry Configure after re-installing dependencies
    INSTALL_DEPENDENCIES(${MODULE_NAME})
    CTEST_CONFIGURE(RETURN_VALUE RETURNED)
    IF(NOT ${RETURNED} EQUAL 0)
      MESSAGE("  CTEST_CONFIGURE failed after re-installing dependencies. ret: ${RETURNED}")
      MESSAGE("  ERROR : Skipping build for ${MODULE_NAME}")
      #Submitting configure failure results to cdash
      CTEST_SUBMIT()
      SET(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
      SET(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
      RETURN()
    ELSE()
      MESSAGE("  Configured ${MODULE_NAME} after re-installing dependencies.")
    ENDIF()
  ELSE()
    MESSAGE("  Configured ${MODULE_NAME}.")
  ENDIF()

  #clean up module
  EXECUTE_PROCESS(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
      COMMAND ${CTEST_CMAKE_COMMAND} --build . --target clean --config ${CTEST_BUILD_CONFIGURATION}
      RESULT_VARIABLE ret_var
      OUTPUT_VARIABLE out_var
      )
  IF(NOT ${ret_var} EQUAL 0)
    MESSAGE("  Cleaning ${MODULE_NAME} returned ${ret_var}.Output:${out_var}")
    MESSAGE("  ERROR : Skipping test for ${MODULE_NAME}")
    SET(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
    SET(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
    RETURN()
  ENDIF()
  MESSAGE("  Cleaned ${MODULE_NAME}.")

  CTEST_BUILD(RETURN_VALUE RETURNED)
  IF(NOT ${RETURNED} EQUAL 0)
    #Submitting build failure results to cdash
    CTEST_SUBMIT()
    MESSAGE("  CTEST_BUILD failed ret: ${RETURNED}")

    SET(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
    RETURN()
  ENDIF()
  MESSAGE("  Built ${MODULE_NAME}.")

  CTEST_TEST()
  MESSAGE("  Ran all tests for ${MODULE_NAME}.")

  CTEST_SUBMIT(PARTS Update Configure Build Test)

  IF(MEMORY_CHECK_NEEDED AND CTEST_MEMORYCHECK_COMMAND)
    MESSAGE("  Starting Memory check for ${MODULE_NAME}...")
    EXECUTE_PROCESS(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY} COMMAND ${CTEST_CMAKE_COMMAND} --build . --target clean --config ${CTEST_BUILD_CONFIGURATION})
    SET(CTEST_CONFIGURE_COMMAND "${BASIC_CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\" ${COVERAGE_FLAG_OFF} ${MEMORYCHECK_FLAG_ON}")
    CTEST_CONFIGURE()
    CTEST_BUILD()
    CTEST_MEMCHECK()
    CTEST_SUBMIT(PARTS MemCheck)
    SET(CTEST_CONFIGURE_COMMAND "${BASIC_CTEST_CONFIGURE_COMMAND} \"${CTEST_SOURCE_DIRECTORY}\" ${MEMORYCHECK_FLAG_OFF}")
    CTEST_CONFIGURE()
    MESSAGE("  Ran memory check for ${MODULE_NAME}.")
  ELSEIF(MEMORY_CHECK_NEEDED)
    MESSAGE("  ERROR : No memory check program found. Skipping Memory check for ${MODULE_NAME}.")
  ENDIF()

  IF(CTEST_COVERAGE_COMMAND)
    CTEST_COVERAGE()
    CTEST_SUBMIT(PARTS Coverage)
    MESSAGE("  Ran coverage for ${MODULE_NAME}.")
  ENDIF()

  #Installing if module needs installation
  IF(${MODULE_NAME}_SHOULD_BE_INSTALLED_AFTER_UPDATE)
    EXECUTE_PROCESS(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
        COMMAND ${CTEST_CMAKE_COMMAND} --build . --config ${CTEST_BUILD_CONFIGURATION} --target install
        RESULT_VARIABLE ret_var
        OUTPUT_VARIABLE out_var
        )
    IF(NOT ${ret_var} EQUAL 0)
      MESSAGE("  Installing ${MODULE_NAME} returned ${ret_var} Output:${out_var}")
      MESSAGE("  Skipping installation of ${MODULE_NAME}")
      SET(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
      SET(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
      RETURN()
    ELSE()
      MESSAGE("  Installed ${MODULE_NAME}.")
    ENDIF()
  ENDIF()
  SET(${MODULE_NAME}_UPDATED 0 PARENT_SCOPE)
  SET(${MODULE_NAME}_NEEDS_FORCE_TEST 0 PARENT_SCOPE)
ENDFUNCTION()

FUNCTION(INSTALL_DEPENDENCIES MODULE_NAME)
  FOREACH(EACH_MODULE ${${MODULE_NAME}_DEPENDS_ON})
    LIST(FIND ALL_MODULE_LIST ${EACH_MODULE} output)
    IF(NOT ${output} EQUAL -1)
      SET(MODULE_BINARY_DIRECTORY ${${EACH_MODULE}_BINARY_DIRECTORY})
      MESSAGE("Installing " ${EACH_MODULE})
      IF(${EACH_MODULE}_SHOULD_BE_INSTALLED_AFTER_UPDATE)
        EXECUTE_PROCESS(WORKING_DIRECTORY ${MODULE_BINARY_DIRECTORY}
          COMMAND ${CTEST_CMAKE_COMMAND} --build . --config ${CTEST_BUILD_CONFIGURATION} --target install
          RESULT_VARIABLE ret_var
          OUTPUT_VARIABLE out_var
          )
        IF(NOT ${ret_var} EQUAL 0)
          MESSAGE("  ERROR : Installing ${EACH_MODULE} returned ${ret_var} Output:${out_var}")
        ELSE()
          MESSAGE("  Installed ${EACH_MODULE}.")
        ENDIF()
      ENDIF()
    ENDIF()
  ENDFOREACH()
ENDFUNCTION()

FUNCTION(GET_CURRENT_BRANCH_FOR_ALL_MODULES)
  MESSAGE("  Current Branch of Modules :")
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    EXECUTE_PROCESS(WORKING_DIRECTORY ${${EACH_MODULE}_SOURCE_DIRECTORY}
          COMMAND ${Git_EXECUTABLE} status --short --branch
          RESULT_VARIABLE ret_var
          OUTPUT_VARIABLE out_var
          )
    IF(${ret_var} EQUAL 0)
      STRING(REGEX REPLACE "\n.*" "" CURRENT_BRANCH ${out_var})
      STRING(REGEX REPLACE "[.][.][.].*" "" CURRENT_BRANCH ${CURRENT_BRANCH})
      STRING(REGEX REPLACE "## " "" CURRENT_BRANCH ${CURRENT_BRANCH})
      MESSAGE("    ${EACH_MODULE} : ${CURRENT_BRANCH}")
      SET(${EACH_MODULE}_CURRENT_BRANCH ${CURRENT_BRANCH} PARENT_SCOPE)
    ELSEIF()
      SET(${EACH_MODULE}_CURRENT_BRANCH unknown PARENT_SCOPE)
    ENDIF()
  ENDFOREACH()
MESSAGE("================================================================================")
ENDFUNCTION()

FUNCTION(SET_BUILD_NAME_FOR_ALL_MODULES)
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    SET(${EACH_MODULE}_BUILD_NAME "${CTEST_BUILD_NAME} (${CTEST_BUILD_CONFIGURATION}-${${EACH_MODULE}_CURRENT_BRANCH} v${SCRIPT_VERSION})" PARENT_SCOPE)
  ENDFOREACH()
ENDFUNCTION()
###############################################################################
# TEST                                                                        #
###############################################################################
GET_CURRENT_BRANCH_FOR_ALL_MODULES()
SET_BUILD_NAME_FOR_ALL_MODULES()

IF("${DASHBOARD_MODEL}" STREQUAL "Continuous") ### DASHBOARD_MODEL Continuous
  WHILE(1)
    MESSAGE("Starting Continuous test now...")
    FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
      CHECK_UPDATE_STATUS_FOR_MODULE(${EACH_MODULE})
      IF(${${EACH_MODULE}_UPDATED} GREATER 0)
        MESSAGE("${EACH_MODULE} has changed; Will run tests for module and its dependants...")
        SET_FORCE_TEST_FOR_DEPENDANTS(${EACH_MODULE} 1)
        BREAK()
      ENDIF()
      RUN_TEST_ONCE(${EACH_MODULE})
      IF(${${EACH_MODULE}_UPDATED} GREATER 0)
        MESSAGE("${EACH_MODULE} has changed; starting over again...")
        SET_FORCE_TEST_FOR_DEPENDANTS(${EACH_MODULE} 1)
        BREAK()
      ENDIF()
    ENDFOREACH()
    CTEST_SLEEP(60)
  ENDWHILE()
ELSEIF("${DASHBOARD_MODEL}" STREQUAL "Nightly") ### DASHBOARD_MODEL Nightly
  MESSAGE("Starting Nightly test now...")
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    SET_FORCE_TEST_FOR_ALL_MODULE(${EACH_MODULE} 1)
    RUN_TEST_ONCE(${EACH_MODULE})
  ENDFOREACH()
  MESSAGE("Nightly test completed. Exiting the script now...")
ELSEIF("${DASHBOARD_MODEL}" STREQUAL "Experimental") ### DASHBOARD_MODEL Experimental
  MESSAGE("Starting Experimental test now...")
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    SET_FORCE_TEST_FOR_ALL_MODULE(${EACH_MODULE} 1)
    RUN_TEST_ONCE(${EACH_MODULE})
  ENDFOREACH()
  MESSAGE("Experimental test completed. Exiting the script now...")
ELSEIF("${DASHBOARD_MODEL}" STREQUAL "Weekly") ### DASHBOARD_MODEL Weekly Memory Check
  MESSAGE("Starting Weekly Memory Check test now...")
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    CHECK_UPDATE_STATUS_FOR_MODULE(${EACH_MODULE})
  ENDFOREACH()
  MESSAGE("Updated all Modules. Will start tests now...")
  FOREACH(EACH_MODULE ${ALL_MODULE_LIST})
    SET_FORCE_TEST_FOR_ALL_MODULE(${EACH_MODULE} 1)
    RUN_TEST_ONCE(${EACH_MODULE})
  ENDFOREACH()
  MESSAGE("Weekly memory check test completed. Exiting the script now...")
ENDIF()
