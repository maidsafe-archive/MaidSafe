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
#  Module used to locate Callback File System (CBFS) lib and header.                               #
#                                                                                                  #
#  Required variable for using CBFS is:                                                            #
#    CBFS_KEY                                                                                      #
#                                                                                                  #
#  This should be set to the value of the EldoS licence key, which must be provided to use CBFS.   #
#  This is automatically set via cloning a private MaidSafe repo for MaidSafe's use only.  To use  #
#  a different key, set CBFS_KEY when invoking cmake.  You can either provide the value of the     #
#  key or else the path to a file containing only the key's value:                                 #
#    cmake . -DCBFS_KEY=<Value of key>                                                             #
#  or                                                                                              #
#    cmake . -DCBFS_KEY=<Path to key file>                                                         #
#                                                                                                  #
#  Settable variables to aid with the CBFS module are:                                             #
#    CBFS_ROOT_DIR and DONT_USE_CBFS                                                               #
#                                                                                                  #
#  Variables set and cached by this module are:                                                    #
#    Cbfs_INCLUDE_DIR, Cbfs_LIBRARY_DIR, Cbfs_LIBRARY_DIR_DEBUG, Cbfs_LIBRARY,                     #
#    Cbfs_LIBRARY_DEBUG, Cbfs_LIBRARIES, Cbfs_KEY and Cbfs_FOUND                                   #
#                                                                                                  #
#==================================================================================================#


# Always retry to find CBFS in case user has provided a new location
unset(Cbfs_INCLUDE_DIR CACHE)
unset(Cbfs_LIBRARY_DIR CACHE)
unset(Cbfs_LIBRARY_DIR_DEBUG CACHE)
unset(Cbfs_LIBRARY CACHE)
unset(Cbfs_LIBRARY_DEBUG CACHE)
unset(Cbfs_LIBRARIES CACHE)
unset(Cbfs_KEY CACHE)
set(Cbfs_FOUND FALSE CACHE INTERNAL "")

# Allow user to explicitly avoid using CBFS
macro(not_using_cbfs)
  message(STATUS "Not using Callback File System.")
  return()
endmacro()

if(DONT_USE_CBFS)
  not_using_cbfs()
endif()

# If DONT_USE_CBFS=FALSE, assume CBFS is a requirement
if(DEFINED DONT_USE_CBFS)
  # DONT_USE_CBFS has been explicitly defined to FALSE, not just undefined
  set(CbfsRequired ON)
else()
  set(CbfsRequired OFF)
endif()

# If user defined CBFS_ROOT_DIR, assume CBFS is a requirement
if(CBFS_ROOT_DIR)
  set(CbfsRequired ON)
  set(CBFS_ROOT_DIR ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)
else()
  set(CbfsRequired ${CbfsRequired})
  set(CBFS_ROOT_DIR
        "C:/Program Files/EldoS/Callback File System"
        "D:/Program Files/EldoS/Callback File System"
        "E:/Program Files/EldoS/Callback File System"
        "C:/Program Files (x86)/EldoS/Callback File System"
        "D:/Program Files (x86)/EldoS/Callback File System"
        "E:/Program Files (x86)/EldoS/Callback File System")
endif()

# Prepare to find CBFS libs and headers
if(CMAKE_CL_64)
  set(CBFS_LIBPATH_SUFFIX "SourceCode/CallbackFS/CPP/x64/Release" "SourceCode/CBFS/CPP/x64/Release" "CPP/VC2008/64bit/static_runtime(MT)")
  set(CBFS_LIBPATH_SUFFIX_DEBUG "SourceCode/CallbackFS/CPP/x64/Debug" "SourceCode/CBFS/CPP/x64/Debug" "CPP/VC2008/64bit/static_runtime(MT)")
  set(CBFS_INCPATH_SUFFIX "SourceCode/CallbackFS/CPP" "SourceCode/CBFS/CPP" "CPP/VC2008/64bit/static_runtime(MT)")
else()
  set(CBFS_LIBPATH_SUFFIX "SourceCode/CallbackFS/CPP/Release" "SourceCode/CBFS/CPP/Release" "CPP/VC2008/32bit/static_runtime(MT)")
  set(CBFS_LIBPATH_SUFFIX_DEBUG "SourceCode/CallbackFS/CPP/Debug" "SourceCode/CBFS/CPP/Debug" "CPP/VC2008/32bit/static_runtime(MT)")
  set(CBFS_INCPATH_SUFFIX "SourceCode/CallbackFS/CPP" "SourceCode/CBFS/CPP" "CPP/VC2008/32bit/static_runtime(MT)")
endif()

function(fatal_find_error MissingComponent)
  set(ErrorMessage "\nCould not find Callback File System.  NO ${MissingComponent} - ")
  set(ErrorMessage "${ErrorMessage}If Cbfs is already installed, run:\n")
  set(ErrorMessage "${ErrorMessage}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=<Path to Cbfs root directory>\n")
  set(ErrorMessage "${ErrorMessage}e.g.\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_ROOT_DIR=\"C:\\Program Files\\EldoS\\Callback File System\"\n\n")
  message(FATAL_ERROR "${ErrorMessage}")
endfunction()

# Find CBFS Release lib
find_library(Cbfs_LIBRARY NAMES cbfs PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_LIBPATH_SUFFIX} NO_DEFAULT_PATH)
if(NOT Cbfs_LIBRARY)
  if(CbfsRequired)
    fatal_find_error("CBFS LIBRARY")
  else()
    not_using_cbfs()
  endif()
endif()

# Find CBFS Debug lib
find_library(Cbfs_LIBRARY_DEBUG NAMES cbfs PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_LIBPATH_SUFFIX_DEBUG} NO_DEFAULT_PATH)
if(NOT Cbfs_LIBRARY_DEBUG)
  if(CbfsRequired)
    fatal_find_error("CBFS DEBUG LIBRARY")
  else()
    not_using_cbfs()
  endif()
endif()

# Find CBFS header
find_path(Cbfs_INCLUDE_DIR CbFS.h PATHS ${CBFS_ROOT_DIR} PATH_SUFFIXES ${CBFS_INCPATH_SUFFIX} NO_DEFAULT_PATH)
if(NOT Cbfs_INCLUDE_DIR)
  if(CbfsRequired)
    fatal_find_error("CbFS.h")
  else()
    not_using_cbfs()
  endif()
endif()

# Set CBFS variables and include header path
get_filename_component(CBFS_ROOT_DIR ${Cbfs_LIBRARY} PATH)
set(Cbfs_LIBRARY_DIR ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)
get_filename_component(CBFS_ROOT_DIR ${Cbfs_LIBRARY_DEBUG} PATH)
set(Cbfs_LIBRARY_DIR_DEBUG ${CBFS_ROOT_DIR} CACHE PATH "Path to Callback File System library directory" FORCE)
set(Cbfs_LIBRARIES optimized ${Cbfs_LIBRARY} debug ${Cbfs_LIBRARY_DEBUG})

include_directories(SYSTEM ${Cbfs_INCLUDE_DIR})
if(CMAKE_INCLUDE_DIRECTORIES_BEFORE)
  set(INCLUDE_DIRS ${Cbfs_INCLUDE_DIR} ${INCLUDE_DIRS})
else()
  set(INCLUDE_DIRS ${INCLUDE_DIRS} ${Cbfs_INCLUDE_DIR})
endif()

message(STATUS "Found library ${Cbfs_LIBRARY}")
message(STATUS "Found library ${Cbfs_LIBRARY_DEBUG}")

# Find licence key
if(CBFS_KEY)
  if(EXISTS ${CBFS_KEY})  # User set CBFS_KEY to path to key file
    file(READ ${CBFS_KEY} Cbfs_KEY)
    set(Cbfs_KEY ${Cbfs_KEY} CACHE INTERNAL "")
  else()  # User set CBFS_KEY to value of key
    set(Cbfs_KEY ${CBFS_KEY} CACHE INTERNAL "")
  endif()
else()
  # Use MaidSafe's key
  set(LicenseFile ${CMAKE_BINARY_DIR}/MaidSafe-Drive-Private/eldos_licence_key.txt)
  set(ExpectedSHA512 e2de4a324268710fe780cd9c80841ce6c6d916411345001f9d06dfe3d0dc049e4df613acdaccb9b89232aa3654714985ed7245f93cf2c97c6060889291db0906)
  if(NOT EXISTS ${LicenseFile})
    # Clone MaidSafe-Drive-Private
    execute_process(COMMAND ${Git_EXECUTABLE} clone git@github.com:maidsafe/MaidSafe-Drive-Private.git
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                    TIMEOUT 20
                    RESULT_VARIABLE ResultVar
                    OUTPUT_VARIABLE OutputVar
                    ERROR_VARIABLE ErrorVar)
    if(NOT ${ResultVar} EQUAL 0)
      set(ErrorMessage "\nFailed to clone MaidSafe-Drive-Private.\n\n${OutputVar}\n\n${ErrorVar}\n\n")
      set(ErrorMessage "${ErrorMessage}If you don't have permission to clone ")
      set(ErrorMessage "${ErrorMessage}git@github.com:maidsafe/MaidSafe-Drive-Private.git, you need ")
      set(ErrorMessage "${ErrorMessage}to set CBFS_KEY when invoking cmake.  You can either provide ")
      set(ErrorMessage "${ErrorMessage}the value of the key or else the path to a file containing ")
      set(ErrorMessage "${ErrorMessage}only the key's value:\n")
      set(ErrorMessage "${ErrorMessage}${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_KEY=<Value of key>\n")
      set(ErrorMessage "${ErrorMessage}or\n${ERROR_MESSAGE_CMAKE_PATH} -DCBFS_KEY=<Path to key file>\n")
      message(FATAL_ERROR "${ErrorMessage}")
    endif()
    # Hash check file
    file(SHA512 ${LicenseFile} CbfsSHA512)
    if(NOT ${CbfsSHA512} STREQUAL ${ExpectedSHA512})
      file(RENAME ${CMAKE_BINARY_DIR}/MaidSafe-Drive-Private ${CMAKE_BINARY_DIR}/MaidSafe-Drive-Private-Failed)
      message(FATAL_ERROR "Failed hash check in MaidSafe-Drive-Private.")
    endif()
  else()
    # Hash check file
    file(SHA512 ${LicenseFile} CbfsSHA512)
    if(NOT ${CbfsSHA512} STREQUAL ${ExpectedSHA512})
      # Try pulling to get updated key file
      execute_process(COMMAND ${Git_EXECUTABLE} pull
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/MaidSafe-Drive-Private
                      TIMEOUT 20
                      RESULT_VARIABLE ResultVar
                      OUTPUT_VARIABLE OutputVar
                      ERROR_VARIABLE ErrorVar)
      if(NOT ${ResultVar} EQUAL 0)
        message(FATAL_ERROR "Failed to pull in MaidSafe-Drive-Private.\n\n${OutputVar}\n\n${ErrorVar}\n\n")
      endif()
      # Hash check updated file
      file(SHA512 ${LicenseFile} CbfsSHA512)
      if(NOT ${CbfsSHA512} STREQUAL ${ExpectedSHA512})
        file(RENAME ${CMAKE_BINARY_DIR}/MaidSafe-Drive-Private ${CMAKE_BINARY_DIR}/MaidSafe-Drive-Private-Failed)
        message(FATAL_ERROR "Failed hash check in MaidSafe-Drive-Private.")
      endif()
    endif()
  endif()
  # Read in the file contents
  file(READ ${LicenseFile} Cbfs_KEY)
  set(Cbfs_KEY ${Cbfs_KEY} CACHE INTERNAL "")
endif()

set(Cbfs_FOUND TRUE CACHE INTERNAL "")
