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

# This module tests if thread sanitizer is supported by the compiler,
# and creates a TSan build type (i.e. set CMAKE_BUILD_TYPE=TSan to use
# it). This sets the following variables:
#
# CMAKE_C_FLAGS_TSAN - Flags to use for C with tsan
# CMAKE_CXX_FLAGS_TSAN  - Flags to use for C++ with tsan
# HAVE_THREAD_SANITIZER - True or false if the TSan build type is available

include(CheckCCompilerFlag)


# Set -Werror to catch "argument unused during compilation" warnings
#set(CMAKE_REQUIRED_FLAGS "-Werror -fthread-sanitizer") # Also needs to be a link flag for test to pass
#check_c_compiler_flag("-fthread-sanitizer" HAVE_FLAG_THREAD_SANITIZER)

set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=thread") # Also needs to be a link flag for test to pass
check_c_compiler_flag("-fsanitize=thread" HAVE_FLAG_SANITIZE_THREAD)
unset(CMAKE_REQUIRED_FLAGS)

if(NOT HAVE_FLAG_SANITIZE_THREAD)
  set(CMAKE_REQUIRED_FLAGS "-Werror -fsanitize=thread -fPIC -pie") # Also needs to be a link flag for test to pass
  check_c_compiler_flag("-fsanitize=thread" HAVE_FLAG_SANITIZE_THREAD_NEEDS_PIC_PIE)
  unset(CMAKE_REQUIRED_FLAGS)
endif()

# A special test that uses threads seems to not be necessary. tsan
# symbols are used even in just int main() { return 0; }


if(HAVE_FLAG_SANITIZE_THREAD)
  # Clang 3.2+ use this version
  set(THREAD_SANITIZER_FLAG "-fsanitize=thread")
  set(THREAD_SANITIZER_FLAG_EXE "-fsanitize=thread")
elseif(HAVE_FLAG_SANITIZE_THREAD_NEEDS_PIC_PIE)
  # GCC 4.8+ use this version
  set(THREAD_SANITIZER_FLAG "-fsanitize=thread -fPIC")
  set(THREAD_SANITIZER_FLAG_EXE "-fsanitize=thread -pie")
elseif(HAVE_FLAG_THREAD_SANITIZER)
  # Older deprecated flag for TSan
  set(THREAD_SANITIZER_FLAG_FLAG "-fthread-sanitizer")
else()
  if(CMAKE_BUILD_TYPE STREQUAL "Tsan")
    message(FATAL_ERROR "\nYou have specified \"Tsan\" as the build type, but Thread Sanitiser is not supported on this platform with the chosen compiler.\n")
  endif()
  set(HAVE_THREAD_SANITIZER FALSE)
  return()
endif()

set(HAVE_THREAD_SANITIZER TRUE)

set(CMAKE_C_FLAGS_TSAN "-O0 -g ${THREAD_SANITIZER_FLAG} -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline"
    CACHE STRING "Flags used by the C compiler during TSan builds."
    FORCE
    )
set(CMAKE_CXX_FLAGS_TSAN "-O0 -g ${THREAD_SANITIZER_FLAG} -fno-omit-frame-pointer -fno-optimize-sibling-calls -fno-inline"
    CACHE STRING "Flags used by the C++ compiler during TSan builds."
    FORCE)
set(CMAKE_EXE_LINKER_FLAGS_TSAN "${THREAD_SANITIZER_FLAG_EXE}"
    CACHE STRING "Flags used for linking binaries during TSan builds."
    FORCE)
set(CMAKE_SHARED_LINKER_FLAGS_TSAN "${THREAD_SANITIZER_FLAG}"
    CACHE STRING "Flags used by the shared libraries linker during TSan builds."
    FORCE)
mark_as_advanced(CMAKE_C_FLAGS_TSAN
                 CMAKE_CXX_FLAGS_TSAN
                 CMAKE_EXE_LINKER_FLAGS_TSAN
                 CMAKE_SHARED_LINKER_FLAGS_TSAN)
