#==============================================================================#
#                                                                              #
#  Copyright (c) 2012 MaidSafe.net limited                                     #
#                                                                              #
#  The following source code is property of MaidSafe.net limited and is not    #
#  meant for external use.  The use of this code is governed by the license    #
#  file licence.txt found in the root directory of this project and also on    #
#  www.maidsafe.net.                                                           #
#                                                                              #
#  You are not free to copy, amend or otherwise use this source code without   #
#  the explicit written permission of the board of directors of MaidSafe.net.  #
#                                                                              #
#==============================================================================#
#                                                                              #
#  Utility functions.                                                          #
#                                                                              #
#==============================================================================#


# Adds a static library with CMake Target name of "maidsafe_${LIB_OUTPUT_NAME}".
function(ms_add_static_library LIB_OUTPUT_NAME)
  set(FILES ${ARGV})
  list(REMOVE_AT FILES 0)
  string(TOLOWER ${LIB_OUTPUT_NAME} LIB)
  set(ALL_LIBRARIES ${ALL_LIBRARIES} maidsafe_${LIB} PARENT_SCOPE)
  add_library(maidsafe_${LIB} STATIC ${FILES})
  if(${CamelCaseProjectName} STREQUAL "Pd")
    set(LabelName Vault)
  else()
    set(LabelName ${CamelCaseProjectName})
  endif()
  set_target_properties(maidsafe_${LIB} PROPERTIES LABELS ${LabelName} FOLDER "MaidSafe/Libraries")
endfunction()


# Adds an executable with CMake Target name of "${EXE}".
# "${FOLDER_NAME}" defines the folder in which the executable appears if the
# chosen IDE supports folders for projects.
function(ms_add_executable EXE FOLDER_NAME)
  set(FILES ${ARGV})
  list(REMOVE_AT FILES 0 1)
  set(AllExesForCurrentProject ${AllExesForCurrentProject} ${EXE} PARENT_SCOPE)
  add_executable(${EXE} ${FILES})
  if(${CamelCaseProjectName} STREQUAL "Pd")
    set(LabelName Vault)
  else()
    set(LabelName ${CamelCaseProjectName})
  endif()
  set_target_properties(${EXE} PROPERTIES LABELS ${LabelName} FOLDER "MaidSafe/Executables/${FOLDER_NAME}")
  string(REPLACE "Tests/" "" TEST_FOLDER_NAME ${FOLDER_NAME})
  if(NOT ${TEST_FOLDER_NAME} STREQUAL ${FOLDER_NAME})
    SET(TEST_FOLDER_NAME ${TEST_FOLDER_NAME} PARENT_SCOPE)
  endif()
endfunction()


function(ms_disable_warnings FILE_LIST)
  if(MSVC)
    set_source_files_properties(${ARGV} PROPERTIES COMPILE_FLAGS "/W0")
  elseif(UNIX)
    set_source_files_properties(${ARGV} PROPERTIES COMPILE_FLAGS "-w")
  endif()
endfunction()


function(add_style_test)
  set(ThisTestName ${CamelCaseProjectName}StyleCheck)
  if(PYTHONINTERP_FOUND)
    set(FILES ${ARGV})
    list(REMOVE_AT FILES 0)
    if(UNIX)
      add_test(${ThisTestName} python ${maidsafe_SOURCE_DIR}/tools/cpplint.py ${FILES})
    else()
      string(REPLACE "/" "\\\\" STYLE_CHECK_SOURCE ${PROJECT_SOURCE_DIR})
      add_test(${ThisTestName} ${maidsafe_SOURCE_DIR}/tools/Windows/run_cpplint.bat ${STYLE_CHECK_SOURCE} ${maidsafe_SOURCE_DIR}/tools/cpplint.py)
    endif()
    if(${CamelCaseProjectName} STREQUAL "Pd")
      set(LabelName Vault)
    else()
      set(LabelName ${CamelCaseProjectName})
    endif()
    set_property(TEST ${ThisTestName} PROPERTY LABELS ${LabelName} CodingStyle)
  endif()
endfunction()


function(add_project_experimental)
  add_custom_target(All${CamelCaseProjectName} DEPENDS ${AllExesForCurrentProject})
  set_target_properties(All${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/All")
  add_custom_target(Exper${CamelCaseProjectName} COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -M Experimental -T Start -T Build --build-noclean -T Test -T Coverage -T Submit
                                                 DEPENDS All${CamelCaseProjectName})
  set_target_properties(Exper${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/Executables/Tests/${TEST_FOLDER_NAME}")
endfunction()


function(test_summary_output)
  list(LENGTH ALL_GTESTS GTEST_COUNT)
  message(STATUS "${MAIDSAFE_TEST_TYPE_MESSAGE}.   ${GTEST_COUNT} Google test(s) enabled.")
endfunction()


function(add_coverage_exclude REGEX)
  file(APPEND ${CMAKE_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_COVERAGE_EXCLUDE \${CTEST_CUSTOM_COVERAGE_EXCLUDE} \"${REGEX}\")\n")
endfunction()


function(add_memcheck_ignore TEST_NAME)
  file(APPEND ${CMAKE_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_MEMCHECK_IGNORE \${CTEST_CUSTOM_MEMCHECK_IGNORE} \"${TEST_NAME}\")\n")
endfunction()


function(label_as_critical_tests)
  foreach(CriticalTest ${ARGV})
    get_test_property(${CriticalTest} LABELS CurrentLabels)
    set_property(TEST ${CriticalTest} PROPERTY LABELS Critical ${CurrentLabels})
  endforeach()
endfunction()


# Appends ".old" to executable files found (recursively) within the build tree
# which don't match current target filenames.  This avoids accidentally running
# outdated executables in the case of renaming a CMake Target.
macro(rename_outdated_built_exes)
  if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_CURRENT_SOURCE_DIR})
    set(AllExesForAllProjects ${AllExesForAllProjects}
        cryptest protoc CompilerIdC CompilerIdCXX
        LifeStuff_${APPLICATION_VERSION_MAJOR}.${APPLICATION_VERSION_MINOR}.${APPLICATION_VERSION_PATCH}_${TargetPlatform}_${TargetArchitecture})
    if(MSVC)
      file(GLOB_RECURSE BuiltExes RELATIVE ${CMAKE_BINARY_DIR} "${CMAKE_BINARY_DIR}/*.exe")
    else()
      # TODO - Run bash script to generate list of executables and read in to CMake
    endif()
    foreach(BuiltExe ${BuiltExes})
      get_filename_component(BuiltExeName ${BuiltExe} NAME_WE)
      list(FIND AllExesForAllProjects ${BuiltExeName} CurrentExe)
      if(${CurrentExe} STRLESS 0)
        string(REGEX MATCH "build_qt" InQtBuildDir ${BuiltExe})
        if(NOT InQtBuildDir)
          file(RENAME ${CMAKE_BINARY_DIR}/${BuiltExe} ${CMAKE_BINARY_DIR}/${BuiltExe}.old)
          message(STATUS "Renaming outdated executable \"${BuiltExe}\" to \"${BuiltExe}.old\"")
        endif()
      endif()
    endforeach()
  else()
    set(AllExesForAllProjects ${AllExesForAllProjects} ${AllExesForCurrentProject} PARENT_SCOPE)
  endif()
endmacro()


# Copies executable to <current build dir>/package/bin/<config type>/ for use by the package tool
function(ms_copy_to_package_folder maidsafe_target)
  add_custom_command(TARGET ${maidsafe_target}
                       POST_BUILD
                       COMMAND ${CMAKE_COMMAND} -E copy_if_different $<TARGET_FILE:${maidsafe_target}>
                         ${CMAKE_BINARY_DIR}/package/bin/$<CONFIGURATION>/$<TARGET_FILE_NAME:${maidsafe_target}>)
endfunction()

# Searches for and removes old generated .pb.cc and .pb.h files in the source tree
function(remove_old_proto_files)
  file(GLOB_RECURSE PB_FILES RELATIVE ${PROJECT_SOURCE_DIR}/src ${PROJECT_SOURCE_DIR}/src/*.pb.*)
  list(LENGTH PB_FILES PB_FILES_COUNT)
  if(NOT ${PB_FILES_COUNT} EQUAL 0)
    foreach(PB_FILE ${PB_FILES})
      get_filename_component(PB_FILE_PATH ${PB_FILE} PATH)
      get_filename_component(PB_FILE_WE ${PB_FILE} NAME_WE)
      list(FIND PROTO_FILES "${PB_FILE_PATH}/${PB_FILE_WE}.proto" FOUND)
      if (FOUND EQUAL -1)
        file(REMOVE ${PROJECT_SOURCE_DIR}/src/${PB_FILE})
        string(REGEX REPLACE "[\\/.:]" "_" PROTO_CACHE_NAME "${PB_FILE_PATH}/${PB_FILE_WE}.proto")
        unset(${PROTO_CACHE_NAME} CACHE)
        message(STATUS "Removed ${PB_FILE}")
      endif()
    endforeach()
  endif()
endfunction()


# Searches for and removes old test directories that may have been left in %temp%
function(cleanup_temp_dir)
  if(WIN32)
    if(NOT CLEAN_TEMP)
      set(CLEAN_TEMP "OFF" CACHE INTERNAL "Cleanup of temp test folders, options are: ONCE, OFF, ALWAYS" FORCE)
    endif(NOT CLEAN_TEMP)
    execute_process(COMMAND CMD /C ECHO %TEMP% OUTPUT_VARIABLE temp_path OUTPUT_STRIP_TRAILING_WHITESPACE)
    string(REPLACE "\\" "/" temp_path ${temp_path})
    file(GLOB maidsafe_temp_dirs ${temp_path}/MaidSafe_Test*)
    file(GLOB sigmoid_temp_dirs ${temp_path}/Sigmoid_Test*)
    set(temp_dirs ${maidsafe_temp_dirs} ${sigmoid_temp_dirs})
    list(LENGTH temp_dirs temp_dir_count)
    if(NOT ${temp_dir_count} EQUAL 0)
      if(CLEAN_TEMP MATCHES ONCE OR CLEAN_TEMP MATCHES ALWAYS)
        message(STATUS "Cleaning up temporary test folders.")
        foreach(temp_dir ${temp_dirs})
          file(REMOVE_RECURSE ${temp_dir})
          message(STATUS "Removed ${temp_dir}")
        endforeach()
      else()
        message(STATUS "The following temporary test folders could be cleaned up:")
        foreach(temp_dir ${temp_dirs})
          message(STATUS "Found ${temp_dir}")
        endforeach()
        message(STATUS "To cleanup, run cmake . -DCLEAN_TEMP=ONCE or cmake . -DCLEAN_TEMP=ALWAYS")
      endif()
    endif()
    if(NOT CLEAN_TEMP MATCHES ALWAYS)
      set(CLEAN_TEMP "OFF" CACHE INTERNAL "Cleanup of temp test folders, options are: ONCE, OFF, ALWAYS" FORCE)
    endif()
  endif()
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
    foreach(TestConfType Debug Release)
      foreach(DashType Experimental Continuous Nightly Weekly)
        configure_file(${CMAKE_SOURCE_DIR}/CI.cmake.in ${CMAKE_BINARY_DIR}/CI_${DashType}_${TestConfType}.cmake ESCAPE_QUOTES)
      endforeach()
    endforeach()
  endif()
endfunction()

# Gets the target platform name
function(get_target_platform)
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
