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

# MaidSafe imports
import client_environment
import utils

def main():
  if len(sys.argv) != 4:
    print("Wrong number of parameters for the test")
    return -1

  ctest_command = sys.argv[1]
  ctest_configuration = sys.argv[2]
  tools_path = sys.argv[3]

  proc_count = utils.CountProcs("lifestuff")
  if proc_count > 0:
    print "lifestuf processes running"
    return -1

#  retcode = subprocess.call([sys.executable, os.path.join(tools_path, "client_environment.py")])
#  if retcode != 0:
#    print("Failed executing lifestuff_killer.py")
#    return retcode
  retcode = client_environment.ParameterBasedStartup(None, None, None, None, None, None)
  if retcode != 0:
    print "Failed to set up environment"
    return -1

  execution_array = [ctest_command,
                     '-C', ctest_configuration,
                     '-M', 'Experimental',
                     '-L', 'Lifestuff',
                     '-T', 'Start',
                     '-T', 'Build',
                     '--build-noclean',
                     '-T', 'Test',
                     '-T', 'Coverage',
                     '-T', 'Submit']
  retcode = subprocess.call(execution_array)
  if retcode != 0:
    print "result of ctest execution: ", retcode

  retcode = subprocess.call([sys.executable, os.path.join(tools_path, "lifestuff_killer.py")])
  if retcode != 0:
    print("Failed executing lifestuff_killer.py")
    return retcode

  return retcode

if __name__ == "__main__":
  sys.exit(main())

