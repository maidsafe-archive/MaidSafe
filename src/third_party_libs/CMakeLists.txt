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


set(HR "================================================================================")
message("${HR}\nConfiguring third party projects\n--------------------------------\n")


##########################################################################
# MaidSafe System Flags                                                  #
##########################################################################
if(WIN32)
  if (CMAKE_CL_64)
    add_definitions(-DWIN32 -D_WIN32 -D__WINDOWS__ -D__WIN32__ -DMAIDSAFE_WIN32 -DMAIDSAFE_WIN64)
  else()
    add_definitions(-DWIN32 -D_WIN32 -D__WINDOWS__ -D__WIN32__ -DMAIDSAFE_WIN32)
  endif()
elseif(UNIX)
  add_definitions(-D_FILE_OFFSET_BITS=64)
  if(APPLE)
    add_definitions(-DMAIDSAFE_APPLE -DHAVE_PTHREAD)
  else()
    add_definitions(-DMAIDSAFE_LINUX -DHAVE_PTHREAD)
  endif()
endif()


# This is duplicated from src\third_party_libs\boost\tools\build\CMake\BoostExternals.cmake
# which defaults each option to "ON".  This causes ALL options to be set ON during initial CMake
# run forcing us to preempt the definition of the options here if we want to avoid having all ON
# during the first run.
foreach(BoostExternal
    BZip2
    Doxygen
    Expat
    ICU
    MPI
    Python
    Xsltproc
    Valgrind
    ZLib
    )
  string(TOUPPER ${BoostExternal} BoostExternal)
  option(WITH_${BoostExternal} "Attempt to find and configure ${BoostExternal}" OFF)
endforeach()
set(WITH_PYTHON ON)
add_subdirectory(boost)
if(WIN32)
  set(BoostChronoLibs debug boost_chrono-static-debug optimized boost_chrono-static PARENT_SCOPE)
  set(BoostDateTimeLibs debug boost_date_time-static-debug optimized boost_date_time-static PARENT_SCOPE)
  set(BoostFilesystemLibs debug boost_filesystem-static-debug optimized boost_filesystem-static PARENT_SCOPE)
  set(BoostIostreamsLibs debug boost_iostreams-static-debug optimized boost_iostreams-static PARENT_SCOPE)
  set(BoostProgramOptionsLibs debug boost_program_options-static-debug optimized boost_program_options-static PARENT_SCOPE)
  set(BoostSystemLibs debug boost_system-static-debug optimized boost_system-static PARENT_SCOPE)
  set(BoostThreadLibs debug boost_thread-static-debug optimized boost_thread-static PARENT_SCOPE)
  set(BoostRegexLibs debug boost_regex-static-debug optimized boost_regex-static PARENT_SCOPE)
  set(BoostPythonLibs debug boost_python-static-debug optimized boost_python-static PARENT_SCOPE)
else()
  set(BoostChronoLibs boost_chrono-static PARENT_SCOPE)
  set(BoostDateTimeLibs boost_date_time-static PARENT_SCOPE)
  set(BoostFilesystemLibs boost_filesystem-static PARENT_SCOPE)
  set(BoostIostreamsLibs boost_iostreams-static PARENT_SCOPE)
  set(BoostProgramOptionsLibs boost_program_options-static PARENT_SCOPE)
  set(BoostSystemLibs boost_system-static PARENT_SCOPE)
  set(BoostThreadLibs boost_thread-static PARENT_SCOPE)
  set(BoostRegexLibs boost_regex-static PARENT_SCOPE)
  set(BoostPythonLibs boost_python-static PARENT_SCOPE)
endif()

if(WIN32)
  set(BoostChronoLibs debug boost_chrono-static-debug optimized boost_chrono-static)
  set(BoostDateTimeLibs debug boost_date_time-static-debug optimized boost_date_time-static)
  set(BoostFilesystemLibs debug boost_filesystem-static-debug optimized boost_filesystem-static)
  set(BoostIostreamsLibs debug boost_iostreams-static-debug optimized boost_iostreams-static)
  set(BoostProgramOptionsLibs debug boost_program_options-static-debug optimized boost_program_options-static)
  set(BoostSystemLibs debug boost_system-static-debug optimized boost_system-static)
  set(BoostThreadLibs debug boost_thread-static-debug optimized boost_thread-static)
  set(BoostRegexLibs debug boost_regex-static-debug optimized boost_regex-static)
  set(BoostPythonLibs debug boost_python-static-debug optimized boost_python-static)
else()
  set(BoostChronoLibs boost_chrono-static)
  set(BoostDateTimeLibs boost_date_time-static)
  set(BoostFilesystemLibs boost_filesystem-static)
  set(BoostIostreamsLibs boost_iostreams-static)
  set(BoostProgramOptionsLibs boost_program_options-static)
  set(BoostSystemLibs boost_system-static)
  set(BoostThreadLibs boost_thread-static)
  set(BoostRegexLibs boost_regex-static)
  set(BoostPythonLibs boost_python-static)
endif()

add_subdirectory(cryptopp)
add_subdirectory(googlemock)
add_subdirectory(protobuf)
add_subdirectory(leveldb)
