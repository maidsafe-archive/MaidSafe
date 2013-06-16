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


message("${HR}\nConfiguring third party projects\n--------------------------------")


# if(WIN32)
#   if (CMAKE_CL_64)
#     add_definitions(-DWIN32 -D_WIN32 -D__WINDOWS__ -D__WIN32__ -DMAIDSAFE_WIN32 -DMAIDSAFE_WIN64)
#   else()
#     add_definitions(-DWIN32 -D_WIN32 -D__WINDOWS__ -D__WIN32__ -DMAIDSAFE_WIN32)
#   endif()
# elseif(UNIX)
#   add_definitions(-D_FILE_OFFSET_BITS=64)
#   if(APPLE)
#     add_definitions(-DMAIDSAFE_APPLE -DHAVE_PTHREAD)
#   else()
#     add_definitions(-DMAIDSAFE_LINUX -DHAVE_PTHREAD)
#   endif()
# endif()


include(add_boost)
add_subdirectory(cryptopp)
add_subdirectory(googlemock)
add_subdirectory(protobuf)
add_subdirectory(leveldb)
