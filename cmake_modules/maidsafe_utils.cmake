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
  set_target_properties(maidsafe_${LIB} PROPERTIES FOLDER "MaidSafe/Libraries")
endfunction()


# Adds an executable with CMake Target name of "${EXE}".
# "${FOLDER_NAME}" defines the folder in which the executable appears if the
# chosen IDE supports folders for projects.
function(ms_add_executable EXE FOLDER_NAME)
  set(FILES ${ARGV})
  list(REMOVE_AT FILES 0 1)
  set(ALL_EXECUTABLES ${ALL_EXECUTABLES} ${EXE} PARENT_SCOPE)
  add_executable(${EXE} ${FILES})
  set_target_properties(${EXE} PROPERTIES FOLDER "MaidSafe/Executables/${FOLDER_NAME}")
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
    set_property(TEST ${ThisTestName} PROPERTY LABELS ${CamelCaseProjectName} ${PROJECT_NAME} Functional CodingStyle)
  endif()
endfunction()


function(add_project_experimental)
  add_custom_target(Exper${CamelCaseProjectName} COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -M Experimental -T Start -T Build --build-noclean -T Test -T Coverage -T Submit)
  set_target_properties(Exper${CamelCaseProjectName} PROPERTIES FOLDER "MaidSafe/Executables/Tests/${TEST_FOLDER_NAME}")
endfunction()


function(test_summary_output)
  list(LENGTH ALL_GTESTS GTEST_COUNT)
  message(STATUS "${MAIDSAFE_TEST_TYPE_MESSAGE}.   ${GTEST_COUNT} Google test(s) enabled.")
endfunction()


function(add_coverage_exclude REGEX)
  file(APPEND ${PROJECT_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_COVERAGE_EXCLUDE \${CTEST_CUSTOM_COVERAGE_EXCLUDE} \"${REGEX}\")\n")
endfunction()


function(add_memcheck_ignore TEST_NAME)
  file(APPEND ${PROJECT_BINARY_DIR}/CTestCustom.cmake "SET(CTEST_CUSTOM_MEMCHECK_IGNORE \${CTEST_CUSTOM_MEMCHECK_IGNORE} \"${TEST_NAME}\")\n")
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
      message(STATUS "Renaming outdated executable \"${BUILT_EXE}\" to \"${BUILT_EXE}.old\"")
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
