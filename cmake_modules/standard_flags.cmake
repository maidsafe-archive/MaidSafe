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
#  Module used to set standard linker flags.                                                       #
#                                                                                                  #
#==================================================================================================#


if(MSVC)
  set_property(TARGET ${AllStaticLibsForCurrentProject} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_RELEASE " /LTCG /WX ")
  set_property(TARGET ${AllStaticLibsForCurrentProject} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_DEBUG " /WX ")
  set_property(TARGET ${AllStaticLibsForCurrentProject} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_RELWITHDEBINFO " /LTCG /WX ")
  set_property(TARGET ${AllStaticLibsForCurrentProject} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_MINSIZEREL " /WX ")
  set_property(TARGET ${AllStaticLibsForCurrentProject} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_RELEASENOINLINE " /LTCG /WX ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " /OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO /WX ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_DEBUG " /DEBUG /WX ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO " /OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO /DEBUG /WX ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_MINSIZEREL " /LTCG /WX ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_RELEASENOINLINE " /LTCG /INCREMENTAL:NO /DEBUG /WX ")
endif()


find_program(CCACHE "ccache")
if (CCACHE)
    message( STATUS "Using ccache")
  SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
  SET_PROPERTY(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE)


# Avoid including anything else twice
if(STANDARD_FLAGS_INCLUDED)
  return()
else()
  set(STANDARD_FLAGS_INCLUDED TRUE)
endif()

if(HAVE_LIBC++)
  set(LibCXX "-stdlib=libc++")
endif()
if(HAVE_LIBC++ABI)
  set(LibCXXAbi "-lc++abi")
endif()

if(COVERAGE)
  set(CoverageFlags -pg -fprofile-arcs -ftest-coverage)
else()
  set(CoverageFlags)
endif()

# Configure a ReleaseNoInline build type
if(MSVC)
  set(RELEASENOINLINE_FLAGS "/Z7 /Oy- /Ob0")
else()
  set(RELEASENOINLINE_FLAGS "-g -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline")
endif()
set(CMAKE_C_FLAGS_RELEASENOINLINE "${CMAKE_C_FLAGS_RELEASE} ${RELEASENOINLINE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASENOINLINE "${CMAKE_CXX_FLAGS_RELEASE} ${RELEASENOINLINE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS_RELEASENOINLINE "${CMAKE_EXE_LINKER_FLAGS_RELEASE}")
mark_as_advanced(RELEASENOINLINE_FLAGS CMAKE_C_FLAGS_RELEASENOINLINE CMAKE_CXX_FLAGS_RELEASENOINLINE CMAKE_EXE_LINKER_FLAGS_RELEASENOINLINE)

# Configure a DebugLibStdcxx build type (includes checked iterators)
if(UNIX AND NOT HAVE_LIBC++)
  set(CMAKE_CXX_FLAGS_DEBUGLIBSTDCXX "${CMAKE_CXX_FLAGS_DEBUG} -D_GLIBCXX_DEBUG")
endif()
mark_as_advanced(CMAKE_CXX_FLAGS_DEBUGLIBSTDCXX)

if(NO_UBSAN OR CMAKE_BUILD_TYPE STREQUAL "Release")
  message(STATUS "Undefined behaviour sanitiser is disabled.")
elseif(HAVE_UNDEFINED_BEHAVIOR_SANITIZER AND HAVE_FLAG_SANITIZE_BLACKLIST)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_UBSAN}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_UBSAN}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_UBSAN}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_UBSAN}")
  message(STATUS "Undefined behaviour sanitiser is enabled. Use -DNO_UBSAN=TRUE to prevent.")
  # Multiply all test timeouts by a factor of 10.
  ms_set_global_test_timeout_factor(10)
else()
  message(STATUS "Undefined behaviour sanitiser not available in this compiler.")
endif()

# Needs to come after everything else
if(HAVE_FLAG_SANITIZE_BLACKLIST AND NOT NO_UBSAN)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${SANITIZE_BLACKLIST_FLAG}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SANITIZE_BLACKLIST_FLAG}")
endif()

if(UNIX)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread")
  if(${CMAKE_CXX_COMPILER_ID} MATCHES "^(Apple)?Clang$")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LibCXX} ${LibCXXAbi}")
  if(CCACHE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}  -Qunused-arguments")
  endif(CCACHE)
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # Workaround for GCC bug https://bugs.launchpad.net/ubuntu/+source/gcc-defaults/+bug/1228201
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
  endif()
  if(COVERAGE)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
    find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
  endif()
endif()
