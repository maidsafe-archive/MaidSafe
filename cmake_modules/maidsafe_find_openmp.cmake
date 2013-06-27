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
  if(CMAKE_CL_64)
    set(OPENMP_ROOT "$ENV{VCInstallDir}/redist/x64/Microsoft.VC110.OpenMP")
  else()
    set(OPENMP_ROOT "$ENV{VCInstallDir}/redist/x86/Microsoft.VC110.OPENMP")
  endif()
  find_file(OpenMP_DLL NAMES vcomp110.dll PATHS ${OPENMP_ROOT} NO_DEFAULT_PATH)
  if(OpenMP_DLL)
    message(STATUS "Found library ${OpenMP_DLL}")
  else()
    set(ERROR_MESSAGE "\nCould not find library vcomp110.dll.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}\nRun cmake from a Visual Studio Command Prompt.")
    message(FATAL_ERROR "${ERROR_MESSAGE}")
  endif()
endif()
