########################################################################
# CMake build script for Google Test.
#
# To run the tests for Google Test itself on Linux, use 'make test' or
# ctest.  You can select which tests to run using 'ctest -R regex'.
# For more options, run 'ctest --help'.

# BUILD_SHARED_LIBS is a standard CMake variable, but we declare it here to
# make it prominent in the GUI.
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

# When other libraries are using a shared version of runtime libraries,
# Google Test also has to use one.
option(
  gtest_force_shared_crt
  "Use shared (DLL) run-time lib even when Google Test is built as static lib."
  OFF)

option(gtest_build_tests "Build all of gtest's own tests." OFF)

option(gtest_build_samples "Build gtest's sample programs." OFF)

option(gtest_disable_pthreads "Disable uses of pthreads in gtest." OFF)

# Defines pre_project_set_up_hermetic_build() and set_up_hermetic_build().
include(cmake/hermetic_build.cmake OPTIONAL)

if (COMMAND pre_project_set_up_hermetic_build)
  pre_project_set_up_hermetic_build()
endif()

########################################################################
#
# Project-wide settings

# Name of the project.
#
# CMake files in this project can refer to the root source directory
# as ${gtest_SOURCE_DIR} and to the root binary directory as
# ${gtest_BINARY_DIR}.
# Language "C" is required for find_package(Threads).
project(gtest CXX C)
cmake_minimum_required(VERSION 2.8.12.2 FATAL_ERROR)

if (COMMAND set_up_hermetic_build)
  set_up_hermetic_build()
endif()

# Define helper functions and macros used by Google Test.
include(cmake/internal_utils.cmake)

config_compiler_and_linker()  # Defined in internal_utils.cmake.

# Where Google Test's .h files can be found.
# include_directories(
#   ${gtest_SOURCE_DIR}/include
#   ${gtest_SOURCE_DIR}
#   ${CMAKE_SOURCE_DIR}/src/common/include
#   ${CMAKE_SOURCE_DIR}/src/third_party_libs/boost
#   ${CMAKE_SOURCE_DIR}/src/third_party_libs/googlemock/gtest/include
#   )

# Where Google Test's libraries can be found.
# link_directories(${CMAKE_BINARY_DIR})

########################################################################
#
# Defines the gtest & gtest_main libraries.  User tests should link
# with one of them.

# Google Test libraries.  We build them using more strict warnings than what
# are used for other targets, to ensure that gtest can be compiled by a user
# aggressive about warnings.
cxx_library(gtest "${cxx_strict}" src/gtest-all.cc)
cxx_library(gtest_no_maidsafe_log "${cxx_strict}" src/gtest-all.cc)
target_include_directories(gtest PRIVATE ${CMAKE_SOURCE_DIR}/src/common/include)
ms_target_include_system_dirs(gtest PUBLIC ${gtest_SOURCE_DIR}/include PRIVATE ${gtest_SOURCE_DIR} ${BoostSourceDir})
ms_target_include_system_dirs(gtest_no_maidsafe_log PUBLIC ${gtest_SOURCE_DIR}/include PRIVATE ${gtest_SOURCE_DIR})

target_compile_definitions(gtest
  INTERFACE
    GTEST_DONT_DEFINE_FAIL=1
    GTEST_DONT_DEFINE_SUCCEED=1
    GTEST_USE_OWN_TUPLE=0
  PUBLIC
    USE_MAIDSAFE_LOG=1
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:_FILE_OFFSET_BITS=64>
)
target_compile_definitions(gtest_no_maidsafe_log
  INTERFACE
    GTEST_DONT_DEFINE_FAIL=1
    GTEST_DONT_DEFINE_SUCCEED=1
    GTEST_USE_OWN_TUPLE=0
  PUBLIC
    USE_MAIDSAFE_LOG=0
    $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>>:_FILE_OFFSET_BITS=64>
)

# These next 4 commands are a workaround to avoid having to link maidsafe_common via a target_link_libraries call.
# GTest depends on Common since we use log.h, but this would involve including this file from within the
# CMakeLists.txt of Common.
target_compile_options(gtest PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/wd4512>)
target_compile_options(gtest_no_maidsafe_log PRIVATE $<$<CXX_COMPILER_ID:MSVC>:/wd4512>)
target_compile_definitions(gtest PRIVATE $<$<BOOL:${WIN32}>:MAIDSAFE_WIN32> $<$<BOOL:${MSVC}>:BOOST_ALL_NO_LIB>)
target_compile_definitions(gtest_no_maidsafe_log PRIVATE $<$<BOOL:${WIN32}>:MAIDSAFE_WIN32> $<$<BOOL:${MSVC}>:BOOST_ALL_NO_LIB>)
target_link_libraries(gtest maidsafe_common)

if(MSVC)
  set_target_properties(gtest PROPERTIES
                        STATIC_LIBRARY_FLAGS_RELEASE " /LTCG "
                        STATIC_LIBRARY_FLAGS_RELWITHDEBINFO " /LTCG "
                        STATIC_LIBRARY_FLAGS_RELEASENOINLINE " /LTCG ")
endif()

cxx_library(gtest_main "${cxx_strict}" src/gtest_main.cc)
target_include_directories(gtest_main PRIVATE ${gtest_SOURCE_DIR}/include)

########################################################################
#
# Samples on how to link user tests with gtest or gtest_main.
#
# They are not built by default.  To build them, set the
# gtest_build_samples option to ON.  You can do it by running ccmake
# or specifying the -Dbuild_gtest_samples=ON flag when running cmake.

if (gtest_build_samples)
  cxx_executable(sample1_unittest samples gtest_main samples/sample1.cc)
  cxx_executable(sample2_unittest samples gtest_main samples/sample2.cc)
  cxx_executable(sample3_unittest samples gtest_main)
  cxx_executable(sample4_unittest samples gtest_main samples/sample4.cc)
  cxx_executable(sample5_unittest samples gtest_main samples/sample1.cc)
  cxx_executable(sample6_unittest samples gtest_main)
  cxx_executable(sample7_unittest samples gtest_main)
  cxx_executable(sample8_unittest samples gtest_main)
  cxx_executable(sample9_unittest samples gtest)
  cxx_executable(sample10_unittest samples gtest)
endif()

########################################################################
#
# Google Test's own tests.
#
# You can skip this section if you aren't interested in testing
# Google Test itself.
#
# The tests are not built by default.  To build them, set the
# gtest_build_tests option to ON.  You can do it by running ccmake
# or specifying the -Dgtest_build_tests=ON flag when running cmake.

if(gtest_build_tests)
  # This must be set in the root directory for the tests to be run by
  # 'make test' or ctest.
  enable_testing()

  ############################################################
  # C++ tests built with standard compiler flags.
if(UNIX AND NOT APPLE)
  cxx_test(gtest-death-test_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest_environment_test gtest)
  cxx_test(gtest-param-test_test gtest
    test/gtest-param-test2_test.cc)
  cxx_test(gtest_repeat_test gtest)
  cxx_test(gtest_stress_test gtest)
  cxx_test(gtest_throw_on_failure_ex_test gtest)
endif()
  # cxx_test(gtest-filepath_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest-linked_ptr_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest-listener_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest_main_unittest "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest-message_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest_no_test_unittest gtest)
  cxx_test(gtest-options_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest-port_test "gtest_main;gtest_no_maidsafe_log")
  target_compile_definitions(gtest-port_test PRIVATE $<$<BOOL:${UNIX}>:GTEST_HAS_POSIX_REGTEST_HAS_POSIX_RE>)
  target_compile_options(gtest-port_test PRIVATE $<$<BOOL:${MSVC}>:/J>)
  cxx_test(gtest_pred_impl_unittest "gtest_main;gtest_no_maidsafe_log")
  # cxx_test(gtest-printers_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest_prod_test "gtest_main;gtest_no_maidsafe_log"
    test/production.cc)
  # cxx_test(gtest_sole_header_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest-test-part_test "gtest_main;gtest_no_maidsafe_log")
  cxx_test(gtest-typed-test_test "gtest_main;gtest_no_maidsafe_log"
    test/gtest-typed-test2_test.cc)
  # cxx_test(gtest_unittest "gtest_main;gtest_no_maidsafe_log")
  # cxx_test(gtest-unittest-api_test gtest)

  ############################################################
  # C++ tests built with non-standard compiler flags.
#
#   cxx_library(gtest_no_exception "${cxx_no_exception}"
#     src/gtest-all.cc)
#   cxx_library(gtest_main_no_exception "${cxx_no_exception}"
#     src/gtest-all.cc src/gtest_main.cc)
#   cxx_library(gtest_main_no_rtti "${cxx_no_rtti}"
#     src/gtest-all.cc src/gtest_main.cc)
#
#   # cxx_test_with_flags(gtest-death-test_ex_nocatch_test
#   #   "${cxx_exception} -DGTEST_ENABLE_CATCH_EXCEPTIONS_=0"
#   #   gtest test/gtest-death-test_ex_test.cc)
#   # cxx_test_with_flags(gtest-death-test_ex_catch_test
#   #   "${cxx_exception} -DGTEST_ENABLE_CATCH_EXCEPTIONS_=1"
#   #   gtest test/gtest-death-test_ex_test.cc)
#
#   cxx_test_with_flags(gtest_no_rtti_unittest "${cxx_no_rtti}"
#     gtest_main_no_rtti test/gtest_unittest.cc)
#   #
#   # cxx_shared_library(gtest_dll "${cxx_default}"
#   #   src/gtest-all.cc src/gtest_main.cc)
#   #
#   # cxx_executable_with_flags(gtest_dll_test_ "${cxx_default}"
#   #   gtest_dll test/gtest_all_test.cc)
#   # set_target_properties(gtest_dll_test_
#   #                       PROPERTIES
#   #                       COMPILE_DEFINITIONS "GTEST_LINKED_AS_SHARED_LIBRARY=1")
#
#   if (NOT MSVC OR NOT MSVC_VERSION EQUAL "1600")
#     # The C++ Standard specifies tuple_element<int, class>.
#     # Yet MSVC 10's <utility> declares tuple_element<size_t, class>.
#     # That declaration conflicts with our own standard-conforming
#     # tuple implementation.  Therefore using our own tuple with
#     # MSVC 10 doesn't compile.
#     cxx_library(gtest_main_use_own_tuple "${cxx_use_own_tuple}"
#       src/gtest-all.cc src/gtest_main.cc)
#
#     cxx_test_with_flags(gtest-tuple_test "${cxx_use_own_tuple}"
#       gtest_main_use_own_tuple test/gtest-tuple_test.cc)
#
#     cxx_test_with_flags(gtest_use_own_tuple_test "${cxx_use_own_tuple}"
#       gtest_main_use_own_tuple
#       test/gtest-param-test_test.cc test/gtest-param-test2_test.cc)
#   endif()
  set(AllGTestTests ${AllGMockGTestTests} PARENT_SCOPE)
endif()


#==================================================================================================#
# Package                                                                                          #
#==================================================================================================#
set(EXPORT_NAME gtest)
install(TARGETS gtest EXPORT ${EXPORT_NAME} ARCHIVE DESTINATION lib)

install(DIRECTORY ${gtest_SOURCE_DIR}/include DESTINATION include/maidsafe/third_party_libs/googlemock/gtest)
