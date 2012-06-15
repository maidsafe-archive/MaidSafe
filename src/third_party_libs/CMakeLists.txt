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

add_subdirectory(boost)
if(WIN32)
  set(BoostDateTimeLibs debug boost_date_time-static-debug optimized boost_date_time-static PARENT_SCOPE)
  set(BoostFilesystemLibs debug boost_filesystem-static-debug optimized boost_filesystem-static PARENT_SCOPE)
  set(BoostProgramOptionsLibs debug boost_program_options-static-debug optimized boost_program_options-static PARENT_SCOPE)
  set(BoostSerializationLibs debug boost_serialization-static-debug optimized boost_serialization-static PARENT_SCOPE)
  set(BoostSystemLibs debug boost_system-static-debug optimized boost_system-static PARENT_SCOPE)
  set(BoostThreadLibs debug boost_thread-static-debug optimized boost_thread-static PARENT_SCOPE)
  set(BoostRegexLibs debug boost_regex-static-debug optimized boost_regex-static PARENT_SCOPE)
  set(BoostRegexLibs debug boost_regex-static-debug optimized boost_regex-static PARENT_SCOPE)
else()
  set(BoostDateTimeLibs boost_date_time-static PARENT_SCOPE)
  set(BoostFilesystemLibs boost_filesystem-static PARENT_SCOPE)
  set(BoostProgramOptionsLibs boost_program_options-static PARENT_SCOPE)
  set(BoostSerializationLibs boost_serialization-static PARENT_SCOPE)
  set(BoostSystemLibs boost_system-static PARENT_SCOPE)
  set(BoostThreadLibs boost_thread-static PARENT_SCOPE)
  set(BoostRegexLibs boost_regex-static PARENT_SCOPE)
endif()

add_subdirectory(cryptopp)
add_subdirectory(googlemock)
add_subdirectory(protobuf)
