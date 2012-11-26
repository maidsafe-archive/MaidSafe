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
import os
import subprocess
from subprocess import PIPE, STDOUT
import utils
import re
import datetime
import shutil

def SetupBootstraps(num):
  print("Setting up keys ... ")
  prog = utils.GetProg('pd_key_helper')
  print prog;
  proc = subprocess.Popen([prog, '-c', '-b', '-n', str(num)], shell = False, stdout=PIPE)
  print("Started bootstrap with PID " + str(proc.pid))
  i = 0
  line_limit = 50
  t_start = datetime.datetime.now()
  time_delta = datetime.datetime.now() - t_start
  timeout = 30
  while i < line_limit and time_delta < datetime.timedelta(seconds=timeout):
    line = proc.stdout.readline()
    print line
    if line.find('Endpoints') != -1:
      data = re.split(r':', line)
      ep = data[2].split()
      boostrap_endpoint1 = ep[0]
      boostrap_endpoint2 = data[2]
      if SetUpNextNode(data[1]+ ':' + ep[0]):
        break
      else:
        return False
    i = i + 1
    time_delta = datetime.datetime.now() - t_start
  if i == line_limit or time_delta >= datetime.timedelta(seconds=timeout):
    print "Failed to get endpoint (timeout ??)"
    return False
  return True

def CreateChunkStores(num):
  for dir_num in range(num):
    directory = os.path.join(os.curdir, 'cs', str(dir_num))
    if not os.path.exists(directory):
          os.makedirs(directory)

def RemoveChunkStores(num):
  for dir_num in range(num):
    directory = os.path.join(os.curdir, 'cs', str(dir_num))
    if os.path.exists(directory):
          sh.rmtree(directory)

def work(cmd):
    return subprocess.call(cmd, shell=False)

def RunNetwork(number_of_vaults):
  pool = multiprocessing.Pool(processes=number_of_vaults)
  pool.map(work, [utils.FindFile('TESTcommon', os.curdir)] * number_of_vaults)


def SetUpNextNode(endpoint):
  prog = utils.GetProg('lifestuff_vault')
  print(prog, '--peer=' + endpoint.lstrip(), '--identity_index=2', '--chunk_path=.cs2')
  return subprocess.Popen([prog, '--peer=' + endpoint.lstrip(), '--identity_index=2', '--chunk_path=.cs2', '--start'])


def SetUpRemainingNodes():
    print("todo")

def SanityCheck(num):
  pid = SetupBootstraps(10)
  if pid == False:
    print("Vault Sanity Check failed")
    return False
  else:
    print("Vault Sanity Check Passed")
    return True

def main():
  print("This is the suite of Qa anaysis info for vaults")

if __name__ == "__main__":
  sys.exit(main())

