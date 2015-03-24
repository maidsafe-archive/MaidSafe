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
#  Utility functions.                                                                              #
#                                                                                                  #
#==================================================================================================#


include(add_protoc_command)


# Oddly cmake is fairly limited in standard platform defines
function(ms_extra_platforms)
  if(UNIX AND NOT APPLE)
    message(STATUS "This system is called ${CMAKE_SYSTEM_NAME}.")
    if(CMAKE_SYSTEM_NAME MATCHES ".*Linux")
      set(LINUX TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "kFreeBSD.*")
      set(BSD TRUE PARENT_SCOPE)
      set(FREEBSD TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "kNetBSD.*|NetBSD.*")
      set(BSD TRUE PARENT_SCOPE)
      set(NETBSD TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "kOpenBSD.*|OpenBSD.*")
      set(BSD TRUE PARENT_SCOPE)
      set(OPENBSD TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*GNU.*")
      set(GNU TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES ".*BSDI.*")
      set(BSDI TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "DragonFly.*|FreeBSD")
      set(BSD TRUE PARENT_SCOPE)
      set(FREEBSD TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "SYSV5.*")
      set(SYSV5 TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Solaris.*")
      set(SOLARIS TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "HP-UX.*")
      set(HPUX TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "AIX.*")
      set(AIX TRUE PARENT_SCOPE)
    elseif(CMAKE_SYSTEM_NAME MATCHES "Minix.*")
      set(MINIX TRUE PARENT_SCOPE)
    endif()
  endif()
endfunction()


# Sets the postfixes to be used with all MaidSafe libraries
macro(ms_set_postfixes)
  set(CMAKE_DEBUG_POSTFIX -d)
  set(CMAKE_RELWITHDEBINFO_POSTFIX -rwdi)
  set(CMAKE_MINSIZEREL_POSTFIX -msr)
  set(CMAKE_RELEASENOINLINE_POSTFIX -rni)
endmacro()


function(ms_check_compiler)
  # If the path to the CMAKE_CXX_COMPILER doesn't change, CMake doesn't detect a version change
  # in the compiler.  We cache the output of running the compiler with '--version' and check
  # on each subsequent configure that the output is identical.  Note, with MSVC the command
  # fails ('--version' is an unrecognised arg), but still outputs the comiler version.
  #
  # This check (but not the minimum version check) can be skipped by setting the variable
  # IGNORE_COMPILER_VERSION_CHANGE to ON, but we strongly discourage doing this.  It is arguably
  # useful if you want to upgrade your compiler version on a very frequent basis (e.g. updating and
  # building Clang from source daily).
  execute_process(COMMAND ${CMAKE_CXX_COMPILER} --version
                  OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  string(REPLACE "\n" ";" CombinedOutput "${OutputVar}${ErrorVar}")
  if(CheckCompilerVersion AND NOT IGNORE_COMPILER_VERSION_CHANGE)
    if(NOT CheckCompilerVersion STREQUAL CombinedOutput)
      set(Msg "\n\nThe C++ compiler \"${CMAKE_CXX_COMPILER}\" has changed since the previous run of CMake.")
      set(Msg "${Msg}  This requires a clean build folder, so either delete all contents from this")
      set(Msg "${Msg}  folder, or create a new one and run CMake from there.\n\n")
      message(FATAL_ERROR "${Msg}")
    endif()
  else()
    set(CheckCompilerVersion "${CombinedOutput}" CACHE INTERNAL "")
  endif()
  if("x${CMAKE_CXX_COMPILER_ID}" STREQUAL "xMSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS 18)  # i.e for MSVC < Visual Studio 12
      message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of Visual Studio less than 12.")
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.3")
      message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of Clang less than 3.3")
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "5.1")
      message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of AppleClang less than 5.1")
    endif()
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.8")
      message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of GCC less than 4.8")
    endif()
  endif()
endfunction()


# Creates variables as a result of globbing the given directory and corresponding "includes" dir.
# Example usage:
# > ms_glob_dir(DataHolder ${PROJECT_SOURCE_DIR}/src/maidsafe/vault/data_holder "Data Holder\\\\")
# Available variables are ${DataHolderApi}, ${DataHolderSources}, ${DataHolderHeaders},
# ${DataHolderProtos} and ${DataHolderAllFiles}.  ${DataHolderProtos} contains the contents of
# ${DataHolderProtoSources}, ${DataHolderProtoHeaders} and all .proto files.
macro(ms_glob_dir BaseName Dir SourceGroupName)
  string(REPLACE "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include" ApiDir ${Dir})
  file(GLOB ${BaseName}Api ${ApiDir}/*.h)
  set(ProtoRootDir ${PROJECT_SOURCE_DIR}/src)
  string(REPLACE "${ProtoRootDir}" "" ProtoRelativeDir "${Dir}")
  ms_add_protoc_command(${BaseName} "${ProtoRootDir}" "${ProtoRelativeDir}")
  file(GLOB ${BaseName}Sources ${Dir}/*.cc)
  file(GLOB ${BaseName}Headers ${Dir}/*.h)
  set(${BaseName}AllFiles ${${BaseName}Api} ${${BaseName}Sources} ${${BaseName}Headers} ${${BaseName}Protos})
  set(${BaseName}SourceGroupName "${SourceGroupName} ")
  string(REPLACE "\\ " "\\" ${BaseName}SourceGroupName "${${BaseName}SourceGroupName}")
  source_group("${${BaseName}SourceGroupName}API Files" FILES ${${BaseName}Api})
  source_group("${${BaseName}SourceGroupName}Source Files" FILES ${${BaseName}Sources})
  source_group("${${BaseName}SourceGroupName}Header Files" FILES ${${BaseName}Headers})
  source_group("${${BaseName}SourceGroupName}Proto Files" FILES ${${BaseName}Protos})
endmacro()


# Checks that the given file includes an appropriate license block at the top.
function(ms_check_license_block File)
  set(MaidSafeCopyrightBlock
      ""
      "    This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,"
      "    version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which"
      "    licence you accepted on initial access to the Software (the \"Licences\")."
      ""
      "    By contributing code to the MaidSafe Software, or to this project generally, you agree to be"
      "    bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root"
      "    directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also"
      "    available at: http://www.maidsafe.net/licenses"
      ""
      "    Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed"
      "    under the GPL Licence is distributed on an \"AS IS\" BASIS, WITHOUT WARRANTIES OR CONDITIONS"
      "    OF ANY KIND, either express or implied."
      ""
      "    See the Licences for the specific language governing permissions and limitations relating to"
      "    use of the MaidSafe Software.                                                                 */")
  if(PROJECT_NAME STREQUAL "api" OR
     PROJECT_NAME STREQUAL "common" OR
     PROJECT_NAME STREQUAL "drive" OR
     PROJECT_NAME STREQUAL "encrypt" OR
     PROJECT_NAME STREQUAL "nfs" OR
     PROJECT_NAME STREQUAL "passport" OR
     PROJECT_NAME STREQUAL "routing" OR
     PROJECT_NAME STREQUAL "vault" OR
     PROJECT_NAME STREQUAL "vault_manager" OR
     PROJECT_NAME STREQUAL "launcher")
    if(EXISTS "${File}")
      file(STRINGS ${File} CopyrightBlock LIMIT_COUNT 17)
    endif()
    list(LENGTH CopyrightBlock CopyrightBlockLength)
    unset(Found)
    unset(ExcludeFromCheck)
    if(CopyrightBlockLength GREATER "0")
      list(GET CopyrightBlock 0 FirstLine)
      list(REMOVE_AT CopyrightBlock 0)
      if(FirstLine STREQUAL "// NoCheck")
        set(ExcludeFromCheck ON)
      else()
        string(REGEX MATCH "^/\\*  Copyright 20[01][0-9] MaidSafe.net limited$" Found "${FirstLine}")
      endif()
    endif()
    if(NOT CopyrightBlockLength EQUAL "17" OR NOT Found OR NOT CopyrightBlock STREQUAL MaidSafeCopyrightBlock)
      string(REPLACE "${PROJECT_SOURCE_DIR}/" "" RelativePath "${File}")
      string(REGEX MATCH "\\.meta$" IsMeta "${File}")
      if(NOT IsMeta AND EXISTS "${File}" AND NOT ExcludeFromCheck)
        set(Msg "\nThe copyright block at the top of \"${RelativePath}\" is missing or incorrect.\n")
        set(Msg "${Msg}If this is a MaidSafe file, please add the correct copyright block.\n")
        set(Msg "${Msg}If this isn't a MaidSafe file, please add '// NoCheck' as the first line ")
        set(Msg "${Msg}in this file to avoid it being checked.\n\n")
        message(AUTHOR_WARNING "${Msg}")
      endif()
    endif()
  endif()
endfunction()


# As suggested in pull request https://github.com/maidsafe/MaidSafe/pull/140 from
# https://github.com/smspillaz, issue a developer warning if duplicate files are added to a target.
# While this *shouldn't* cause any significant problems, it could be a symptom of a bug in the
# current CMakeLists.txt
function(ms_check_for_duplicates TargetName)
  set(Files ${ARGN})
  list(LENGTH Files FilesLength)
  string(LENGTH "${CMAKE_CURRENT_SOURCE_DIR}/" SourceDirLength)
  while(FilesLength)
    list(GET Files 0 File)
    math(EXPR ExpectedLength ${FilesLength}-1)
    list(REMOVE_ITEM Files ${File})
    list(LENGTH Files FilesLength)
    if(NOT FilesLength EQUAL ExpectedLength)
      string(SUBSTRING "${File}" ${SourceDirLength} -1 File)
      set(DuplicateFiles "${DuplicateFiles}\n\t${File}")
    endif()
  endwhile()
  if(DuplicateFiles)
    message(AUTHOR_WARNING "\n\nDuplicate source files found for target '${TargetName}':${DuplicateFiles}\n\n")
  endif()
endfunction()


# Adds a static library with CMake Target name of "${Lib}".
#
# All files to be added (sources, headers, others) should be listed after 'Lib'
function(ms_add_static_library Lib)
  ms_check_for_duplicates(${Lib} ${ARGN})
  foreach(File ${ARGN})
    ms_check_license_block(${File})
  endforeach()
  # Check for correct naming convention
  string(REGEX MATCH "^maidsafe_[a-z]" Found "${Lib}")
  string(TOLOWER ${Lib} LibLowerCase)
  if(NOT Found OR NOT Lib STREQUAL LibLowerCase)
    set(Msg "\nYou have called ms_add_static_library with lib name of \"${Lib}\".")
    set(Msg "${Msg}  MaidSafe lib names should be lowercase starting with \"maidsafe_\".\n")
    message(AUTHOR_WARNING "${Msg}")
  endif()
  set(AllStaticLibsForCurrentProject ${AllStaticLibsForCurrentProject} ${Lib} PARENT_SCOPE)
  set(AllStaticLibs ${AllStaticLibs} ${Lib} CACHE INTERNAL "")
  add_library(${Lib} STATIC ${ARGN})
  set_target_properties(${Lib} PROPERTIES LABELS ${CamelCaseProjectName} FOLDER "MaidSafe/Libraries/${CamelCaseProjectName}")
endfunction()


# Adds an executable with CMake Target name of "${Exe}".
#
# "${FolderName}" defines the folder in which the executable appears if the chosen IDE supports
# folders for projects.
#
# All files to be added (sources, headers, others) should be listed after 'FolderName'
#
# The exe will have the preprocessor definition APPLICATION_NAME=${AppName}.  AppName is the value
# of ${Exe}Name if it exists, otherwise it's the camel-case name of the exe.  (e.g. the exe
# 'test_common', will have APPLICATION_NAME=TestCommon unless 'test_commonName' is set, in which
# case it will have APPLICATION_NAME=${test_commonName})
function(ms_add_executable Exe FolderName)
  ms_check_for_duplicates(${Exe} ${ARGN})
  foreach(File ${ARGN})
    ms_check_license_block(${File})
  endforeach()
  set(AllExesForCurrentProject ${AllExesForCurrentProject} ${Exe} PARENT_SCOPE)
  add_executable(${Exe} ${ARGN})
  if(${Exe}Name)
    set(AppName ${${Exe}Name})
  else()
    ms_underscores_to_camel_case(${Exe} AppName)
  endif()
  target_compile_definitions(${Exe} PRIVATE COMPANY_NAME=MaidSafe APPLICATION_NAME=${AppName})
  set_target_properties(${Exe} PROPERTIES LABELS ${CamelCaseProjectName} FOLDER "MaidSafe/Executables/${FolderName}")
  string(REPLACE "Tests/" "" TEST_FOLDER_NAME ${FolderName})
  if(NOT TEST_FOLDER_NAME STREQUAL FolderName)
    SET(TEST_FOLDER_NAME ${TEST_FOLDER_NAME} PARENT_SCOPE)
  endif()
endfunction()


# Workaround for the Xcode's missing ability to pass -isystem to the compiler.
function(ms_target_include_system_dirs Target)
  if(XCODE OR (UNIX AND NOT CMAKE_VERSION VERSION_LESS "3.0"))
    foreach(Arg ${ARGN})
      string(REGEX MATCH "\\$<" IsGeneratorExpression "${Arg}")
      if(Arg STREQUAL "PRIVATE" OR Arg STREQUAL "PUBLIC" OR Arg STREQUAL "INTERFACE")
        set(Scope ${Arg})
      elseif(NOT IsGeneratorExpression STREQUAL "")
        message(AUTHOR_WARNING "This function doesn't handle generator expressions; skipping ${Arg}")
      else()
        target_compile_options(${Target} ${Scope} -isystem${Arg})
      endif()
    endforeach()
  else()
    target_include_directories(${Target} SYSTEM ${Scope} ${ARGN})
  endif()
endfunction()


function(ms_add_style_test)
  if(NOT INCLUDE_TESTS)
    return()
  endif()
  set(ExcludeRegexes *.pb.* *qt_push_headers.h *qt_pop_headers.h)
  file(GLOB_RECURSE AllSources *.cc)
  file(GLOB_RECURSE AllHeaders *.h)
  set(AllFiles ${AllSources} ${AllHeaders})
  foreach(ExcludeRegex ${ExcludeRegexes})
    file(GLOB_RECURSE ExcludeFiles ${ExcludeRegex})
    if(ExcludeFiles)
      list(REMOVE_ITEM AllFiles ${ExcludeFiles})
    endif()
  endforeach()
  set(ThisTestName ${CamelCaseProjectName}StyleCheck)
  add_test(${ThisTestName} python ${maidsafe_SOURCE_DIR}/tools/cpplint.py ${AllFiles})
  set_property(TEST ${ThisTestName} PROPERTY LABELS ${CamelCaseProjectName} CodingStyle ${TASK_LABEL})
endfunction()


# Adds a target which includes all public headers for project in two different translation units.
# The test is simply to build the target successfully.  To exclude platform-specific files, add
# their relative paths (as they'd appear inside a #include statement) to a variable 'Exclusions'.
function(ms_add_test_for_multiple_definition_errors)
  file(GLOB_RECURSE ApiFiles RELATIVE "${PROJECT_SOURCE_DIR}/include" "${PROJECT_SOURCE_DIR}/include/maidsafe/${PROJECT_NAME}/*.h" "${PROJECT_SOURCE_DIR}/include/maidsafe/${PROJECT_NAME}/*.hpp")
  if(Exclusions)
    list(REMOVE_ITEM ApiFiles ${Exclusions})
  endif()
  set(HeaderIncludingAllApiFiles "${CMAKE_CURRENT_BINARY_DIR}/all_apis.h")
  set(Contents "// NoCheck\n// This file is auto-generated by CMake ('ms_add_test_for_multiple_definition_errors' function in\n// utils.cmake)\n")
  foreach(ApiFile ${ApiFiles})
    set(Contents "${Contents}#include \"${ApiFile}\"\n")
  endforeach()
  file(WRITE "${HeaderIncludingAllApiFiles}.copy" "${Contents}")
  configure_file("${HeaderIncludingAllApiFiles}.copy" "${HeaderIncludingAllApiFiles}" COPYONLY)

  set(Contents "// NoCheck\n#include \"all_apis.h\"")
  configure_file("${CMAKE_SOURCE_DIR}/cmake_modules/generic_contents.in" "${CMAKE_CURRENT_BINARY_DIR}/translation_unit_one.cc")

  set(Contents "// NoCheck\n#include \"all_apis.h\"\nint main() { return 0; }")
  configure_file("${CMAKE_SOURCE_DIR}/cmake_modules/generic_contents.in" "${CMAKE_CURRENT_BINARY_DIR}/translation_unit_two.cc")

  string(REGEX MATCH "test_[a-z_]+" MainTestExe "${AllExesForCurrentProject}")
  get_target_property(Folder ${MainTestExe} FOLDER)
  string(REPLACE "MaidSafe/Executables/" "" Folder "${Folder}")

  set(ExeName test_${PROJECT_NAME}_multiple_definitions)
  ms_add_executable(${ExeName} "${Folder}"
                    "${HeaderIncludingAllApiFiles};${CMAKE_CURRENT_BINARY_DIR}/translation_unit_one.cc;${CMAKE_CURRENT_BINARY_DIR}/translation_unit_two.cc")
  target_include_directories(${ExeName} PRIVATE ${CMAKE_CURRENT_BINARY_DIR}/src)

  # Link all libs for that project not excluded from ALL_BUILD to pull in their flags and public include paths
  unset(LinkLibs)
  foreach(Lib ${AllStaticLibsForCurrentProject})
    get_target_property(IsExcludedFromAll ${Lib} EXCLUDE_FROM_ALL)
    if(NOT IsExcludedFromAll)
      list(APPEND LinkLibs ${Lib})
    endif()
  endforeach()
  target_link_libraries(${ExeName} ${LinkLibs})
  set(AllExesForCurrentProject ${AllExesForCurrentProject} PARENT_SCOPE)

  set(ThisTestName ${CamelCaseProjectName}MultipleDefinitionsCheck)
  add_test(NAME ${ThisTestName} COMMAND ${ExeName})
  set_property(TEST ${ThisTestName} PROPERTY LABELS ${CamelCaseProjectName} MultipleDefinitions ${TASK_LABEL})
endfunction()


# Adds two targets to the current project; AllXXX and ExperXXX where XXX is the project name.
function(ms_add_project_experimental)
  add_custom_target(All${CamelCaseProjectName} DEPENDS ${AllExesForCurrentProject})
  set_target_properties(All${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/All")
  foreach(CTEST_CONFIGURATION_TYPE ${CMAKE_CONFIGURATION_TYPES} ${CMAKE_BUILD_TYPE})
    configure_file(${CMAKE_SOURCE_DIR}/cmake_modules/run_experimental.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/run_experimental_${CTEST_CONFIGURATION_TYPE}.cmake
                   @ONLY)
    configure_file(${CMAKE_SOURCE_DIR}/cmake_modules/run_check.cmake.in
                   ${CMAKE_CURRENT_BINARY_DIR}/run_check_${CTEST_CONFIGURATION_TYPE}.cmake
                   @ONLY)
  endforeach()
  add_custom_target(Exper${CamelCaseProjectName} COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -S ${CMAKE_CURRENT_BINARY_DIR}/run_experimental_$<CONFIGURATION>.cmake -V)
  set_target_properties(Exper${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/Experimentals")
  add_custom_target(Check${CamelCaseProjectName} COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -S ${CMAKE_CURRENT_BINARY_DIR}/run_check_$<CONFIGURATION>.cmake -V)
  set_target_properties(Check${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/Checks")
endfunction()


# Adds the style test, multiple definitions test, and Exper target
function(ms_add_default_tests)
  ms_add_style_test()
  ms_add_test_for_multiple_definition_errors()
  ms_add_project_experimental()
endfunction()


function(ms_test_summary_output)
  list(LENGTH AllGtests GtestCount)
  message(STATUS "${MAIDSAFE_TEST_TYPE_MESSAGE}${GtestCount} Google test(s) enabled.")
endfunction()


function(ms_add_coverage_exclude Regex)
  file(APPEND ${CMAKE_BINARY_DIR}/CTestCustom.cmake "set(CTEST_CUSTOM_COVERAGE_EXCLUDE \${CTEST_CUSTOM_COVERAGE_EXCLUDE} \"${Regex}\")\n")
endfunction()


function(ms_add_memcheck_ignore TestName)
  file(APPEND ${CMAKE_BINARY_DIR}/CTestCustom.cmake "set(CTEST_CUSTOM_MEMCHECK_IGNORE \${CTEST_CUSTOM_MEMCHECK_IGNORE} \"${TestName}\")\n")
endfunction()


function(ms_underscores_to_camel_case VarIn VarOut)
  string(REPLACE "_" ";" Pieces ${VarIn})
  foreach(Part ${Pieces})
    string(SUBSTRING ${Part} 0 1 Initial)
    string(SUBSTRING ${Part} 1 -1 Part)
    string(TOUPPER ${Initial} Initial)
    set(CamelCase ${CamelCase}${Initial}${Part})
  endforeach()
  set(${VarOut} ${CamelCase} PARENT_SCOPE)
endfunction()


# Moves executable files found within the build tree which don't match current target filenames to
# a directory named 'old' in the build tree root.  This avoids accidentally running outdated
# executables in the case of renaming a CMake Target.
function(ms_rename_outdated_built_exes)
  if(NOT CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    return()
  endif()

  if(MSVC)
#    file(GLOB_RECURSE BuiltExes RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/*.exe")
    file(GLOB_RECURSE BuiltExesDebug RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/Debug/*.exe")
    file(GLOB_RECURSE BuiltExesMinSizeRel RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/MinSizeRel/*.exe")
    file(GLOB_RECURSE BuiltExesRelease RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/Release/*.exe")
    file(GLOB_RECURSE BuiltExesRelWithDebInfo RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/RelWithDebInfo/*.exe")
    file(GLOB_RECURSE BuiltExesReleaseNoInline RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/ReleaseNoInline/*.exe")
    set(BuiltExes ${BuiltExesDebug} ${BuiltExesBuiltExesMinSizeRel} ${BuiltExesRelease} ${BuiltExesRelWithDebInfo} ${BuiltExesReleaseNoInline})
  else()
    if(APPLE OR BSD)
      execute_process(COMMAND find . -maxdepth 1 -perm +111 -type f
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                      OUTPUT_VARIABLE FindResult)
    else()
      execute_process(COMMAND find . -maxdepth 1 -executable -type f
                      WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                      OUTPUT_VARIABLE FindResult)
    endif()
    string(REPLACE "\n" ";" BuiltExes "${FindResult}")
  endif()

  foreach(BuiltExe ${BuiltExes})
    get_filename_component(BuiltExeNameWe ${BuiltExe} NAME_WE)
    # Accommodate debug postfix in SFML examples
    string(REGEX REPLACE "-d$" "" BuiltExeNameWithoutDebugPostfix "${BuiltExeNameWe}")
    if(NOT TARGET ${BuiltExeNameWe} AND NOT TARGET ${BuiltExeNameWithoutDebugPostfix} AND NOT BuiltExeNameWe MATCHES "CompilerIdC[X]?[X]?$")
      string(REGEX MATCH "build_qt" InQtBuildDir ${BuiltExe})
      string(REGEX MATCH "src/boost" InBoostBuildDir ${BuiltExe})
      string(REGEX MATCH "old/" AlreadyArchived ${BuiltExe})
      if(NOT InQtBuildDir AND NOT InBoostBuildDir AND NOT AlreadyArchived)
        get_filename_component(BuiltExePath ${BuiltExe} PATH)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/old/${BuiltExePath})
        file(RENAME ${CMAKE_BINARY_DIR}/${BuiltExe} ${CMAKE_BINARY_DIR}/old/${BuiltExe})
        if(BuiltExePath STREQUAL ".")
          set(OldName ${BuiltExe})
          get_filename_component(BuiltExeName ${BuiltExe} NAME)
          set(NewName "./old/${BuiltExeName}")
        else()
          set(OldName "./${BuiltExe}")
          set(NewName "./old/${BuiltExe}")
        endif()
        message(STATUS "Moved outdated executable \"${OldName}\" to \"${NewName}\"")
      endif()
    endif()
  endforeach()
endfunction()


# Gets the path to the temp directory using the same method as Boost.Filesystem:
# http://www.boost.org/doc/libs/release/libs/filesystem/doc/reference.html#temp_directory_path
function(ms_get_temp_dir)
  if(TempDir)
    return()
  elseif(WIN32)
    file(TO_CMAKE_PATH "$ENV{TEMP}" WindowsTempDir)
    set(Temp "${WindowsTempDir}")
  else()
    foreach(Var TMPDIR TMP TEMP TEMPDIR)
      if(IS_DIRECTORY "$ENV{${Var}}")
        set(Temp $ENV{${Var}})
        break()
      endif()
    endforeach()
    if(NOT TempDir AND IS_DIRECTORY "/tmp")
      set(Temp /tmp)
    endif()
  endif()
  set(TempDir "${Temp}" CACHE INTERNAL "Path to temp directory")
endfunction()


# Searches for and removes old test directories that may have been left in %temp%
function(ms_cleanup_temp_dir)
  # Find MaidSafe-specific directories
  ms_get_temp_dir()
  file(GLOB MaidSafeTempDirs ${TempDir}/MaidSafe_Test*)
  list(LENGTH MaidSafeTempDirs DirCount)
  if(NOT DirCount)
    return()
  endif()

  # Get directories which are > 1 day old
  foreach(MaidSafeTempDir ${MaidSafeTempDirs})
    file(TIMESTAMP ${MaidSafeTempDir} TempDirTimestamp "%j")
    string(TIMESTAMP Now "%j")
    math(EXPR AgeOfDir ${Now}-${TempDirTimestamp})
    if(AgeOfDir GREATER "1")
      list(APPEND DirsForRemoval ${MaidSafeTempDir})
    endif()
  endforeach()
  list(LENGTH DirsForRemoval DirCount)
  if(NOT DirCount)
    return()
  endif()

  if(DEFINED CLEAN_TEMP AND NOT CLEAN_TEMP)
    message(STATUS "Not cleaning up old temporary test folders.  To enable in future, run cmake . -UCLEAN_TEMP")
    return()
  endif()
  message(STATUS "Cleaning up old temporary test folders.  To disable in future, run cmake . -DCLEAN_TEMP=OFF")

  # Remove out-of-date directories
  foreach(DirForRemoval ${DirsForRemoval})
    file(REMOVE_RECURSE ${DirForRemoval})
    message(STATUS "Removed ${DirForRemoval}")
  endforeach()
endfunction()


function(ms_get_command_line_args)
  get_cmake_property(CacheVars CACHE_VARIABLES)
  foreach(CacheVar ${CacheVars})
    get_property(CacheVarHelpString CACHE ${CacheVar} PROPERTY HELPSTRING)
    if(CacheVarHelpString STREQUAL "No help, variable specified on the command line.")
      get_property(CacheVarType CACHE ${CacheVar} PROPERTY TYPE)
      if(CacheVarType STREQUAL "UNINITIALIZED")
        set(CacheVarType)
      else()
        set(CacheVarType :${CacheVarType})
      endif()
      if(${CacheVar})
        file(TO_CMAKE_PATH ${${CacheVar}} ${CacheVar})
      endif()
      set(CMakeArgs ${CMakeArgs} "-D${CacheVar}${CacheVarType}=${${CacheVar}}")
    endif()
  endforeach()
  if(USE_BOOST_CACHE)
    set(CMakeArgs ${CMakeArgs} "-DUSE_BOOST_CACHE=ON")
  endif()
  if(USE_JUST_THREADS)
    set(CMakeArgs ${CMakeArgs} "-DUSE_JUST_THREADS=ON")
  endif()
  set(CMakeArgs ${CMakeArgs} PARENT_SCOPE)
endfunction()


# Set up CI test scripts
function(ms_setup_ci_scripts)
  if(RUNNING_AS_CTEST_SCRIPT)
    return()
  endif()
  if(NOT EXISTS "${CMAKE_BINARY_DIR}/ContinuousIntegration")
    message(STATUS "Cloning git@github.com:maidsafe/ContinuousIntegration")
    execute_process(COMMAND ${Git_EXECUTABLE} clone git@github.com:maidsafe/ContinuousIntegration
                    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
                    RESULT_VARIABLE ResultVar
                    ERROR_FILE ${CMAKE_BINARY_DIR}/clone_ci_error.txt)
    if(ResultVar EQUAL "0")
      file(REMOVE ${CMAKE_BINARY_DIR}/clone_ci_error.txt)
    else()
      set(Msg "Failed to clone ContinuousIntegration.  CI test scripts will be unavailable.")
      set(Msg "${Msg}  See clone_ci_error.txt in the current directory for details of the attempt.")
      message(STATUS "${Msg}")
    endif()
  endif()
  if(EXISTS "${CMAKE_BINARY_DIR}/ContinuousIntegration")
    ms_get_command_line_args()
    include(${CMAKE_SOURCE_DIR}/cmake_modules/maidsafe_find_git.cmake)
    find_program(HostnameCommand NAMES hostname)
    execute_process(COMMAND ${HostnameCommand} OUTPUT_VARIABLE Hostname OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(ThisSite "${Hostname}")
    if(TargetPlatform STREQUAL "Win8")
      set(MachineType kWindows8)
    elseif(TargetPlatform STREQUAL "Win7")
      set(MachineType kWindows7)
    elseif(TargetPlatform STREQUAL "Vista")
      set(MachineType kWindowsVista)
    elseif(TargetPlatform STREQUAL "Linux")
      set(MachineType kLinux)
    elseif(TargetPlatform MATCHES "^OSX[0-9]+[.][0-9]+")
      set(MachineType kMac)
    else()
      set(MachineType Unsupported)
    endif()
    string(TOLOWER "${CMAKE_MAKE_PROGRAM}" MakeProgram)
    if(MakeProgram MATCHES "msbuild")
      set(UsingMsBuild TRUE)
    else()
      set(UsingMsBuild FALSE)
    endif()
    file(GLOB CiFiles "${CMAKE_BINARY_DIR}/CI_*.cmake")
    if(CiFiles)
      file(REMOVE ${CiFiles})
    endif()
    set(TestConfTypes Debug Release)
    if(NOT NO_UBSAN AND HAVE_UNDEFINED_BEHAVIOR_SANITIZER AND HAVE_FLAG_SANITIZE_BLACKLIST)
      list(APPEND TestConfTypes Asan Msan Tsan Ubsan)
    endif()
    foreach(TestConfType ${TestConfTypes})
      foreach(DashType Experimental Continuous Nightly)
        string(SUBSTRING "#  This script runs the ${DashType} tests on all submodules of MaidSafe in ${TestConfType} mode.                                                                       "
               0 99 Documentation)
        configure_file(${CMAKE_SOURCE_DIR}/CI.cmake.in ${CMAKE_BINARY_DIR}/CI_${DashType}_${TestConfType}.cmake ESCAPE_QUOTES)
      endforeach()
    endforeach()
  endif()
endfunction()


# Gets and caches the target platform name
function(ms_get_target_platform)
  if(WIN32)
    # See http://en.wikipedia.org/wiki/Comparison_of_Windows_versions
    if(CMAKE_SYSTEM_VERSION VERSION_EQUAL "6.2")
      # Windows 8
      set(Platform Win8)
    elseif(CMAKE_SYSTEM_VERSION VERSION_EQUAL "6.1")
      # Windows 7, Windows Server 2008 R2, Windows Home Server 2011
      set(Platform Win7)
    elseif(CMAKE_SYSTEM_VERSION VERSION_EQUAL "6.0")
      # Windows Server 2008
      set(Platform Vista)
    else()
      set(Platform Unsupported)
    endif()
  elseif(UNIX)
    if(APPLE)
      # See http://en.wikipedia.org/wiki/Darwin_%28operating_system%29
      if(CMAKE_SYSTEM_VERSION VERSION_LESS "12")
        set(Platform Unsupported)
      elseif(CMAKE_SYSTEM_VERSION VERSION_LESS "13")
        # OS X v10.8 "Mountain Lion"
        set(Platform OSX10.8)
      elseif(CMAKE_SYSTEM_VERSION VERSION_LESS "14")
        # OS X v10.9 "Mavericks"
        set(Platform OSX10.9)
      elseif(CMAKE_SYSTEM_VERSION VERSION_LESS "15")
        # OS X v10.10 "Yosemite"
        set(Platform OSX10.10)
      else()
        set(Platform Unsupported)
      endif()
    else()
      set(Platform Linux)
    endif()
  endif()
  set(TargetPlatform "${Platform}" CACHE INTERNAL "")
endfunction()


# Gets and caches the target architecture
# Copied from https://github.com/petroules/solar-cmake/blob/master/TargetArch.cmake
# and described at http://stackoverflow.com/a/12024211/424459
#
# Set ppc_support to TRUE before including this file or ppc and ppc64
# will be treated as invalid architectures since they are no longer supported by Apple
function(ms_get_target_architecture)
  if(TargetArchitecture)
    return()
  endif()
  if(APPLE AND CMAKE_OSX_ARCHITECTURES)
    # On OS X we use CMAKE_OSX_ARCHITECTURES *if* it was set
    # First let's normalize the order of the values

    # Note that it's not possible to compile PowerPC applications if you are using
    # the OS X SDK version 10.6 or later - you'll need 10.4/10.5 for that, so we
    # disable it by default
    # See this page for more information:
    # http://stackoverflow.com/questions/5333490/how-can-we-restore-ppc-ppc64-as-well-as-full-10-4-10-5-sdk-support-to-xcode-4

    # Architecture defaults to i386 or ppc on OS X 10.5 and earlier, depending on the CPU type detected at runtime.
    # On OS X 10.6+ the default is x86_64 if the CPU supports it, i386 otherwise.
    foreach(osx_arch ${CMAKE_OSX_ARCHITECTURES})
      if(osx_arch STREQUAL "ppc" AND ppc_support)
        set(osx_arch_ppc TRUE)
      elseif(osx_arch STREQUAL "i386")
        set(osx_arch_i386 TRUE)
      elseif(osx_arch STREQUAL "x86_64")
        set(osx_arch_x86_64 TRUE)
      elseif(osx_arch STREQUAL "ppc64" AND ppc_support)
        set(osx_arch_ppc64 TRUE)
      else()
        message(FATAL_ERROR "Invalid OS X arch name: ${osx_arch}")
      endif()
    endforeach()

    # Now add all the architectures in our normalized order
    if(osx_arch_ppc)
      list(APPEND ARCH ppc)
    endif()

    if(osx_arch_i386)
      list(APPEND ARCH i386)
    endif()

    if(osx_arch_x86_64)
      list(APPEND ARCH x86_64)
    endif()

    if(osx_arch_ppc64)
      list(APPEND ARCH ppc64)
    endif()
  else()

    # Based on the Qt 5 processor detection code, so should be very accurate
    # https://qt.gitorious.org/qt/qtbase/blobs/master/src/corelib/global/qprocessordetection.h
    # Currently handles arm (v5, v6, v7), x86 (32/64), ia64, and ppc (32/64)
    #
    # Regarding POWER/PowerPC, just as is noted in the Qt source,
    # "There are many more known variants/revisions that we do not handle/detect."
    set(archdetect_c_code "
    #if defined(__arm__) || defined(__TARGET_ARCH_ARM)
    #if defined(__ARM_ARCH_7__) \\
    || defined(__ARM_ARCH_7A__) \\
    || defined(__ARM_ARCH_7R__) \\
    || defined(__ARM_ARCH_7M__) \\
    || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 7)
    #error cmake_ARCH armv7
    #elif defined(__ARM_ARCH_6__) \\
    || defined(__ARM_ARCH_6J__) \\
    || defined(__ARM_ARCH_6T2__) \\
    || defined(__ARM_ARCH_6Z__) \\
    || defined(__ARM_ARCH_6K__) \\
    || defined(__ARM_ARCH_6ZK__) \\
    || defined(__ARM_ARCH_6M__) \\
    || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 6)
    #error cmake_ARCH armv6
    #elif defined(__ARM_ARCH_5TEJ__) \\
    || (defined(__TARGET_ARCH_ARM) && __TARGET_ARCH_ARM-0 >= 5)
    #error cmake_ARCH armv5
    #else
    #error cmake_ARCH arm
    #endif
    #elif defined(__i386) || defined(__i386__) || defined(_M_IX86)
    #error cmake_ARCH i386
    #elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
    #error cmake_ARCH x86_64
    #elif defined(__ia64) || defined(__ia64__) || defined(_M_IA64)
    #error cmake_ARCH ia64
    #elif defined(__ppc__) || defined(__ppc) || defined(__powerpc__) \\
    || defined(_ARCH_COM) || defined(_ARCH_PWR) || defined(_ARCH_PPC) \\
    || defined(_M_MPPC) || defined(_M_PPC)
    #if defined(__ppc64__) || defined(__powerpc64__) || defined(__64BIT__)
    #error cmake_ARCH ppc64
    #else
    #error cmake_ARCH ppc
    #endif
    #endif

    #error cmake_ARCH unknown
    ")
    set(TempTestFile "${CMAKE_BINARY_DIR}/arch.c")
    file(WRITE "${TempTestFile}" "${archdetect_c_code}")
    enable_language(C)

    # Detect the architecture in a rather creative way...
    # This compiles a small C program which is a series of ifdefs that selects a
    # particular #error preprocessor directive whose message string contains the
    # target architecture. The program will always fail to compile (both because
    # file is not a valid C program, and obviously because of the presence of the
    # #error preprocessor directives... but by exploiting the preprocessor in this
    # way, we can detect the correct target architecture even when cross-compiling,
    # since the program itself never needs to be run (only the compiler/preprocessor)
    try_run(run_result_unused
            compile_result_unused
            "${CMAKE_BINARY_DIR}"
            "${TempTestFile}"
            COMPILE_OUTPUT_VARIABLE ARCH
            CMAKE_FLAGS CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES})
    file(REMOVE "${TempTestFile}")

    # Parse the architecture name from the compiler output
    string(REGEX MATCH "cmake_ARCH ([a-zA-Z0-9_]+)" ARCH "${ARCH}")

    # Get rid of the value marker leaving just the architecture name
    string(REPLACE "cmake_ARCH " "" ARCH "${ARCH}")

    # If we are compiling with an unknown architecture this variable should
    # already be set to "unknown" but in the case that it's empty (i.e. due
    # to a typo in the code), then set it to unknown
    if(NOT ARCH)
      set(ARCH unknown)
    endif()
  endif()

  set(TargetArchitecture "${ARCH}" CACHE INTERNAL "")
endfunction()


# Gets all dependencies for the given Target
function(ms_get_dependencies Target OptimizedDeps DebugDeps UseImported)
  # Recursively get all dependencies
  macro(detail_recursive_get_dependencies Target Var)
    if(TARGET ${Target} AND NOT GotDependsFor${Target})
      set(GotDependsFor${Target} TRUE)
      if(${UseImported})
        get_target_property(ImportedReleaseDeps ${Target} IMPORTED_LINK_INTERFACE_LIBRARIES_RELEASE)
        foreach(ImportedReleaseDep ${ImportedReleaseDeps})
          if(TARGET ${ImportedReleaseDep})
            list(APPEND Depends ${ImportedReleaseDep})
            list(APPEND ${Var} ${ImportedReleaseDep})
          elseif(ImportedReleaseDep)
            list(APPEND AllImportedReleases "\"${ImportedReleaseDep}\"")
          endif()
        endforeach()
        get_target_property(ImportedDebugDeps ${Target} IMPORTED_LINK_INTERFACE_LIBRARIES_DEBUG)
        foreach(ImportedDebugDep ${ImportedDebugDeps})
          if(TARGET ${ImportedDebugDep})
            list(APPEND Depends ${ImportedDebugDep})
            list(APPEND ${Var} ${ImportedDebugDep})
          elseif(ImportedDebugDep)
            list(APPEND AllImportedDebugs "\"${ImportedDebugDep}\"")
          endif()
        endforeach()
      else()
        get_target_property(Depends ${Target} LINK_LIBRARIES)
        list(APPEND ${Var} ${Depends})
      endif()
      foreach(Depend ${Depends})
        detail_recursive_get_dependencies(${Depend} ${Var})
      endforeach()
    endif()
  endmacro()

  detail_recursive_get_dependencies(${Target} Deps)
  # Remove duplicates
  list(LENGTH Deps DepsLength)
  if(${DepsLength})
    list(REMOVE_DUPLICATES Deps)
  endif()
  list(LENGTH AllImportedReleases AllImportedReleasesLength)
  if(${AllImportedReleasesLength})
    list(REMOVE_DUPLICATES AllImportedReleases)
  endif()
  list(LENGTH AllImportedDebugs AllImportedDebugsLength)
  if(${AllImportedDebugsLength})
    list(REMOVE_DUPLICATES AllImportedDebugs)
  endif()

  # Sort into Release and Debug
  foreach(Dep ${Deps})
    if(TARGET ${Dep})
      get_target_property(ReleaseLocation ${Dep} LOCATION_RELEASE)
      get_target_property(DebugLocation ${Dep} LOCATION_DEBUG)
      list(APPEND AllReleases "\"${ReleaseLocation}\"")
      list(APPEND AllDebugs "\"${DebugLocation}\"")
    elseif(Dep AND NOT ${UseImported})
      string(REGEX REPLACE "\\$<\\$<NOT:\\$<CONFIG:DEBUG>>:([^>]+)>" "\\1" ReleaseDependency ${Dep})
      if(NOT ReleaseDependency STREQUAL Dep)
        list(APPEND AllReleases "\"${ReleaseDependency}\"")
      else()
        string(REGEX REPLACE "\\$<\\$<CONFIG:DEBUG>:([^>]+)>" "\\1" DebugDependency ${Dep})
        if(DebugDependency STREQUAL Dep)
          set(ErrorMessage "\n\nExpected to find Release or Debug configuration:\nDep -        ${Dep}")
          set(ErrorMessage "${ErrorMessage}\nReleaseDependency - ${ReleaseDependency}\nDebugDependency -   ${DebugDependency}\n")
          message(FATAL_ERROR "${ErrorMessage}")
        endif()
        list(APPEND AllDebugs "\"${DebugDependency}\"")
      endif()
    endif()
  endforeach()

  # Copy into parent scope
  set(${OptimizedDeps} ${AllReleases} ${AllImportedReleases} PARENT_SCOPE)
  set(${DebugDeps} ${AllDebugs} ${AllImportedDebugs} PARENT_SCOPE)
endfunction()


# Sets up the custom commands needed by the 'configure_meta_files.cmake' for
# auto-generating Message typedefs and boost::variants of these.
function(ms_set_meta_files_custom_commands OutputFile InputFile MetaFiles OutputFileSourceGroup CMakeFilesSourceGroup)
  set(IntermediateDir "${CMAKE_CURRENT_BINARY_DIR}/copied_message_types")
  # An apparent bug in CMake means that file(READ...) can only be done from within
  # CMAKE_CURRENT_BINARY_DIR.  Hence message_types.meta files are copied here to allow
  # configure_meta_files.cmake to work.
  foreach(MetaFile ${MetaFiles})
    string(REPLACE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/" "" RelativeMetaFile "${MetaFile}")
    set(IntermediateOutputFile "${IntermediateDir}/${RelativeMetaFile}")
    get_filename_component(FilePath "${IntermediateOutputFile}" PATH)
    list(APPEND IntermediateOutputFiles "${IntermediateOutputFile}")
    execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory "${FilePath}")
    add_custom_command(OUTPUT "${IntermediateOutputFile}"
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different "${MetaFile}" "${IntermediateOutputFile}"
                       DEPENDS "${MetaFile}"
                       COMMENT "Copying ${RelativeMetaFile} to ${IntermediateDir}/")
  endforeach()

  # Grab the list of files currently in the binary dir and remove any which are no longer required
  file(GLOB_RECURSE ExtraMetaFiles "${IntermediateDir}/*.message_types.meta")
  list(REMOVE_ITEM ExtraMetaFiles ${IntermediateOutputFiles})
  foreach(ExtraMetaFile ${ExtraMetaFiles})
    execute_process(COMMAND ${CMAKE_COMMAND} -E remove "${ExtraMetaFile}")
    message(STATUS "Removed \"${ExtraMetaFile}\"")
  endforeach()

  set(ConfigureCMakeFile "${CMAKE_SOURCE_DIR}/cmake_modules/configure_meta_files.cmake")
  set(CMAKE_DISABLE_SOURCE_CHANGES OFF)
  add_custom_command(OUTPUT ${OutputFile}
                     COMMAND ${CMAKE_COMMAND} -DOutputFile="${OutputFile}"
                                              -DInputFile="${InputFile}"
                                              -P ${ConfigureCMakeFile}
                     DEPENDS ${MetaFile}
                             ${IntermediateOutputFiles}
                             ${InputFile}
                             ${ConfigureCMakeFile}
                     COMMENT "Configuring message_types.h")
  set(CMAKE_DISABLE_SOURCE_CHANGES ON)
  set_source_files_properties(${OutputFile} PROPERTIES GENERATED TRUE)
  source_group("${OutputFileSourceGroup}" FILES ${OutputFile})
  source_group("${CMakeFilesSourceGroup}" FILES ${CMAKE_CURRENT_LIST_FILE} ${InputFile} ${MetaFiles})
endfunction()


macro(ms_get_branch_and_commit BranchName CommitName)
  execute_process(COMMAND "${Git_EXECUTABLE}" rev-parse --sq --abbrev-ref HEAD
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  RESULT_VARIABLE Result
                  OUTPUT_VARIABLE ${BranchName}
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(NOT Result EQUAL "0")
    set(${BranchName} "unknown")
  endif()

  execute_process(COMMAND "${Git_EXECUTABLE}" rev-parse --sq --short HEAD
                  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                  RESULT_VARIABLE Result
                  OUTPUT_VARIABLE ${CommitName}
                  OUTPUT_STRIP_TRAILING_WHITESPACE)
  if(Result EQUAL "0")
    execute_process(COMMAND "${Git_EXECUTABLE}" diff-index --ignore-submodules --quiet HEAD
                    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
                    RESULT_VARIABLE Result)
    if(NOT Result EQUAL "0")
      set(${CommitName} "${${CommitName}} (dirty)")
    endif()
  else()
    set(${CommitName} "unknown")
  endif()
endmacro()


# This removes the contents between all C++ block comments (i.e. /* ... */) for a C++ file whose
# contents have been read into '${CppCode}'.
function(ms_remove_block_comments CppCode)
  string(ASCII 2 CMakeBeginBlockComment)
  string(ASCII 3 CMakeEndBlockComment)
  string(REGEX REPLACE "/\\*" "${CMakeBeginBlockComment}" ${CppCode} "${${CppCode}}")
  string(REGEX REPLACE "\\*/" "${CMakeEndBlockComment}" ${CppCode} "${${CppCode}}")
  string(REGEX REPLACE "${CMakeBeginBlockComment}[^${CMakeEndBlockComment}]*${CMakeEndBlockComment}" "" ${CppCode} "${${CppCode}}")
  set(${CppCode} "${${CppCode}}" PARENT_SCOPE)
endfunction()


# This creates a folder in the build root named 'temp/<today's date>' and removes any old versions
# of such folders.  The name of today's folder is set in 'TodaysTempFolder'.  Since the folder is
# regularly deleted, it should *not* be used for files which are needed at build- or run-time.
function(ms_get_todays_temp_folder)
  string(TIMESTAMP TempFolderName "${CMAKE_BINARY_DIR}/Temp/%d%m%y")
  set(TodaysTempFolder "${TempFolderName}" PARENT_SCOPE)
  if(NOT EXISTS "${TempFolderName}")
    file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/Temp")
    file(MAKE_DIRECTORY "${TempFolderName}")
    set(Msg "This folder (\"${CMAKE_BINARY_DIR}/Temp\") should only be used\n")
    set(Msg "${Msg}to store files which are consumed by CMake.\n\n")
    set(Msg "${Msg}It is cleared out every day when CMake is run, so it is unsuitable for storing\n")
    set(Msg "${Msg}files which are used at build- or run-time, since reconfiguring (running CMake)\n")
    set(Msg "${Msg}might delete them.\n")
    file(WRITE "${CMAKE_BINARY_DIR}/Temp/README.txt" "${Msg}")
  endif()
endfunction()


# Sets a factor by which all test timeouts are multiplied.  'TimeoutFactor' must be > 1 and need not be an integer.
function(ms_set_global_test_timeout_factor TimeoutFactor)
  if(NOT "1" LESS TimeoutFactor)
    message(FATAL_ERROR "'TimeoutFactor' (${TimeoutFactor}) is not > 1")
  endif()
  if(GlobalTestTimeoutFactor AND NOT TimeoutFactor GREATER GlobalTestTimeoutFactor)
    # We won't decrease the existing factor.
    return()
  endif()
  set(GlobalTestTimeoutFactor ${TimeoutFactor} CACHE INTERNAL "A factor by which all test timeouts are multiplied")
endfunction()


# Applies the global timeout factor if it exists.
function(ms_update_test_timeout TimeoutVarToBeUpdated)
  if(GlobalTestTimeoutFactor)
    math(EXPR NewTimeout ${${TimeoutVarToBeUpdated}}*${GlobalTestTimeoutFactor})
    set(${TimeoutVarToBeUpdated} ${NewTimeout} PARENT_SCOPE)
  endif()
endfunction()
