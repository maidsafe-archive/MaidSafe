set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

########################################################################
# CMake build script for Google Mock.
#
# To run the tests for Google Mock itself on Linux, use 'make test' or
# ctest.  You can select which tests to run using 'ctest -R regex'.
# For more options, run 'ctest --help'.

# BUILD_SHARED_LIBS is a standard CMake variable, but we declare it here to
# make it prominent in the GUI.
option(BUILD_SHARED_LIBS "Build shared libraries (DLLs)." OFF)

# Forces BUILD_SHARED_LIBS to OFF as Google Mock currently does not support
# working in a DLL.
# TODO(vladl@google.com): Implement building gMock as a DLL.
set(BUILD_SHARED_LIBS OFF)

option(gmock_build_tests "Build all of Google Mock's own tests." OFF)

# A directory to find Google Test sources.
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/gtest/CMakeLists.txt")
  set(gtest_dir gtest)
else()
  set(gtest_dir ../gtest)
endif()

# Defines pre_project_set_up_hermetic_build() and set_up_hermetic_build().
include("${gtest_dir}/cmake/hermetic_build.cmake" OPTIONAL)

if (COMMAND pre_project_set_up_hermetic_build)
  # Google Test also calls hermetic setup functions from add_subdirectory,
  # although its changes will not affect things at the current scope.
  pre_project_set_up_hermetic_build()
endif()

########################################################################
#
# Project-wide settings

# Name of the project.
#
# CMake files in this project can refer to the root source directory
# as ${gmock_SOURCE_DIR} and to the root binary directory as
# ${gmock_BINARY_DIR}.
# Language "C" is required for find_package(Threads).
project(gmock CXX C)
cmake_minimum_required(VERSION 2.6.2)

if (COMMAND set_up_hermetic_build)
  set_up_hermetic_build()
endif()

# Instructs CMake to process Google Test's CMakeLists.txt and add its
# targets to the current scope.  We are placing Google Test's binary
# directory in a subdirectory of our own as VC compilation may break
# if they are the same (the default).
add_subdirectory("${gtest_dir}" "${gmock_BINARY_DIR}/gtest")
set(AllGTestTests ${AllGTestTests} PARENT_SCOPE)

# Although Google Test's CMakeLists.txt calls this function, the
# changes there don't affect the current scope.  Therefore we have to
# call it again here.
config_compiler_and_linker()  # from ${gtest_dir}/cmake/internal_utils.cmake

# if(CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
#   add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++ -ldl -lcxxrt")
# endif()

# Adds Google Mock's and Google Test's header directories to the search path.
# include_directories("${gmock_SOURCE_DIR}/include"
#                     "${gmock_SOURCE_DIR}"
#                     "${gtest_SOURCE_DIR}/include"
#                     # This directory is needed to build directly from Google
#                     # Test sources.
#                     "${gtest_SOURCE_DIR}")

########################################################################
#
# Defines the gmock & gmock_main libraries.  User tests should link
# with one of them.

# Google Mock libraries.  We build them using more strict warnings than what
# are used for other targets, to ensure that Google Mock can be compiled by
# a user aggressive about warnings.
cxx_library(gmock "${cxx_strict}" src/gmock-all.cc)
ms_target_include_system_dirs(gmock PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(gmock gtest)
if(MSVC)
  set_target_properties(gmock PROPERTIES
                        STATIC_LIBRARY_FLAGS_RELEASE " /LTCG "
                        STATIC_LIBRARY_FLAGS_RELWITHDEBINFO " /LTCG "
                        STATIC_LIBRARY_FLAGS_RELEASENOINLINE " /LTCG ")
endif()

cxx_library(gmock_main "${cxx_strict}" src/gmock_main.cc)
target_link_libraries(gmock_main gmock)
cxx_library(gmock_main_no_maidsafe_log "${cxx_strict}" src/gmock_main.cc src/gmock-all.cc)
ms_target_include_system_dirs(gmock_main_no_maidsafe_log PUBLIC ${PROJECT_SOURCE_DIR}/include PRIVATE ${PROJECT_SOURCE_DIR})
target_link_libraries(gmock_main_no_maidsafe_log gtest_no_maidsafe_log maidsafe_common)  # link Common to get the flags and defs.

########################################################################
#
# Google Mock's own tests.
#
# You can skip this section if you aren't interested in testing
# Google Mock itself.
#
# The tests are not built by default.  To build them, set the
# gmock_build_tests option to ON.  You can do it by running ccmake
# or specifying the -Dgmock_build_tests=ON flag when running cmake.

if (gmock_build_tests)
  # This must be set in the root directory for the tests to be run by
  # 'make test' or ctest.
  enable_testing()

  ############################################################
  # C++ tests built with standard compiler flags.

  cxx_test(gmock-actions_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-cardinalities_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-generated-actions_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-generated-function-mockers_test gmock_main_no_maidsafe_log)
  target_compile_options(gmock-generated-function-mockers_test PRIVATE $<$<BOOL:${MSVC}>:/wd4373>)
  cxx_test(gmock-generated-internal-utils_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-generated-matchers_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-internal-utils_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-matchers_test gmock_main_no_maidsafe_log)
  target_compile_options(gmock-matchers_test PRIVATE $<$<BOOL:${MSVC}>:/wd4267 /bigobj>)
  cxx_test(gmock-more-actions_test gmock_main_no_maidsafe_log)
  target_compile_options(gmock-more-actions_test PRIVATE $<$<BOOL:${MSVC}>:/wd4996>)
  cxx_test(gmock-nice-strict_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-port_test gmock_main_no_maidsafe_log)
  cxx_test(gmock-spec-builders_test gmock_main_no_maidsafe_log)
  cxx_test(gmock_link_test gmock_main_no_maidsafe_log test/gmock_link2_test.cc)
  target_include_directories(gmock_link_test PRIVATE ${gmock_SOURCE_DIR})
  target_compile_options(gmock_link_test PRIVATE $<$<BOOL:${MSVC}>:/wd4996>)
  if(NOT MSVC)
    cxx_test(gmock_stress_test gmock)
  endif()
  cxx_test(gmock_test gmock_main_no_maidsafe_log)

  # gmock_all_test is commented to save time building and running tests.
  # Uncomment if necessary.
  # cxx_test(gmock_all_test gmock_main_no_maidsafe_log)

  ############################################################
  # C++ tests built with non-standard compiler flags.
#
#   cxx_library(gmock_main_no_exception "${cxx_no_exception}"
#     "${gtest_dir}/src/gtest-all.cc" src/gmock-all.cc src/gmock_main.cc)
#   cxx_library(gmock_main_no_rtti "${cxx_no_rtti}"
#     "${gtest_dir}/src/gtest-all.cc" src/gmock-all.cc src/gmock_main.cc)
#   cxx_library(gmock_main_use_own_tuple "${cxx_use_own_tuple}"
#     "${gtest_dir}/src/gtest-all.cc" src/gmock-all.cc src/gmock_main.cc)
#
#   cxx_test_with_flags(gmock-more-actions_no_exception_test "${cxx_no_exception}"
#     gmock_main_no_exception test/gmock-more-actions_test.cc)
#
#   cxx_test_with_flags(gmock_no_rtti_test "${cxx_no_rtti}"
#     gmock_main_no_rtti test/gmock-spec-builders_test.cc)
#
#   cxx_test_with_flags(gmock_use_own_tuple_test "${cxx_use_own_tuple}"
#     gmock_main_use_own_tuple test/gmock-spec-builders_test.cc)
  set(AllGMockTests ${AllGMockGTestTests} PARENT_SCOPE)
endif()


#==================================================================================================#
# Package                                                                                          #
#==================================================================================================#
set(EXPORT_NAME gmock)
install(TARGETS gmock EXPORT ${EXPORT_NAME} ARCHIVE DESTINATION lib)

install(DIRECTORY ${gmock_SOURCE_DIR}/include DESTINATION include/maidsafe/third_party_libs/googlemock)
