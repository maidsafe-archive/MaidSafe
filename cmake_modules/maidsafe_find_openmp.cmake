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
#  Module used to define OpenMP flags required and for MSVC, locate the MS redistributable OpenMP  #
#  shared library.                                                                                 #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    OpenMP_CXX_FLAGS, OpenMP_C_FLAGS, and for MSVC only, OpenMP_DLL                               #
#                                                                                                  #
#==================================================================================================#

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  # OMP is currently unavailable with Clang - avoid generating a mountain of
  # output with every CMake run when this will fail every time.
  return()
endif()

include(FindOpenMP)

if(MSVC)
  string(REPLACE "v" "" Version ${CMAKE_VS_PLATFORM_TOOLSET})
  if(CMAKE_CL_64)
    set(OPENMP_ROOT "$ENV{VCInstallDir}/redist/x64/Microsoft.VC${Version}.OpenMP")
  else()
    set(OPENMP_ROOT "$ENV{VCInstallDir}/redist/x86/Microsoft.VC${Version}.OPENMP")
  endif()
  find_file(OpenMP_DLL NAMES vcomp${Version}.dll PATHS ${OPENMP_ROOT} NO_DEFAULT_PATH)
  if(OpenMP_DLL)
    message(STATUS "Found library ${OpenMP_DLL}")
  else()
    set(ERROR_MESSAGE "\nCould not find library vcomp110.dll.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}\nRun cmake from a Visual Studio Command Prompt.")
    message(FATAL_ERROR "${ERROR_MESSAGE}")
  endif()
endif()
