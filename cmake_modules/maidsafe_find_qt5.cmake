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

# Copy dlls to binary directory
if(MSVC)
  file(GLOB QtImageFormatsPluginsRelease "${QT_BIN_DIR}/../plugins/imageformats/*[^d].dll")
  file(GLOB QtImageFormatsPluginsDebug "${QT_BIN_DIR}/../plugins/imageformats/*d.dll")
  file(GLOB QtLibsRelease "${QT_BIN_DIR}/*[^d].dll")
  file(GLOB QtLibsDebug "${QT_BIN_DIR}/*d.dll" "${QT_BIN_DIR}/icuin51.dll" "${QT_BIN_DIR}/icuuc51.dll" "${QT_BIN_DIR}/icudt51.dll")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Release/plugins/imageformats")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Debug/plugins/imageformats")
  foreach(QtReleaseDll ${QtLibsRelease})
    get_filename_component(FileName "${QtReleaseDll}" NAME)
    configure_file("${QtReleaseDll}" "${CMAKE_BINARY_DIR}/Release/${FileName}" COPYONLY)
  endforeach()
  foreach(QtReleasePluginDll ${QtImageFormatsPluginsRelease})
    get_filename_component(FileName "${QtReleasePluginDll}" NAME)
    configure_file("${QtReleasePluginDll}" "${CMAKE_BINARY_DIR}/Release/plugins/imageformats/${FileName}" COPYONLY)
  endforeach()
  foreach(QtDebugDll ${QtLibsDebug})
    get_filename_component(FileName "${QtDebugDll}" NAME)
    configure_file("${QtDebugDll}" "${CMAKE_BINARY_DIR}/Debug/${FileName}" COPYONLY)
  endforeach()
  foreach(QtDebugPluginDll ${QtImageFormatsPluginsDebug})
    get_filename_component(FileName "${QtDebugPluginDll}" NAME)
    configure_file("${QtDebugPluginDll}" "${CMAKE_BINARY_DIR}/Debug/plugins/imageformats/${FileName}" COPYONLY)
  endforeach()
endif()

