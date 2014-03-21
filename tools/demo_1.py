#!/usr/bin/env python
# Copyright (c) 2014 maidsafe.net limited
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
import signal
from subprocess import PIPE, STDOUT
from multiprocessing import Process, Pool
import multiprocessing
import utils
import re
import psutil
import time
import datetime
import random
import shutil
import lifestuff_killer

processes = {}


def SetupBootstraps(num):
  print("Setting up keys ... ")
  prog = utils.GetProg('vault_key_helper')
  proc = subprocess.Popen([prog, '-c', '-b', '-n', str(num + 6)],
                          shell = False, stdout = PIPE, stderr = None)
  print("Started bootstrap with PID " + str(proc.pid))
  i = 0
  line_limit = 50
  t_start = datetime.datetime.now()
  time_delta = datetime.datetime.now() - t_start
  timeout = 300000
  while i < line_limit and time_delta < datetime.timedelta(seconds=timeout):
    line = str(proc.stdout.readline())
    print(line)
    if line.find("Endpoints") != -1:
      data = re.split(r':', line)
      ep = data[2].split()
      processes[2] = SetUpNextNode(data[1] + ':' + ep[0], 2)
      time.sleep(1)
      processes[3] = work(3)
      if processes[2] and processes[3]:
        print("Wait 5 secs for bootstrap")
        time.sleep(5)
        break
      else:
        proc.kill()
        return False
    i = i + 1
    time_delta = datetime.datetime.now() - t_start
  if i == line_limit or time_delta >= datetime.timedelta(seconds=timeout):
    proc.kill()
    return False
  proc.kill()
  lifestuff_killer.KillVaultKeyHelper()
  print("Wait 10 secs for bootstrap nodes disappear from routingtable")
  time.sleep(10)
  return True


def SetUpNextNode(endpoint, index):
  prog = utils.GetProg('vault')
  log_file = open('vault_' + str(index) + '.txt', 'w')
  return subprocess.Popen([prog, '--log_no_async', 'true',
                          '--log_vault', 'V', '--log_nfs', 'V', '--log_*', 'E',
                          '--peer=' + endpoint.lstrip(),
                          '--disable_ctrl_c=true',
                          '--identity_index=' + str(index),
                          '--chunk_path=.cs' + str(index)],
                          shell = False, stdout = log_file, stderr = log_file)


def work(number):
  prog = utils.GetProg('vault')
  log_file = open('vault_' + str(number) + '.txt', 'w')
  return subprocess.Popen([prog, '--log_no_async', 'true',
                          '--log_vault', 'V', '--log_nfs', 'V', '--log_*', 'E',
                          '--disable_ctrl_c=true',
                          '--identity_index=' + str(number),
                          '--chunk_path=.cs' + str(number)],
                          shell = False, stdout = log_file, stderr = log_file)


def RemoveChunkStores(num):
  for dir_num in range(int(num)):
    directory = os.path.join(os.curdir, '.cs' + str(dir_num))
    if os.path.exists(directory):
          shutil.rmtree(directory)


def RunBootstrapAndVaultSetup():
  num = 38
  RemoveChunkStores(num)
  pid = SetupBootstraps(num)
  if pid == False:
    print("Bootstrap setup failed")
    return False
  else:
    print("Bootstrap setup succeeded")

  for vault in range(4, num):
    processes[vault]= work(vault)
    print("Vault " + str(vault) + " is starting up ... ")
    time.sleep(1)
  print("Wait 5 secs for network")
  time.sleep(5)
  return True

def SaveKeys():
  num_of_keys = 44
  prog = utils.GetProg('vault_key_helper')
  proc = subprocess.Popen([prog, '-ls', '-k', str(10)],
                          shell = False, stdout = PIPE, stderr = None)
  if utils.TimeOut(utils.LookingFor, (proc, 'PublicPmidKey stored and verified', 50, num_of_keys,),
                   timeout_duration=5*num_of_keys, default=False):
    print("keys successfully stored to network")
    result = 0
  else:
    print("failure in storing keys to network")
  proc.kill
  lifestuff_killer.KillVaultKeyHelper()

def PrintVaultMenu():
  utils.ClearScreen()
  procs = utils.CountProcs('vault')
  print(str(procs) + " Vaults running on this machine")
  print ("================================")
  return procs


def VaultMenu():
  option = 1
  utils.ResetScreen()
  RunBootstrapAndVaultSetup()
  SaveKeys()
  while(option != 0):
    procs = PrintVaultMenu()
    option = input("Vault Network with 36 nodes established up (0 for quit): ")
    option = int(option)
  lifestuff_killer.KillLifeStuff()
  processes.clear()

def main():
  print("This is the suite of demo for vaults")
  VaultMenu()
if __name__ == "__main__":
  sys.exit(main())

