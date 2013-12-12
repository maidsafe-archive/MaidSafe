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


get_branch(Branch)
set(Msg "Configuring third party projects on ${Branch} branch of super-project")
string(REGEX REPLACE . "-" Underscore ${Msg})
message("${HR}\n${Msg}\n${Underscore}")


include(add_boost)
include(add_catch)
add_subdirectory(cryptopp)
add_subdirectory(googlemock)
add_subdirectory(protobuf)
add_subdirectory(leveldb)
