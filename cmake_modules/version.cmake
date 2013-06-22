#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2013 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#


set(ApplicationVersionMajor 0)
set(ApplicationVersionMinor 1)
set(ApplicationVersionPatch 002)

# Testing environment if ApplicationVersionMinor is odd
math(EXPR MaidsafeTesting ${ApplicationVersionMinor}%2)
