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


set(CONTRIBUTOR_SHA1 3b84057fcdf973545740f563e8b88f8d23138ea3)
set(COPYING_SHA1 8624bcdae55baeef00cd11d5dfcfa60f68710a02)
set(LICENSE_SHA1 881acf26e7ead794a31309f0da1e350095dc839c)
set(BaseURL http://maidsafe.net/licenses)



function(ms_get_file Filename)
  set(DownloadedFile ${CMAKE_BINARY_DIR}/${Filename}.txt)
  if(EXISTS ${DownloadedFile})
    # Download a fresh copy of the file each day
    string(TIMESTAMP Now %Y%j UTC)
    file(TIMESTAMP ${DownloadedFile} LicenseTimestamp %Y%j UTC)
    math(EXPR CopyExpires ${LicenseTimestamp}+1)
    if(CopyExpires LESS Now)
      file(REMOVE ${DownloadedFile})
    else()
      # If the existing copy matches the SHA1, we're done.  Otherwise delete it and download again.
      file(SHA1 "${DownloadedFile}" CopySHA1)
      if("${CopySHA1}" STREQUAL "${${Filename}_SHA1}")
        return()
      else()
        file(REMOVE ${DownloadedFile})
      endif()
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
    file(SHA1 "${DownloadedFile}" CopySHA1)
    if(NOT "${CopySHA1}" STREQUAL "${${Filename}_SHA1}")
      message(AUTHOR_WARNING "\n${BaseURL}/${File}.txt SHA1 not as expected.\n")
    endif()
  endif()
endfunction()



function(ms_check_licenses)
  string(REGEX MATCH "third_party_libs" ThirdPartyLib "${CMAKE_CURRENT_SOURCE_DIR}")
  string(REGEX MATCH "routing/tools/network_viewer" NetworkViewer "${CMAKE_CURRENT_SOURCE_DIR}")
  if(ThirdPartyLib OR NetworkViewer)
    return()
  endif()

  foreach(File CONTRIBUTOR COPYING LICENSE)
    ms_get_file(${File})
    if(NOT EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/${File})
      message(AUTHOR_WARNING "\nThis repository should contain \"${File}\" in its root, identical to ${BaseURL}/${File}.txt\n")
    else()
      file(SHA1 "${CMAKE_CURRENT_SOURCE_DIR}/${File}" FileSHA1)
      if(NOT "${FileSHA1}" STREQUAL "${${File}_SHA1}")
        message(AUTHOR_WARNING "\n${CMAKE_CURRENT_SOURCE_DIR}/${File} SHA1 not as expected.\n")
      endif()
    endif()
  endforeach()
endfunction()
