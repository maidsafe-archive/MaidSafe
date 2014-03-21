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

if(NO_UBSAN)
  message(STATUS "Undefined behaviour sanitiser is disabled.")
elseif(HAVE_UNDEFINED_BEHAVIOR_SANITIZER)
  set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_C_FLAGS_UBSAN}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_UBSAN}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${CMAKE_EXE_LINKER_FLAGS_UBSAN}")
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${CMAKE_SHARED_LINKER_FLAGS_UBSAN}")
  message(STATUS "Undefined behaviour sanitiser is enabled. Use -DNO_UBSAN=TRUE to prevent.")
else()
  message(STATUS "Undefined behaviour sanitiser not available in this compiler.")
endif()

if(MSVC)
  set_property(TARGET ${AllStaticLibsForCurrentProject} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_RELEASE " /LTCG ")
  set_property(TARGET ${AllStaticLibsForCurrentProject} APPEND_STRING PROPERTY STATIC_LIBRARY_FLAGS_RELWITHDEBINFO " /LTCG ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_RELEASE " /OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_DEBUG " /DEBUG ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_RELWITHDEBINFO " /OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO /DEBUG ")
  set_property(TARGET ${AllExesForCurrentProject} APPEND_STRING PROPERTY LINK_FLAGS_MINSIZEREL " /LTCG ")
elseif(UNIX)
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -lpthread ${OpenMP_CXX_FLAGS}")
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LibCXX} ${LibCXXAbi}")
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    # Workaround for GCC bug https://bugs.launchpad.net/ubuntu/+source/gcc-defaults/+bug/1228201
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,--no-as-needed")
  endif()
  if(COVERAGE)
    set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} -fprofile-arcs -ftest-coverage")
    find_program(CTEST_COVERAGE_COMMAND NAMES gcov)
  endif()
endif()
