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


# Checks VERSION_H file for dependent MaidSafe library versions.  For a project
# with e.g. "#define MAIDSAFE_COMMON_VERSION 010215" in its version.h then the following
# variables are set:
# ${THIS_VERSION_FILE}            maidsafe/common/version.h
# ${THIS_VERSION}                 MAIDSAFE_COMMON_VERSION
# ${${THIS_VERSION}}              10215
# ${CPACK_PACKAGE_VERSION_MAJOR}  1
# ${CPACK_PACKAGE_VERSION_MINOR}  02
# ${CPACK_PACKAGE_VERSION_PATCH}  15
# ${CPACK_PACKAGE_VERSION}        1.02.15
# ${MAIDSAFE_LIBRARY_POSTFIX}     -1_02_15
# Also, the MAIDSAFE_LIBRARY_POSTFIX is appended to each of the CMAKE_<build type>_POSTFIX variables.
# Finally, a preprocessor definition CMAKE_${THIS_VERSION} is added to force a re-run of CMake
# after changing the version in version.h
function(handle_versions VERSION_H)
  file(GLOB THIS_VERSION_FILE RELATIVE ${PROJECT_SOURCE_DIR}/src ${VERSION_H})
  set(THIS_VERSION_FILE ${THIS_VERSION_FILE} PARENT_SCOPE)
  file(STRINGS ${VERSION_H} VERSIONS REGEX "#define [A-Z_]+VERSION [0-9]+$")
  foreach(VERSN ${VERSIONS})
    string(REPLACE "#define " "" VERSN ${VERSN})
    string(REGEX REPLACE "[ ]+" ";" VERSN ${VERSN})
    list(GET VERSN 0 VERSION_VARIABLE_NAME)
    list(GET VERSN 1 VERSION_VARIABLE_VALUE)
    string(REGEX MATCH "THIS_NEEDS_" IS_DEPENDENCY ${VERSION_VARIABLE_NAME})
    if(IS_DEPENDENCY)
      string(REGEX REPLACE "THIS_NEEDS_" "" DEPENDENCY_VARIABLE_NAME ${VERSION_VARIABLE_NAME})
      if(NOT ${${DEPENDENCY_VARIABLE_NAME}} MATCHES ${VERSION_VARIABLE_VALUE})
        set(ERROR_MESSAGE "\n\nThis project needs ${DEPENDENCY_VARIABLE_NAME} ${VERSION_VARIABLE_VALUE}\n")
        set(ERROR_MESSAGE "${ERROR_MESSAGE}Found ${DEPENDENCY_VARIABLE_NAME} ${${DEPENDENCY_VARIABLE_NAME}}\n\n")
        message(FATAL_ERROR "${ERROR_MESSAGE}")
      endif()
    else()
      set(${VERSION_VARIABLE_NAME} ${VERSION_VARIABLE_VALUE} PARENT_SCOPE)
      set(THIS_VERSION ${VERSION_VARIABLE_NAME} PARENT_SCOPE)
      math(EXPR VERSION_MAJOR ${VERSION_VARIABLE_VALUE}/10000)
      math(EXPR VERSION_MINOR ${VERSION_VARIABLE_VALUE}/100%100)
      math(EXPR VERSION_PATCH ${VERSION_VARIABLE_VALUE}%100)
      if(VERSION_MINOR LESS 10)
        set(VERSION_MINOR 0${VERSION_MINOR})
      endif()
      if(VERSION_PATCH LESS 10)
        set(VERSION_PATCH 0${VERSION_PATCH})
      endif()
      set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR} PARENT_SCOPE)
      set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR} PARENT_SCOPE)
      set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH} PARENT_SCOPE)
      set(CPACK_PACKAGE_VERSION ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH} PARENT_SCOPE)
      set(MAIDSAFE_LIBRARY_POSTFIX -${VERSION_MAJOR}_${VERSION_MINOR}_${VERSION_PATCH})
      set(MAIDSAFE_LIBRARY_POSTFIX ${MAIDSAFE_LIBRARY_POSTFIX} PARENT_SCOPE)
      set(CMAKE_RELEASE_POSTFIX ${CMAKE_RELEASE_POSTFIX}${MAIDSAFE_LIBRARY_POSTFIX} PARENT_SCOPE)
      set(CMAKE_DEBUG_POSTFIX ${CMAKE_DEBUG_POSTFIX}${MAIDSAFE_LIBRARY_POSTFIX} PARENT_SCOPE)
      set(CMAKE_RELWITHDEBINFO_POSTFIX ${CMAKE_RELWITHDEBINFO_POSTFIX}${MAIDSAFE_LIBRARY_POSTFIX} PARENT_SCOPE)
      set(CMAKE_MINSIZEREL_POSTFIX ${CMAKE_MINSIZEREL_POSTFIX}${MAIDSAFE_LIBRARY_POSTFIX} PARENT_SCOPE)
      add_definitions(-DCMAKE_${VERSION_VARIABLE_NAME}=${VERSION_VARIABLE_VALUE})
    endif()
  endforeach()
endfunction()

function(set_maidsafe_variables)

  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(CLANG = TRUE)
    set(COMPILER = "Clang")
  endif()

  if(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    set(LINUX = TRUE)
  endif()

  if(${CMAKE_COMPILER_IS_GNUCC})
    set (GCC = TRUE)
    set(COMPILER=GCC)
  endif()

  if(MSVC)
    set(COMPILER = MSVC)
  endif()

endfunction()

# Adds a static library with CMake Target name of "maidsafe_${LIB_OUTPUT_NAME}".
function(ms_add_static_library LIB_OUTPUT_NAME)
  set(FILES ${ARGV})
  list(REMOVE_AT FILES 0)
  string(TOLOWER ${LIB_OUTPUT_NAME} LIB)
  set(ALL_LIBRARIES ${ALL_LIBRARIES} maidsafe_${LIB} PARENT_SCOPE)
  add_library(maidsafe_${LIB} STATIC ${FILES})
  message("ADD LIB  maidsafe_${LIB}")
  set_target_properties(maidsafe_${LIB} PROPERTIES FOLDER "MaidSafe Libraries")
endfunction()


# Adds an executable with CMake Target name of "${EXE}".
# "${FOLDER_NAME}" defines the folder in which the executable appears if the
# chosen IDE supports folders for projects.
function(ms_add_executable EXE FOLDER_NAME)
  set(FILES ${ARGV})
  list(REMOVE_AT FILES 0 1)
  set(ALL_EXECUTABLES ${ALL_EXECUTABLES} ${EXE} PARENT_SCOPE)
  add_executable(${EXE} ${FILES})
  set_target_properties(${EXE} PROPERTIES FOLDER ${FOLDER_NAME})
endfunction()


function(ms_disable_warnings FILE_LIST)
  if(MSVC)
    set_source_files_properties(${ARGV} PROPERTIES COMPILE_FLAGS "/W0")
  elseif(UNIX)
    set_source_files_properties(${ARGV} PROPERTIES COMPILE_FLAGS "-w")
  endif()
endfunction()


function(add_style_test)
  if(PYTHONINTERP_FOUND)
    set(FILES ${ARGV})
    list(REMOVE_AT FILES 0)
    if(UNIX)
      add_test(STYLE_CHECK python ${MaidSafeCommon_TOOLS_DIR}/cpplint.py ${FILES})
    else()
      string(REPLACE "/" "\\\\" STYLE_CHECK_SOURCE ${PROJECT_SOURCE_DIR})
      add_test(STYLE_CHECK ${MaidSafeCommon_TOOLS_DIR}/run_cpplint.bat ${STYLE_CHECK_SOURCE} ${MaidSafeCommon_TOOLS_DIR}/cpplint.py)
    endif()
    set_property(TEST STYLE_CHECK PROPERTY LABELS Functional CodingStyle)
  endif()
endfunction()


function(test_summary_output)
  list(LENGTH ALL_GTESTS GTEST_COUNT)
  message("\n${MAIDSAFE_TEST_TYPE_MESSAGE}.   ${GTEST_COUNT} Google Tests.\n")
  message("    To include all tests,                ${ERROR_MESSAGE_CMAKE_PATH} -DMAIDSAFE_TEST_TYPE=ALL")
  message("    To include behavioural tests,        ${ERROR_MESSAGE_CMAKE_PATH} -DMAIDSAFE_TEST_TYPE=BEH")
  message("    To include functional tests,        ${ERROR_MESSAGE_CMAKE_PATH} -DMAIDSAFE_TEST_TYPE=FUNC")
  message("================================================================================")
endfunction()


function(add_coverage_exclude REGEX)
  file(APPEND ${PROJECT_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_COVERAGE_EXCLUDE \${CTEST_CUSTOM_COVERAGE_EXCLUDE} \"${REGEX}\")\n")
endfunction()


function(add_memcheck_ignore TEST_NAME)
  file(APPEND ${PROJECT_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_MEMCHECK_IGNORE \${CTEST_CUSTOM_MEMCHECK_IGNORE} \"${TEST_NAME}\")\n")
endfunction()


function(final_message)
  message("\nThe library and headers will be installed to:\n")
  message("    \"${CMAKE_INSTALL_PREFIX_MESSAGE}\"\n\n")
  message("To include this project in any other MaidSafe project, use:\n")
  message("    -DMAIDSAFE_COMMON_INSTALL_DIR:PATH=\"${CMAKE_INSTALL_PREFIX_MESSAGE}\"\n\n")
  message("To build and install this project now, run:\n")
  if(MSVC)
    message("    cmake --build . --config Release --target install")
    message("    cmake --build . --config Debug --target install")
  else()
    message("    cmake --build . --target install")
  endif()
  message("\n\n================================================================================"\n)
endfunction()


# Appends ".old" to executable files found (recursively) within the build tree
# which don't match current target filenames.  This avoids accidentally running
# outdated executables in the case of renaming a CMake Target.
function(rename_outdated_built_exes)
  if(MSVC)
    file(GLOB_RECURSE BUILT_EXES RELATIVE ${PROJECT_BINARY_DIR} "${PROJECT_BINARY_DIR}/*.exe")
  else()
    # TODO - Run bash script to generate list of executables and read in to CMake
  endif()
  foreach(BUILT_EXE ${BUILT_EXES})
    get_filename_component(BUILT_EXE_NAME ${BUILT_EXE} NAME_WE)
    list(FIND ALL_EXECUTABLES ${BUILT_EXE_NAME} CURRENT_EXE)
    if(${CURRENT_EXE} STRLESS 0)
      file(RENAME ${PROJECT_BINARY_DIR}/${BUILT_EXE} ${PROJECT_BINARY_DIR}/${BUILT_EXE}.old)
      message("-- Renaming outdated executable \"${BUILT_EXE}\" to \"${BUILT_EXE}.old\"")
      message("================================================================================\n")
    endif()
  endforeach()
endfunction()


function(ms_install_libs)
  install(TARGETS ${ARGV} export ${MS_PROJECT_NAME} ARCHIVE DESTINATION lib)
endfunction()


# Ensure all headers for a given destination folder are passed in a single call,
# as this function clears out all existing headers from the destination folder
# (non-recursively).
function(ms_install_headers HEADERS_DESTINATION)
  set(MS_HEADERS ${ARGV})
  list(REMOVE_AT MS_HEADERS 0)
  foreach(HEADER ${MS_HEADERS})
    get_filename_component(HEADER_EXTENSION ${HEADER} EXT)
    get_filename_component(HEADER_NAME ${HEADER} NAME)
    if((NOT ${HEADER_NAME} STREQUAL "version.h") AND (NOT ${HEADER_EXTENSION} STREQUAL ".pb.h"))
      file(STRINGS ${HEADER} VERSION_LINE REGEX "#if ${THIS_VERSION} != ${${THIS_VERSION}}")
      if("${VERSION_LINE}" STREQUAL "")
        set(ERROR_MESSAGE "\n\n${HEADER} is missing the version guard block.\n")
        set(ERROR_MESSAGE "${ERROR_MESSAGE}Ensure the following code is included in the file:\n\n")
        set(ERROR_MESSAGE "${ERROR_MESSAGE}\t#include \"${THIS_VERSION_FILE}\"\n")
        set(ERROR_MESSAGE "${ERROR_MESSAGE}\t#if ${THIS_VERSION} != ${${THIS_VERSION}}\n")
        set(ERROR_MESSAGE "${ERROR_MESSAGE}\t#  error This API is not compatible with the installed library.\\\n")
        set(ERROR_MESSAGE "${ERROR_MESSAGE}\t    Please update the ${MS_PROJECT_NAME} library.\n")
        set(ERROR_MESSAGE "${ERROR_MESSAGE}\t#endif\n\n\n")
        message(FATAL_ERROR ${ERROR_MESSAGE})
      endif()
    endif()
  endforeach()
  install(CODE "FILE(GLOB INSTALLED \"\${CMAKE_INSTALL_PREFIX}/${DESTINATION}/*.h*\")\nIF(INSTALLED)\nFILE(REMOVE \${INSTALLED})\nENDIF()")
  install(FILES ${MS_HEADERS} DESTINATION include/maidsafe/${HEADERS_DESTINATION})
endfunction()


function(ms_install_export)
  install(export ${MS_PROJECT_NAME} DESTINATION share/maidsafe)
  # Append version info to the export file.
  install(CODE "STRING(TOLOWER \${CMAKE_INSTALL_CONFIG_NAME} CMAKE_INSTALL_CONFIG_NAME_LOWER)")
  install(CODE "FIND_FILE(INSTALL_FILE ${MS_PROJECT_NAME}-\${CMAKE_INSTALL_CONFIG_NAME_LOWER}.cmake PATHS \${CMAKE_INSTALL_PREFIX}/share/maidsafe)")
  install(CODE "FILE(APPEND \${INSTALL_FILE} \"\\n\\n# Definition of this library's version\\n\")")
  install(CODE "FILE(APPEND \${INSTALL_FILE} \"SET(${THIS_VERSION} ${${THIS_VERSION}})\\n\")")
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
        message("-- Removed ${PB_FILE}")
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
      message("")
      if(CLEAN_TEMP MATCHES ONCE OR CLEAN_TEMP MATCHES ALWAYS)
        message("Cleaning up temporary test folders.\n")
        foreach(temp_dir ${temp_dirs})
          file(REMOVE_RECURSE ${temp_dir})
          message("-- Removed ${temp_dir}")
        endforeach()
      else()
        message("The following temporary test folders could be cleaned up:\n")
        foreach(temp_dir ${temp_dirs})
          message("-- Found ${temp_dir}")
        endforeach()
        message("")
        message("To cleanup, run cmake ../.. -DCLEAN_TEMP=ONCE or cmake ../.. -DCLEAN_TEMP=ALWAYS")
      endif()
      message("================================================================================")
    endif()
    if(NOT CLEAN_TEMP MATCHES ALWAYS)
      set(CLEAN_TEMP "OFF" CACHE INTERNAL "Cleanup of temp test folders, options are: ONCE, OFF, ALWAYS" FORCE)
    endif()
  endif()
endfunction()
