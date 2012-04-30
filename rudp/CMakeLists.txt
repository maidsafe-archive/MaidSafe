#==============================================================================#
#  Copyright 2012 MaidSafe.net limited                                         #
#                                                                              #
#   The following source code is property of MaidSafe.net limited and is not   #
#   meant for external use.  The use of this code is governed by the licence   #
#   file licence.txt found in the root of this directory and also on           #
#   www.maidsafe.net.                                                          #
#                                                                              #
#   You are not free to copy, amend or otherwise use this source code without  #
#   the explicit written permission of the board of directors of maidsafe.net. #
#==============================================================================#


SET(MS_PROJECT_NAME maidsafe_rudp)

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.4 FATAL_ERROR)
MESSAGE("================================================================================\n")

GET_FILENAME_COMPONENT(CMAKE_BUILD_TYPE_DIR ${CMAKE_BINARY_DIR} NAME)

# Variable MSVC is not set until after "PROJECT" command below
IF(CMAKE_BUILD_TYPE_DIR MATCHES "Win_MSVC")
  SET(CMAKE_BUILD_TYPE_DIR Release)
ELSEIF(CMAKE_BUILD_TYPE_DIR MATCHES "kdev")
  SET(CMAKE_BUILD_TYPE_DIR Debug)
  SET(KDEV 1)
ENDIF()

IF(NOT ${CMAKE_C_COMPILER_WORKS})
  MESSAGE(FATAL_ERROR "No generator previously specified.\nTo see a full list of generators, run:\n\tcmake --help\n")
ENDIF()

# This "PROJECT" command resets CMAKE_BUILD_TYPE, hence it's copied from CMAKE_BUILD_TYPE_DIR which is set above.
IF((CMAKE_BUILD_TYPE_DIR MATCHES "Release") OR (DEFINED ${KDEV}))
  PROJECT(${MS_PROJECT_NAME})
ELSE()
  STRING(TOLOWER ${CMAKE_BUILD_TYPE_DIR} MS_PROJECT_NAME_APPENDIX)
  PROJECT(${MS_PROJECT_NAME}_${MS_PROJECT_NAME_APPENDIX})
ENDIF()

SET(CMAKE_BUILD_TYPE ${CMAKE_BUILD_TYPE_DIR} CACHE INTERNAL "Except for MSVC, the build type is set by running cmake from the appropriate build subdirectory e.g. for a Debug build, \"cd build/Linux/Debug && cmake ../../..\"." FORCE)


###################################################################################################
# MaidSafe Common library search                                                                  #
###################################################################################################
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/build)
INCLUDE(maidsafe_find_common)


###################################################################################################
# Standard setup                                                                                  #
###################################################################################################
INCLUDE(maidsafe_standard_setup)
HANDLE_VERSIONS(${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/version.h)


###################################################################################################
# Set up all files as GLOBs                                                                       #
###################################################################################################
####    CODE    ####
FILE(GLOB SOURCE_FILES_RUDP_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/*.cc")
FILE(GLOB HEADER_FILES_RUDP_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/*.h")
SET(ALL_FILES_RUDP_DIR ${SOURCE_FILES_RUDP_DIR} ${HEADER_FILES_RUDP_DIR})
SOURCE_GROUP("Main Source Files" FILES ${SOURCE_FILES_RUDP_DIR})
SOURCE_GROUP("Main Header Files" FILES ${HEADER_FILES_RUDP_DIR})

FILE(GLOB SOURCE_FILES_RUDP_CORE_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/core/*.cc")
FILE(GLOB HEADER_FILES_RUDP_CORE_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/core/*.h")
SET(ALL_FILES_RUDP_CORE_DIR ${SOURCE_FILES_RUDP_CORE_DIR} ${HEADER_FILES_RUDP_CORE_DIR})
SOURCE_GROUP("Core Source Files" FILES ${SOURCE_FILES_RUDP_CORE_DIR})
SOURCE_GROUP("Core Header Files" FILES ${HEADER_FILES_RUDP_CORE_DIR})

FILE(GLOB SOURCE_FILES_RUDP_OPERATIONS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/operations/*.cc")
FILE(GLOB HEADER_FILES_RUDP_OPERATIONS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/operations/*.h")
SET(ALL_FILES_RUDP_OPERATIONS_DIR ${SOURCE_FILES_RUDP_OPERATIONS_DIR} ${HEADER_FILES_RUDP_OPERATIONS_DIR})
SOURCE_GROUP("Operations Source Files" FILES ${SOURCE_FILES_RUDP_OPERATIONS_DIR})
SOURCE_GROUP("Operations Header Files" FILES ${HEADER_FILES_RUDP_OPERATIONS_DIR})

FILE(GLOB SOURCE_FILES_RUDP_PACKETS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/packets/*.cc")
FILE(GLOB HEADER_FILES_RUDP_PACKETS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/packets/*.h")
SET(ALL_FILES_RUDP_PACKETS_DIR ${SOURCE_FILES_RUDP_PACKETS_DIR} ${HEADER_FILES_RUDP_PACKETS_DIR})
SOURCE_GROUP("Packets Source Files" FILES ${SOURCE_FILES_RUDP_PACKETS_DIR})
SOURCE_GROUP("Packets Header Files" FILES ${HEADER_FILES_RUDP_PACKETS_DIR})

SET(ALL_FILES
    ${ALL_FILES_RUDP_DIR}
    ${ALL_FILES_RUDP_CORE_DIR}
    ${ALL_FILES_RUDP_OPERATIONS_DIR}
    ${ALL_FILES_RUDP_PACKETS_DIR})
SET(STYLE_RUDP ${ALL_FILES})

####    TESTS    ####
FILE(GLOB SOURCE_FILES_RUDP_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/tests/*.cc")
FILE(GLOB HEADER_FILES_RUDP_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/tests/*.h")
SET(ALL_FILES_RUDP_TESTS_DIR ${SOURCE_FILES_RUDP_TESTS_DIR} ${HEADER_FILES_RUDP_TESTS_DIR})
SOURCE_GROUP("Main Test Source Files" FILES ${SOURCE_FILES_RUDP_TESTS_DIR})
SOURCE_GROUP("Main Test Header Files" FILES ${HEADER_FILES_RUDP_TESTS_DIR})

FILE(GLOB SOURCE_FILES_RUDP_CORE_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/core/tests/*.cc")
FILE(GLOB HEADER_FILES_RUDP_CORE_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/core/tests/*.h")
SET(ALL_FILES_RUDP_CORE_TESTS_DIR ${SOURCE_FILES_RUDP_CORE_TESTS_DIR} ${HEADER_FILES_RUDP_CORE_TESTS_DIR})
SOURCE_GROUP("Core Test Source Files" FILES ${SOURCE_FILES_RUDP_CORE_TESTS_DIR})
SOURCE_GROUP("Core Test Header Files" FILES ${HEADER_FILES_RUDP_CORE_TESTS_DIR})

FILE(GLOB SOURCE_FILES_RUDP_OPERATIONS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/operations/tests/*.cc")
FILE(GLOB HEADER_FILES_RUDP_OPERATIONS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/operations/tests/*.h")
SET(ALL_FILES_RUDP_OPERATIONS_TESTS_DIR ${SOURCE_FILES_RUDP_OPERATIONS_TESTS_DIR} ${HEADER_FILES_RUDP_OPERATIONS_TESTS_DIR})
SOURCE_GROUP("Operations Test Source Files" FILES ${SOURCE_FILES_RUDP_OPERATIONS_TESTS_DIR})
SOURCE_GROUP("Operations Test Header Files" FILES ${HEADER_FILES_RUDP_OPERATIONS_TESTS_DIR})

FILE(GLOB SOURCE_FILES_RUDP_PACKETS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/packets/tests/*.cc")
FILE(GLOB HEADER_FILES_RUDP_PACKETS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/packets/tests/*.h")
SET(ALL_FILES_RUDP_PACKETS_TESTS_DIR ${SOURCE_FILES_RUDP_PACKETS_TESTS_DIR} ${HEADER_FILES_RUDP_PACKETS_TESTS_DIR})
SOURCE_GROUP("Packets Test Source Files" FILES ${SOURCE_FILES_RUDP_PACKETS_TESTS_DIR})
SOURCE_GROUP("Packets Test Header Files" FILES ${HEADER_FILES_RUDP_PACKETS_TESTS_DIR})

SET(ALL_TEST_FILES
    ${ALL_FILES_RUDP_TESTS_DIR}
    ${ALL_FILES_RUDP_CORE_TESTS_DIR}
#     ${ALL_FILES_RUDP_OPERATIONS_TESTS_DIR}
    ${ALL_FILES_RUDP_PACKETS_TESTS_DIR})
SET(STYLE_RUDP ${STYLE_RUDP} ${ALL_TEST_FILES})


###################################################################################################
# Define MaidSafe libraries and executables                                                       #
###################################################################################################
MS_ADD_STATIC_LIBRARY(${MS_PROJECT_NAME} ${ALL_FILES})
MS_ADD_EXECUTABLE(TESTrudp Tests ${ALL_TEST_FILES})

TARGET_LINK_LIBRARIES(maidsafe_rudp_static glog_static maidsafe_common_static)
TARGET_LINK_LIBRARIES(TESTrudp maidsafe_rudp_static gtest_static gmock_static)

###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
INCLUDE(maidsafe_standard_flags)
MS_DISABLE_WARNINGS(${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/messages.pb.cc)


###################################################################################################
# Tests                                                                                           #
###################################################################################################
ADD_STYLE_TEST(${STYLE_RUDP})
ADD_GTESTS(TESTrudp)
TEST_SUMMARY_OUTPUT()

# Remove files from coverage stats
ADD_COVERAGE_EXCLUDE(messages.pb.cc)


###################################################################################################
# Install details                                                                                 #
###################################################################################################
FINAL_MESSAGE()
RENAME_OUTDATED_BUILT_EXES()

SET(MAIDSAFE_RUDP_INSTALL_FILES
      ${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/managed_connections.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/version.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/parameters.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/rudp/return_codes.h)

MS_INSTALL_LIBS(maidsafe_rudp_static)
MS_INSTALL_HEADERS(rudp ${MAIDSAFE_RUDP_INSTALL_FILES})
MS_INSTALL_EXPORT()
