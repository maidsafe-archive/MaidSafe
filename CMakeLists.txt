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


cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)
project(maidsafe)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
set(LIBRARY_OUTPUT_PATH ${CMAKE_CURRENT_BINARY_DIR})
# set(MAIDSAFE_INSTALL_PREFIX "${PROJECT_SOURCE_DIR}/${COMPILER}/installed" )
# set(MAIDSAFE_INCLUDE_DIR "${MAIDSAFE_INSTALL_PREFIX}/include")
add_subdirectory(src/third_party_libs)
add_subdirectory(src/common)
add_subdirectory(src/private)
add_subdirectory(src/transport)
# add_subdirectory(src/rudp)
# add_subdirectory(src/routing)
add_subdirectory(src/dht)
# add_subdirectory(src/encrypt)
# add_subdirectory(src/pki)
# add_subdirectory(src/passport)
# add_subdirectory(src/pd)
# add_subdirectory(src/drive)
# add_subdirectory(src/lifestuff)

message("${HR}")


# When this target is built, it removes all .gcda files from the build directory and its subdirectories
if(UNIX)
  find_file(CLEAN_COVERAGE clean_coverage.cmake cmake_modules/)
  if(CLEAN_COVERAGE)
    add_custom_target(CleanCoverage COMMAND ${CMAKE_COMMAND} -DSEARCH_DIR=${CMAKE_BINARY_DIR} -P ${CLEAN_COVERAGE})
  endif()
endif()

cleanup_temp_dir()
