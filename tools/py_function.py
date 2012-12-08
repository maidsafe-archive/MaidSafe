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
import sys
import time
import socket

# MaidSafe imports
import client_environment
import utils


def main():
  try:
  # Fork a child process so the parent can exit.  This returns control to
  # the command-line or shell.  It also guarantees that the child will not
  # be a process group leader, since the child receives a new process ID
  # and inherits the parent's process group ID.  This step is required
  # to insure that the next call to os.setsid is successful.
   pid = os.fork()
  except OSError, e:
    raise Exception, "%s [%d]" % (e.strerror, e.errno)

  if (pid == 0):  # The first child.
   # To become the session leader of this new session and the process group
   # leader of the new process group, we call os.setsid().  The process is
   # also guaranteed not to have a controlling terminal.
   os.setsid()
   result = client_environment.ParameterBasedStartup(None, None, None, None, None, "smer")
   print "client_environment.ParameterBasedStartup result:", result
  else:
    print "Wait 70 secs for network"
    time.sleep(70)
    print "Network should be ready"
    os._exit(os.EX_OK)
    # return 0

if __name__ == "__main__":
  sys.exit(main())
