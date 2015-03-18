#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
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
#  Checks that the license files and contributor agreement in the root of the project are the      #
#  same as the ones on maidsafe.net/licenses                                                       #
#                                                                                                  #
#==================================================================================================#


set(CONTRIBUTOR_SHA1 d784aed1244b46dc713fd56e443709d3e3f74e98)
set(COPYING_SHA1 83d6bdeba338394c5e260b658944913f1b86a435)
set(LICENSE_SHA1 8085a72a68312ed412a8acee09e8367ec3df9885)
set(BaseURL http://maidsafe.net/licenses)



function(ms_get_file Filename)
  ms_get_todays_temp_folder()
  set(DownloadedFile "${TodaysTempFolder}/${Filename}.txt")
  if(EXISTS ${DownloadedFile})
    # If the existing copy matches the SHA1, we're done.  Otherwise delete it and download again.
    file(STRINGS ${DownloadedFile} Contents NEWLINE_CONSUME)
    string(SHA1 CopySHA1 "${Contents}")
    if(CopySHA1 STREQUAL "${${Filename}_SHA1}")
      return()
    else()
      file(REMOVE ${DownloadedFile})
    endif()
  endif()

  # Don't use the SHA1 inside the download command, since a 404 will cause CMake to fail.
  file(DOWNLOAD ${BaseURL}/${Filename}.txt ${DownloadedFile} TIMEOUT 5 STATUS Status)
  list(GET Status 0 ErrorCode)
  if(ErrorCode)
    message(STATUS "Failed to download ${BaseURL}/${Filename}.txt   ${ErrorCode}")
    file(REMOVE ${DownloadedFile})
  endif()

  if(EXISTS ${DownloadedFile})
    file(STRINGS ${DownloadedFile} Contents NEWLINE_CONSUME)
    string(SHA1 CopySHA1 "${Contents}")
    if(NOT CopySHA1 STREQUAL "${${Filename}_SHA1}")
      message(AUTHOR_WARNING "\n${BaseURL}/${File}.txt SHA1 not as expected.\n")
    endif()
  endif()
endfunction()



function(ms_check_licenses)
  string(REGEX MATCH "third_party_libs" ThirdPartyLib "${CMAKE_CURRENT_SOURCE_DIR}")
  string(REGEX MATCH "launcher/ui" LauncherUi "${CMAKE_CURRENT_SOURCE_DIR}")
  string(REGEX MATCH "crux" Crux "${CMAKE_CURRENT_SOURCE_DIR}")
  if(ThirdPartyLib OR LauncherUi OR Crux)
    return()
  endif()

  foreach(File CONTRIBUTOR COPYING LICENSE)
    ms_get_file(${File})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${File})
      message(AUTHOR_WARNING "\nThis repository should contain \"${File}\" in its root, identical to ${BaseURL}/${File}.txt\n")
    else()
      file(STRINGS ${CMAKE_CURRENT_SOURCE_DIR}/${File} Contents NEWLINE_CONSUME)
      string(SHA1 FileSHA1 "${Contents}")
      if(NOT FileSHA1 STREQUAL "${${File}_SHA1}")
        message(AUTHOR_WARNING "\n${CMAKE_CURRENT_SOURCE_DIR}/${File} SHA1 not as expected.\n")
      endif()
    endif()
  endforeach()
endfunction()
