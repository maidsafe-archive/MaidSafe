#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2013 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
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
