#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed under the MaidSafe.net Commercial License, version 1.0 or    #
#  later, and The General Public License (GPL), version 3. By contributing code to this project    #
#  You agree to the terms laid out in the MaidSafe Contributor Agreement, version 1.0, found in    #
#  the root directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also    #
#  available at:                                                                                   #
#                                                                                                  #
#    http://www.novinet.com/license                                                                #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, software distributed under the       #
#  License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND,       #
#  either express or implied. See the License for the specific language governing permissions      #
#  and limitations under the License.                                                              #
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
