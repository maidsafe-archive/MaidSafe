#==================================================================================================#
#                                                                                                  #
#  Copyright 2012 MaidSafe.net limited                                                             #
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


ms_get_branch_and_commit(Branch Commit)
set(Msg "Configuring third party projects on ${Branch} of super-project at commit ${Commit}")
string(REGEX REPLACE . "-" Underscore ${Msg})
message("${HR}\n${Msg}\n${Underscore}")

# Patch googlemock to work on FreeBSD
if(BSD)
  message("Patching googlemock to run on FreeBSD ...")
  execute_process(COMMAND patch -N -p1
                  INPUT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/patches/googlemock/freebsd_support.patch
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/googlemock
                  RESULT_VARIABLE patch_output)
  message("  Patch output: ${patch_output}")
endif()


include(add_boost)
add_subdirectory(header_only)
add_subdirectory(cryptopp)
set(gmock_build_tests ON CACHE BOOL "Build all of Google Mock's own tests.")
set(gtest_build_tests ON CACHE BOOL "Build gtest's sample programs.")
add_subdirectory(googlemock)
mark_as_advanced(FORCE b2Path gmock_build_tests gtest_build_samples gtest_build_tests gtest_disable_pthreads gtest_force_shared_crt)
add_subdirectory(protobuf)
add_subdirectory(sqlite)


# Add third party tests
if(MaidsafeTesting)
  # Crypto++
  set(CamelCaseProjectName ThirdParty)
  set(AllExesForCurrentProject cryptest protobuf_lite_tests protobuf_tests ${AllSQLiteTests}
      ${AllGMockTests} ${AllGTestTests})
  ms_add_project_experimental()
  set(Timeout 60)
  ms_update_test_timeout(Timeout)
  set(Labels ThirdParty Behavioural CryptoPP)
  add_test(NAME "\"CryptoPP full validation\"" COMMAND cryptest v)
  set_tests_properties("\"CryptoPP full validation\"" PROPERTIES TIMEOUT ${Timeout} LABELS "${Labels}")
  ms_add_memcheck_ignore("CryptoPP full validation")
  add_test(NAME "\"CryptoPP test vectors for AES\"" COMMAND cryptest tv aes)
  set_tests_properties("\"CryptoPP test vectors for AES\"" PROPERTIES TIMEOUT ${Timeout} LABELS "${Labels}")
  ms_add_memcheck_ignore("CryptoPP test vectors for AES")
  add_test(NAME "\"CryptoPP test vectors for SHA\"" COMMAND cryptest tv sha)
  set_tests_properties("\"CryptoPP test vectors for SHA\"" PROPERTIES TIMEOUT ${Timeout} LABELS "${Labels}")
  ms_add_memcheck_ignore("CryptoPP test vectors for SHA")
  add_test(NAME "\"CryptoPP test vectors for RSA-PSS\"" COMMAND cryptest tv rsa_pss)
  set_tests_properties("\"CryptoPP test vectors for RSA-PSS\"" PROPERTIES TIMEOUT ${Timeout} LABELS "${Labels}")
  ms_add_memcheck_ignore("CryptoPP test vectors for RSA-PSS")
  add_test(NAME "\"CryptoPP test vectors for RSA-OAEP\"" COMMAND cryptest tv rsa_oaep)
  set_tests_properties("\"CryptoPP test vectors for RSA-OAEP\"" PROPERTIES TIMEOUT ${Timeout} LABELS "${Labels}")
  ms_add_memcheck_ignore("CryptoPP test vectors for RSA-OAEP")
  add_test(NAME "\"CryptoPP test vectors for RSA-PKCS1 v1.5\"" COMMAND cryptest tv rsa_pkcs1_1_5)
  set_tests_properties("\"CryptoPP test vectors for RSA-PKCS1 v1.5\"" PROPERTIES TIMEOUT ${Timeout} LABELS "${Labels}")
  ms_add_memcheck_ignore("CryptoPP test vectors for RSA-PKCS1 v1.5")

  # Protobuf
  set(Timeout 10)
  ms_update_test_timeout(Timeout)
  add_test(NAME protobuf_lite_tests COMMAND protobuf_lite_tests)
  add_test(NAME protobuf_tests COMMAND protobuf_tests)
  set_tests_properties(protobuf_lite_tests protobuf_tests PROPERTIES TIMEOUT ${Timeout} LABELS "ThirdParty;Behavioural;Protobuf")

  # SQLite
  add_test(NAME sqlite_test COMMAND sqlite_test)
  set_tests_properties(sqlite_test PROPERTIES TIMEOUT ${Timeout} LABELS "ThirdParty;Behavioural;SQLite")

  # GMock
  foreach(GMockTest ${AllGMockTests})
    add_test(NAME ${GMockTest} COMMAND ${GMockTest})
    if("${GMockTest}" STREQUAL "gmock-matchers_test")
      set(ExtendedTimeout 60)
      ms_update_test_timeout(ExtendedTimeout)
      set_tests_properties(gmock-matchers_test PROPERTIES TIMEOUT ${ExtendedTimeout} LABELS "ThirdParty;Behavioural;GMock")
    else()
      set_tests_properties(${GMockTest} PROPERTIES TIMEOUT ${Timeout} LABELS "ThirdParty;Behavioural;GMock")
    endif()
  endforeach()

  # GTest
  foreach(GTestTest ${AllGTestTests})
    add_test(NAME ${GTestTest} COMMAND ${GTestTest})
    set_tests_properties(${GTestTest} PROPERTIES TIMEOUT ${Timeout} LABELS "ThirdParty;Behavioural;GTest")
  endforeach()

endif()
