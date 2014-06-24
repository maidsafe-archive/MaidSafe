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
set(BlacklistFile "${CMAKE_SOURCE_DIR}/tools/suppressions/blacklist.txt")
check_c_compiler_flag("-fsanitize-blacklist='${BlacklistFile}'" HAVE_FLAG_SANITIZE_BLACKLIST)
unset(CMAKE_REQUIRED_FLAGS)

if(HAVE_FLAG_SANITIZE_BLACKLIST)
  # Add PP def which changes with the contents of the blacklist file to force recompilation if required.
  file(MD5 "${BlacklistFile}" Hash)
  set(SANITIZE_BLACKLIST_FLAG "-fsanitize-blacklist='${BlacklistFile}' -DSANITIZE_BLACKLIST_MD5=${Hash}")

  # Add a target which when built checks the current contents of the blacklist file against the contents
  # when CMake was last run.  If they don't match, targets which depend on this one will fail to build
  # until CMake is rerun.
  add_custom_target(check_sanitizer_blacklist ALL
      ${CMAKE_COMMAND} "-DBlacklistFile=${BlacklistFile}"
                       -DBlacklistFileHash=${Hash}
                       -P "${CMAKE_SOURCE_DIR}/tools/suppressions/blacklist_check.cmake")
endif()
