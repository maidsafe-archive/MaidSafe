#==============================================================================#
#                                                                              #
#  Copyright (c) 2012 MaidSafe.net limited                                     #
#                                                                              #
#  The following source code is property of MaidSafe.net limited and is not    #
#  meant for external use.  The use of this code is governed by the license    #
#  file licence.txt found in the root directory of this project and also on    #
#  www.maidsafe.net.                                                           #
#                                                                              #
#  You are not free to copy, amend or otherwise use this source code without   #
#  the explicit written permission of the board of directors of MaidSafe.net.  #
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


unset(QT_QMAKE_EXECUTABLE CACHE)
unset(QT_LIBRARIES CACHE)
unset(QT_LIBRARY_DIR CACHE)
unset(QT_MOC_EXECUTABLE CACHE)
unset(QT_INCLUDE_DIR CACHE)
unset(QT_INCLUDES CACHE)


set(QT_USE_IMPORTED_TARGETS FALSE)
if(QT_ROOT_DIR)
  set(QT_ROOT_DIR ${QT_ROOT_DIR} CACHE PATH "Path to Qt root directory" FORCE)
  set(ENV{QTDIR} ${QT_ROOT_DIR})
endif()

find_program(QT_QMAKE_EXECUTABLE NAMES qmake qmake4 qmake-qt4 PATHS ${QT_ROOT_DIR}/bin NO_DEFAULT_PATH)
find_program(QT_QMAKE_EXECUTABLE NAMES qmake qmake4 qmake-qt4 PATHS ${QT_ROOT_DIR}/bin)
if(NOT QT_QMAKE_EXECUTABLE)
  set(ERROR_MESSAGE "\nCould not find Qt.  NO QMAKE EXECUTABLE ")
  if(WIN32)
    set(ERROR_MESSAGE "${ERROR_MESSAGE}(Tried to find qmake.exe in ${QT_ROOT_DIR}\\bin and the system path.)")
  else()
    set(ERROR_MESSAGE "${ERROR_MESSAGE}(Tried to find qmake in ${QT_ROOT_DIR}/bin and the system path.)")
  endif()
  set(ERROR_MESSAGE "${ERROR_MESSAGE}\nYou can download Qt at http://qt.nokia.com/downloads\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}cmake . -DQT_ROOT_DIR=<Path to Qt root directory>")
  if(WIN32)
    set(ERROR_MESSAGE "${ERROR_MESSAGE}\n(such that qmake.exe is in \"<Path to Qt root directory>\\bin\").")
  else()
    set(ERROR_MESSAGE "${ERROR_MESSAGE}\n(such that qmake is in \"<Path to Qt root directory>/bin\").")
  endif()
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

set(QT_STATIC 1)
find_package(Qt4 4.8.0 COMPONENTS QtCore QtGui QtMain QtXml QtSql REQUIRED)
include(${QT_USE_FILE})

if(NOT QT_QTCORE_FOUND)
  set(ERROR_MESSAGE "\nCould not find Qt.  NO QT CORE LIBRARY - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}You can download Qt at http://qt.nokia.com/downloads\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}cmake . -DQT_ROOT_DIR=<Path to Qt root directory>")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()
if(QT_LIBRARIES)
  if(NOT QT_INCLUDES)
    set(ERROR_MESSAGE "\nCould not find Qt.  AT LEAST ONE HEADER FILE IS MISSING - ")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}You can download Qt at http://qt.nokia.com/downloads\n")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}cmake . -DQT_ROOT_DIR=<Path to Qt root directory>")
    message(FATAL_ERROR "${ERROR_MESSAGE}")
  endif()
else()
  set(ERROR_MESSAGE "\nCould not find Qt.  NO QT LIBRARIES - ")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}You can download Qt at http://qt.nokia.com/downloads\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already installed, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}cmake . -DQT_ROOT_DIR=<Path to Qt root directory>")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()

if(NOT WIN32)
  # These are required for static builds
  if(APPLE)
    set(QT_EXTRA_LIBRARIES AppKit Security)
  else()
    set(QT_EXTRA_LIBRARIES z Xext X11 xcb Xau Xdmcp)
  endif()
  foreach(QT_EXTRA_LIBRARY ${QT_EXTRA_LIBRARIES})
    find_library(CURRENT_LIB ${QT_EXTRA_LIBRARY})
    if(NOT CURRENT_LIB)
      set(ERROR_MESSAGE "\nCould not find library ${QT_EXTRA_LIBRARY}.")
      message(FATAL_ERROR "${ERROR_MESSAGE}")
    else()
      set(QT_LIBRARIES ${QT_LIBRARIES} ${CURRENT_LIB})
    endif()
    unset(CURRENT_LIB CACHE)
  endforeach()
endif()

function(maidsafe_qt4_wrap_ui UIC_FILES_OUT UIC_FILES_IN)
  set(COMPILED_UI_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR}/compiled_ui_files)
  file(MAKE_DIRECTORY ${COMPILED_UI_FILES_DIR})
  set(CMAKE_CURRENT_BINARY_DIR_BEFORE ${CMAKE_CURRENT_BINARY_DIR})
  set(CMAKE_CURRENT_BINARY_DIR ${COMPILED_UI_FILES_DIR})
  QT4_WRAP_UI(${UIC_FILES_OUT} ${${UIC_FILES_IN}})
  include_directories(BEFORE SYSTEM ${COMPILED_UI_FILES_DIR})
  set(${UIC_FILES_OUT} ${${UIC_FILES_OUT}} PARENT_SCOPE)
  set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR_BEFORE})
endfunction()

