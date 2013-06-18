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
#  Module used to locate Git.                                                                      #
#                                                                                                  #
#  Settable variables to aid with finding Git are:                                                 #
#    GIT_ROOT_DIR                                                                                  #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    Git_EXECUTABLE                                                                                #
#                                                                                                  #
#==================================================================================================#


if(Git_EXECUTABLE)
  # Check the exe path is still correct
  execute_process(COMMAND ${Git_EXECUTABLE} --version RESULT_VARIABLE ResultVar OUTPUT_QUIET ERROR_QUIET)
  if(ResultVar EQUAL 0)
    return()
  endif()
endif()


if(GIT_ROOT_DIR)
  set(GIT_ROOT_DIR ${GIT_ROOT_DIR} CACHE PATH "Path to Git directory" FORCE)
else()
  set(GIT_ROOT_DIR
          "C:/Program Files/Git"
          "C:/Program Files (x86)/Git"
          "D:/Program Files/Git"
          "D:/Program Files (x86)/Git"
          "E:/Program Files/Git"
          "E:/Program Files (x86)/Git"
          "usr" "usr/local")
endif()

set(GIT_PATH_SUFFIXES cmd bin)

set(GIT_NAMES git)
if(WIN32 AND NOT CMAKE_GENERATOR MATCHES "MSYS")
  set(GIT_NAMES git.cmd git)
endif()

find_program(Git_EXECUTABLE NAMES ${GIT_NAMES} PATHS ${GIT_ROOT_DIR} PATH_SUFFIXES ${GIT_PATH_SUFFIXES})

if(NOT Git_EXECUTABLE)
  message(FATAL_ERROR "\n\nCouldn't find Git executable. Specify path to Git root as e.g. -DGIT_ROOT_DIR=\"C:/Program Files/Git\"\n\n")
else()
  message(STATUS "Found Git")
endif()
