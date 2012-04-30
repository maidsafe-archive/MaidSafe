#==============================================================================#
#                                                                              #
#  Copyright (c) 2011 maidsafe.net limited                                     #
#  All rights reserved.                                                        #
#                                                                              #
#  Redistribution and use in source and binary forms, with or without          #
#  modification, are permitted provided that the following conditions are met: #
#                                                                              #
#      * Redistributions of source code must retain the above copyright        #
#        notice, this list of conditions and the following disclaimer.         #
#      * Redistributions in binary form must reproduce the above copyright     #
#        notice, this list of conditions and the following disclaimer in the   #
#        documentation and/or other materials provided with the distribution.  #
#      * Neither the name of the maidsafe.net limited nor the names of its     #
#        contributors may be used to endorse or promote products derived from  #
#        this software without specific prior written permission.              #
#                                                                              #
#  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
#  AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
#  IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
#  ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE  #
#  LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
#  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
#  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
#  INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
#  CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
#  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
#  POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Written by maidsafe.net team                                                #
#                                                                              #
#==============================================================================#
#                                                                              #
#  For a test target named test_stuff, the module is invoked by calling        #
#  ADD_GTESTS(test_stuff)                                                      #
#                                                                              #
#  This module adds individual gtests by parsing the tests files at a very     #
#  basic level (e.g. there is no support for namespaces).  It also currently   #
#  doesn't support all gtest options (e.g. the only value parameter generators #
#  supported are "Range(begin, end[, step])", "Values(v1, v2, ..., vN)" and    #
#  "Bool()").                                                                  #
#                                                                              #
#  There is basic support for TEST(...), TEST_F(...), TEST_P(...),             #
#  TYPED_TEST(...) and TYPED_TEST_P(...).                                      #
#                                                                              #
#  There is also support for the MaidSafe macro style of                       #
#  TEST_MS_NET(fixture_name, test_type(FUNC or BEH), general_name, test_name)  #
#                                                                              #
#  All test names should be of the form "BEH_..." or "FUNC_..." (with an       #
#  optional "DISABLED_" prepended.  Tests named BEH_ will be treated as        #
#  behavioural tests and will have a CTest timeout of BEHAVIOURAL_TEST_TIMEOUT #
#  which can be set before invoking this module, or will default to 60s.       #
#  Tests named FUNC_ will be treated as functional tests and will have a CTest #
#  timeout of FUNCTIONAL_TEST_TIMEOUT which can also be set externally, or     #
#  will default to 600s.                                                       #
#                                                                              #
#  The variable MAIDSAFE_TEST_TYPE can be set to control which test types will #
#  be added; BEH for behavioural, FUNC for functional, and anything else for   #
#  all types.                                                                  #
#                                                                              #
#  If the test executables have postfixes included in their names, the         #
#  variable TEST_POSTFIX should be set appropriately.                          #
#                                                                              #
#  Individual tests to be excluded should have their full test name included   #
#  in the variable EXCLUDED_TESTS.                                             #
#                                                                              #
#  The variable ALL_GTESTS will maintain a list of all tests added via this    #
#  module.                                                                     #
#                                                                              #
#==============================================================================#


# Main function - the only one designed to be called from outside this module.
FUNCTION(ADD_GTESTS TEST_TARGET)
  IF(NOT BEHAVIOURAL_TEST_TIMEOUT)
    SET(BEHAVIOURAL_TEST_TIMEOUT 60)
  ENDIF()
  IF(NOT FUNCTIONAL_TEST_TIMEOUT)
    SET(FUNCTIONAL_TEST_TIMEOUT 600)
  ENDIF()

  IF(${MEMORY_CHECK})
    MATH(EXPR BEHAVIOURAL_TEST_TIMEOUT ${BEHAVIOURAL_TEST_TIMEOUT}*100)
    MATH(EXPR FUNCTIONAL_TEST_TIMEOUT ${FUNCTIONAL_TEST_TIMEOUT}*100)
  ENDIF()

  GET_TARGET_PROPERTY(GTEST_SOURCE_FILES ${TEST_TARGET} SOURCES)

  FOREACH(GTEST_SOURCE_FILE ${GTEST_SOURCE_FILES})
    GET_GTEST_TYPEDEF_TYPES(${GTEST_SOURCE_FILE})
  ENDFOREACH()

  FOREACH(GTEST_SOURCE_FILE ${GTEST_SOURCE_FILES})
    GET_GTEST_FIXTURES_TYPES(${GTEST_SOURCE_FILE})
    GET_GTEST_FIXTURES_VALUE_PARAMETERS(${GTEST_SOURCE_FILE})
    GET_GTEST_FIXTURES_TYPE_PARAMETERS(${GTEST_SOURCE_FILE})
  ENDFOREACH()

  FOREACH(GTEST_SOURCE_FILE ${GTEST_SOURCE_FILES})
    ADD_GTEST_REGISTERED_TYPED_TESTS(${GTEST_SOURCE_FILE} ${TEST_TARGET})
    ADD_GTEST_NON_TYPE_PARAMETERISED(${GTEST_SOURCE_FILE} ${TEST_TARGET})
  ENDFOREACH()
  SET(ALL_GTESTS ${ALL_GTESTS} PARENT_SCOPE)
ENDFUNCTION()


# Gets all type values from all "typedef testing::Types<type, type, ...> varname" statements.
# For each varname, a variable named "varname_VAR" is set in parent_scope which contains all
# the types specified in the template parameters.
FUNCTION(GET_GTEST_TYPEDEF_TYPES GTEST_SOURCE_FILE)
  FILE(STRINGS ${GTEST_SOURCE_FILE} FILE_AS_STRING NEWLINE_CONSUME)
  STRING(REGEX MATCHALL testing::Types[.\n]*[^;]* TYPEDEFS ${FILE_AS_STRING})
  IF(NOT TYPEDEFS)
    RETURN()
  ENDIF()
  FOREACH(TYPEDEF ${TYPEDEFS})
    STRING(REGEX REPLACE testing::Types<+ "" TYPEDEF ${TYPEDEF})
    STRING(REGEX REPLACE \n "" TYPEDEF ${TYPEDEF})
    STRING(REGEX MATCH >.* TYPEDEF_NAME ${TYPEDEF})
    STRING(REGEX REPLACE ${TYPEDEF_NAME} "" TYPEDEF ${TYPEDEF})
    STRING(REGEX REPLACE ["\ >"] "" TYPEDEF_NAME ${TYPEDEF_NAME})
    STRING(REGEX REPLACE ,["\ "]* ";" TYPEDEF ${TYPEDEF})
    SET(${TYPEDEF_NAME}_VAR ${TYPEDEF} PARENT_SCOPE)
  ENDFOREACH()
ENDFUNCTION()


# Gets instances of typed tests per test fixture.  For a fixture named test_fixture, a variable named
# "test_fixture_TYPES" is set in parent scope which contains a list of the fixture's names, but with
# the test case name portion replaced with "@@@@@" in preparation for adding the actual test case names
# for the fixture.  The names will be of the form "test_fixture/0.@@@@@", "test_fixture/1.@@@@@", ...
# "test_fixture/x.@@@@@" with "x" equal to the number of different types - 1 (as count starts at 0).
FUNCTION(GET_GTEST_FIXTURES_TYPES GTEST_SOURCE_FILE)
  FILE(STRINGS ${GTEST_SOURCE_FILE} FILE_AS_STRING NEWLINE_CONSUME)
  STRING(REGEX MATCHALL TYPED_TEST_CASE[.\n]*[^;]* TEST_TYPES ${FILE_AS_STRING})
  IF(NOT TEST_TYPES)
    RETURN()
  ENDIF()
  FOREACH(TEST_TYPE ${TEST_TYPES})
    IF(NOT "${TEST_TYPE}" MATCHES "^TYPED_TEST_CASE_P")
      STRING(REGEX REPLACE TYPED_TEST_CASE["\("]+ "" TEST_TYPE ${TEST_TYPE})
      STRING(REGEX REPLACE [\n\ ] "" TEST_TYPE ${TEST_TYPE})
      STRING(REGEX MATCH [^,]+ GTEST_FIXTURE_NAME ${TEST_TYPE})
      STRING(REGEX REPLACE ${GTEST_FIXTURE_NAME}, "" TEST_TYPE ${TEST_TYPE})
      STRING(REGEX REPLACE ["\)"] "" TYPEDEF_NAME ${TEST_TYPE})
      SET(PARAMETER_COUNT 0)
      FOREACH(TYPE ${${TYPEDEF_NAME}_VAR})
        SET(${GTEST_FIXTURE_NAME}_TYPES ${${GTEST_FIXTURE_NAME}_TYPES} "${GTEST_FIXTURE_NAME}/${PARAMETER_COUNT}.@@@@@")
        MATH(EXPR PARAMETER_COUNT ${PARAMETER_COUNT}+1)
      ENDFOREACH()
      SET(${GTEST_FIXTURE_NAME}_TYPES ${${GTEST_FIXTURE_NAME}_TYPES} PARENT_SCOPE)
    ENDIF()
  ENDFOREACH()
ENDFUNCTION()


# Gets instances of value-parameterised tests per test fixture.  For a fixture named test_fixture,
# a variable named "test_fixture_VALUE_PARAMETERS" is set in parent scope which contains a list of
# the fixture's names, but with the test case name portion replaced with "@@@@@" in preparation for
# adding the actual test case names for the fixture.  The names will be of the form
# "instantiation_name/test_fixture.@@@@@/0", "instantiation_name/test_fixture.@@@@@/1",  ...
# "instantiation_name/test_fixture.@@@@@/x", with "x" equal to the number of different parameterised
# values - 1 (as count starts at 0).
FUNCTION(GET_GTEST_FIXTURES_VALUE_PARAMETERS GTEST_SOURCE_FILE)
  FILE(STRINGS ${GTEST_SOURCE_FILE} FILE_AS_STRING NEWLINE_CONSUME)
  STRING(REGEX MATCHALL INSTANTIATE_TEST_CASE_P[.\n]*[^;]* INSTANTIATIONS ${FILE_AS_STRING})
  IF(NOT INSTANTIATIONS)
    RETURN()
  ENDIF()
  FOREACH(INSTANTIATION ${INSTANTIATIONS})
    STRING(REGEX REPLACE INSTANTIATE_TEST_CASE_P["\("]+ "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX REPLACE "//[^\n]*" "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX REPLACE [\n\ ] "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX MATCH [^,]+ PARTIAL_TEST_NAME ${INSTANTIATION})
    STRING(REGEX REPLACE ^${PARTIAL_TEST_NAME}, "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX MATCH [^,]+ GTEST_FIXTURE_NAME ${INSTANTIATION})
    SET(PARTIAL_TEST_NAME ${PARTIAL_TEST_NAME}/${GTEST_FIXTURE_NAME}.@@@@@)
    STRING(REGEX REPLACE ${GTEST_FIXTURE_NAME}, "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX MATCH Range RANGE_TYPE ${INSTANTIATION})
    STRING(REGEX MATCH Values VALUES_TYPE ${INSTANTIATION})
    STRING(REGEX MATCH Bool BOOL_TYPE ${INSTANTIATION})
    SET(PARAMETER_COUNT 0)
    IF(RANGE_TYPE)
      STRING(REGEX MATCHALL [0-9\\.]+ VALUES ${INSTANTIATION})
      LIST(GET VALUES 0 CURRENT)
      LIST(GET VALUES 1 STOP)
      LIST(LENGTH VALUES LIST_LENGTH)
      IF(LIST_LENGTH EQUAL 2)
        SET(STEP 1)
      ELSE()
        LIST(GET VALUES 2 STEP)
      ENDIF()
      WHILE(CURRENT LESS STOP)
        SET(${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS ${${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS} "${PARTIAL_TEST_NAME}/${PARAMETER_COUNT}")
        MATH(EXPR PARAMETER_COUNT ${PARAMETER_COUNT}+1)
        MATH(EXPR CURRENT ${CURRENT}+${STEP})
      ENDWHILE()
    ELSEIF(VALUES_TYPE)
      STRING(REGEX REPLACE "[^\(]*[\(](.*)[\)][\)]" "\\1" INSTANTIATION ${INSTANTIATION})
      STRING(REGEX REPLACE "[\(][^\)]*[\)]" "" INSTANTIATION ${INSTANTIATION})
      STRING(REGEX REPLACE "," ";" VALUES ${INSTANTIATION})
      FOREACH(VALUE ${VALUES})
        SET(${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS ${${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS} "${PARTIAL_TEST_NAME}/${PARAMETER_COUNT}")
        MATH(EXPR PARAMETER_COUNT ${PARAMETER_COUNT}+1)
      ENDFOREACH()
    ELSEIF(BOOL_TYPE)
      SET(${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS ${${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS} "${PARTIAL_TEST_NAME}/0;${PARTIAL_TEST_NAME}/1")
    ENDIF()
    SET(${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS ${${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS} PARENT_SCOPE)
  ENDFOREACH()
ENDFUNCTION()


# Gets instances of type-parameterised tests per test fixture.  For a fixture named test_fixture,
# a variable named "test_fixture_TYPE_PARAMETERS" is set in parent scope which contains a list of
# the fixture's names, but with the test case name portion replaced with "@@@@@" in preparation for
# adding the actual test case names for the fixture.  The names will be of the form
# "instantiation_name/test_fixture/0.@@@@@", "instantiation_name/test_fixture/1.@@@@@",  ...
# "instantiation_name/test_fixture/x.@@@@@", with "x" equal to the number of different parameterised
# types - 1 (as count starts at 0).
FUNCTION(GET_GTEST_FIXTURES_TYPE_PARAMETERS GTEST_SOURCE_FILE)
  FILE(STRINGS ${GTEST_SOURCE_FILE} FILE_AS_STRING NEWLINE_CONSUME)
  STRING(REGEX MATCHALL INSTANTIATE_TYPED_TEST_CASE_P[.\n]*[^;]* INSTANTIATIONS ${FILE_AS_STRING})
  IF(NOT INSTANTIATIONS)
    RETURN()
  ENDIF()
  FOREACH(INSTANTIATION ${INSTANTIATIONS})
    STRING(REGEX REPLACE INSTANTIATE_TYPED_TEST_CASE_P["\("]+ "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX REPLACE [\n\ ] "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX MATCH [^,]+ PARTIAL_TEST_NAME ${INSTANTIATION})
    STRING(REGEX REPLACE ${PARTIAL_TEST_NAME}, "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX MATCH [^,]+ GTEST_FIXTURE_NAME ${INSTANTIATION})
    SET(PARTIAL_TEST_NAME ${PARTIAL_TEST_NAME}/${GTEST_FIXTURE_NAME})
    STRING(REGEX REPLACE ${GTEST_FIXTURE_NAME}, "" INSTANTIATION ${INSTANTIATION})
    STRING(REGEX REPLACE ["\)"] "" TYPEDEF_NAME ${INSTANTIATION})
    # The 3rd parameter of INSTANTIATE_TYPED_TEST_CASE_P can be either a single type to be run, or a typedef
    # of a testing::Types with several types to be run.  If we can't find a parsed typedef to match we'll
    # assume it's a single type.
    IF(DEFINED ${TYPEDEF_NAME}_VAR)
      SET(PARAMETER_COUNT 0)
      FOREACH(TYPE ${${TYPEDEF_NAME}_VAR})
        SET(${GTEST_FIXTURE_NAME}_TYPE_PARAMETERS ${${GTEST_FIXTURE_NAME}_TYPE_PARAMETERS} ${PARTIAL_TEST_NAME}/${PARAMETER_COUNT}.@@@@@)
        MATH(EXPR PARAMETER_COUNT ${PARAMETER_COUNT}+1)
      ENDFOREACH()
    ELSE()
      SET(${GTEST_FIXTURE_NAME}_TYPE_PARAMETERS ${${GTEST_FIXTURE_NAME}_TYPE_PARAMETERS} ${PARTIAL_TEST_NAME}/0.@@@@@)
    ENDIF()
    SET(${GTEST_FIXTURE_NAME}_TYPE_PARAMETERS ${${GTEST_FIXTURE_NAME}_TYPE_PARAMETERS} PARENT_SCOPE)
  ENDFOREACH()
ENDFUNCTION()


# For type-parameterised tests, this reads the contents of macros REGISTER_TYPED_TEST_CASE_P(...).
# These contain the test fixture and all the test cases for that fixture.  This consequently
# adds tests by replacing the "@@@@@"s in each value in "test_fixture_TYPE_PARAMETERS" (which is
# generated by the function GET_GTEST_FIXTURES_TYPE_PARAMETERS above) multiple times - once per
# test name.
FUNCTION(ADD_GTEST_REGISTERED_TYPED_TESTS GTEST_SOURCE_FILE TEST_TARGET)
  SET(TEST_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_TARGET}${TEST_POSTFIX})
  FILE(STRINGS ${GTEST_SOURCE_FILE} FILE_AS_STRING NEWLINE_CONSUME)
  STRING(REGEX MATCHALL REGISTER_TYPED_TEST_CASE_P[.\n]*[^;]* REGISTEREDS ${FILE_AS_STRING})
  IF(NOT REGISTEREDS)
    RETURN()
  ENDIF()
  FOREACH(REGISTERED ${REGISTEREDS})
    STRING(REGEX REPLACE REGISTER_TYPED_TEST_CASE_P["\("]+ "" REGISTERED ${REGISTERED})
    STRING(REGEX REPLACE [\n\ ] "" REGISTERED ${REGISTERED})
    STRING(REGEX MATCH [^,]+ GTEST_FIXTURE_NAME ${REGISTERED})
    STRING(REGEX REPLACE ${GTEST_FIXTURE_NAME}, "" REGISTERED ${REGISTERED})
    STRING(REGEX REPLACE ["\)"] "" REGISTERED ${REGISTERED})
    STRING(REGEX REPLACE [,] ";" GTEST_NAMES ${REGISTERED})
    FOREACH(GTEST_NAME ${GTEST_NAMES})
      FOREACH(TYPE_PARAMETER ${${GTEST_FIXTURE_NAME}_TYPE_PARAMETERS})
        STRING(REGEX REPLACE "@@@@@" ${GTEST_NAME} FULL_GTEST_NAME ${TYPE_PARAMETER})
        ADD_MAIDSAFE_TEST(${GTEST_FIXTURE_NAME} ${GTEST_NAME} ${FULL_GTEST_NAME} ${TEST_EXECUTABLE})
      ENDFOREACH()
    ENDFOREACH()
  ENDFOREACH()
  SET(ALL_GTESTS ${ALL_GTESTS} PARENT_SCOPE)
ENDFUNCTION()


# This reads the contents of macros TEST(...), TEST_F(...), TEST_P(...), and TYPED_TEST(...)
# (i.e. all tests other than type-parameterised tests).  These contain the test fixture and
# test name.  This consequently adds value-parameterised tests by replacing the "@@@@@"s in
# each value in "test_fixture_VALUE_PARAMETERS" (which is generated by the function
# GET_GTEST_FIXTURES_VALUE_PARAMETERS above) multiple times - once per test name.  It also
# adds typed tests by replacing the "@@@@@"s in each value in "test_fixture_TYPES" (which is
# generated by the function GET_GTEST_FIXTURES_TYPES above) multiple times - once per test
# name.  It also adds basic tests from TEST(...) and TEST_F(...).
FUNCTION(ADD_GTEST_NON_TYPE_PARAMETERISED GTEST_SOURCE_FILE TEST_TARGET)
  SET(TEST_EXECUTABLE ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${TEST_TARGET}${TEST_POSTFIX})
  FILE(STRINGS ${GTEST_SOURCE_FILE} GTEST_NAMES REGEX "^(TYPED_)?TEST(_[FP])?\\(")
  FOREACH(GTEST_NAME ${GTEST_NAMES})
    STRING(REGEX MATCH "TEST_MS_NET" TEST_IS_MS_NETWORK_TYPE ${GTEST_NAME})
    STRING(REGEX REPLACE ["\) \(,"] ";" GTEST_NAME ${GTEST_NAME})
    LIST(GET GTEST_NAME 0 GTEST_TEST_TYPE)
    LIST(GET GTEST_NAME 1 GTEST_FIXTURE_NAME)
    IF(TEST_IS_MS_NETWORK_TYPE)
      LIST(GET GTEST_NAME 3 GTEST_NAME_PART1)
      LIST(GET GTEST_NAME 5 GTEST_NAME_PART2)
      LIST(GET GTEST_NAME 7 GTEST_NAME_PART3)
      SET(GTEST_NAME "${GTEST_NAME_PART1}_${GTEST_NAME_PART2}_${GTEST_NAME_PART3}")
    ELSE()
      LIST(GET GTEST_NAME 3 GTEST_NAME)
    ENDIF()
    IF(${GTEST_TEST_TYPE} MATCHES ^TEST_P$)  # Value-parameterised tests
      FOREACH(VALUE_PARAMETER ${${GTEST_FIXTURE_NAME}_VALUE_PARAMETERS})
        STRING(REGEX REPLACE "@@@@@" ${GTEST_NAME} FULL_GTEST_NAME ${VALUE_PARAMETER})
        ADD_MAIDSAFE_TEST(${GTEST_FIXTURE_NAME} ${GTEST_NAME} ${FULL_GTEST_NAME} ${TEST_EXECUTABLE})
      ENDFOREACH()
    ELSEIF(${GTEST_TEST_TYPE} MATCHES ^TYPED_TEST$)  # Typed tests
      FOREACH(TYPE ${${GTEST_FIXTURE_NAME}_TYPES})
        STRING(REGEX REPLACE "@@@@@" ${GTEST_NAME} FULL_GTEST_NAME ${TYPE})
        ADD_MAIDSAFE_TEST(${GTEST_FIXTURE_NAME} ${GTEST_NAME} ${FULL_GTEST_NAME} ${TEST_EXECUTABLE})
      ENDFOREACH()
    ELSEIF(NOT ${GTEST_TEST_TYPE} MATCHES ^TYPED_TEST_P$)  # Others, excluding type-parameterised tests
      ADD_MAIDSAFE_TEST(${GTEST_FIXTURE_NAME} ${GTEST_NAME} ${GTEST_FIXTURE_NAME}.${GTEST_NAME} ${TEST_EXECUTABLE})
    ENDIF()
  ENDFOREACH()
  SET(ALL_GTESTS ${ALL_GTESTS} PARENT_SCOPE)
ENDFUNCTION()


# This adds the test (after checking it is of appropriate type and not explicitly excluded)
# and sets label and timeout properties.
FUNCTION(ADD_MAIDSAFE_TEST GTEST_FIXTURE_NAME GTEST_NAME FULL_GTEST_NAME TEST_EXECUTABLE)
  IF((${GTEST_NAME} MATCHES ${MAIDSAFE_TEST_TYPE}.+) OR (${MAIDSAFE_TEST_TYPE} MATCHES "ALL"))
    LIST(FIND EXCLUDED_TESTS ${FULL_GTEST_NAME} THIS_GTEST_EXCLUDED)
    IF(THIS_GTEST_EXCLUDED EQUAL -1)
      IF("${GTEST_NAME}" MATCHES "^DISABLED_")
        ADD_TEST(${FULL_GTEST_NAME} "__disabled__")
      ELSE()
        ADD_TEST(${FULL_GTEST_NAME} ${TEST_EXECUTABLE} --gtest_filter=${FULL_GTEST_NAME})
      ENDIF()
      IF("${GTEST_NAME}" MATCHES "^FUNC_" OR "${GTEST_NAME}" MATCHES "^DISABLED_FUNC_")
        SET_PROPERTY(TEST ${FULL_GTEST_NAME} PROPERTY LABELS Functional ${GTEST_FIXTURE_NAME})
        SET_PROPERTY(TEST ${FULL_GTEST_NAME} PROPERTY TIMEOUT ${FUNCTIONAL_TEST_TIMEOUT})
      ELSEIF("${GTEST_NAME}" MATCHES "^BEH_" OR "${GTEST_NAME}" MATCHES "^DISABLED_BEH_")
        SET_PROPERTY(TEST ${FULL_GTEST_NAME} PROPERTY LABELS Behavioural ${GTEST_FIXTURE_NAME})
        SET_PROPERTY(TEST ${FULL_GTEST_NAME} PROPERTY TIMEOUT ${BEHAVIOURAL_TEST_TIMEOUT})
      ELSE()
        MESSAGE("")
        MESSAGE("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
        MESSAGE("")
        MESSAGE(AUTHOR_WARNING "${GTEST_NAME} should be named \"BEH_...\" or \"FUNC_...\" (with an optional \"DISABLED_\" prepended).")
        MESSAGE("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
      ENDIF()
      SET(ALL_GTESTS ${ALL_GTESTS} ${FULL_GTEST_NAME} PARENT_SCOPE)
    ENDIF()
  ENDIF()
ENDFUNCTION()
