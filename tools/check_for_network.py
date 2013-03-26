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

import sys
import socket

# MaidSafe imports
import utils

def CheckLiveUdpIsOpen():
  s = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
  host = utils.GetIp()
  try:
   # s.connect((host, 5483))
   s.bind((host, 5483))
   s.shutdown(2)
   return -1
  except:
   return 0

def RunCheckNetwork():
  count = utils.CountProcs("lifestuff_vault")
  if count < 10:
    print "Not enough vaults:", count
    return -1

  count = utils.CountProcs("lifestuff_mgr")
  if count != 1:
    print "Wrong number of lifestuff_mgr running:", count
    return -1

  if CheckLiveUdpIsOpen() != 0:
    print "UDP port on LIVE(5483) is available. Probably means no bootstrap node."
    return -1

  return 0

def main():
  return RunCheckNetwork()

if __name__ == "__main__":
  sys.exit(main())
