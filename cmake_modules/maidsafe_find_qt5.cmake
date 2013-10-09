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


# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the Qt libraries
if(QT_BIN_DIR)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_BIN_DIR}/..)
endif()
find_package(Qt5Core QUIET)
if(NOT Qt5Core_FOUND)
  if(Qt5Required)
    set(ERROR_MESSAGE "\nCould not find Qt5. ")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}QT_BIN_DIR is currently set to \"${QT_BIN_DIR}\"")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}\nIf Qt5 is not installed, see\n   https://github.com/maidsafe/MaidSafe/wiki/Installing-Prerequisites-and-Optional-Components#installing-qt-5\n")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}If Qt5 is already installed, run:\n")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}cmake . -DQT_BIN_DIR=\"<Path to Qt5 bin directory>\"\n")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}e.g.\ncmake . -DQT_BIN_DIR=\"C:\\Qt\\Qt5.1.0\\5.1.0\\msvc2012_64\\bin\"\n\n")
    message(FATAL_ERROR "${ERROR_MESSAGE}")
  else()
    return()
  endif()
endif()

find_package(Qt5Concurrent REQUIRED)
find_package(Qt5Gui REQUIRED)
find_package(Qt5Widgets REQUIRED)
find_package(Qt5Network REQUIRED)
find_package(Qt5WebKit REQUIRED)
find_package(Qt5WebKitWidgets REQUIRED)
find_package(Qt5LinguistTools REQUIRED)
find_package(Qt5Svg REQUIRED)

# Copy dlls to binary directory
if(MSVC)
  # Image format plugins
  file(TO_CMAKE_PATH "${QT_BIN_DIR}/../plugins" QtPluginsPath)
  file(GLOB_RECURSE QtPluginsRelease "${QtPluginsPath}/*[^d].dll")
  file(GLOB_RECURSE QtPluginsDebug "${QtPluginsPath}/*d.dll")

  # QML Platforms dll's
  file(TO_CMAKE_PATH "${QT_BIN_DIR}/../qml" QtQmlPath)
  file(GLOB_RECURSE QtQmlCollection "${QtQmlPath}/*[^.pdb]")
  # file(GLOB_RECURSE QtQmlDebug "${QtQmlPath}/*d.dll")

  # Required Qt Libraries
  set(REQUIRED_QT_LIBS  "d3dcompiler_46"
                        "icudt51"
                        "icuin51"
                        "icuuc51"
                        "libEGL"
                        "libGLESv2"
                        "Qt5Concurrent"
                        "Qt5Core"
                        "Qt5Gui"
                        "Qt5Network"
                        "Qt5Qml"
                        "Qt5Quick"
                        "Qt5QuickParticles"
                        "Qt5Svg"
                        "Qt5V8"
                        "Qt5Widgets")

  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Release")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Debug")

  # Qt Plugins Dll's
  function(TransferPluginDlls QtDlls BuildType)
    foreach(QtDll ${${QtDlls}})
      get_filename_component(DirPath "${QtDll}" PATH)
      get_filename_component(DirName "${DirPath}" NAME)
      if (${DirName} STREQUAL "platforms")
        set(OUTPUT_PATH "${CMAKE_BINARY_DIR}/${BuildType}")
      else()
        set(OUTPUT_PATH "${CMAKE_BINARY_DIR}/${BuildType}/plugins")
      endif()
      string(REPLACE "${QtPluginsPath}" "${OUTPUT_PATH}" DestPath ${QtDll})
      get_filename_component(DestPath "${DestPath}" PATH)
      file(COPY ${QtDll} DESTINATION ${DestPath})
    endforeach()
  endfunction()

  TransferPluginDlls(QtPluginsRelease "Release")
  TransferPluginDlls(QtPluginsDebug "Debug")

  # Qt Qml dependencies
  function(TransferQMLDependencies QmlFiles BuildType)
    foreach(QmlFile ${${QmlFiles}})
      set(OUTPUT_PATH "${CMAKE_BINARY_DIR}/${BuildType}/qml")
      string(REPLACE "${QtQmlPath}" "${OUTPUT_PATH}" DestPath ${QmlFile})
      get_filename_component(DestPath "${DestPath}" PATH)
      if (${BuildType} STREQUAL "Debug")
        if (NOT ${QmlFile} MATCHES "[^d].dll$")
          file(COPY ${QmlFile} DESTINATION ${DestPath})
        endif()
      else()
        if (NOT ${QmlFile} MATCHES "d.dll$")
          file(COPY ${QmlFile} DESTINATION ${DestPath})
        endif()
      endif()
    endforeach()
  endfunction()

  TransferQMLDependencies(QtQmlCollection "Release")
  TransferQMLDependencies(QtQmlCollection "Debug")

  # Qt Bin Dll's
  function(TransferQtBinDlls QtDlls BuildType)
    foreach(QtDll ${${QtDlls}})
      set(OUTPUT_PATH "${CMAKE_BINARY_DIR}/${BuildType}")
      if (${BuildType} STREQUAL "Debug" AND ${QtDll} MATCHES "^(Qt|lib)")
        set(FORMATTED_NAME "${QtDll}d")
      else()
        set(FORMATTED_NAME "${QtDll}")
      endif()
      file(COPY "${QT_BIN_DIR}/${FORMATTED_NAME}.dll" DESTINATION ${OUTPUT_PATH})
    endforeach()
  endfunction()

  TransferQtBinDlls(REQUIRED_QT_LIBS "Release")
  TransferQtBinDlls(REQUIRED_QT_LIBS "Debug")
endif()

