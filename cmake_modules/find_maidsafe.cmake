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
#  Sets and caches MaidSafe_FOUND, MaidSafe_INCLUDE_DIRS and MaidSafe_LIBRARIES if successful.     #
#                                                                                                  #
#==================================================================================================#


set(MaidSafeLibs
    maidsafe_launcher
    maidsafe_api
    maidsafe_vault_manager
    maidsafe_drive_launcher
    maidsafe_drive
    maidsafe_nfs_client
    maidsafe_nfs_vault
    maidsafe_nfs_core
    maidsafe_encrypt
    maidsafe_network_viewer
    maidsafe_routing
    maidsafe_crux
    maidsafe_passport
    maidsafe_common
    boost_chrono
    boost_date_time
    boost_filesystem
    boost_locale
    boost_program_options
    boost_regex
    boost_system
    boost_thread
    cryptopp
    protobuf
    )

set(FoundMaidSafeIncludes TRUE)
set(FoundMaidSafeIncludes TRUE)

if(NOT FoundMaidSafeDebug)
  set(FoundMaidSafeDebug TRUE)
  foreach(Lib ${MaidSafeLibs})
    find_library(${Lib}PathDebug NAMES ${Lib}-d ${Lib}-mt lib${Lib}-vc120-mt-gd-1_55 PATHS ${CMAKE_CURRENT_LIST_DIR}/lib NO_DEFAULT_PATH)
    if(${Lib}PathDebug)
      set(MaidSafeDebugLibs ${MaidSafeDebugLibs} debug ${${Lib}PathDebug})
    else()
      set(FoundMaidSafeDebug FALSE)
    endif()
  endforeach()
  if(NOT FoundMaidSafeDebug)
    message(STATUS "Failed to find all Debug versions of MaidSafe libs.")
  endif()
endif()

if(NOT FoundMaidSafeRelease)
  set(FoundMaidSafeRelease TRUE)
  foreach(Lib ${MaidSafeLibs})
    find_library(${Lib}PathRelease NAMES ${Lib} ${Lib}-mt lib${Lib}-vc120-mt-1_55 PATHS ${CMAKE_CURRENT_LIST_DIR}/lib NO_DEFAULT_PATH)
    if(${Lib}PathRelease)
      set(MaidSafeReleaseLibs ${MaidSafeReleaseLibs} optimized ${${Lib}PathRelease})
    else()
      set(FoundMaidSafeRelease FALSE)
    endif()
  endforeach()
  if(NOT FoundMaidSafeRelease)
    message(STATUS "Failed to find all Release versions of MaidSafe libs.")
  endif()
endif()

if(NOT MaidSafeInclude)
  find_path(MaidSafeInclude NAMES maidsafe.h PATHS ${CMAKE_CURRENT_LIST_DIR}/include NO_DEFAULT_PATH)
  if(NOT MaidSafeInclude)
    message(STATUS "Failed to find MaidSafe includes")
  endif()
endif()

if(NOT MaidSafeThirdPartyInclude)
  find_path(MaidSafeThirdPartyInclude NAMES boost PATHS ${CMAKE_CURRENT_LIST_DIR}/include/maidsafe/third_party_libs NO_DEFAULT_PATH)
  if(NOT MaidSafeThirdPartyInclude)
    message(STATUS "Failed to find MaidSafe third-party includes")
  endif()
endif()

if(FoundMaidSafeRelease AND MaidSafeInclude AND MaidSafeThirdPartyInclude)
  set(MaidSafe_FOUND TRUE CACHE INTERNAL "" FORCE)
  set(MaidSafe_INCLUDE_DIRS "${MaidSafeInclude}" "${MaidSafeThirdPartyInclude}" CACHE INTERNAL "" FORCE)
  set(MaidSafe_LIBRARIES ${MaidSafeReleaseLibs})
  if(FoundMaidSafeDebug)
    list(APPEND MaidSafe_LIBRARIES ${MaidSafeDebugLibs})
  endif()
  set(MaidSafe_LIBRARIES ${MaidSafe_LIBRARIES} CACHE INTERNAL "" FORCE)
else()
  set(MaidSafe_FOUND FALSE CACHE INTERNAL "" FORCE)
endif()
