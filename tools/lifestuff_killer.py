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

import platform
import sys

try:
  import psutil
except ImportError:
  print "please install psutil 'pip install psutil'"
  print "or 'easy_install psutil'"
  print "Website : http://code.google.com/p/psutil/"
  sys.exit(1)

PROC = "lifestuff_"

def KillLifeStuff():
  for proc in psutil.process_iter():
    if proc.name.find(PROC) >= 0:
      print "Killing process:", proc.name
      try:
        if platform.system() == "Windows":
          proc.terminate()
        else:
          proc.kill()
      except:
        print "Could not kill all instances"

def Exists():
  for proc in psutil.process_iter():
    if proc.name.find(PROC) >= 0:
      print "Failed to kill process:", proc.name
      return -1;
    print "No lifestuff processes running (now)"
    return 0

def RunKilling():
  KillLifeStuff()
  return Exists()

def main():
  return RunKilling()

if __name__ == "__main__":
  sys.exit(main())

