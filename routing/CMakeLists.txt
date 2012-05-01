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


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
message("================================================================================\n")

project(routing)


###################################################################################################
# Standard setup                                                                                  #
###################################################################################################
include(../cmake_modules/maidsafe_standard_setup.cmake)
# handle_versions(${PROJECT_SOURCE_DIR}/src/maidsafe/routing/version.h)

add_subdirectory(../rudp ../rudp)


###################################################################################################
# Set up all files as GLOBs                                                                       #
###################################################################################################
set(PROTO_SOURCE_DIR ${PROJECT_SOURCE_DIR}/src)
file(GLOB PROTO_FILES RELATIVE ${PROTO_SOURCE_DIR} "${PROTO_SOURCE_DIR}/maidsafe/routing/*.proto")
include(maidsafe_run_protoc)

####    CODE    ####
file(GLOB SOURCE_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.cc")
file(GLOB HEADER_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.h")
file(GLOB PROTO_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.proto")
file(GLOB SOURCE_PROTO_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*.pb.cc")
file(GLOB HEADER_PROTO_FILES_ROUTING_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/*pb.h")
set(ALL_FILES_ROUTING_DIR ${SOURCE_FILES_ROUTING_DIR} ${HEADER_FILES_ROUTING_DIR} ${PROTO_FILES_ROUTING_DIR})
set(STYLE_ROUTING ${ALL_FILES_ROUTING_DIR})
list(REMOVE_ITEM STYLE_ROUTING ${SOURCE_PROTO_FILES_ROUTING_DIR} ${HEADER_PROTO_FILES_ROUTING_DIR} ${PROTO_FILES_ROUTING_DIR})
list(REMOVE_ITEM SOURCE_FILES_ROUTING_DIR ${SOURCE_PROTO_FILES_ROUTING_DIR})
list(REMOVE_ITEM HEADER_FILES_ROUTING_DIR ${HEADER_PROTO_FILES_ROUTING_DIR})
source_group("Routing Source Files" FILES ${SOURCE_FILES_ROUTING_DIR})
source_group("Routing Header Files" FILES ${HEADER_FILES_ROUTING_DIR})
source_group("Routing Proto Files" FILES ${PROTO_FILES_ROUTING_DIR} ${SOURCE_PROTO_FILES_ROUTING_DIR} ${HEADER_PROTO_FILES_ROUTING_DIR})

####    TESTS    ####
file(GLOB SOURCE_FILES_ROUTING_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/tests/*.cc")
file(GLOB HEADER_FILES_ROUTING_TESTS_DIR "${PROJECT_SOURCE_DIR}/src/maidsafe/routing/tests/*.h")
set(ALL_FILES_ROUTING_TESTS_DIR ${SOURCE_FILES_ROUTING_TESTS_DIR} ${HEADER_FILES_ROUTING_TESTS_DIR})
set(STYLE_ROUTING_TESTS ${ALL_FILES_ROUTING_TESTS_DIR})
source_group("Routing Tests Source Files" FILES ${SOURCE_FILES_ROUTING_TESTS_DIR})
source_group("Routing Tests Header Files" FILES ${HEADER_FILES_ROUTING_TESTS_DIR})


###################################################################################################
# Define MaidSafe libraries and executables                                                       #
###################################################################################################
ms_add_static_library(routing ${ALL_FILES_ROUTING_DIR})
ms_add_executable(TESTRouting Tests ${ALL_FILES_ROUTING_TESTS_DIR})

target_link_libraries(maidsafe_routing maidsafe_rudp)
target_link_libraries(TESTRouting maidsafe_routing gmock)


###################################################################################################
# Set compiler and linker flags                                                                   #
###################################################################################################
include(maidsafe_standard_flags)
ms_disable_warnings(${SOURCE_PROTO_FILES_ROUTING_DIR})


###################################################################################################
# Tests                                                                                           #
###################################################################################################
add_style_test(${STYLE_ROUTING} ${STYLE_ROUTING_TESTS})
add_gtests(TESTRouting)

test_summary_output()

# Remove most lengthy tests and style checkers from MemCheck runs.
add_memcheck_ignore(STYLE_CHECK)


###################################################################################################
# Install details                                                                                 #
###################################################################################################
final_message()
rename_outdated_built_exes()

set(MAIDSAFE_ROUTING_INSTALL_FILES
      ${PROJECT_SOURCE_DIR}/src/maidsafe/routing/routing_api.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/routing/return_codes.h
      ${PROJECT_SOURCE_DIR}/src/maidsafe/routing/version.h)


ms_install_libs(maidsafe_routing)
ms_install_headers(routing ${MAIDSAFE_ROUTING_INSTALL_FILES})
ms_install_export()