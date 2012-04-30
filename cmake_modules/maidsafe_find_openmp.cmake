#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 maidsafe.net limited                                     #
#  All rights reserved.                                                        #
#                                                                              #
#  Redistribution and use in source and binary forms, with or without          #
#  modification, are permitted provided that the following conditions are met: #
#                                                                              #
#      * Redistributions of source code must retain the above copyright        #
#        notice, this list of conditions and the following disclaimer.         #
#      * Redistributions in binary form must reproduce the above copyright     #
#        notice, this list of conditions and the following disclaimer in the   #
#        documentation and/or other materials provided with the distribution.  #
#      * Neither the name of the maidsafe.net limited nor the names of its     #
#        contributors may be used to endorse or promote products derived from  #
#        this software without specific prior written permission.              #
#                                                                              #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  #
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
#  POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Written by maidsafe.net team                                                #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Module used to define OpenMP flags required and for MSVC, locate the MS     #
#  redistributable OpenMP shared library.                                      #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    OpenMP_CXX_FLAGS, OpenMP_C_FLAGS, and for MSVC only, OpenMP_DLL           #
#                                                                              #
#==============================================================================#


UNSET(OpenMP_DLL CACHE)

INCLUDE(FindOpenMP)

IF(MSVC)
  SET(VC_DIR ${CMAKE_C_COMPILER})
  GET_FILENAME_COMPONENT(VC_DIR ${VC_DIR} PATH)
  GET_FILENAME_COMPONENT(VC_DIR ${VC_DIR} PATH)
  GET_FILENAME_COMPONENT(VC_DIR_NAME ${VC_DIR} NAME)
  IF(VC_DIR_NAME MATCHES "VC")
    SET(OPENMP_ROOT ${VC_DIR})
  ELSE()
    GET_FILENAME_COMPONENT(OPENMP_ROOT ${VC_DIR} PATH)
  ENDIF()
  IF(CMAKE_CL_64)
    SET(OPENMP_ROOT "${OPENMP_ROOT}/redist/x64/Microsoft.VC110.OpenMP")
  ELSE()
    SET(OPENMP_ROOT "${OPENMP_ROOT}/redist/x86/Microsoft.VC110.OPENMP")
  ENDIF()
  FIND_FILE(OpenMP_DLL NAMES vcomp110.dll PATHS ${OPENMP_ROOT} NO_DEFAULT_PATH)
  IF(OpenMP_DLL)
    MESSAGE("-- Found library ${OpenMP_DLL}")
  ELSE()
    SET(ERROR_MESSAGE "\nCould not find library vcomp110.dll.")
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}\nRun cmake from a Visual Studio Command Prompt.")
    MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
  ENDIF()
ENDIF()
