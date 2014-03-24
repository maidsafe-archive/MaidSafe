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

include(CheckCCompilerFlag)

set(CMAKE_REQUIRED_FLAGS "-Werror")
check_c_compiler_flag("-fsanitize-blacklist=${CMAKE_SOURCE_DIR}/tools/suppressions/blacklist.txt" HAVE_FLAG_SANITIZE_BLACKLIST)
unset(CMAKE_REQUIRED_FLAGS)

if(HAVE_FLAG_SANITIZE_BLACKLIST)
  set(SANITIZE_BLACKLIST_FLAG "-fsanitize-blacklist=${CMAKE_SOURCE_DIR}/tools/suppressions/blacklist.txt")
endif()

if(NOT HAVE_FLAG_SANITIZE_BLACKLIST)
  return()
endif()

# 2014-03-24 ned: How do I get changes to the blacklist to reinvoke cmake config?
#add_dependencies(${CMAKE_PROJECT_NAME} "${CMAKE_SOURCE_DIR}/tools/suppressions/blacklist.txt")
