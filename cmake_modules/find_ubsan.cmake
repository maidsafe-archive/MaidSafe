#
# The MIT License (MIT)
#
# Copyright (c) 2013 Matthew Arsenault
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#

# Check if the compiler supports a working ubsan. Provides a UBSan
# build type, which is essentially Debug + ubsan. The flag can be used
# independently to compose it with other build types or sanitizers.
#
# Sets these variables:
#
# HAVE_UNDEFINED_BEHAVIOR_SANITIZER - True or false if the UBSan is available
# UNDEFINED_BEHAVIOR_SANITIZER_FLAG - Flag to add to compiler to use ubsan if supported
#
# CMAKE_C_FLAGS_UBSAN - Flags to use for C with ubsan
# CMAKE_CXX_FLAGS_UBSAN  - Flags to use for C++ with ubsan
##
#


include(CheckCXXCompilerFlag)
include(CheckCXXSourceRuns)

# Set -Werror to catch "argument unused during compilation" warnings
set(CMAKE_REQUIRED_FLAGS "-Werror")
check_cxx_compiler_flag("-fsanitize=undefined" HAVE_FLAG_SANITIZE_UNDEFINED)
# Ignore misalignment (libcrypto++ does a lot of this)
check_cxx_compiler_flag("-fno-sanitize=alignment" HAVE_FLAG_NO_SANITIZE_ALIGNMENT)
#check_cxx_compiler_flag("-fcatch-undefined-behavior" HAVE_FLAG_CATCH_UNDEFINED_BEHAVIOR)
if(HAVE_FLAG_SANITIZE_UNDEFINED)
  set(UNDEFINED_BEHAVIOR_SANITIZER_FLAG "-fsanitize=undefined")
elseif(HAVE_FLAG_CATCH_UNDEFINED_BEHAVIOR)
  set(UNDEFINED_BEHAVIOR_SANITIZER_FLAG "-fcatch-undefined-behavior")
else()
  set(HAVE_UNDEFINED_BEHAVIOR_SANITIZER FALSE)
  return()
endif()
unset(CMAKE_REQUIRED_FLAGS)


# It isn't sufficient to check if the flag works since the
# check_c_compiler_flag test doesn't link the output.
#
# Most clang packages ship broken packages (the autotools build
# produces a broken package which doesn't include the ubsan
# compiler-rt, so check that it actually works with a linked program
# before trying to use it
set(CMAKE_REQUIRED_FLAGS "${UNDEFINED_BEHAVIOR_SANITIZER_FLAG}")

check_cxx_source_runs(
"
#include <cstdio>
#include <cstdlib>
#include <iostream>

class BarB
{
    public:
        float y;
        /* Include something that uses a virtual function. The symbols
           that are broken on current OS X libc++ involve this */
        virtual int arst(int o)
        {
            return 4 + o;
        }
    };

/* Just include something that ubsan will need to check */
int main(int argc, const char* argv[])
{
    BarB* b = new BarB();
    if (argc > 1)
    {
        fputs(argv[atoi(argv[1])], stdout);
        std::cout << b->arst(atoi(argv[1]));
    }

    delete b;
    return 0;
}
"
  HAVE_UNDEFINED_BEHAVIOR_SANITIZER)
unset(CMAKE_REQUIRED_FLAGS)

if(NOT HAVE_UNDEFINED_BEHAVIOR_SANITIZER)
  return()
endif()

if(HAVE_FLAG_NO_SANITIZE_ALIGNMENT)
  # On x86/x64 libcrypto++ does lots of unaligned accesses for speed, it generates
  # a ton of noise some of which cant't be blacklisted, so disable.
  check_cxx_source_runs(
  "
  int main(int argc, const char* argv[])
  {
  #if defined(__i386) || defined(__i386__) || defined(_M_IX86)
      /* I am x86 */
  #elif defined(__x86_64) || defined(__x86_64__) || defined(__amd64) || defined(_M_X64)
      /* I am x64 */
  #else
  #error I am neither x86 nor x64
  #endif
      return 0;
  }
  "
    HAVE_X86_OR_X64_TARGET)
  if(HAVE_X86_OR_X64_TARGET)
    set(UNDEFINED_BEHAVIOR_SANITIZER_FLAG "${UNDEFINED_BEHAVIOR_SANITIZER_FLAG} -fno-sanitize=alignment")
  endif()
endif()

set(CMAKE_C_FLAGS_UBSAN "${UNDEFINED_BEHAVIOR_SANITIZER_FLAG}"
    CACHE STRING "Flags used by the C compiler during UBSan builds."
    FORCE)
set(CMAKE_CXX_FLAGS_UBSAN "${UNDEFINED_BEHAVIOR_SANITIZER_FLAG}"
    CACHE STRING "Flags used by the C++ compiler during UBSan builds."
    FORCE)
set(CMAKE_EXE_LINKER_FLAGS_UBSAN "${UNDEFINED_BEHAVIOR_SANITIZER_FLAG}"
    CACHE STRING "Flags used for linking binaries during UBSan builds."
    FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_UBSAN "${UNDEFINED_BEHAVIOR_SANITIZER_FLAG}"
    CACHE STRING "Flags used by the shared libraries linker during UBSan builds."
    FORCE)
mark_as_advanced(CMAKE_C_FLAGS_UBSAN
                 CMAKE_CXX_FLAGS_UBSAN
                 CMAKE_EXE_LINKER_FLAGS_UBSAN
                 CMAKE_SHARED_LINKER_FLAGS_UBSAN)


