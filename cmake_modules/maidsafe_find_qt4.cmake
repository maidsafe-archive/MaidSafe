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
#  Uses built-in CMake module FindQt4 to locate QT libs and headers.  This FindQt4 module          #
#  requires that the Qt4 qmake executable is available via the system path.  If this is the        #
#  case, then the module sets the variable QT_USE_FILE which is the path to a CMake file that is   #
#  included in order to compile Qt 4 applications and libraries. It sets up the compilation        #
#  environment for include directories, preprocessor defines and populates a QT_LIBRARIES          #
#  variable.                                                                                       #
#                                                                                                  #
#  If Qt needs to be built first, set BUILD_QT=ON                                                  #
#                                                                                                  #
#  Settable variable to aid with finding Qt is QT_SRC_DIR                                          #
#                                                                                                  #
#  Since this module uses the option to include(${QT_USE_FILE}), the compilation environment is    #
#  automatically set up with include directories and preprocessor definitions.  The requested Qt   #
#  libs are set up as targets, e.g. QT_QTCORE_LIBRARY.  All libs are added to the variable         #
#  QT_LIBRARIES.  See documentation of FindQt4 for further info.                                   #
#                                                                                                  #
#==================================================================================================#


unset(QT_QMAKE_EXECUTABLE CACHE)
unset(QT_LIBRARIES CACHE)
unset(QT_LIBRARY_DIR CACHE)
unset(QT_MOC_EXECUTABLE CACHE)
unset(QT_INCLUDE_DIR CACHE)
unset(QT_INCLUDES CACHE)

set(QT_USE_IMPORTED_TARGETS FALSE)
if(BUILD_QT OR BUILD_QT_IN_SOURCE)
  # Clean out old Qt dlls from binary directory
  file(GLOB QtReleaseDlls "${CMAKE_BINARY_DIR}/Release/q*.dll")
  file(GLOB QtDebugDlls "${CMAKE_BINARY_DIR}/Debug/q*.dll")
  if(QtReleaseDlls OR QtDebugDlls)
    file(REMOVE ${QtReleaseDlls} ${QtDebugDlls})
  endif()
  include(${CMAKE_SOURCE_DIR}/cmake_modules/build_qt4.cmake)
  unset(BUILD_QT CACHE)
  unset(BUILD_QT_IN_SOURCE CACHE)
elseif(QT_SRC_DIR)
  set(QT_ROOT_DIR ${QT_SRC_DIR} CACHE PATH "Path to Qt source and built libraries' root directory" FORCE)
  unset(QT_SRC_DIR CACHE)
endif()


set(ENV{QTDIR} ${QT_ROOT_DIR})
find_program(QT_QMAKE_EXECUTABLE NAMES qmake qmake4 qmake-qt4 PATHS ${QT_ROOT_DIR}/bin NO_DEFAULT_PATH)
find_program(QT_QMAKE_EXECUTABLE NAMES qmake qmake4 qmake-qt4 PATHS ${QT_ROOT_DIR}/bin)
if(NOT QT_QMAKE_EXECUTABLE AND WIN32)
  set(QT_ROOT_DIR ${PROJECT_BINARY_DIR}/build_qt CACHE PATH "Path to Qt source and built libraries' root directory" FORCE)
  find_program(QT_QMAKE_EXECUTABLE NAMES qmake qmake4 qmake-qt4 PATHS ${QT_ROOT_DIR}/bin NO_DEFAULT_PATH)
endif()

if(NOT QT_QMAKE_EXECUTABLE)
  set(ERROR_MESSAGE "\nCould not find Qt.  NO QMAKE EXECUTABLE.")
  if(WIN32)
    set(ERROR_MESSAGE "${ERROR_MESSAGE}(Tried to find qmake.exe in ${QT_ROOT_DIR}\\bin and the system path.)")
  else()
    set(ERROR_MESSAGE "${ERROR_MESSAGE}(Tried to find qmake in ${QT_ROOT_DIR}/bin and the system path.)")
  endif()
  set(ERROR_MESSAGE "${ERROR_MESSAGE}\nTo build Qt, see\n   https://github.com/maidsafe/MaidSafe/wiki/Installing-Prerequisites-and-Optional-Components#building-qt-484\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already built, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=<Path to Qt root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=\"C:\\Devel\\qt-everywhere-opensource-src-4.8.4\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()


set(MS_QT_REQUIRED_LIBRARIES QtCore QtGui QtNetwork QtWebKit)
if(WIN32)
  set(MS_QT_REQUIRED_LIBRARIES ${MS_QT_REQUIRED_LIBRARIES} QtMain)
  find_package(Qt4 4.8.4 COMPONENTS ${MS_QT_REQUIRED_LIBRARIES} REQUIRED)
else()
  find_package(Qt4 4.8.4 COMPONENTS ${MS_QT_REQUIRED_LIBRARIES} REQUIRED)
endif()
include(${QT_USE_FILE})


set(ALL_QT_LIBRARIES_FOUND 1)
foreach(MS_QT_REQUIRED_LIBRARY ${MS_QT_REQUIRED_LIBRARIES})
  string(TOUPPER ${MS_QT_REQUIRED_LIBRARY} MS_QT_REQUIRED_LIBRARY)
  if(NOT QT_${MS_QT_REQUIRED_LIBRARY}_FOUND)
    set(ERROR_MESSAGE "${ERROR_MESSAGE}Could not find ${MS_QT_REQUIRED_LIBRARY}\n")
    set(ALL_QT_LIBRARIES_FOUND 0)
  endif()
endforeach()
if(NOT ALL_QT_LIBRARIES_FOUND)
  set(ERROR_MESSAGE "${ERROR_MESSAGE}\nTo build Qt, see\n   https://github.com/maidsafe/MaidSafe/wiki/Installing-Prerequisites-and-Optional-Components#building-qt-484\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt is already built, run:\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=<Path to Qt root directory>\n")
  set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DQT_ROOT_DIR=\"C:\\Devel\\qt-everywhere-opensource-src-4.8.4\"\n\n")
  message(FATAL_ERROR "${ERROR_MESSAGE}")
endif()


# Copy dlls to binary directory
if(WIN32)
  file(TO_CMAKE_PATH ${QT_ROOT_DIR} QtRootDir)
  file(GLOB QtImageFormatsPluginsRelease "${QtRootDir}/plugins/imageformats/*[^d]4.dll")
  file(GLOB QtImageFormatsPluginsDebug "${QtRootDir}/plugins/imageformats/*d4.dll")
  file(GLOB QtLibsRelease "${QtRootDir}/bin/QtCore4.dll" "${QtRootDir}/bin/QtGui4.dll" "${QtRootDir}/bin/QtNetwork4.dll" "${QtRootDir}/bin/QtWebKit4.dll")
  file(GLOB QtLibsDebug "${QtRootDir}/bin/QtCored4.dll" "${QtRootDir}/bin/QtGuid4.dll" "${QtRootDir}/bin/QtNetworkd4.dll" "${QtRootDir}/bin/QtWebKitd4.dll")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Release/plugins/imageformats")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Debug/plugins/imageformats")
  foreach(QtReleaseDll ${QtLibsRelease})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QtReleaseDll}" "${CMAKE_BINARY_DIR}/Release")
  endforeach()
  foreach(QtReleasePluginDll ${QtImageFormatsPluginsRelease})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QtReleasePluginDll}" "${CMAKE_BINARY_DIR}/Release/plugins/imageformats")
  endforeach()
  foreach(QtDebugDll ${QtLibsDebug})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QtDebugDll}" "${CMAKE_BINARY_DIR}/Debug")
  endforeach()
  foreach(QtDebugPluginDll ${QtImageFormatsPluginsDebug})
    execute_process(COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QtDebugPluginDll}" "${CMAKE_BINARY_DIR}/Debug/plugins/imageformats")
  endforeach()
endif()


function(maidsafe_qt4_wrap_ui UIC_FILES_OUT UIC_FILES_IN)
  set(COMPILED_UI_FILES_DIR ${CMAKE_CURRENT_BINARY_DIR}/compiled_ui_files)
  file(MAKE_DIRECTORY ${COMPILED_UI_FILES_DIR})
  set(CMAKE_CURRENT_BINARY_DIR_BEFORE ${CMAKE_CURRENT_BINARY_DIR})
  set(CMAKE_CURRENT_BINARY_DIR ${COMPILED_UI_FILES_DIR})
  QT4_WRAP_UI(${UIC_FILES_OUT} ${${UIC_FILES_IN}})
  include_directories(${COMPILED_UI_FILES_DIR})
  set(${UIC_FILES_OUT} ${${UIC_FILES_OUT}} PARENT_SCOPE)
  set(CMAKE_CURRENT_BINARY_DIR ${CMAKE_CURRENT_BINARY_DIR_BEFORE})
endfunction()
