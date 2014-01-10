#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
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


if(NOT PYTHON_EXECUTABLE OR NOT PYTHON_LIBRARY)
  message("${HR}")
endif()
set(Python_ADDITIONAL_VERSIONS 3.9 3.8 3.7 3.6 3.5 3.4 3.3 3.2 3.1 3.0)
find_package(PythonInterp REQUIRED)
if(NOT PYTHONINTERP_FOUND)
  message(FATAL_ERROR "Didn't find python executable.")
endif()
if(WIN32)
  get_filename_component(PythonPath ${PYTHON_EXECUTABLE} PATH)
  get_filename_component(PythonLibName ${PythonPath} NAME)
  string(TOLOWER "${PythonLibName}" PythonLibName)
  find_library(PythonLibrary NAMES ${PythonLibName} PATHS ${PythonPath}/libs NO_DEFAULT_PATH)
  find_path(PythonIncludeDir NAMES Python.h PATHS ${PythonPath}/include NO_DEFAULT_PATH)
  set(PYTHON_LIBRARY "${PythonLibrary}")
  set(PYTHON_INCLUDE_DIR "${PythonIncludeDir}")
endif()
find_package(PythonLibs REQUIRED)
