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
#  Finds and sets up Qt5+ as dependency.                                                           #
#                                                                                                  #
#  Variables required by this module are:                                                          #
#    Qt5RequiredLibs - List of Qt5 Libs needed.                                                    #
#      example: set(Qt5RequiredLibs Qt5Core Qt5Gui Qt5Widgets)                                     #
#                                                                                                  #
#  Variables set by this module are:                                                               #
#    Qt5TargetLibs - Formatted Qt5 Target Link libs                                                #
#      example: target_link_libraries(my_project ${Qt5TargetLibs})                                 #
#                                                                                                  #
#    Qt5AllLibsPathsDebug & Qt5AllLibsPathsRelease - Full path to all libs in Qt5TargetLibs and    #
#    all of their non-system dependencies.                                                         #
#                                                                                                  #
#==================================================================================================#

# Required Qt version
set(Qt5RequiredVersion 5.2.0)

# Check for valid input variables
list(LENGTH Qt5RequiredLibs Qt5RequiredLibsLength)
if(Qt5RequiredLibsLength EQUAL "0")
  set(ErrorMessage "${ErrorMessage}Qt5RequiredLibs is currently empty.")
  set(ErrorMessage "${ErrorMessage}\nThis variable needs to be set for this module")
  set(ErrorMessage "${ErrorMessage}\nPlease check maidsafe_find_qt5.cmake for example specification.\n\n")
  message(FATAL_ERROR "${ErrorMessage}")
endif()

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the Qt libraries
if(QT_BIN_DIR)
  set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${QT_BIN_DIR}/..)
endif()
unset(Qt5TargetLibs)
unset(Qt5AllLibsPathsRelease)
unset(Qt5AllLibsPathsDebug)
set(Found TRUE)
set(ErrorMessage "\nCould not find all required components of Qt${Qt5RequiredVersion}:\n")
foreach(QtLib ${Qt5RequiredLibs})
  find_package(${QtLib} ${Qt5RequiredVersion} QUIET)
  if(${QtLib}_FOUND)
    set(ErrorMessage "${ErrorMessage}  Found ${QtLib}\n")
    if(NOT QtLib STREQUAL "Qt5LinguistTools")
      string(REPLACE "Qt5" "Qt5::" QtLib ${QtLib})
      list(APPEND Qt5TargetLibs "${QtLib}")
    endif()
  else()
    set(Found FALSE)
    set(ErrorMessage "${ErrorMessage}  Did not find ${QtLib}\n")
  endif()
endforeach()

# Verify if all required libs are found
set(AllQt5_FOUND ${Found} CACHE INTERNAL "Whether all required Qt5 modules were found or not.")
if(NOT AllQt5_FOUND)
  if(Qt5Required)
    set(ErrorMessage "${ErrorMessage}QT_BIN_DIR is currently set to \"${QT_BIN_DIR}\"")
    set(ErrorMessage "${ErrorMessage}\nIf Qt${Qt5RequiredVersion} is not installed, see\n   https://github.com/maidsafe/MaidSafe/wiki/Installing-Prerequisites-and-Optional-Components#installing-qt-5\n")
    set(ErrorMessage "${ErrorMessage}If Qt${Qt5RequiredVersion} is already installed, run:\n")
    set(ErrorMessage "${ErrorMessage}cmake . -DQT_BIN_DIR=\"<Path to Qt${Qt5RequiredVersion} bin directory>\"\n")
    set(ErrorMessage "${ErrorMessage}e.g.\ncmake . -DQT_BIN_DIR=\"C:\\Qt\\Qt${Qt5RequiredVersion}\\${Qt5RequiredVersion}\\msvc2013_64\\bin\"\n\n")
    message(FATAL_ERROR "${ErrorMessage}")
  else()
    return()
  endif()
endif()

# Get list of prerequisites for each required Qt library
include(GetPrerequisites)
file(TO_CMAKE_PATH "${QT_BIN_DIR}" QtBinDir)
foreach(QtLib ${Qt5TargetLibs})
  foreach(BuildType Release Debug)
    string(TOUPPER ${BuildType} BuildTypeUpperCase)
    get_target_property(${QtLib}Location${BuildType} ${QtLib} LOCATION_${BuildTypeUpperCase})
    get_filename_component(${QtLib}Location${BuildType} "${${QtLib}Location${BuildType}}" REALPATH)
    list(APPEND Qt5AllLibsPaths${BuildType} "${${QtLib}Location${BuildType}}")
    if(NOT ${QtLib}Location${BuildType}Dependencies)
      get_prerequisites(${${QtLib}Location${BuildType}} ${QtLib}Location${BuildType}Dependencies 1 1 ${CMAKE_BINARY_DIR} "")
      set(${QtLib}Location${BuildType}Dependencies ${${QtLib}Location${BuildType}Dependencies} CACHE INTERNAL "")
    endif()
    foreach(Dependency ${${QtLib}Location${BuildType}Dependencies})
      if(EXISTS "${Dependency}")
        list(APPEND Qt5AllLibsPaths${BuildType} "${Dependency}")
      elseif(EXISTS "${QtBinDir}/${Dependency}")
        list(APPEND Qt5AllLibsPaths${BuildType} "${QtBinDir}/${Dependency}")
      else()
        message(FATAL_ERROR "Unable to locate \"${Dependency}\" which is a dependency of ${QtLib}")
      endif()
    endforeach()
  endforeach()
endforeach()
list(REMOVE_DUPLICATES Qt5AllLibsPathsRelease)
list(REMOVE_DUPLICATES Qt5AllLibsPathsDebug)

# Windows - Copy required dlls to binary directory
if(MSVC)
  # Image format plugins
  file(TO_CMAKE_PATH "${QT_BIN_DIR}/../plugins" QtPluginsPath)
  file(GLOB_RECURSE QtPluginsRelease "${QtPluginsPath}/*[^d].dll")
  file(GLOB_RECURSE QtPluginsDebug "${QtPluginsPath}/*d.dll")

  # QML Platforms dll's
  file(TO_CMAKE_PATH "${QT_BIN_DIR}/../qml" QtQmlPath)
  file(GLOB_RECURSE QtQmlCollection "${QtQmlPath}/*[^.pdb]")

  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Release")
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/Debug")

  # Qt Plugins DLLs
  function(TransferPluginDlls QtDlls BuildType)
    foreach(QtDll ${${QtDlls}})
      get_filename_component(DirPath "${QtDll}" PATH)
      get_filename_component(DirName "${DirPath}" NAME)
      if(DirName STREQUAL "platforms")
        set(OutputPath "${CMAKE_BINARY_DIR}/${BuildType}")
      else()
        set(OutputPath "${CMAKE_BINARY_DIR}/${BuildType}/plugins")
      endif()
      string(REPLACE "${QtPluginsPath}" "${OutputPath}" DestPath ${QtDll})
      get_filename_component(DestPath "${DestPath}" PATH)
      file(COPY ${QtDll} DESTINATION ${DestPath})
    endforeach()
  endfunction()

  TransferPluginDlls(QtPluginsRelease "Release")
  TransferPluginDlls(QtPluginsDebug "Debug")

  # Qt Qml dependencies
  function(TransferQMLDependencies QmlFiles BuildType)
    foreach(QmlFile ${${QmlFiles}})
      set(OutputPath "${CMAKE_BINARY_DIR}/${BuildType}/qml")
      string(REPLACE "${QtQmlPath}" "${OutputPath}" DestPath ${QmlFile})
      get_filename_component(DestPath "${DestPath}" PATH)
      if(BuildType STREQUAL "Debug")
        if(NOT QmlFile MATCHES "[^d].dll$")
          file(COPY ${QmlFile} DESTINATION ${DestPath})
        endif()
      else()
        if(NOT QmlFile MATCHES "d.dll$")
          file(COPY ${QmlFile} DESTINATION ${DestPath})
        endif()
      endif()
    endforeach()
  endfunction()

  TransferQMLDependencies(QtQmlCollection "Release")
  TransferQMLDependencies(QtQmlCollection "Debug")

  # Qt Bin DLLs
  function(TransferQtBinDlls QtDlls)
    string(REGEX MATCH Release ReleaseBuild ${QtDlls})
    string(REGEX MATCH Debug DebugBuild ${QtDlls})
    set(BuildType ${ReleaseBuild}${DebugBuild})
    foreach(QtDll ${${QtDlls}})
      file(COPY "${QtDll}" DESTINATION "${CMAKE_BINARY_DIR}/${BuildType}")
    endforeach()
  endfunction()

  TransferQtBinDlls(Qt5AllLibsPathsRelease)
  TransferQtBinDlls(Qt5AllLibsPathsDebug)
endif()

