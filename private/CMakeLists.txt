#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 MaidSafe.net limited                                     #
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


SET(MS_PROJECT_NAME maidsafe_private)

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
SET(PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
FILE(GLOB_RECURSE PROTO_FILES RELATIVE ${PROTO_SOURCE_DIR} "${PROTO_SOURCE_DIR}/maidsafe/*.proto")
INCLUDE(maidsafe_standard_setup)
HANDLE_VERSIONS(${PROJECT_SOURCE_DIR}/src/maidsafe/private/version.h)


###################################################################################################
# Set up all files as GLOBs                                                                       #
###################################################################################################
####    CODE    ####
FILE(GLOB SOURCE_FILES_PRIVATE_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/*.cc")
FILE(GLOB HEADER_FILES_PRIVATE_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/*.h")
SET(ALL_FILES_PRIVATE_DIR ${SOURCE_FILES_PRIVATE_DIR} ${HEADER_FILES_PRIVATE_DIR})
SOURCE_GROUP("Private Source Files" FILES ${SOURCE_FILES_PRIVATE_DIR})
SOURCE_GROUP("Private Header Files" FILES ${HEADER_FILES_PRIVATE_DIR})

FILE(GLOB SOURCE_FILES_PRIVATE_CHUNK_ACTIONS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/*.cc")
FILE(GLOB HEADER_FILES_PRIVATE_CHUNK_ACTIONS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/*.h")
SET(ALL_FILES_PRIVATE_CHUNK_ACTIONS_DIR ${SOURCE_FILES_PRIVATE_CHUNK_ACTIONS_DIR} ${HEADER_FILES_PRIVATE_CHUNK_ACTIONS_DIR})
SOURCE_GROUP("Chunk Actions Source Files" FILES ${SOURCE_FILES_PRIVATE_CHUNK_ACTIONS_DIR})
SOURCE_GROUP("Chunk Actions Header Files" FILES ${HEADER_FILES_PRIVATE_CHUNK_ACTIONS_DIR})

FILE(GLOB SOURCE_FILES_PRIVATE_CHUNK_STORE_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/*.cc")
FILE(GLOB HEADER_FILES_PRIVATE_CHUNK_STORE_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/*.h")
SET(ALL_FILES_PRIVATE_CHUNK_STORE_DIR ${SOURCE_FILES_PRIVATE_CHUNK_STORE_DIR} ${HEADER_FILES_PRIVATE_CHUNK_STORE_DIR})
SOURCE_GROUP("Chunk Store Source Files" FILES ${SOURCE_FILES_PRIVATE_CHUNK_STORE_DIR})
SOURCE_GROUP("Chunk Store Header Files" FILES ${HEADER_FILES_PRIVATE_CHUNK_STORE_DIR})

####    TESTS    ####
FILE(GLOB SOURCE_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/tests/*.cc")
FILE(GLOB HEADER_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/tests/*.h")
SET(ALL_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR ${SOURCE_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR} ${HEADER_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR})
SOURCE_GROUP("Chunk Actions Tests Source Files" FILES ${SOURCE_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR})
SOURCE_GROUP("Chunk Actions Tests Header Files" FILES ${HEADER_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR})

FILE(GLOB SOURCE_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/tests/*.cc")
FILE(GLOB HEADER_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/tests/*.h")
SET(ALL_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR ${SOURCE_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR} ${HEADER_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR})
SOURCE_GROUP("Chunk Store Tests Source Files" FILES ${SOURCE_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR})
SOURCE_GROUP("Chunk Store Tests Header Files" FILES ${HEADER_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR})


###################################################################################################
# Define MaidSafe libraries and executables                                                       #
###################################################################################################
MS_ADD_STATIC_LIBRARY(${MS_PROJECT_NAME} ${ALL_FILES_PRIVATE_DIR} ${ALL_FILES_PRIVATE_CHUNK_ACTIONS_DIR} ${ALL_FILES_PRIVATE_CHUNK_STORE_DIR})
MS_ADD_EXECUTABLE(TESTprivate Tests ${ALL_FILES_PRIVATE_CHUNK_ACTIONS_TESTS_DIR} ${ALL_FILES_PRIVATE_CHUNK_STORE_TESTS_DIR})

TARGET_LINK_LIBRARIES(maidsafe_private_static maidsafe_common_static protobuf_static)
TARGET_LINK_LIBRARIES(TESTprivate maidsafe_private_static gtest_static gmock_static)


###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
INCLUDE(maidsafe_standard_flags)

FILE(GLOB_RECURSE PROTO_SOURCE_FILES "${PROTO_SOURCE_DIR}/maidsafe/*.pb.cc")
FILE(GLOB_RECURSE PROTO_HEADER_FILES "${PROTO_SOURCE_DIR}/maidsafe/*.pb.h")
IF(MSVC)
  SET_SOURCE_FILES_PROPERTIES(${PROTO_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "/W0")
ELSEIF(UNIX)
  SET_SOURCE_FILES_PROPERTIES("${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/tests/mock_chunk_manager.cc"
                              "${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/tests/mock_chunk_manager.h"
                               PROPERTIES COMPILE_FLAGS "-Wno-effc++")
  SET_SOURCE_FILES_PROPERTIES(${PROTO_SOURCE_FILES} PROPERTIES COMPILE_FLAGS "-w")
  SET_SOURCE_FILES_PROPERTIES(${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/tests/threadsafe_chunk_store_test.cc
                                PROPERTIES COMPILE_FLAGS "-Wstrict-overflow=2")
ENDIF()


###################################################################################################
# Tests                                                                                           #
###################################################################################################
SET(ALL_PRIVATE_STYLE ${ALL_FILES_PRIVATE_DIR} ${ALL_FILES_PRIVATE_TESTS_DIR} ${ALL_FILES_PRIVATE_CHUNK_ACTIONS_DIR} ${ALL_FILES_PRIVATE_CHUNK_STORE_DIR})
LIST(REMOVE_ITEM ALL_PRIVATE_STYLE ${PROTO_SOURCE_FILES} ${PROTO_HEADER_FILES})
ADD_STYLE_TEST(${ALL_PRIVATE_STYLE})
ADD_GTESTS(TESTprivate)
TEST_SUMMARY_OUTPUT()


###################################################################################################
# Install details                                                                                 #
###################################################################################################
FINAL_MESSAGE()
RENAME_OUTDATED_BUILT_EXES()

SET(MAIDSAFE_PRIVATE_INSTALL_FILES
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/return_codes.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/version.h)
SET(MAIDSAFE_PRIVATE_CHUNK_ACTIONS_INSTALL_FILES
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/chunk_action_authority.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/chunk.pb.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/chunk_pb.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/chunk_types.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/default_rules.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/appendable_by_all.pb.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/appendable_by_all_pb.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/appendable_by_all_rules.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_actions/modifiable_by_owner_rules.h)
SET(MAIDSAFE_PRIVATE_CHUNK_STORE_INSTALL_FILES
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/buffered_chunk_store.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/chunk_manager.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/chunk_store.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/local_chunk_manager.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/private/chunk_store/remote_chunk_store.h)

MS_INSTALL_LIBS(maidsafe_private_static)
MS_INSTALL_HEADERS(private ${MAIDSAFE_PRIVATE_INSTALL_FILES})
MS_INSTALL_HEADERS(private/chunk_actions ${MAIDSAFE_PRIVATE_CHUNK_ACTIONS_INSTALL_FILES})
MS_INSTALL_HEADERS(private/chunk_store ${MAIDSAFE_PRIVATE_CHUNK_STORE_INSTALL_FILES})
MS_INSTALL_EXPORT()
