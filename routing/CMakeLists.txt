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


SET(MS_PROJECT_NAME maidsafe_routing)

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
# MaidSafe Common & RUDP library search                                                      #
###################################################################################################
SET(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${PROJECT_SOURCE_DIR}/build)
INCLUDE(maidsafe_find_common)
ADD_MAIDSAFE_LIBRARY(MaidSafe-RUDP)


###################################################################################################
# Standard setup                                                                                  #
###################################################################################################
SET(PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
FILE(GLOB PROTO_FILES RELATIVE ${PROTO_SOURCE_DIR} "${PROTO_SOURCE_DIR}/maidsafe/routing/*.proto")
INCLUDE(maidsafe_standard_setup)
HANDLE_VERSIONS(${PROJECT_SOURCE_DIR}/src/maidsafe/routing/version.h)


###################################################################################################
# Set up all files as GLOBs                                                                       #
###################################################################################################
####    CODE    ####
FILE(GLOB SOURCE_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.cc")
FILE(GLOB HEADER_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.h")
FILE(GLOB PROTO_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.proto")
FILE(GLOB SOURCE_PROTO_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.pb.cc")
FILE(GLOB HEADER_PROTO_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*pb.h")
SET(ALL_FILES_ROUTING_DIR ${SOURCE_FILES_ROUTING_DIR} ${HEADER_FILES_ROUTING_DIR} ${PROTO_FILES_ROUTING_DIR})
SET(STYLE_ROUTING ${ALL_FILES_ROUTING_DIR})
LIST(REMOVE_ITEM STYLE_ROUTING ${SOURCE_PROTO_FILES_ROUTING_DIR} ${HEADER_PROTO_FILES_ROUTING_DIR} ${PROTO_FILES_ROUTING_DIR})
LIST(REMOVE_ITEM SOURCE_FILES_ROUTING_DIR ${SOURCE_PROTO_FILES_ROUTING_DIR})
LIST(REMOVE_ITEM HEADER_FILES_ROUTING_DIR ${HEADER_PROTO_FILES_ROUTING_DIR})
SOURCE_GROUP("Routing Source Files" FILES ${SOURCE_FILES_ROUTING_DIR})
SOURCE_GROUP("Routing Header Files" FILES ${HEADER_FILES_ROUTING_DIR})
SOURCE_GROUP("Routing Proto Files" FILES ${PROTO_FILES_ROUTING_DIR} ${SOURCE_PROTO_FILES_ROUTING_DIR} ${HEADER_PROTO_FILES_ROUTING_DIR})

####    TESTS    ####
FILE(GLOB SOURCE_FILES_ROUTING_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/tests/*.cc")
FILE(GLOB HEADER_FILES_ROUTING_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/tests/*.h")
SET(ALL_FILES_ROUTING_TESTS_DIR ${SOURCE_FILES_ROUTING_TESTS_DIR} ${HEADER_FILES_ROUTING_TESTS_DIR})
SET(STYLE_ROUTING_TESTS ${ALL_FILES_ROUTING_TESTS_DIR})
SOURCE_GROUP("Routing Tests Source Files" FILES ${SOURCE_FILES_ROUTING_TESTS_DIR})
SOURCE_GROUP("Routing Tests Header Files" FILES ${HEADER_FILES_ROUTING_TESTS_DIR})


###################################################################################################
# Define MaidSafe libraries and executables                                                       #
###################################################################################################
MS_ADD_STATIC_LIBRARY(${MS_PROJECT_NAME} ${ALL_FILES_ROUTING_DIR})
MS_ADD_EXECUTABLE(TESTRouting Tests ${ALL_FILES_ROUTING_TESTS_DIR})

TARGET_LINK_LIBRARIES(maidsafe_routing_static maidsafe_rudp_static)
TARGET_LINK_LIBRARIES(TESTRouting maidsafe_routing_static gmock_static)


###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
INCLUDE(maidsafe_standard_flags)

# Warnings switched off for protocol buffer generated files
IF(MSVC)
  SET_SOURCE_FILES_PROPERTIES(${SOURCE_PROTO_FILES_ROUTING_DIR} PROPERTIES COMPILE_FLAGS "/W0")
ELSEIF(UNIX)
  SET_SOURCE_FILES_PROPERTIES(${SOURCE_PROTO_FILES_ROUTING_DIR} PROPERTIES COMPILE_FLAGS "-w")
ENDIF()


###################################################################################################
# Tests                                                                                           #
###################################################################################################
ADD_STYLE_TEST(${STYLE_ROUTING} ${STYLE_ROUTING_TESTS})
ADD_GTESTS(TESTRouting)

TEST_SUMMARY_OUTPUT()

# Remove most lengthy tests and style checkers from MemCheck runs.
ADD_MEMCHECK_IGNORE(STYLE_CHECK)


###################################################################################################
# Install details                                                                                 #
###################################################################################################
FINAL_MESSAGE()
RENAME_OUTDATED_BUILT_EXES()

SET(MAIDSAFE_ROUTING_INSTALL_FILES
      ${PROJECT_SOURCE_DIR}/src/maidsafe/routing/routing_api.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/routing/return_codes.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/routing/version.h)


MS_INSTALL_LIBS(maidsafe_routing_static)
MS_INSTALL_HEADERS(routing ${MAIDSAFE_ROUTING_INSTALL_FILES})
MS_INSTALL_EXPORT()
