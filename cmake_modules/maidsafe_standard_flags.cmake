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
#  Module used to set standard compiler and linker flags.                      #
#                                                                              #
#==============================================================================#


add_definitions(-DCOMPANY_NAME=maidsafe -DAPPLICATION_NAME=lifestuff)
add_definitions(-DBOOST_FILESYSTEM_NO_DEPRECATED -DBOOST_FILESYSTEM_VERSION=3)

if(MSVC)
  set(CMAKE_CXX_FLAGS)
  set(CMAKE_CXX_FLAGS_INIT)
endif()

# enable libc++ if available
if(APPLE OR HAVE_LIBC++)
  set(LIBC++ "-stdlib=libc++")
endif()
  
if(WIN32)
  add_definitions(-DWIN32 -D_WIN32 -D__WINDOWS__ -D__WIN32__ -DMAIDSAFE_WIN32)
elseif(UNIX)
  add_definitions(-D_FILE_OFFSET_BITS=64)
  if(APPLE)
    add_definitions(-DMAIDSAFE_APPLE -DHAVE_PTHREAD)
  else()
    add_definitions(-DMAIDSAFE_LINUX -DHAVE_PTHREAD)
  endif()
endif()


if(MSVC)
  add_definitions(-D__MSVC__ -DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x501)
  add_definitions(-D_CONSOLE -D_UNICODE -DUNICODE -D_BIND_TO_CURRENT_VCLIBS_VERSION=1)

  # VC11 contains std::tuple with variadic templates emulation macro.
  # _VARIADIC_MAX defaulted to 5 but gtest requires 10.
  add_definitions(-D_VARIADIC_MAX=10)

  # prevents std::min() and std::max() to be overwritten
  add_definitions(-DNOMINMAX)

  # flag to link to static version of Google Glog
  add_definitions(-DGOOGLE_GLOG_DLL_DECL=)

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
  # C4996 'Function call with parameters that may be unsafe' caused by boost signals2
  # Disabled as per advice at https://svn.boost.org/trac/boost/wiki/Guidelines/WarningsGuidelines
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /wd4351 /wd4503 /wd4512 /wd4996")

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
  # MT - Use the multithread, static version of the C run-time library.
  set(CMAKE_CXX_FLAGS_RELEASE "/O2 /GL /D \"NDEBUG\" /MD")

  # Zi -   Produce a program database (.pdb) that contains type information and
  #        symbolic debugging information.
  # Od -   No optimizations in the program (speeds compilation).
  # RTC1 - Enables stack frame run-time error checking and checking for
  #        unintialised variables.
  # MTd -  Use the debug multithread, static version of the C run-time library.
  set(CMAKE_CXX_FLAGS_DEBUG "/Zi /Od /D \"_DEBUG\" /D \"DEBUG\" /RTC1 /MDd")
  set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD")
  set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "/MDd")

  set_target_properties(${ALL_LIBRARIES} PROPERTIES STATIC_LIBRARY_FLAGS_RELEASE "/LTCG")

  set_target_properties(${ALL_EXECUTABLES} PROPERTIES
                          LINK_FLAGS_RELEASE "/OPT:REF /OPT:ICF /LTCG /INCREMENTAL:NO"
                          LINK_FLAGS_DEBUG "/DEBUG"
                          LINK_FLAGS_RELWITHDEBINFO "/DEBUG /LTCG /INCREMENTAL:NO"
                          LINK_FLAGS_MINSIZEREL "/LTCG")
elseif(UNIX)
  if(DEFINED COVERAGE)
    if(${COVERAGE})
      set(COVERAGE_FLAGS "-pg -fprofile-arcs -ftest-coverage")
    else()
      set(COVERAGE_FLAGS)
    endif()
  else()
    if($ENV{COVERAGE})
      set(COVERAGE_FLAGS "-pg -fprofile-arcs -ftest-coverage")
      set(COVERAGE ON)
    else()
      set(COVERAGE_FLAGS)
    endif()
  endif()
  if(${COVERAGE})
    message(STATUS "Coverage ON")
  else()
    message(STATUS "Coverage OFF.  To enable, do:   export COVERAGE=ON   and re-run CMake")
  endif()
  set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0 -fno-inline -fno-eliminate-unused-debug-types -g3 -ggdb ${COVERAGE_FLAGS}")
  if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    add_definitions(-DGTEST_USE_OWN_TR1_TUPLE=1 -D_FILE_OFFSET_BITS=64)
    add_definitions(-DCRYPTOPP_DISABLE_ASM -DCRYPTOPP_DISABLE_UNCAUGHT_EXCEPTION)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Wextra -Wno-unused-parameter ")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC ${LIBC++}")
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fdiagnostics-format=clang -fdiagnostics-show-option -fdiagnostics-fixit-info -ftrapv -fcatch-undefined-behavior") #  -faddress-sanitizer")default
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${LIBC++} -ldl")
  endif()
  if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion OUTPUT_VARIABLE GCC_VERSION)
    if(GCC_VERSION VERSION_GREATER 4.7 OR GCC_VERSION VERSION_EQUAL 4.7)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -static-libstdc++ -fopenmp")
    elseif(GCC_VERSION VERSION_GREATER 4.6 OR GCC_VERSION VERSION_EQUAL 4.6)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x -static-libstdc++ -fopenmp")
    else()
      message(FATAL_ERROR "Unsupported version of GCC, minimum 4.6 required")
    endif()
  endif()
  if(CMAKE_BUILD_TYPE MATCHES "Debug")
    add_definitions(-DDEBUG)
  endif()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wextra -Wunused-parameter -Wno-system-headers -Wno-deprecated")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wwrite-strings -Wundef -D_FORTIFY_SOURCE=2")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wuninitialized -Wparentheses")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wfloat-equal -Wstrict-overflow -Wstrict-overflow=5 -Wredundant-decls")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -pedantic -pedantic-errors -Weffc++  ")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g -ggdb ${COVERAGE_FLAGS}")
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O2")
  set(CMAKE_EXE_LINKER_FLAGS_DEBUG "${CMAKE_EXE_LINKER_FLAGS_DEBUG} ${COVERAGE_FLAGS}")
  unset(COVERAGE CACHE)
endif()

