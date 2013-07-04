#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or    #
#  later, and The General Public License (GPL), version 3. By contributing code to this project    #
#  You agree to the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in    #
#  the root directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also    #
#  available at:                                                                                   #
#                                                                                                  #
#    http://www.novinet.com/license                                                                #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, software distributed under the       #
#  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       #
#  either express or implied. See the License for the specific language governing permissions      #
#  and limitations under the License.                                                              #
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
