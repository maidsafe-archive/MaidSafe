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
import signal
from subprocess import PIPE, STDOUT
from multiprocessing import Process, Pool
import multiprocessing
import utils
import re
import psutil
import time
import datetime
import shutil
import lifestuff_killer

processes = []
stop_churn = 'd'


def SetupBootstraps(num, user_id):
  print("Setting up keys ... ")
  prog = utils.GetProg('vault_key_helper')
  print prog;
  proc = subprocess.Popen([prog, '-c', '-b', '-n', str(num + 6)],
                          shell = False, stdout = PIPE, stderr = None)
  print("Started bootstrap with PID " + str(proc.pid))
  i = 0
  line_limit = 50
  t_start = datetime.datetime.now()
  time_delta = datetime.datetime.now() - t_start
  timeout = 300000
  while i < line_limit and time_delta < datetime.timedelta(seconds=timeout):
    line = proc.stdout.readline()
    print line
    if line.find('Endpoints') != -1:
      data = re.split(r':', line)
      ep = data[2].split()
      if SetUpNextNode(data[1]+ ':' + ep[0], 2, user_id) and\
         SetUpNextNode(data[1]+ ':' + ep[0], 3, user_id):
        time.sleep(2) # allow node to bootstrap
        break
      else:
        proc.kill()
        return False
    i = i + 1
    time_delta = datetime.datetime.now() - t_start
  if i == line_limit or time_delta >= datetime.timedelta(seconds=timeout):
    print "Failed to get endpoint (timeout ??)"
    proc.kill()
    return False
  proc.kill()
  RunNetwork(num, data[1], user_id)
  print("Wait 2 secs for network")
  time.sleep(2)
  return True

def SaveKeys(peer):
  prog = utils.GetProg('vault_key_helper')
  return subprocess.call([prog, '-ls', '--peer=' + peer + ':5483'])

def ExtendedTest(num):
  prog = utils.GetProg('vault_key_helper')
  SaveKeys(utils.GetIp())
  subprocess.call([prog, '-lx', '--peer=' + utils.GetIp() + ':5483',
                  '--chunk_set_count=' + str(num)],
                  shell = False, stdout = None, stderr = None)
  raw_input("Press any key to continue")

def SetUpKeys(num):
  print("Setting up keys ... ")
  prog = utils.GetProg('vault_key_helper')
  CreateChunkStores(num)
  print prog;
  return subprocess.call([prog, '-c', '-n', str(num) ],
                         shell = False, stdout = None, stderr = None)

def CreateChunkStores(num):
  RemoveChunkStores(num)
  for dir_num in range(int(num)):
    directory = os.path.join(os.curdir, '.cs' + str(dir_num))
    if not os.path.exists(directory):
      os.makedirs(directory)

def RemoveChunkStores(num):
  for dir_num in range(int(num)):
    directory = os.path.join(os.curdir, '.cs' + str(dir_num))
    if os.path.exists(directory):
          shutil.rmtree(directory)

def work(number, ip_address, user_id):
  prog = utils.GetProg('lifestuff_vault')
  if user_id == None:
    return subprocess.Popen([prog, '--peer=' + ip_address.lstrip() + ':5483',
                            '--identity_index=' + str(number),
                            '--chunk_path=.cs' + str(number), '--start'],
                            shell = False, stdout = None, stderr = None)
  else:
    return subprocess.Popen([prog,
                            '--peer=' + ip_address.lstrip() + ':5483',
                            '--identity_index=' + str(number),
                            '--chunk_path=.cs' + str(number),
                            '--usr_id=' + user_id,
                            '--start'],
                            shell = False, stdout = None, stderr = None)

def RunNetwork(number_of_vaults, ip_address, user_id):
  for vault in range(4, number_of_vaults):
    processes.append(work(vault, ip_address, user_id))
    time.sleep(2)

def SignalHandler(signal, frame):
  print("Exiting churn ")
  global stop_churn
  stop_churn = 'q'

signal.signal(signal.SIGINT, SignalHandler)

def Churn(percent_per_minute):
  num_vaults = len(processes)
  per_second_churn = (num_vaults * (100 / percent_per_minute)) / 60
  print("Running churn test churn test at a rate of " + str(per_second_churn) + " per second")
  print("press Ctrl-C to stop")
  stopped = []
  global stop_churn
  while  stop_churn != 'q':
    time.sleep(per_second_churn)
#    for i in range(per_second_churn)
#      stopped.append(random.choice(processes))
#      if len(stopped) > 0:
#        work(stopped.pop(random.choice(stopped)), GetIp())
  stop_churn = 'g'

def SetUpNextNode(endpoint, index, user_id):
  prog = utils.GetProg('lifestuff_vault')
  if user_id == None:
    return subprocess.Popen([prog,
                            '--peer=' + endpoint.lstrip(),
                            '--identity_index=' + str(index),
                            '--chunk_path=.cs' + str(index),
                            '--start'],
                            shell = False, stdout = None, stderr = None)
  else:
    return subprocess.Popen([prog,
                            '--peer=' + endpoint.lstrip(),
                            '--identity_index=' + str(index),
                            '--chunk_path=.cs' + str(index),
                            '--usr_id=' + user_id,
                            '--start'],
                            shell = False, stdout = None, stderr = None)

def SanityCheck(num, user_id):
  pid = SetupBootstraps(num, user_id)
  if pid == False:
    print("Vault Sanity Check failed")
    return False
  else:
    print("Vault Sanity Check Passed")
    return True

def PrintVaultMenu():
  utils.ClearScreen()
  procs = utils.CountProcs('lifestuff_vault')
  print(str(procs) + " Vaults running on this machine")
  print ("================================")
  print ("MaidSafe Quality Assurance Suite | Vault Actions")
  print ("================================")
  print ("1: Bootstrap and set up vaults")
  if procs == 0:
    print ("2: Set up vaults only (bootstrap elsewhere)")
  else:
    print ("3: Run store test")
    print ("4: Kill all vaults on this machine")
    print ("5: Random churn on this machine, rate (%% churn per minute) (not yet implemented)")
  return procs

def RunBootstrapAndVaultSetup():
  num = 0
  while 12 > num:
    number = raw_input("Please input number of vaults to run (minimum and default 12): ")
    if number == "":
      num = 12
    elif not number.isdigit():
      continue
    else:
      num = int(number)
  RemoveChunkStores(num)
  SanityCheck(num + 2, None)
  # SaveKeys(utils.GetIp())

def ValidOption(procs, option):
  if not option.isdigit():

    return False
  if option == "2" and procs != 0:
    return False
  if int(option) > 2 and procs == 0:
    return False
  return True

def GetPositiveNumber(message):
  number = 0
  while number < 1:
    number = raw_input(message)
  return int(number)

def StartVaultsWithGivenBootstrap():
  number = GetPositiveNumber("Please input number of vaults to run: ")
  ip = raw_input("Please input ip address of bootstrap machine: ")
  prog = utils.GetProg('vault_key_helper')
  print prog
  CreateChunkStores(number)
  subprocess.call([prog, '-c', '-n', str(int(number) + 3)])
  if SaveKeys(ip) == 0:
    RunNetwork(int(number) + 3, ip, None)
  else:
    raw_input("Could not store keys, giving up! (press any key)")

def VaultMenu():
  option = 'a'
  utils.ClearScreen()
  while(option != 'm'):
    procs = PrintVaultMenu()
    option = raw_input("Please select an option (m for main QA menu): ").lower()
    if not ValidOption(procs, option):
      continue
    if option == "1":
      RunBootstrapAndVaultSetup()
    elif (option == "2"):
      StartVaultsWithGivenBootstrap()
    elif (option == "3"):
      number = GetPositiveNumber("Please input number of chunks in test: ")
      TestStore(number)
    elif (option == "4"):
      lifestuff_killer.KillLifeStuff()
    elif (option == "5"):
      pass
      print "Not yet implemented."

  utils.ClearScreen()

def main():
  print("This is the suite of QA anaysis info for vaults")
  VaultMenu()
if __name__ == "__main__":
  sys.exit(main())

