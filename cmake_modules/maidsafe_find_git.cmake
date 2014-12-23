#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.maidsafe.net/licenses                                                            #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
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


if(HaveAlreadyRunFindGit)
  return()
else()
  set(HaveAlreadyRunFindGit ON)
endif()


if(Git_EXECUTABLE)
  # Check the exe path is still correct
  execute_process(COMMAND ${Git_EXECUTABLE} --version RESULT_VARIABLE ResultVar OUTPUT_QUIET ERROR_QUIET)
  if(ResultVar EQUAL "0")
    # Set the path to git's folder so that CMake's built-in FindGit module can work if required later.
    get_filename_component(GitFolder ${Git_EXECUTABLE} PATH)
    list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${GitFolder})
    return()
  endif()
endif()


if(GIT_ROOT_DIR)
  # Leave the helpstring as the default to allow the 'ms_get_command_line_args' function in utils.cmake
  # to identify this as a command line arg.
  set(GIT_ROOT_DIR ${GIT_ROOT_DIR} CACHE PATH "No help, variable specified on the command line." FORCE)
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
  get_filename_component(GitFolder ${Git_EXECUTABLE} PATH)
  list(APPEND CMAKE_SYSTEM_PREFIX_PATH ${GitFolder})
  find_package(Git)
endif()
