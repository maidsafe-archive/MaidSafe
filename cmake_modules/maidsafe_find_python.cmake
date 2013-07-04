#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
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
endif()
find_package(PythonLibs REQUIRED)
