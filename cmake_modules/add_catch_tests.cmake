#==================================================================================================#
#                                                                                                  #
#  Copyright 2013 MaidSafe.net limited                                                             #
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
#  Overview                                                                                        #
#  ========                                                                                        #
#  This module adds individual Catch tests by parsing the test files at a very basic level.  It    #
#  supports the macros TEST_CASE, SCENARIO, TEST_CASE_METHOD, CATCH_TEST_CASE, CATCH_SCENARIO,     #
#  and CATCH_TEST_CASE_METHOD.                                                                     #
#                                                                                                  #
#  The variable 'AllCatchTests' will maintain a list of all enabled tests added via this module.   #
#  The variable 'HiddenCatchTests' will maintain a list of all disabled tests added via this       #
#  module.                                                                                         #
#                                                                                                  #
#  Tags                                                                                            #
#  ====                                                                                            #
#  All tests must provide a name and tag(s).  The tags are case-insensitive and must contain at    #
#  least one of [unit], [behavioural] or [functional].  All Catch tags are added as LABELS         #
#  (without the square brackets) to the corresponding CTest test.                                  #
#                                                                                                  #
#  Disabling a Test                                                                                #
#  ================                                                                                #
#  A test can be disabled by adding the tag [hide].  Such a test will not be added to the list of  #
#  CTests, but its name will be added to the CMake variable HiddenCatchTests.  Even though the     #
#  test is disabled, it is still compiled.                                                         #
#                                                                                                  #
#  Timeouts                                                                                        #
#  ========                                                                                        #
#  All tests will be given a timeout corresponding to one of the required tags.  If more than one  #
#  such tag applies, the largest of the tag timeouts is chosen.  The default timeout for a test    #
#  can be overridden by adding a comment after the opening brace of the test body (on the same     #
#  line) of the form '// Timeout <value in seconds>', for example:                                 #
#    TEST_CASE("Crypto", "[Unit][RSA]") {  // Timeout 30                                           #
#                                                                                                  #
#  If 'GlobalTestTimeoutFactor' is defined, all timeouts are multiplied by this value, even        #
#  overridden ones.                                                                                #
#                                                                                                  #
#  Usage                                                                                           #
#  =====                                                                                           #
#  For a test target named test_vault, the module is invoked in a CMake file by calling:           #
#    ms_add_catch_tests(test_vault)                                                                #
#                                                                                                  #
#  To run e.g. all behavioural tests in Debug mode, from the build dir run:                        #
#    ctest -C Debug -L behavioural                                                                 #
#                                                                                                  #
#==================================================================================================#


# Default timeouts in seconds
set(FunctionalTimeout 600)
set(BehaviouralTimeout 60)
set(UnitTimeout 1)


# Main function - the only one designed to be called from outside this module.
function(ms_add_catch_tests TestTarget)
  target_compile_definitions(${TestTarget} PRIVATE USE_CATCH)
  get_target_property(SourceFiles ${TestTarget} SOURCES)
  foreach(SourceFile ${SourceFiles})
    ms_parse_file(${SourceFile} ${TestTarget})
  endforeach()
  set(AllCatchTests ${AllCatchTests} PARENT_SCOPE)
  set(HiddenCatchTests ${HiddenCatchTests} PARENT_SCOPE)
endfunction()


# Worker function
function(ms_parse_file SourceFile TestTarget)
  if(NOT EXISTS ${SourceFile})
    return()
  endif()
  file(STRINGS ${SourceFile} Contents NEWLINE_CONSUME)

  # Remove block comments and *full line* comments (leave partial line comments)
  ms_remove_block_comments(Contents)
  string(REGEX REPLACE "\n[ \t]*//+[^\n]+" "\n" Contents "${Contents}")

  # Find definition of test names
  string(REGEX MATCHALL "[ \t]*(CATCH_)?(TEST_CASE_METHOD|SCENARIO|TEST_CASE)[ \t]*\\([^\)]+\\)+[ \t]*{+[ \t]*(//[^\n]*[Tt][Ii][Mm][Ee][Oo][Uu][Tt][ \t]*[0-9]+)*" Tests "${Contents}")

  foreach(TestName ${Tests})
    # Strip newlines
    string(REGEX REPLACE "\\\\\n|\n" "" TestName "${TestName}")

    # Get test type and fixture if applicable
    string(REGEX MATCH "(CATCH_)?(TEST_CASE_METHOD|SCENARIO|TEST_CASE)[ \t]*\\([^,^\"]*" TestTypeAndFixture "${TestName}")
    string(REGEX MATCH "(CATCH_)?(TEST_CASE_METHOD|SCENARIO|TEST_CASE)" TestType "${TestTypeAndFixture}")
    string(REPLACE "${TestType}(" "" TestFixture "${TestTypeAndFixture}")

    # Get string parts of test definition
    string(REGEX MATCHALL "\"+([^\\^\"]|\\\\\")+\"+" TestStrings "${TestName}")

    # Get timeout comment if it exists
    string(REGEX REPLACE "(.*//[^\n]*[Tt][Ii][Mm][Ee][Oo][Uu][Tt][ \t]*)([0-9]+)" "\\2" ExplicitTimeout "${TestName}")

    # Strip wrapping quotation marks
    string(REGEX REPLACE "^\"(.*)\"$" "\\1" TestStrings "${TestStrings}")
    string(REPLACE "\";\"" ";" TestStrings "${TestStrings}")

    # Validate that a test name and tags have been provided
    list(LENGTH TestStrings TestStringsLength)
    if(NOT TestStringsLength EQUAL 2)
      message(FATAL_ERROR "You must provide a valid test name and tags for all tests in ${SourceFile}")
    endif()

    # Assign name and tags
    list(GET TestStrings 0 Name)
    if("${TestType}" STREQUAL "SCENARIO")
      set(Name "Scenario: ${Name}")
    endif()
    if(TestFixture)
      set(CTestName "${TestFixture}:${Name}")
    else()
      set(CTestName "${Name}")
    endif()
    list(GET TestStrings 1 Tags)
    string(TOLOWER "${Tags}" Tags)
    string(REPLACE "]" ";" Tags "${Tags}")
    string(REPLACE "[" "" Tags "${Tags}")

    # Validate that at least one of the required tags is provided and choose appropriate timeout
    set(Timeout 0)
    list(FIND Tags "unit" UnitFound)
    if(UnitFound GREATER -1)
      set(Timeout ${UnitTimeout})
    endif()
    list(FIND Tags "behavioural" BehaviouralFound)
    if(BehaviouralFound GREATER -1)
      set(Timeout ${BehaviouralTimeout})
    endif()
    list(FIND Tags "functional" FunctionalFound)
    if(FunctionalFound GREATER -1)
      set(Timeout ${FunctionalTimeout})
    endif()
    if(NOT Timeout)
      set(Warning "\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n")
      set(Warning "${Warning}You must set at least one tag of value [unit], [behavioural], or ")
      set(Warning "${Warning}[functional] for \"${Name}\" in ${SourceFile}\n")
      set(Warning "${Warning}!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
      message(AUTHOR_WARNING "${Warning}")
      set(Timeout ${UnitTimeout})
    endif()
    if(ExplicitTimeout GREATER 0)
      set(Timeout ${ExplicitTimeout})
    endif()

    # Disable test if "[hide]" is a tag
    list(FIND Tags "hide" HideFound)
    if(HideFound GREATER -1)
      set(HiddenCatchTests ${HiddenCatchTests} ${CTestName})
      set(HiddenCatchTests ${HiddenCatchTests} PARENT_SCOPE)
    else()
      # Add the test and set its properties
      add_test(NAME "\"${CTestName}\""
               COMMAND ${TestTarget} ${Name} --durations yes --warn NoAssertions
                       --name "${TestTarget} ($<CONFIGURATION> build)")

      ms_update_test_timeout(Timeout)
      set_tests_properties("\"${CTestName}\"" PROPERTIES
                           FAIL_REGULAR_EXPRESSION "No tests ran"
                           TIMEOUT ${Timeout}
                           LABELS "${CamelCaseProjectName};${Tags}")

      set(AllCatchTests ${AllCatchTests} "\"${CTestName}\"")
      set(AllCatchTests ${AllCatchTests} PARENT_SCOPE)
    endif()
  endforeach()
endfunction()
