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
