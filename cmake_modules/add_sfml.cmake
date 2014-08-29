#==================================================================================================#
#                                                                                                  #
#  Copyright 2014 MaidSafe.net limited                                                             #
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
#  Downloads, extracts and patches SFML.                                                           #
#                                                                                                  #
#  Requires CMAKE_VERSION >= 3.0.0 on Windows                                                      #
#                                                                                                  #
#  Only the first 2 variables should require regular maintenance, i.e. SfmlVersion & SfmlSHA1.     #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    SfmlAvailable (TRUE only if all libraries of SFML are available)                              #
#                                                                                                  #
#  If SFML is available, the added libraries are proper CMake targets, named sfml-audio,           #
#    sfml-graphics, sfml-network, sfml-system and sfml-window.  On Windows, an additional target   #
#    named sfml-main exists as an entry-point for a Windows GUI app.                               #
#                                                                                                  #
#==================================================================================================#


set(SfmlVersion 2.1)
set(SfmlSHA1 c27bdffdc4bedb5f6a20db03ceca715d42aa5752)


set(SfmlSuccess TRUE)

# Disable SFML on Windows with CMAKE < 3.0.0 as the tar extract fails
if(WIN32 AND NOT "${CMAKE_VERSION}" VERSION_GREATER 2.8.12.2)
  set(SfmlSuccess FALSE)
  message(WARNING "\nCMake version 3.0.0 or higher is required for SFML on Windows.\n")
endif()

if("${HAVE_LIBC++}")
  set(SfmlSuccess FALSE)
  message(WARNING "\nSFML not available with libc++.\n")
endif()

# Check prerequisites on Unix (Windows ones and some OS X ones are provided along with SFML)
if(UNIX AND SfmlSuccess)
  if(APPLE)
    set(Packages OpenGL OpenAL)
  else()
    set(Packages GLEW OpenGL OpenAL Freetype JPEG X11)
  endif()
  foreach(Package ${Packages})
    find_package(${Package} QUIET)
    string(TOUPPER ${Package} UppercasePackageName)
    if(NOT ${Package}_FOUND AND NOT ${UppercasePackageName}_FOUND)
      message(WARNING "SFML is unavailable due to missing prerequisite: ${Package}")
      set(SfmlSuccess FALSE)
      break()
    endif()
  endforeach()
  if(NOT APPLE)
    find_path(SndfileIncludeDir sndfile.h)
    find_library(SndfileLib NAMES sndfile sndfile-1)
    if(NOT SndfileIncludeDir OR NOT SndfileLib)
      message(WARNING "SFML is unavailable due to missing prerequisite: Sndfile")
      set(SfmlSuccess FALSE)
    endif()
  endif()
endif()

# Create build folder with name derived from version
if(SfmlSuccess)
  set(SfmlFolderName sfml_${SfmlVersion})
  string(REPLACE "." "_" SfmlFolderName ${SfmlFolderName})
  set(SfmlRoot "${CMAKE_BINARY_DIR}/${SfmlFolderName}")
  file(MAKE_DIRECTORY "${SfmlRoot}")

  # Download SFML
  set(SfmlUrl "http://www.sfml-dev.org/download/sfml/${SfmlVersion}/SFML-${SfmlVersion}-sources.zip")
  set(SfmlZip "${SfmlRoot}/SFML-${SfmlVersion}-sources.zip")
  set(SfmlTimeout 60)
  if(NOT EXISTS "${SfmlZip}")
    message(STATUS "Downloading SFML ${SfmlVersion} to ${SfmlRoot}")
  endif()
  file(DOWNLOAD "${SfmlUrl}" "${SfmlZip}"
       STATUS Status
       SHOW_PROGRESS
       EXPECTED_HASH SHA1=${SfmlSHA1}
       )
  list(GET Status 0 StatusCode)
  list(GET Status 1 StatusString)
  if(NOT StatusCode EQUAL 0)
    message(WARNING "SFML is unavailable due to error while downloading '${SfmlUrl}'.  StatusCode: ${StatusCode}.  StatusString: ${StatusString}.  Log: ${Log}")
    set(SfmlSuccess FALSE)
  endif()
endif()

# Extract SFML
if(SfmlSuccess AND NOT IS_DIRECTORY "${SfmlRoot}/sfml")
  message(STATUS "Extracting SFML ${SfmlVersion} to ${SfmlRoot}/extracted")
  file(MAKE_DIRECTORY "${SfmlRoot}/extracted")
  execute_process(COMMAND ${CMAKE_COMMAND} -E tar xfz ${SfmlZip}
                  WORKING_DIRECTORY ${SfmlRoot}/extracted
                  RESULT_VARIABLE Result
                  OUTPUT_QUIET
                  ERROR_QUIET
                  )
  if(Result EQUAL 0)
    file(GLOB ExtractedContents "${SfmlRoot}/extracted/*")
    get_filename_component(ExtractedContents ${ExtractedContents} ABSOLUTE)
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory "${SfmlRoot}/sfml")
    file(RENAME "${ExtractedContents}" "${SfmlRoot}/sfml")
    file(REMOVE_RECURSE "${SfmlRoot}/extracted")
  else()
    file(REMOVE_RECURSE "${SfmlRoot}/extracted")
    message(WARNING "SFML is unavailable due to error while extracting '${SfmlZip}'.")
    set(SfmlSuccess FALSE)
  endif()
endif()

# Patch SFML
if(SfmlSuccess AND MSVC)
  configure_file(patches/${SfmlFolderName}/Macros.cmake ${SfmlRoot}/sfml/cmake/Macros.cmake COPYONLY)
endif()

# Add SFML as a subdirectory
if(SfmlSuccess)
  set(SFML_BUILD_EXAMPLES ON)
  add_subdirectory("${SfmlRoot}/sfml" ${CMAKE_CURRENT_BINARY_DIR}/sfml)
  set(SfmlLibs sfml-audio sfml-graphics sfml-network sfml-system sfml-window)
  if(WIN32)
    list(APPEND SfmlLibs sfml-main)
  endif()
  foreach(SfmlLib ${SfmlLibs})
    if(NOT TARGET ${SfmlLib})
      message(WARNING "SFML is unavailable due to missing target ${SfmlLib}.")
      set(SfmlSuccess FALSE)
    else()
      if(MSVC)
        target_compile_options(${SfmlLib} PRIVATE $<$<BOOL:${MSVC}>:/wd4244 /wd4267 /wd4996>)
        set_property(TARGET ${SfmlLib} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS " /ignore:4006 /ignore:4221 ")
      endif()
      set_target_properties(${SfmlLib} PROPERTIES FOLDER "Third Party/SFML/Libraries")
      get_target_property(IncDirs ${SfmlLib} INCLUDE_DIRECTORIES)
      set_target_properties(${SfmlLib} PROPERTIES INCLUDE_DIRECTORIES "")
      list(REMOVE_ITEM IncDirs "${SfmlRoot}/sfml/src")
      target_include_directories(${SfmlLib} PUBLIC ${IncDirs} PRIVATE "${SfmlRoot}/sfml/src")
    endif()
  endforeach()
endif()

# Copy DLLs to binary dirs for Windows
if(SfmlSuccess AND MSVC)
  if(CMAKE_CL_64)
    file(GLOB Dlls "${SfmlRoot}/sfml/extlibs/bin/x64/*")
  else()
    file(GLOB Dlls "${SfmlRoot}/sfml/extlibs/bin/x86/*")
  endif()
  foreach(ConfigType ${CMAKE_CONFIGURATION_TYPES})
    file(COPY ${Dlls} DESTINATION ${CMAKE_BINARY_DIR}/${ConfigType})
  endforeach()
endif()

set(SfmlAvailable ${SfmlSuccess} CACHE INTERNAL "Availability of SFML libraries")
