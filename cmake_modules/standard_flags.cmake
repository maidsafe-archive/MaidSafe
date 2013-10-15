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
#  Module used to set standard compiler and linker flags.                                          #
#                                                                                                  #
#==================================================================================================#


add_definitions(-DCOMPANY_NAME=MaidSafe -DAPPLICATION_NAME=LifeStuff)

add_definitions(-DAPPLICATION_VERSION_MAJOR=${ApplicationVersionMajor}
                -DAPPLICATION_VERSION_MINOR=${ApplicationVersionMinor}
                -DAPPLICATION_VERSION_PATCH=${ApplicationVersionPatch}
                -DTARGET_PLATFORM=${TargetPlatform}
                -DTARGET_ARCHITECTURE=${TargetArchitecture})

if(MaidsafeTesting)
  add_definitions(-DTESTING)
endif()

add_definitions(-DBOOST_ASIO_HAS_MOVE
                -DBOOST_ASIO_HAS_STD_ARRAY
                -DBOOST_ASIO_HAS_STD_ATOMIC
                -DBOOST_ASIO_HAS_STD_SHARED_PTR
                -DBOOST_ASIO_HAS_STD_CHRONO)
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_FILESYSTEM_VERSION=3)
add_definitions(-DBOOST_PYTHON_STATIC_LIB)
add_definitions(-DBOOST_THREAD_VERSION=4)
add_definitions(-DBOOST_RESULT_OF_USE_DECLTYPE)
add_definitions(-DGTEST_DONT_DEFINE_FAIL=1 -DGTEST_DONT_DEFINE_SUCCEED=1)

if(MSVC)
  set(CMAKE_CXX_FLAGS)
  set(CMAKE_CXX_FLAGS_INIT)
endif()

# enable libc++ if available
if(HAVE_LIBC++)
  set(LibCpp "-stdlib=libc++")
endif()
# enable libc++abi if available
if(HAVE_LIBC++ABI)
  set(LibCppAbi "-lc++abi")
endif()



if(WIN32)
  add_definitions(-DWIN32 -D_WIN32 -D__WINDOWS__ -D__WIN32__ -DMAIDSAFE_WIN32)
  if(CMAKE_CL_64)
    add_definitions(-DMAIDSAFE_WIN64)
  endif()
elseif(UNIX)
  add_definitions(-D_FILE_OFFSET_BITS=64 -DFUSE_USE_VERSION=30)
  if(APPLE)
    add_definitions(-DMAIDSAFE_APPLE -DHAVE_PTHREAD)
  else()
    add_definitions(-DMAIDSAFE_LINUX -DHAVE_PTHREAD)
  endif()
endif()


if(MSVC)
  add_definitions(-D__MSVC__ -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x600)
  add_definitions(-D_CONSOLE -D_UNICODE -DUNICODE -D_BIND_TO_CURRENT_VCLIBS_VERSION=1)
  add_definitions(-DCRYPTOPP_DISABLE_SSE2)

  # VC11 contains std::tuple with variadic templates emulation macro.
  # _VARIADIC_MAX defaulted to 5 but gtest requires 10.
  add_definitions(-D_VARIADIC_MAX=10)

  # prevents std::min() and std::max() to be overwritten
  add_definitions(-DNOMINMAX)

  # flag to link to static version of Google Glog
  add_definitions(-DGOOGLE_GLOG_DLL_DECL)

  # prevents from automatic linking of boost libraries
  add_definitions(-DBOOST_ALL_NO_LIB)

  # W4 -   Set warning level 4.
  # WX -   Treat warnings as errors.
  # MP7 -  Enable multi-processor compilation (max 7).
  # EHsc - Catches C++ exceptions only and tells the compiler to assume that
  #        extern C functions never throw a C++ exception.
  # TP -   Treat sources as C++
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4 /WX /MP7 /EHsc /TP")

  # C4351 'new behavior: elements of array 'array' will be default initialized'
  # unneeded for new code (only applies to code previously compiled with VS 2005).
  # C4503 'decorated name length exceeded' caused by boost multi-index and signals2
  # Disabled as per advice at https://svn.boost.org/trac/boost/wiki/Guidelines/WarningsGuidelines
  # C4512 'assignment operator could not be generated' caused by boost signals2
  # Disabled as per advice at http://lists.boost.org/boost-users/2009/01/44368.php
  # C4913 'default built-in binary operator ',' used' caused by inclusion of boost/utility/result_of.hpp
  # Disabled due to boost bug https://svn.boost.org/trac/boost/ticket/7663
  # C4996 'Function call with parameters that may be unsafe' caused by boost signals2
  # Disabled as per advice at https://svn.boost.org/trac/boost/wiki/Guidelines/WarningsGuidelines
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4351 /wd4503 /wd4512 /wd4913 /wd4996")

  # O2 - Optimise code for maximum speed.  Implies the following:
  #      Og (global optimisations)
  #      Oi (replace some function calls with intrinsic functions),
  #      Ot (favour fast code),
  #      Oy (suppress creation of frame pointers on the call stack),
  #      Ob2 (auto inline),
  #      Gs (control stack probes),
  #      GF (eliminate duplicate strings),
  #      Gy (allows the compiler to package individual functions in the form of
  #          packaged functions)
  # GL - Whole program optimisation
  # MD - Use the multithread, dynamic version of the C run-time library.
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /D \"NDEBUG\" /MD")

  # Zi -   Produce a program database (.pdb) that contains type information and
  #        symbolic debugging information.
  # Od -   No optimizations in the program (speeds compilation).
  # RTC1 - Enables stack frame run-time error checking and checking for
  #        unintialised variables.
  # MDd -  Use the debug multithread, dynamic version of the C run-time library.
  set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /D \"_DEBUG\" /D \"DEBUG\" /RTC1 /MDd")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${CMAKE_CXX_FLAGS_RELEASE} /Zi")
  if(JUST_THREAD_DEADLOCK_CHECK)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /D \"_JUST_THREAD_DEADLOCK_CHECK\"")
  endif()

  set_target_properties(${ALL_LIBRARIES} PROPERTIES
                          STATIC_LIBRARY_FLAGS_RELEASE "/LTCG"
                          STATIC_LIBRARY_FLAGS_RELWITHDEBINFO "/LTCG")

  set_target_properties(${AllExesForCurrentProject} PROPERTIES
                          LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO"
                          LINK_FLAGS_DEBUG "/DEBUG"
                          LINK_FLAGS_RELWITHDEBINFO "/OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO /DEBUG"
                          LINK_FLAGS_MINSIZEREL "/LTCG")

elseif(UNIX)
  if(MaidsafeCoverage)
    set(CoverageFlags "-pg -fprofile-arcs -ftest-coverage")
  else()
    set(CoverageFlags)
  endif()
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fno-inline -fno-eliminate-unused-debug-types -g3 -ggdb ${CoverageFlags}")
  if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1 -D_FILE_OFFSET_BITS=64)
    add_definitions(-DCRYPTOPP_DISABLE_ASM -DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${LibCpp}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fdiagnostics-format=clang -fdiagnostics-show-option -fdiagnostics-fixit-info -Wno-unused-command-line-argument")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LibCpp} ${LibCppAbi}")
  elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -static-libstdc++")
  endif()
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -W -Werror -Wall -Wextra -Wunused-parameter -Wno-system-headers -Wno-deprecated")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings -Wundef")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized -Wparentheses")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal -Wstrict-overflow -Wstrict-overflow=5 -Wredundant-decls")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pedantic -pedantic-errors ")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -ggdb ${CoverageFlags}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2 -DNDEBUG -D_FORTIFY_SOURCE=2")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${CoverageFlags}")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
  if(JUST_THREAD_DEADLOCK_CHECK)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_JUST_THREAD_DEADLOCK_CHECK")
  endif()
  unset(COVERAGE CACHE)
endif()

# Enable OpenMP if available
if(OPENMP_FOUND)
  add_definitions(-DMAIDSAFE_OMP_ENABLED)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
endif()
