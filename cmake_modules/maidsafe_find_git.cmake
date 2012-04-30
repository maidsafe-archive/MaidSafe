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
#  Module used to locate Git.                                                  #
#                                                                              #
#  Settable variables to aid with finding Git are:                             #
#    GIT_ROOT_DIR                                                              #
#                                                                              #
#  Variables set and cached by this module are:                                #
#    Git_EXECUTABLE                                                            #
#                                                                              #
#==============================================================================#


UNSET(Git_EXECUTABLE CACHE)

IF(GIT_ROOT_DIR)
  SET(GIT_ROOT_DIR ${GIT_ROOT_DIR} CACHE PATH "Path to Git directory" FORCE)
ELSE()
  SET(GIT_ROOT_DIR "C:/Program Files/Git" "D:/Program Files/Git" "usr" "usr/local")
ENDIF()

SET(GIT_PATH_SUFFIXES cmd bin)

SET(GIT_NAMES git)
IF(WIN32)
  IF(NOT CMAKE_GENERATOR MATCHES "MSYS")
    SET(GIT_NAMES git.cmd git)
  ENDIF()
ENDIF()

FIND_PROGRAM(Git_EXECUTABLE NAMES ${GIT_NAMES} PATHS ${GIT_ROOT_DIR} PATH_SUFFIXES ${GIT_PATH_SUFFIXES})

IF(NOT Git_EXECUTABLE)
  MESSAGE(FATAL_ERROR "\n\nCouldn't find Git executable. Specify path to Git root as e.g. -DGIT_ROOT_DIR=\"C:/Program Files/Git\"\n\n")
ELSE()
  MESSAGE("-- Found Git")
ENDIF()
