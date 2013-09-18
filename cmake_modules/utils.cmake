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
#  at: http://www.maidsafe.net/licenses                                                              #
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


function(check_compiler)
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "MSVC")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "17")  # i.e for MSVC < Visual Studio 11
      message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of Visual Studio less than 11.")
    endif()
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "3.2")
      message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of Clang less than 3.2")
    endif()
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    if(CMAKE_CXX_COMPILER_VERSION VERSION_LESS "4.7")
      message(FATAL_ERROR "\nIn order to use C++11 features, this library cannot be built using a version of GCC less than 4.7")
    endif()
  endif()
endfunction()

# Creates variables as a result of globbing the given directory and corresponding "includes" dir.
# Example usage:
# > glob_dir(DataHolder ${PROJECT_SOURCE_DIR}/src/maidsafe/vault/data_holder "Data Holder\\\\")
# Available variables are ${DataHolderApi}, ${DataHolderSources}, ${DataHolderHeaders},
# ${DataHolderProtos} and ${DataHolderAllFiles}.  ${DataHolderProtos} contains the contents of
# ${DataHolderProtoSources}, ${DataHolderProtoHeaders} and all .proto files.
macro(glob_dir BaseName Dir SourceGroupName)
  string(REPLACE "${PROJECT_SOURCE_DIR}/src" "${PROJECT_SOURCE_DIR}/include" ApiDir ${Dir})
  file(GLOB ${BaseName}Api ${ApiDir}/*.h)
  set(ProtoRootDir ${PROJECT_SOURCE_DIR}/src)
  string(REPLACE "${ProtoRootDir}" "" ProtoRelativeDir "${Dir}")
  add_protoc_command(${BaseName} "${ProtoRootDir}" "${ProtoRelativeDir}")
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


# Adds a static library with CMake Target name of "maidsafe_${LIB_OUTPUT_NAME}".
function(ms_add_static_library LIB_OUTPUT_NAME)
  string(TOLOWER ${LIB_OUTPUT_NAME} LIB)
  set(ALL_LIBRARIES ${ALL_LIBRARIES} maidsafe_${LIB} PARENT_SCOPE)
  add_library(maidsafe_${LIB} STATIC ${ARGN})
  set_target_properties(maidsafe_${LIB} PROPERTIES LABELS ${CamelCaseProjectName} FOLDER "MaidSafe/Libraries")
endfunction()


# Adds an executable with CMake Target name of "${Exe}".
# "${FolderName}" defines the folder in which the executable appears if the
# chosen IDE supports folders for projects.
function(ms_add_executable Exe FolderName)
  set(AllExesForCurrentProject ${AllExesForCurrentProject} ${Exe} PARENT_SCOPE)
  add_executable(${Exe} ${ARGN})
  set_target_properties(${Exe} PROPERTIES LABELS ${CamelCaseProjectName} FOLDER "MaidSafe/Executables/${FolderName}")
  string(REPLACE "Tests/" "" TEST_FOLDER_NAME ${FolderName})
  if(NOT ${TEST_FOLDER_NAME} STREQUAL ${FolderName})
    SET(TEST_FOLDER_NAME ${TEST_FOLDER_NAME} PARENT_SCOPE)
  endif()
endfunction()


function(add_style_test)
  if(NOT MaidsafeTesting)
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
  set_property(TEST ${ThisTestName} PROPERTY LABELS ${CamelCaseProjectName} CodingStyle)
endfunction()


function(add_project_experimental)
  add_custom_target(All${CamelCaseProjectName} DEPENDS ${AllExesForCurrentProject})
  set_target_properties(All${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/All")
  if(${CamelCaseProjectName} MATCHES "Lifestuff")
    add_custom_target(Exper${CamelCaseProjectName} COMMAND python ${maidsafe_SOURCE_DIR}/tools/run_lifestuff_experimental.py ${CMAKE_CTEST_COMMAND} $<CONFIGURATION> ${CMAKE_SOURCE_DIR}/tools/
                                                   DEPENDS All${CamelCaseProjectName}
                                                   WORKING_DIRECTORY ${CMAKE_BINARY_DIR})
  else()
    add_custom_target(Exper${CamelCaseProjectName} COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -M Experimental -T Start -T Build --build-noclean -T Test -T Coverage -T Submit
                                                   DEPENDS All${CamelCaseProjectName})
  endif()
  set_target_properties(Exper${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/Executables/Tests/${TEST_FOLDER_NAME}")
endfunction()


function(test_summary_output)
  list(LENGTH ALL_GTESTS GtestCount)
  message(STATUS "${MAIDSAFE_TEST_TYPE_MESSAGE}.   ${GtestCount} Google test(s) enabled.")
endfunction()


function(add_coverage_exclude Regex)
  file(APPEND ${CMAKE_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_COVERAGE_EXCLUDE \${CTEST_CUSTOM_COVERAGE_EXCLUDE} \"${Regex}\")\n")
endfunction()


function(add_memcheck_ignore TestName)
  file(APPEND ${CMAKE_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_MEMCHECK_IGNORE \${CTEST_CUSTOM_MEMCHECK_IGNORE} \"${TestName}\")\n")
endfunction()


function(underscores_to_camel_case VarIn VarOut)
  string(REPLACE "_" ";" Pieces ${VarIn})
  foreach(Part ${Pieces})
    string(SUBSTRING ${Part} 0 1 Initial)
    string(SUBSTRING ${Part} 1 -1 Part)
    string(TOUPPER ${Initial} Initial)
    set(CamelCase ${CamelCase}${Initial}${Part})
  endforeach()
  set(${VarOut} ${CamelCase} PARENT_SCOPE)
endfunction()


# Tidy CTestCustom.cmake
function(tidy_ctest_custom)
  file(STRINGS ${CMAKE_BINARY_DIR}/CTestCustom.cmake CTestCustomContents)
  list(REMOVE_DUPLICATES CTestCustomContents)
  list(SORT CTestCustomContents)
  string(REPLACE ";" "\n" CTestCustomContents "${CTestCustomContents}")
  file(WRITE ${CMAKE_BINARY_DIR}/CTestCustom.cmake "${CTestCustomContents}\n")
endfunction()


# Moves executable files found within the build tree which don't match current target filenames to
# a directory named 'old' in the build tree root.  This avoids accidentally running outdated
# executables in the case of renaming a CMake Target.
function(rename_outdated_built_exes)
  if(NOT ${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    return()
  endif()

  if(MSVC)
#    file(GLOB_RECURSE BuiltExes RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/*.exe")
    file(GLOB_RECURSE BuiltExesDebug RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/Debug/*.exe")
    file(GLOB_RECURSE BuiltExesMinSizeRel RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/MinSizeRel/*.exe")
    file(GLOB_RECURSE BuiltExesRelease RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/Release/*.exe")
    file(GLOB_RECURSE BuiltExesRelWithDebInfo RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/RelWithDebInfo/*.exe")
    set(BuiltExes ${BuiltExesDebug} ${BuiltExesBuiltExesMinSizeRel} ${BuiltExesRelease} ${BuiltExesRelWithDebInfo})
  else()
    if(APPLE)
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
    if(NOT TARGET ${BuiltExeNameWe} AND NOT ${BuiltExeNameWe} MATCHES "CompilerIdC[X]?[X]?$")
      string(REGEX MATCH "build_qt" InQtBuildDir ${BuiltExe})
      string(REGEX MATCH "src/boost" InBoostBuildDir ${BuiltExe})
      string(REGEX MATCH "old/" AlreadyArchived ${BuiltExe})
      if(NOT InQtBuildDir AND NOT InBoostBuildDir AND NOT AlreadyArchived)
        get_filename_component(BuiltExePath ${BuiltExe} PATH)
        file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/old/${BuiltExePath})
        file(RENAME ${CMAKE_BINARY_DIR}/${BuiltExe} ${CMAKE_BINARY_DIR}/old/${BuiltExe})
        if(${BuiltExePath} STREQUAL ".")
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


# Copies executable to <current build dir>/package/bin/<config type>/ for use by the package tool
function(ms_copy_to_package_folder maidsafe_target)
  add_custom_command(TARGET ${maidsafe_target}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${maidsafe_target}>
                         ${CMAKE_BINARY_DIR}/package/bin/$<CONFIGURATION>/$<TARGET_FILE_NAME:${maidsafe_target}>)
endfunction()


# Searches for and removes old test directories that may have been left in %temp%
function(cleanup_temp_dir)
  if(CMAKE_VERSION VERSION_LESS 2.8.11)  # Need file(TIMESTAMP ...) and string(TIMESTAMP ...)
    return()
  endif()
  # Get temp dir as per http://www.boost.org/doc/libs/release/libs/filesystem/doc/reference.html#temp_directory_path
  if(DEFINED ENV{TMPDIR})
    set(TempDir "$ENV{TMPDIR}")
  elseif(DEFINED ENV{TMP})
    set(TempDir "$ENV{TMP}")
  elseif(DEFINED ENV{TEMP})
    set(TempDir "$ENV{TEMP}")
  elseif(DEFINED ENV{TEMPDIR})
    set(TempDir "$ENV{TEMPDIR}")
  elseif(NOT WIN32)
    set(TempDir "/tmp")
  else()
    return()
  endif()
  string(REPLACE "\\" "/" TempDir ${TempDir})

  # Find MaidSafe-specific directories
  file(GLOB MaidSafeTempDirs ${TempDir}/MaidSafe_Test*)
  file(GLOB SigmoidTempDirs ${TempDir}/Sigmoid_Test*)
  list(APPEND MaidSafeTempDirs ${SigmoidTempDirs})
  list(LENGTH MaidSafeTempDirs DirCount)
  if(NOT DirCount)
    return()
  endif()

  # Get directories which are > 1 day old
  foreach(MaidSafeTempDir ${MaidSafeTempDirs})
    file(TIMESTAMP ${MaidSafeTempDir} TempDirTimestamp "%j")
    string(TIMESTAMP Now "%j")
    math(EXPR AgeOfDir ${Now}-${TempDirTimestamp})
    if(AgeOfDir GREATER 1)
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


function(get_command_line_args)
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
      file(TO_CMAKE_PATH ${${CacheVar}} ${CacheVar})
      set(CMakeArgs ${CMakeArgs} "-D${CacheVar}${CacheVarType}=${${CacheVar}}" PARENT_SCOPE)
    endif()
  endforeach()
endfunction()


# Set up CI test scripts
function(setup_ci_scripts)
  if(NOT RUNNING_AS_CTEST_SCRIPT)
    get_command_line_args()
    include(${CMAKE_SOURCE_DIR}/cmake_modules/maidsafe_find_git.cmake)
    find_program(HostnameCommand NAMES hostname)
    execute_process(COMMAND ${HostnameCommand} OUTPUT_VARIABLE Hostname OUTPUT_STRIP_TRAILING_WHITESPACE)
    set(ThisSite "${Hostname}")
    if(${TargetPlatform} STREQUAL "Win8")
      set(MachineType kWindows8)
    elseif(${TargetPlatform} STREQUAL "Win7")
      set(MachineType kWindows7)
    elseif(${TargetPlatform} STREQUAL "Vista")
      set(MachineType kWindowsVista)
    elseif(${TargetPlatform} STREQUAL "Linux")
      set(MachineType kLinux)
    elseif(${TargetPlatform} STREQUAL "OSX10.8")
      set(MachineType kMac)
    else()
      set(MachineType Unsupported)
    endif()
    string(TOLOWER "${CMAKE_MAKE_PROGRAM}" MakeProgram)
    if("${MakeProgram}" MATCHES "msbuild")
      set(UsingMsBuild TRUE)
    else()
      set(UsingMsBuild FALSE)
    endif()
    foreach(TestConfType Debug Release)
      foreach(DashType Experimental Continuous Nightly Weekly)
        if(QT_ROOT_DIR)
          file(TO_CMAKE_PATH ${QT_ROOT_DIR} QT_ROOT_DIR)
        endif()
        configure_file(${CMAKE_SOURCE_DIR}/CI.cmake.in ${CMAKE_BINARY_DIR}/CI_${DashType}_${TestConfType}.cmake ESCAPE_QUOTES)
      endforeach()
    endforeach()
  endif()
endfunction()

# Gets the target platform name
function(get_target_platform)
  if(TargetPlatform)
    return()
  endif()
  if(WIN32)
    # See http://en.wikipedia.org/wiki/Comparison_of_Windows_versions
    if(CMAKE_SYSTEM_VERSION VERSION_EQUAL 6.2)
      # Windows 8
      set(Platform Win8)
    elseif(CMAKE_SYSTEM_VERSION VERSION_EQUAL 6.1)
      # Windows 7, Windows Server 2008 R2, Windows Home Server 2011
      set(Platform Win7)
    elseif(CMAKE_SYSTEM_VERSION VERSION_EQUAL 6.0)
      # Windows Server 2008
      set(Platform Vista)
    else()
      set(Platform Unsupported)
    endif()
  elseif(UNIX)
    if(APPLE)
      # See http://en.wikipedia.org/wiki/Darwin_%28operating_system%29
      if(CMAKE_SYSTEM_VERSION VERSION_EQUAL 12 OR CMAKE_SYSTEM_VERSION VERSION_GREATER 12 AND CMAKE_SYSTEM_VERSION VERSION_LESS 13)
        # OS X v10.8 "Mountain Lion"
        set(Platform OSX10.8)
      else()
        set(Platform Unsupported)
      endif()
    else()
      set(Platform Linux)
    endif()
  endif()
  set(TargetPlatform "${Platform}" PARENT_SCOPE)
endfunction()

# Gets the target architecture
# Copied from https://github.com/petroules/solar-cmake/blob/master/TargetArch.cmake
# and described at http://stackoverflow.com/a/12024211/424459

# Based on the Qt 5 processor detection code, so should be very accurate
# https://qt.gitorious.org/qt/qtbase/blobs/master/src/corelib/global/qprocessordetection.h
# Currently handles arm (v5, v6, v7), x86 (32/64), ia64, and ppc (32/64)

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

# Set ppc_support to TRUE before including this file or ppc and ppc64
# will be treated as invalid architectures since they are no longer supported by Apple

function(get_target_architecture)
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
      if("${osx_arch}" STREQUAL "ppc" AND ppc_support)
        set(osx_arch_ppc TRUE)
      elseif("${osx_arch}" STREQUAL "i386")
        set(osx_arch_i386 TRUE)
      elseif("${osx_arch}" STREQUAL "x86_64")
        set(osx_arch_x86_64 TRUE)
      elseif("${osx_arch}" STREQUAL "ppc64" AND ppc_support)
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

    file(WRITE "${CMAKE_BINARY_DIR}/arch.c" "${archdetect_c_code}")
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
            "${CMAKE_BINARY_DIR}/arch.c"
            COMPILE_OUTPUT_VARIABLE ARCH
            CMAKE_FLAGS CMAKE_OSX_ARCHITECTURES=${CMAKE_OSX_ARCHITECTURES})

    # Parse the architecture name from the compiler output
    string(REGEX MATCH "cmake_ARCH ([a-zA-Z0-9_]+)" ARCH "${ARCH}")

    # Get rid of the value marker leaving just the architecture name
    string(REPLACE "cmake_ARCH " "" ARCH "${ARCH}")

    # If we are compiling with an unknown architecture this variable should
    # already be set to "unknown" but in the case that it's empty (i.e. due
    # to a typo in the code), then set it to unknown
    if (NOT ARCH)
      set(ARCH unknown)
    endif()
  endif()

  set(TargetArchitecture "${ARCH}" PARENT_SCOPE)
endfunction()


# Gets all dependencies for the given Target
function(get_dependencies Target OptimizedDeps DebugDeps UseImported)
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
      if(NOT "${ReleaseDependency}" STREQUAL "${Dep}")
        list(APPEND AllReleases "\"${ReleaseDependency}\"")
      else()
        string(REGEX REPLACE "\\$<\\$<CONFIG:DEBUG>:([^>]+)>" "\\1" DebugDependency ${Dep})
        if("${DebugDependency}" STREQUAL "${Dep}")
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
function(set_meta_files_custom_commands OutputFile InputFile MetaFiles OutputFileSourceGroup CMakeFilesSourceGroup)
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
                     DEPENDS ${IntermediateOutputFiles}
                             ${InputFile}
                             ${ConfigureCMakeFile}
                     COMMENT "Configuring message_types.h")
  set(CMAKE_DISABLE_SOURCE_CHANGES ON)
  set_source_files_properties(${OutputFile} PROPERTIES GENERATED TRUE)
  source_group("${OutputFileSourceGroup}" FILES ${OutputFile})
  source_group("${CMakeFilesSourceGroup}" FILES ${CMAKE_CURRENT_LIST_FILE} ${InputFile} ${MetaFiles})
endfunction()
