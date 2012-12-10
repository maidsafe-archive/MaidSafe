#!/usr/bin/env python
# Copyright (c) 2012 maidsafe.net limited
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification,
# are permitted provided that the following conditions are met:
#
#     * Redistributions of source code must retain the above copyright notice,
#     this list of conditions and the following disclaimer.
#     * Redistributions in binary form must reproduce the above copyright notice,
#     this list of conditions and the following disclaimer in the documentation
#     and/or other materials provided with the distribution.
#     * Neither the name of the maidsafe.net limited nor the names of its
#     contributors may be used to endorse or promote products derived from this
#     software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
# ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
# DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
# FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
# DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
# SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
# CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
# TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
# THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

import os
import subprocess
import sys

#MaidSafe imports
import check_for_network
import lifestuff_killer
import client_environment

def main():
  # check all parameters for the test
  if len(sys.argv) != 5:
    print("Wrong number of parameters for the test")
    return -1

  test_exe = sys.argv[1]
  test_filter = "--gtest_filter=" + sys.argv[2]
  catch_exceptions = "--gtest_catch_exceptions=" + sys.argv[3]
  print "Needs network:", sys.argv[4]
  print "Parent path: ", os.path.dirname(test_exe)
  os.chdir(os.path.dirname(test_exe))

  if sys.argv[4] == "1" and check_for_network.RunCheckNetwork() != 0:
    result = lifestuff_killer.RunKilling()
    if result != 0:
      return -1
    result = client_environment.ParameterBasedStartup(None, None, None, None, None, None)
    if result != 0:
      return -1

  # print "Percieved command: ", test_exe, test_filter, catch_exceptions
  retcode = subprocess.call([test_exe, test_filter, catch_exceptions])
  return retcode

if __name__ == "__main__":
  sys.exit(main())
