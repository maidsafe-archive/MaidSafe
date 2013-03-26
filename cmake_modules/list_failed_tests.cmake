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


set(FailedFileName FailedTests.log)
if(EXISTS "Testing/Temporary/LastTestsFailed.log")
  file(STRINGS "Testing/Temporary/LastTestsFailed.log" FailedTests)
  string(REGEX REPLACE "([0-9]+):[^;]*" "\\1" FailedTests "${FailedTests}")
  list(SORT FailedTests)
  list(GET FailedTests 0 FirstTest)
  set(FailedTests "${FirstTest};${FirstTest};;${FailedTests};")
  string(REPLACE ";" "," FailedTests "${FailedTests}")
  file(WRITE ${FailedFileName} ${FailedTests})
else()
  file(WRITE ${FailedFileName} "")
endif()
