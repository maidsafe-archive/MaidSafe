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
#                                                                                                  #
#  For a test target named test_stuff, the module is invoked by calling                            #
#  'ms_add_gtests(test_stuff)' or if the test executable needs to bootstrap to a network prior to  #
#  running (i.e. it needs --bootstrap_file <path to file>) it should use the                       #
#  'ms_add_network_gtests(TESTstuff)' function instead.  In this case the bootstrap filepath can   #
#  be set in the variable 'BOOTSTRAP', or for convenience 'BOOTSTRAP' can be set to "none" (i.e.   #
#  use real SAFE network), "local" (use a machine-local network), or "testnet" (the most current   #
#  public SAFE testnet).  If 'BOOTSTRAP' is not set, "none" is assumed.                            #
#                                                                                                  #
#  This module adds individual gtests by parsing the tests files at a very basic level (e.g.       #
#  there is no support for namespaces).  It also currently doesn't support all gtest options       #
#  (e.g. the only value parameter generators supported are "Range(begin, end[, step])",            #
#  "Values(v1, v2, ..., vN)" and "Bool()").                                                        #
#                                                                                                  #
#  There is basic support for TEST(...), TEST_F(...), TEST_P(...), TYPED_TEST(...) and             #
#  TYPED_TEST_P(...).                                                                              #
#                                                                                                  #
#  All test names should be of the form "BEH_..." or "FUNC_..." (with an optional "DISABLED_"      #
#  prepended.  Tests named BEH_ will be treated as behavioural tests and will have a CTest         #
#  timeout of 'BehaviouralTestTimeout' which can be set before invoking this module, or will       #
#  default to 60s.  Tests named FUNC_ will be treated as functional tests and will have a CTest    #
#  timeout of 'FunctionalTestTimeout' which can also be set externally, or will default to 600s.   #
#                                                                                                  #
#  If 'GlobalTestTimeoutFactor' is defined, all timeouts are multiplied by this value.             #
#                                                                                                  #
#  The variable 'MAIDSAFE_TEST_TYPE' can be set to control which test types will be added; BEH     #
#  for behavioural, FUNC for functional, and anything else for all types.                          #
#                                                                                                  #
#  If the test executables have postfixes included in their names, the variable 'TestPostfix'      #
#  should be set appropriately.                                                                    #
#                                                                                                  #
#  Individual tests to be excluded should have their full test name included in the variable       #
#  'ExcludedTests'.                                                                                #
#                                                                                                  #
#  The variable 'AllGtests' will maintain a list of all tests added via this module.               #
#                                                                                                  #
#==================================================================================================#


# This passes a bootstrap file argument to the test executable.  The arg is specified in the CMake
# variable 'BOOTSTRAP'.  This can be one of "none", "local", or "testnet" (all case insensitive), or
# else can be a path to a bootstrap file.
function(ms_add_network_gtests TestTarget)
  get_network_test_arg()
  set(NetworkTestLabel Network)
  ms_add_gtests(${TestTarget})
  set(AllGtests ${AllGtests} PARENT_SCOPE)
endfunction()


# Main function - only this or 'ms_add_network_gtests' above are designed to be called from outside
# this module.
function(ms_add_gtests TestTarget)
  target_link_libraries(${TestTarget} gmock gtest)

#   if(NOT PrivateTestTimeout)
#     set(PrivateTestTimeout 10)
#   endif()
  if(NOT BehaviouralTestTimeout)
    set(BehaviouralTestTimeout 60)
  endif()
  if(NOT FunctionalTestTimeout)
    set(FunctionalTestTimeout 600)
  endif()

  get_target_property(GtestSourceFiles ${TestTarget} SOURCES)

  unset(RelativeSourceNames)
  foreach(GtestSourceFile ${GtestSourceFiles})
    # Read contents, remove single-line comments and block comments, and assign to file-specific variable
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/" "" FileName "${GtestSourceFile}")
    list(APPEND RelativeSourceNames ${FileName})
    file(STRINGS ${GtestSourceFile} ${FileName}Contents NEWLINE_CONSUME)
    string(REGEX REPLACE "//[^\n]*\n" "" ${FileName}Contents "${${FileName}Contents}")
    ms_remove_block_comments(${FileName}Contents)

    get_gtest_typedef_types(${${FileName}Contents})
  endforeach()

  foreach(FileName ${RelativeSourceNames})
    get_gtest_fixtures_types(${${FileName}Contents})
    get_gtest_fixtures_value_parameters(${${FileName}Contents})
    get_gtest_fixtures_type_parameters(${${FileName}Contents})
  endforeach()

  foreach(FileName ${RelativeSourceNames})
    add_gtest_registered_typed_tests(${${FileName}Contents} ${TestTarget})
    add_gtest_non_type_parameterised(${${FileName}Contents} ${TestTarget})
  endforeach()
  set(AllGtests ${AllGtests} PARENT_SCOPE)
endfunction()


if(NOT DEFINED SkipReturnCodeValue)
  message(AUTHOR_WARNING "\n'SkipReturnCodeValue' is undefined, but should be set to a value between 0 and 255 inclusive.\n")
elseif(NOT (SkipReturnCodeValue GREATER -1 AND SkipReturnCodeValue LESS 256))
  message(AUTHOR_WARNING "\n'SkipReturnCodeValue' is set to ${SkipReturnCodeValue}, but should be between 0 and 255 inclusive.\n")
endif()


macro(get_network_test_arg)
  if(BOOTSTRAP)
    string(TOLOWER "${BOOTSTRAP}" Bootstrap)
  else()
    set(Bootstrap "none")
  endif()

  if("${Bootstrap}" STREQUAL "local")
    set(NetworkTestArg "$<TARGET_FILE_DIR:${TestTarget}>/local_network_bootstrap.dat")
  elseif("${Bootstrap}" STREQUAL "testnet")
    set(NetworkTestArg "none")  # This works for now where the hard-coded fallbacks in Routing *are* the testnet contacts.
#      set(NetworkTestArg "${CMAKE_BINARY_DIR}/bootstrap_files/testnet_bootstrap.dat") # need to uncomment this iff we ever have a testnet and SAFE network at the same time.
  elseif("${Bootstrap}" STREQUAL "none")
    set(NetworkTestArg "none")
  else()
    set(NetworkTestArg "${BOOTSTRAP}")
  endif()
endmacro()


# Gets all type values from all "typedef testing::Types<type, type, ...> varname" statements.
# For each varname, a variable named "varname_VAR" is set in parent_scope which contains all
# the types specified in the template parameters.
function(get_gtest_typedef_types FileContents)
  string(REGEX MATCHALL "testing::Types[.\n]*[^;]*" Typedefs "${FileContents}")
  if(NOT Typedefs)
    return()
  endif()
  foreach(Typedef ${Typedefs})
    string(REGEX REPLACE "testing::Types<+" "" Typedef ${Typedef})
    string(REGEX REPLACE "\n" "" Typedef ${Typedef})
    string(REGEX MATCH ">.*" TypedefName ${Typedef})
    string(REGEX REPLACE ${TypedefName} "" Typedef ${Typedef})
    string(REGEX REPLACE ["\ >"] "" TypedefName ${TypedefName})
    string(REGEX REPLACE ,["\ "]* ";" Typedef ${Typedef})
    set(${TypedefName}Var ${Typedef} PARENT_SCOPE)
  endforeach()
endfunction()


# Gets instances of typed tests per test fixture.  For a fixture named test_fixture, a variable named
# "test_fixtureTypes" is set in parent scope which contains a list of the fixture's names, but with
# the test case name portion replaced with "@@@@@" in preparation for adding the actual test case names
# for the fixture.  The names will be of the form "test_fixture/0.@@@@@", "test_fixture/1.@@@@@", ...
# "test_fixture/x.@@@@@" with "x" equal to the number of different types - 1 (as count starts at 0).
function(get_gtest_fixtures_types FileContents)
  string(REGEX MATCHALL "TYPED_TEST_CASE[.\n]*[^;]*" TestTypes "${FileContents}")
  if(NOT TestTypes)
    return()
  endif()
  foreach(TestType ${TestTypes})
    if(NOT "${TestType}" MATCHES "^TYPED_TEST_CASE_P")
      string(REGEX REPLACE "TYPED_TEST_CASE[\(]+" "" TestType ${TestType})
      string(REGEX REPLACE "[\n\ ]" "" TestType ${TestType})
      string(REGEX MATCH "[^,]+" GtestFixtureName ${TestType})
      string(REGEX REPLACE "${GtestFixtureName}," "" TestType ${TestType})
      string(REGEX REPLACE "[\)]" "" TypedefName ${TestType})
      set(ParamaterCount 0)
      foreach(TYPE ${${TypedefName}Var})
        set(${GtestFixtureName}Types ${${GtestFixtureName}Types} "${GtestFixtureName}/${ParamaterCount}.@@@@@")
        math(EXPR ParamaterCount ${ParamaterCount}+1)
      endforeach()
      set(${GtestFixtureName}Types ${${GtestFixtureName}Types} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()


# Gets instances of value-parameterised tests per test fixture.  For a fixture named test_fixture,
# a variable named "test_fixtureValueParameters" is set in parent scope which contains a list of
# the fixture's names, but with the test case name portion replaced with "@@@@@" in preparation for
# adding the actual test case names for the fixture.  The names will be of the form
# "instantiation_name/test_fixture.@@@@@/0", "instantiation_name/test_fixture.@@@@@/1",  ...
# "instantiation_name/test_fixture.@@@@@/x", with "x" equal to the number of different parameterised
# values - 1 (as count starts at 0).
function(get_gtest_fixtures_value_parameters FileContents)
  string(REGEX MATCHALL "INSTANTIATE_TEST_CASE_P[.\n]*[^;]*" Instantiations "${FileContents}")
  if(NOT Instantiations)
    return()
  endif()
  foreach(Instantiation ${Instantiations})
    string(REGEX REPLACE "INSTANTIATE_TEST_CASE_P[\(]+" "" Instantiation ${Instantiation})
    string(REGEX REPLACE "//[^\n]*" "" Instantiation ${Instantiation})
    string(REGEX REPLACE "[\n\ ]" "" Instantiation ${Instantiation})
    string(REGEX MATCH "[^,]+" PartialTestName ${Instantiation})
    string(REGEX REPLACE "^${PartialTestName}," "" Instantiation ${Instantiation})
    string(REGEX MATCH "[^,]+" GtestFixtureName ${Instantiation})
    set(PartialTestName ${PartialTestName}/${GtestFixtureName}.@@@@@)
    string(REGEX REPLACE "${GtestFixtureName}," "" Instantiation ${Instantiation})
    string(REGEX MATCH "Range" RangeType ${Instantiation})
    string(REGEX MATCH "Values" ValuesType ${Instantiation})
    string(REGEX MATCH "Bool" BoolType ${Instantiation})
    set(ParamaterCount 0)
    if(RangeType)
      string(REGEX MATCHALL "[0-9\\.]+" Values ${Instantiation})
      list(GET Values 0 Current)
      list(GET Values 1 Stop)
      list(LENGTH Values ListLength)
      if(ListLength EQUAL 2)
        set(Step 1)
      else()
        list(GET Values 2 Step)
      endif()
      while(Current LESS Stop)
        set(${GtestFixtureName}ValueParameters ${${GtestFixtureName}ValueParameters} "${PartialTestName}/${ParamaterCount}")
        math(EXPR ParamaterCount ${ParamaterCount}+1)
        math(EXPR Current ${Current}+${Step})
      endwhile()
    elseif(ValuesType)
      string(REGEX REPLACE "[^\(]*[\(](.*)[\)][\)]" "\\1" Instantiation ${Instantiation})
      string(REGEX REPLACE "[\(][^\)]*[\)]" "" Instantiation ${Instantiation})
      string(REGEX REPLACE "," ";" Values ${Instantiation})
      foreach(VALUE ${Values})
        set(${GtestFixtureName}ValueParameters ${${GtestFixtureName}ValueParameters} "${PartialTestName}/${ParamaterCount}")
        math(EXPR ParamaterCount ${ParamaterCount}+1)
      endforeach()
    elseif(BoolType)
      set(${GtestFixtureName}ValueParameters ${${GtestFixtureName}ValueParameters} "${PartialTestName}/0;${PartialTestName}/1")
    endif()
    set(${GtestFixtureName}ValueParameters ${${GtestFixtureName}ValueParameters} PARENT_SCOPE)
  endforeach()
endfunction()


# Gets instances of type-parameterised tests per test fixture.  For a fixture named test_fixture,
# a variable named "test_fixtureTypeParameters" is set in parent scope which contains a list of
# the fixture's names, but with the test case name portion replaced with "@@@@@" in preparation for
# adding the actual test case names for the fixture.  The names will be of the form
# "instantiation_name/test_fixture/0.@@@@@", "instantiation_name/test_fixture/1.@@@@@",  ...
# "instantiation_name/test_fixture/x.@@@@@", with "x" equal to the number of different parameterised
# types - 1 (as count starts at 0).
function(get_gtest_fixtures_type_parameters FileContents)
  string(REGEX MATCHALL "INSTANTIATE_TYPED_TEST_CASE_P[.\n]*[^;]*" Instantiations "${FileContents}")
  if(NOT Instantiations)
    return()
  endif()
  foreach(Instantiation ${Instantiations})
    string(REGEX REPLACE "INSTANTIATE_TYPED_TEST_CASE_P[\(]+" "" Instantiation ${Instantiation})
    string(REGEX REPLACE "[\n\ ]" "" Instantiation ${Instantiation})
    string(REGEX MATCH "[^,]+" PartialTestName ${Instantiation})
    string(REGEX REPLACE "${PartialTestName}," "" Instantiation ${Instantiation})
    string(REGEX MATCH "[^,]+" GtestFixtureName ${Instantiation})
    set(PartialTestName ${PartialTestName}/${GtestFixtureName})
    string(REGEX REPLACE "${GtestFixtureName}," "" Instantiation ${Instantiation})
    string(REGEX REPLACE "[\)]" "" TypedefName ${Instantiation})
    # The 3rd parameter of INSTANTIATE_TYPED_TEST_CASE_P can be either a single type to be run, or a typedef
    # of a testing::Types with several types to be run.  If we can't find a parsed typedef to match we'll
    # assume it's a single type.
    if(DEFINED ${TypedefName}Var)
      set(ParamaterCount 0)
      foreach(TYPE ${${TypedefName}Var})
        set(${GtestFixtureName}TypeParameters ${${GtestFixtureName}TypeParameters} ${PartialTestName}/${ParamaterCount}.@@@@@)
        math(EXPR ParamaterCount ${ParamaterCount}+1)
      endforeach()
    else()
      set(${GtestFixtureName}TypeParameters ${${GtestFixtureName}TypeParameters} ${PartialTestName}/0.@@@@@)
    endif()
    set(${GtestFixtureName}TypeParameters ${${GtestFixtureName}TypeParameters} PARENT_SCOPE)
  endforeach()
endfunction()


# For type-parameterised tests, this reads the contents of macros REGISTER_TYPED_TEST_CASE_P(...).
# These contain the test fixture and all the test cases for that fixture.  This consequently
# adds tests by replacing the "@@@@@"s in each value in "test_fixtureTypeParameters" (which is
# generated by the function get_gtest_fixtures_type_parameters above) multiple times - once per
# test name.
function(add_gtest_registered_typed_tests FileContents TestTarget)
  set(TestExecutable ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TestTarget}${TestPostfix})
  string(REGEX MATCHALL "REGISTER_TYPED_TEST_CASE_P[.\n]*[^;]*" Registereds "${FileContents}")
  if(NOT Registereds)
    return()
  endif()
  foreach(Registered ${Registereds})
    string(REGEX REPLACE "REGISTER_TYPED_TEST_CASE_P[\(]+" "" Registered ${Registered})
    string(REGEX REPLACE "[\n\ ]" "" Registered ${Registered})
    string(REGEX MATCH "[^,]+" GtestFixtureName ${Registered})
    string(REGEX REPLACE "${GtestFixtureName}," "" Registered ${Registered})
    string(REGEX REPLACE "[\)]" "" Registered ${Registered})
    string(REGEX REPLACE "[,]" ";" GtestNames ${Registered})
    foreach(GtestName ${GtestNames})
      foreach(TypeParameter ${${GtestFixtureName}TypeParameters})
        string(REGEX REPLACE "@@@@@" ${GtestName} FullGtestName ${TypeParameter})
        add_maidsafe_test(${GtestFixtureName} ${GtestName} ${FullGtestName} ${TestExecutable})
      endforeach()
    endforeach()
  endforeach()
  set(AllGtests ${AllGtests} PARENT_SCOPE)
endfunction()


# This reads the contents of macros TEST(...), TEST_F(...), TEST_P(...), and TYPED_TEST(...)
# (i.e. all tests other than type-parameterised tests).  These contain the test fixture and
# test name.  This consequently adds value-parameterised tests by replacing the "@@@@@"s in
# each value in "test_fixtureValueParameters" (which is generated by the function
# get_gtest_fixtures_value_parameters above) multiple times - once per test name.  It also
# adds typed tests by replacing the "@@@@@"s in each value in "test_fixtureTypes" (which is
# generated by the function get_gtest_fixtures_types above) multiple times - once per test
# name.  It also adds basic tests from TEST(...) and TEST_F(...).
function(add_gtest_non_type_parameterised FileContents TestTarget)
  set(TestExecutable ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TestTarget}${TestPostfix})
  string(REGEX MATCHALL "(TYPED_)?TEST(_[FP])?\\([^\\)]*\\)" GtestNames "${FileContents}")
  foreach(GtestName ${GtestNames})
    string(REGEX REPLACE "[\) \(,]" ";" GtestName ${GtestName})
    list(GET GtestName 0 GtestTestType)
    list(GET GtestName 1 GtestFixtureName)
    list(GET GtestName 3 GtestName)
    if(${GtestTestType} MATCHES "^TEST_P$")  # Value-parameterised tests
      foreach(ValueParameter ${${GtestFixtureName}ValueParameters})
        string(REGEX REPLACE "@@@@@" ${GtestName} FullGtestName ${ValueParameter})
        add_maidsafe_test(${GtestFixtureName} ${GtestName} ${FullGtestName} ${TestExecutable})
      endforeach()
    elseif(${GtestTestType} MATCHES "^TYPED_TEST$")  # Typed tests
      foreach(TYPE ${${GtestFixtureName}Types})
        string(REGEX REPLACE "@@@@@" ${GtestName} FullGtestName ${TYPE})
        add_maidsafe_test(${GtestFixtureName} ${GtestName} ${FullGtestName} ${TestExecutable})
      endforeach()
    elseif(NOT ${GtestTestType} MATCHES "^TYPED_TEST_P$")  # Others, excluding type-parameterised tests
      add_maidsafe_test(${GtestFixtureName} ${GtestName} ${GtestFixtureName}.${GtestName} ${TestExecutable})
    endif()
  endforeach()
  set(AllGtests ${AllGtests} PARENT_SCOPE)
endfunction()


macro(add_disabled_test)
  set(IsDisabledTest OFF)
  set(FakeExeName "__disabled__")
  list(FIND ExcludedTests ${FullGtestName} ThisGtestExcluded)
  if(NOT ThisGtestExcluded EQUAL -1)  # i.e. this test should be excluded
    set(IsDisabledTest ON)
    if(${CMAKE_VERSION} VERSION_LESS 3.0)
      add_test(${FullGtestName} ${FakeExeName})
    elseif(WIN32)
      get_filename_component(GitRoot "${Git_EXECUTABLE}" DIRECTORY)
      if(GitRoot)
        find_program(Sh NAMES sh PATHS ${GitRoot}/../bin)
        add_test(NAME ${FullGtestName} COMMAND "${Sh}" -c "exit ${SkipReturnCodeValue}")
      else()
        add_test(${FullGtestName} ${FakeExeName})
      endif()
    else()
      add_test(NAME ${FullGtestName} COMMAND sh -c "exit ${SkipReturnCodeValue}")
    endif()
  elseif(${CMAKE_VERSION} VERSION_LESS 3.0 AND "${GtestName}" MATCHES "^DISABLED_")
    set(IsDisabledTest ON)
    add_test(${FullGtestName} ${FakeExeName})
  endif()
endmacro()


# This adds the test (after checking it is of appropriate type and not explicitly excluded)
# and sets label and timeout properties.
function(add_maidsafe_test GtestFixtureName GtestName FullGtestName TestExecutable)
  if((${GtestName} MATCHES "${MAIDSAFE_TEST_TYPE}.+") OR (${MAIDSAFE_TEST_TYPE} MATCHES "ALL"))
    add_disabled_test()
    if(NOT IsDisabledTest)
      string(REPLACE "/" "" TestExecutable ${TestExecutable})
      if(RUNNING_AS_CTEST_SCRIPT)
        set(CATCH_EXCEPTIONS "1")
      else()
        set(CATCH_EXCEPTIONS "0")
      endif()
      add_test(NAME ${FullGtestName}
               COMMAND ${TestExecutable}
                   --gtest_filter=${FullGtestName}
                   --gtest_catch_exceptions=${CATCH_EXCEPTIONS}
                   $<$<BOOL:${NetworkTestArg}>:--bootstrap_file> $<$<BOOL:${NetworkTestArg}>:${NetworkTestArg}>)
    endif()
  endif()
  if("${GtestName}" MATCHES "^FUNC_" OR "${GtestName}" MATCHES "^DISABLED_FUNC_")
    set_property(TEST ${FullGtestName} PROPERTY LABELS ${CamelCaseProjectName} Functional ${NetworkTestLabel} ${TASK_LABEL})
    ms_update_test_timeout(FunctionalTestTimeout)
    set_property(TEST ${FullGtestName} PROPERTY TIMEOUT ${FunctionalTestTimeout})
  elseif("${GtestName}" MATCHES "^BEH_" OR "${GtestName}" MATCHES "^DISABLED_BEH_")
    set_property(TEST ${FullGtestName} PROPERTY LABELS ${CamelCaseProjectName} Behavioural ${NetworkTestLabel} ${TASK_LABEL})
    ms_update_test_timeout(BehaviouralTestTimeout)
    set_property(TEST ${FullGtestName} PROPERTY TIMEOUT ${BehaviouralTestTimeout})
#   elseif("${GtestName}" MATCHES "^PRIV_" OR "${GtestName}" MATCHES "^DISABLED_PRIV_")
#     set_property(TEST ${FullGtestName} PROPERTY LABELS ${CamelCaseProjectName} Private ${NetworkTestLabel} ${TASK_LABEL})
#     ms_update_test_timeout(PrivateTestTimeout)
#     set_property(TEST ${FullGtestName} PROPERTY TIMEOUT ${PrivateTestTimeout})
  elseif(NOT "${GtestName}" MATCHES "^//")
    message("")
    message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
    message("")
    message(AUTHOR_WARNING "${GtestName} should be named \"BEH_...\" or \"FUNC_...\" (with an optional \"DISABLED_\" prepended).")
    message("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
  endif()
  if(NOT ${CMAKE_VERSION} VERSION_LESS 3.0)
    set_property(TEST ${FullGtestName} PROPERTY SKIP_RETURN_CODE ${SkipReturnCodeValue})
  endif()
  set(AllGtests ${AllGtests} ${FullGtestName} PARENT_SCOPE)
endfunction()
