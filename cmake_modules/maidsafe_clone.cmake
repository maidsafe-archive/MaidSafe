#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 maidsafe.net limited                                     #
#  All rights reserved.                                                        #
#                                                                              #
#  Redistribution and use in source and binary forms, with or without          #
#  modification, are permitted provided that the following conditions are met: #
#                                                                              #
#      * Redistributions of source code must retain the above copyright        #
#        notice, this list of conditions and the following disclaimer.         #
#      * Redistributions in binary form must reproduce the above copyright     #
#        notice, this list of conditions and the following disclaimer in the   #
#        documentation and/or other materials provided with the distribution.  #
#      * Neither the name of the maidsafe.net limited nor the names of its     #
#        contributors may be used to endorse or promote products derived from  #
#        this software without specific prior written permission.              #
#                                                                              #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  #
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
#  POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Written by maidsafe.net team                                                #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Module used to clone MaidSafe and Sigmoid repositories on GitHub.           #
#                                                                              #
#  Modules to be cloned should be provided in variable ALL_REPOSITORIES.  Path #
#  to root directory of all clones should be set in REPOSITORIES_ROOT_DIR.     #
#                                                                              #
#==============================================================================#


################################################################################
# Helper functions                                                             #
################################################################################
FUNCTION(HELP_OUTPUT MSG)
  SET(HELP "${MSG}\nUsage:\n")
  SET(HELP "${HELP}  cmake -D REPOSITORIES_ROOT_DIR=\"<path to root dir of all clones>\" -P maidsafe_clone.cmake\n")
  SET(HELP "${HELP}Other options:\n")
  SET(HELP "${HELP}  -D ALL_REPOSITORIES=<semi-colon separated list of repos>\n")
  SET(HELP "${HELP}  -D BRANCH=<branch name> (master or next)\n")
  SET(HELP "${HELP}  -D NO_BUILD=ON (clone and checkout to branch only)\n")
  SET(HELP "${HELP}  -D GENERATOR=<generator name> (e.g. \"Unix Makefiles\")\n")
  SET(HELP "${HELP}Note: All -D options must be listed BEFORE -P maidsafe_clone.cmake\n")
  SET(HELP "${HELP}Examples:\n")
  SET(HELP "${HELP}  cmake -D ALL_REPOSITORIES=MaidSafe-Common\;MaidSafe-Encrypt -D REPOSITORIES_ROOT_DIR=\"/dev/nightly\" -P maidsafe_clone.cmake\n")
  SET(HELP "${HELP}  cmake -D BRANCH=next -D REPOSITORIES_ROOT_DIR=\"/dev/nightly\" -P maidsafe_clone.cmake\n")
  SET(HELP "${HELP}  cmake -D NO_BUILD=ON -D REPOSITORIES_ROOT_DIR=\"/dev/nightly\" -P maidsafe_clone.cmake\n")
  SET(HELP "${HELP}  cmake -D GENERATOR=\"CodeBlocks - Unix Makefiles\" -D REPOSITORIES_ROOT_DIR=\"/dev/nightly\" -P maidsafe_clone.cmake\n")
  SET(HELP "${HELP}  cmake -D REPOSITORIES_ROOT_DIR=\"/dev/nightly\" -P maidsafe_clone.cmake\n\n")
  MESSAGE(FATAL_ERROR ${HELP})
ENDFUNCTION()

FUNCTION(CONFIGURE_AND_MAKE_REPO REPO)
  MESSAGE(">> Configuring ${REPO}")
  IF(WIN32)
    IF(GENERATOR)
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -G ${GENERATOR} ../..
                      WORKING_DIRECTORY "${REPOSITORIES_ROOT_DIR}/${REPO}/${COMMON_BUILD_DIR}"
                      RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
    ELSE()
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} ../..
                      WORKING_DIRECTORY "${REPOSITORIES_ROOT_DIR}/${REPO}/${COMMON_BUILD_DIR}"
                      RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
    ENDIF()
    IF(NOT RES_VAR EQUAL 0)
      MESSAGE(">> Error configuring ${REPO}")
      MESSAGE("  ${ERR_VAR}")
      IF(${REPO} MATCHES MaidSafe-Common)
        MESSAGE(FATAL_ERROR "\n\n\nFailed to configure MaidSafe-Common.  Terminating.\n\n")
      ENDIF()
      RETURN()
    ENDIF()
  ELSE()
    IF(GENERATOR)
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} -G ${GENERATOR} ../../..
                      WORKING_DIRECTORY "${REPOSITORIES_ROOT_DIR}/${REPO}/${COMMON_BUILD_DIR}/Debug"
                      RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
    ELSE()
      EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} ../../..
                      WORKING_DIRECTORY "${REPOSITORIES_ROOT_DIR}/${REPO}/${COMMON_BUILD_DIR}/Debug"
                      RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
    ENDIF()
    IF(NOT RES_VAR EQUAL 0)
      MESSAGE(">> Error configuring ${REPO} in Debug mode.")
      MESSAGE("  ${ERR_VAR}")
      IF(${REPO} MATCHES MaidSafe-Common)
        MESSAGE(FATAL_ERROR "\n\n\nFailed to configure MaidSafe-Common.  Terminating.\n\n")
      ENDIF()
      RETURN()
    ENDIF()
    EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} ../../..
                    WORKING_DIRECTORY "${REPOSITORIES_ROOT_DIR}/${REPO}/${COMMON_BUILD_DIR}/Release"
                    RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
    IF(NOT RES_VAR EQUAL 0)
      MESSAGE(">> Error configuring ${REPO} in Release mode.")
      MESSAGE("  ${ERR_VAR}")
      IF(${REPO} MATCHES MaidSafe-Common)
        MESSAGE(FATAL_ERROR "\n\n\nFailed to configure MaidSafe-Common.  Terminating.\n\n")
      ENDIF()
      RETURN()
    ENDIF()
  ENDIF()

  MESSAGE(">> Building ${REPO}")
  IF(WIN32)
    SET(BUILD_DEBUG_DIR)
    SET(BUILD_RELEASE_DIR)
  ELSE()
    SET(BUILD_DEBUG_DIR Debug)
    SET(BUILD_RELEASE_DIR Release)
  ENDIF()

  LIST(FIND NON_INSTALL_REPOSITORIES ${REPO} NON_INSTALL)
  IF(NON_INSTALL EQUAL -1)
    SET(TARGET_INSTALL "install")
  ELSE()
    SET(TARGET_INSTALL "all")
  ENDIF()

  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} --build . --config Debug --target ${TARGET_INSTALL}
                  WORKING_DIRECTORY "${REPOSITORIES_ROOT_DIR}/${REPO}/${COMMON_BUILD_DIR}/${BUILD_DEBUG_DIR}"
                  RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
  # Use existence of export file as indicator of success as RESULT_VARIABLE is indeed variable for MSVC!
  FIND_FILE(COMMON_DEBUG_EXPORT_CMAKE NAMES maidsafe_common-debug.cmake PATHS ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe)
  IF(NOT COMMON_DEBUG_EXPORT_CMAKE)
    MESSAGE(">> Error installing ${REPO} in Debug mode.")
    MESSAGE("  ${ERR_VAR}")
    IF(${REPO} MATCHES MaidSafe-Common)
      FILE(REMOVE ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe/maidsafe_common.cmake
                  ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe/maidsafe_common-debug.cmake)
      MESSAGE(FATAL_ERROR "\n\n\nFailed to install MaidSafe-Common.  Terminating.\n\n")
    ENDIF()
  ENDIF()
  EXECUTE_PROCESS(COMMAND ${CMAKE_COMMAND} --build . --config Release --target ${TARGET_INSTALL}
                  WORKING_DIRECTORY "${REPOSITORIES_ROOT_DIR}/${REPO}/${COMMON_BUILD_DIR}/${BUILD_RELEASE_DIR}"
                  RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
  FIND_FILE(COMMON_RELEASE_EXPORT_CMAKE NAMES maidsafe_common-release.cmake PATHS ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe)
  IF(NOT COMMON_RELEASE_EXPORT_CMAKE)
    MESSAGE(">> Error installing ${REPO} in Release mode.")
    MESSAGE("  ${ERR_VAR}")
    IF(${REPO} MATCHES MaidSafe-Common)
      FILE(REMOVE ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe/maidsafe_common.cmake
                  ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe/maidsafe_common-debug.cmake
                  ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe/maidsafe_common-release.cmake)
      MESSAGE(FATAL_ERROR "\n\n\nFailed to install MaidSafe-Common.  Terminating.\n\n")
    ENDIF()
  ENDIF()
ENDFUNCTION()

################################################################################
# Start of script                                                              #
################################################################################
IF(NOT REPOSITORIES_ROOT_DIR)
  HELP_OUTPUT("\n\nYou must set REPOSITORIES_ROOT_DIR to the path of the root dir where the repos will be cloned to.")
ENDIF()

IF(NOT BRANCH)
  SET(BRANCH next)
ENDIF()
SET(ALLOWED_BRANCHES master next)
LIST(FIND ALLOWED_BRANCHES ${BRANCH} BRANCH_FOUND)
IF(BRANCH_FOUND EQUAL -1)
  HELP_OUTPUT("\n\n\"${BRANCH}\" is not a valid branch.  Only \"master\" or \"next\" is allowed.")
ENDIF()

IF(NOT ALL_REPOSITORIES)
  SET(ALL_REPOSITORIES
        MaidSafe-Common
        MaidSafe-Private
        MaidSafe-Transport
        MaidSafe-Encrypt
        MaidSafe-DHT
        MaidSafe-PKI
        MaidSafe-Passport
        MaidSafe-PD
        MaidSafe-Drive
        LifeStuff
        LifeStuff-GUI
        SigmoidCore
        SigmoidPro
        SigmoidUltimate
        MaidSafe-Breakpad
        MaidSafe-Documents
        Simulation
        Sandbox
#        AWS-Transporter
        )
ENDIF()

SET(NON_INSTALL_REPOSITORIES
      LifeStuff-GUI
      SigmoidCore
      SigmoidPro
      SigmoidUltimate
      MaidSafe-Breakpad
      MaidSafe-Documents
      Simulation
      Sandbox
      )


SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR})
INCLUDE(maidsafe_find_git)


################################################################################
# Clone repos and checkout to requested branch                                 #
################################################################################
FILE(MAKE_DIRECTORY ${REPOSITORIES_ROOT_DIR})
FOREACH(REPO ${ALL_REPOSITORIES})
  MESSAGE(">> Cloning ${REPO}")
  EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} clone git@github.com:maidsafe/${REPO}.git
                    WORKING_DIRECTORY ${REPOSITORIES_ROOT_DIR}
                    RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
  IF(NOT RES_VAR EQUAL 0)
    MESSAGE(">> Error cloning ${REPO}.  ${ERR_VAR}")
  ELSE()
    IF(NOT ${BRANCH} MATCHES "master")
      EXECUTE_PROCESS(COMMAND ${Git_EXECUTABLE} checkout ${BRANCH}
                      WORKING_DIRECTORY ${REPOSITORIES_ROOT_DIR}/${REPO}
                      RESULT_VARIABLE RES_VAR OUTPUT_VARIABLE OUT_VAR ERROR_VARIABLE ERR_VAR)
      IF(NOT RES_VAR EQUAL 0)
        MESSAGE(">> Error checking out to branch \"${BRANCH}\" for ${REPO}.  ${ERR_VAR}")
      ENDIF()
    ENDIF()
  ENDIF()
ENDFOREACH()

IF(NO_BUILD)
  RETURN()
ENDIF()


################################################################################
# Configure and install MaidSafe-Common                                        #
################################################################################
FIND_FILE(COMMON_EXPORT_CMAKE NAMES maidsafe_common.cmake PATHS ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/installed/share/maidsafe)
# If we found maidsafe_common.cmake, assume Common is correctly installed
IF(COMMON_EXPORT_CMAKE)
  MESSAGE(">> MaidSafe-Common already installed")
ELSE()
  FIND_FILE(COMMON_CMAKELISTS_TXT NAMES CMakeLists.txt PATHS ${REPOSITORIES_ROOT_DIR}/MaidSafe-Common/maidsafe_common_lib)
  IF(NOT COMMON_CMAKELISTS_TXT)
    MESSAGE(">> Can't find MaidSafe-Common/maidsafe_common_lib/CMakeLists.txt")
    RETURN()
  ENDIF()
  IF(WIN32)
    SET(COMMON_BUILD_DIR maidsafe_common_lib/build/Win_MSVC)
  ELSEIF(APPLE)
    SET(COMMON_BUILD_DIR maidsafe_common_lib/build/OSX)
  ELSEIF(UNIX)
    SET(COMMON_BUILD_DIR maidsafe_common_lib/build/Linux)
  ENDIF()
  CONFIGURE_AND_MAKE_REPO(MaidSafe-Common)
ENDIF()
LIST(REMOVE_ITEM ALL_REPOSITORIES "MaidSafe-Common")


################################################################################
# Configure and install others                                                 #
################################################################################
IF(WIN32)
  SET(COMMON_BUILD_DIR build/Win_MSVC)
ELSEIF(APPLE)
  SET(COMMON_BUILD_DIR build/OSX)
ELSEIF(UNIX)
  SET(COMMON_BUILD_DIR build/Linux)
ENDIF()

FOREACH(REPO ${ALL_REPOSITORIES})
  CONFIGURE_AND_MAKE_REPO(${REPO})
ENDFOREACH()
