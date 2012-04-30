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
#  Uses built in CMake module FindQt4 to locate QT libs and headers.  This     #
#  FindQt4 module requires that the Qt4 qmake executable is available via the  #
#  system path.  If this is the case, then the module sets the variable        #
#  QT_USE_FILE which is the path to a CMake file that is included in order to  #
#  compile Qt 4 applications and libraries. It sets up the compilation         #
#  environment for include directories, preprocessor defines and populates a   #
#  QT_LIBRARIES variable.                                                      #
#                                                                              #
#  Settable variable to aid with finding QT is QT_ROOT_DIR                     #
#                                                                              #
#  Since this module uses the option to include(${QT_USE_FILE}), the           #
#  compilation environment is automatically set up with include directories    #
#  and preprocessor definitions.  The requested QT libs are set up as targets, #
#  e.g. QT_QTCORE_LIBRARY.  All libs are added to the variable QT_LIBRARIES.   #
#  See documentation of FindQt4 for further info.                              #
#                                                                              #
#==============================================================================#


UNSET(QT_QMAKE_EXECUTABLE CACHE)
UNSET(QT_LIBRARIES CACHE)
UNSET(QT_LIBRARY_DIR CACHE)
UNSET(QT_MOC_EXECUTABLE CACHE)
UNSET(QT_INCLUDE_DIR CACHE)
UNSET(QT_INCLUDES CACHE)


SET(QT_USE_IMPORTED_TARGETS FALSE)
IF(QT_ROOT_DIR)
  SET(QT_ROOT_DIR ${QT_ROOT_DIR} CACHE PATH "Path to Qt root directory" FORCE)
  SET(ENV{QTDIR} ${QT_ROOT_DIR})
ELSEIF(DEFAULT_THIRD_PARTY_ROOT)
  FIND_THIRD_PARTY_PROJECT(QT_BASE_DIR Qt ${DEFAULT_THIRD_PARTY_ROOT})
  IF(QT_BASE_DIR_CACHED)
    FIND_THIRD_PARTY_PROJECT(QT_ROOT_DIR "" ${QT_BASE_DIR_CACHED})
    IF(QT_ROOT_DIR_CACHED)
      SET(QT_ROOT_DIR ${QT_ROOT_DIR_CACHED})
      SET(ENV{QTDIR} ${QT_ROOT_DIR})
    ENDIF()
  ENDIF()
ENDIF()

FIND_PROGRAM(QT_QMAKE_EXECUTABLE NAMES qmake qmake4 qmake-qt4 PATHS ${QT_ROOT_DIR}/bin)
IF(NOT QT_QMAKE_EXECUTABLE)
  SET(ERROR_MESSAGE "\nCould not find Qt.  NO QMAKE EXECUTABLE ")
  IF(WIN32)
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}(Tried to find qmake.exe in ${QT_ROOT_DIR}\\bin and the system path.)")
  ELSE()
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}(Tried to find qmake in ${QT_ROOT_DIR}/bin and the system path.)")
  ENDIF()
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}\nYou can download Qt at http://qt.nokia.com/downloads\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=<Path to Qt root directory>")
  IF(WIN32)
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}\n(such that qmake.exe is in \"<Path to Qt root directory>\\bin\").")
  ELSE()
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}\n(such that qmake is in \"<Path to Qt root directory>/bin\").")
  ENDIF()
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF()

SET(QT_STATIC 1)
FIND_PACKAGE(Qt4 4.8.0 COMPONENTS QtCore QtGui QtMain QtXml QtSql REQUIRED)
INCLUDE(${QT_USE_FILE})

IF(NOT QT_QTCORE_FOUND)
  SET(ERROR_MESSAGE "\nCould not find Qt.  NO QT CORE LIBRARY - ")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}You can download Qt at http://qt.nokia.com/downloads\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=<Path to Qt root directory>")
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF()
IF(QT_LIBRARIES)
  IF(NOT QT_INCLUDES)
    SET(ERROR_MESSAGE "\nCould not find Qt.  AT LEAST ONE HEADER FILE IS MISSING - ")
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}You can download Qt at http://qt.nokia.com/downloads\n")
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
    SET(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=<Path to Qt root directory>")
    MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
  ENDIF()
ELSE(QT_LIBRARIES)
  SET(ERROR_MESSAGE "\nCould not find Qt.  NO QT LIBRARIES - ")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}You can download Qt at http://qt.nokia.com/downloads\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
  SET(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=<Path to Qt root directory>")
  MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
ENDIF(QT_LIBRARIES)

IF(NOT WIN32)
  # These are required for static builds
  IF(APPLE)
    SET(QT_EXTRA_LIBRARIES z)
  ELSE()
    SET(QT_EXTRA_LIBRARIES z Xext X11 xcb Xau Xdmcp)
  ENDIF()
  FOREACH(QT_EXTRA_LIBRARY ${QT_EXTRA_LIBRARIES})
    FIND_LIBRARY(CURRENT_LIB ${QT_EXTRA_LIBRARY})
    IF(CURRENT_LIB)
      MESSAGE("-- Found library ${CURRENT_LIB}")
    ELSE()
      SET(ERROR_MESSAGE "\nCould not find library ${QT_EXTRA_LIBRARY}.")
      SET(ERROR_MESSAGE "${ERROR_MESSAGE}  Run\n${ERROR_MESSAGE_CMAKE_PATH} -DADD_LIBRARY_DIR=<Path to ${QT_EXTRA_LIBRARY} directory>")
      MESSAGE(FATAL_ERROR "${ERROR_MESSAGE}")
    ENDIF()
    UNSET(CURRENT_LIB CACHE)
  ENDFOREACH()
  SET(QT_LIBRARIES ${QT_LIBRARIES} ${QT_EXTRA_LIBRARIES})
ENDIF()

FUNCTION(MAIDSAFE_QT4_WRAP_UI UIC_FILES_OUT UIC_FILES_IN)
  SET(COMPILED_UI_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR}/compiled_ui_files)
  FILE(MAKE_DIRECTORY ${COMPILED_UI_FILES_DIR})
  SET(CMAKE_CURRENT_BINARY_DIR_BEFORE ${CMAKE_CURRENT_BINARY_DIR})
  SET(CMAKE_CURRENT_BINARY_DIR ${COMPILED_UI_FILES_DIR})
  QT4_WRAP_UI(${UIC_FILES_OUT} ${${UIC_FILES_IN}})
  INCLUDE_DIRECTORIES(BEFORE SYSTEM ${COMPILED_UI_FILES_DIR})
  SET(${UIC_FILES_OUT} ${${UIC_FILES_OUT}} PARENT_SCOPE)
  SET(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR_BEFORE})
ENDFUNCTION()
