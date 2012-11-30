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
import socket
import sys
import tempfile
from optparse import OptionParser
from multiprocessing import Process, Pool

# MaidSafe imports
import utils
import vault

ls_mgr_exe = "lifestuff_mgr"

def CheckPassedInIp(ip_address):
  try:
    socket.inet_aton(ip_address)
  except socket.error:
    print("Not a legal IP address")
    return -1

  if len(ip_address.split('.')) != 4:
    print("Not a fully formed IP address")
    return -1

  return 0

def StartLifeStuffMgr(ip_address, port):
  cwd = os.getcwd()
  full_ls_mgr_exe = os.path.join(cwd, ls_mgr_exe)
  if os.path.exists(full_ls_mgr_exe) != True:
    print("lifestuff_mgr not found in ", cwd)
    return -1

  parameter_list = [full_ls_mgr_exe, "--log_private", "I"]
  parameter_list.append("--vault_path")
  parameter_list.append(cwd)
  parameter_list.append("--root_dir")
  parameter_list.append(tempfile.mkdtemp(prefix="Maidsafe_Qa_Test"))

  if port != None:
    string_port = "" + port
    parameter_list.append("--port")
    parameter_list.append(string_port)
  if ip_address != None:
    parameter_list.append("--bootstrap_ips")
    parameter_list.append(ip_address)
  print parameter_list
  try:
    subprocess.Popen(parameter_list, shell=False, stdout=None, stderr=None)
  except Exception as e:
    print("Failed executing lifestuff_mgr: ", e)
    return -1

  return 0

def CheckOptions(options):
  if options.ip_address != None:
    result = CheckPassedInIp(options.ip_address)
    if result != 0:
      return result

  if options.ls_mgr_port != None:
    if options.ls_mgr_port < 1024 and options.ls_mgr_port > 65535:
      print("Port outside of 1024-65535 range: ", options.ls_mgr_port)
      return -1

  if options.vault_count != None:
    if options.vault_count > 50 and options.vault_count < 10:
      print("Vault count for local network outside of 10-50 range: ", options.vault_count)
      return -1

  return 0

def SetupNetwork(vault_count):
  try:
    vault.RemoveChunkStores(vault_count)
    
    bool_result = vault.SanityCheck(vault_count + 2)
    if bool_result == False:
      print("Failed in SanityCheck")
      return -1

    int_result = vault.SaveKeys()
    if int_result != 0:
      print("Failed in SaveKeys")
      return -1
  except Exception as e:
    print("Failed starting network: ", e)
    return -1

  # print "Looks like sweet success"
  return 0

def main():
  parser = OptionParser()
  parser.add_option("-i", "--ip", dest="ip_address", action="store", type="string",
                    help="IP address of network node")
  parser.add_option("-p", "--ls_mgr_port", dest="ls_mgr_port", action="store", type="string",
                    help="Port to start lifestuff_mrg on")
  parser.add_option("-n", "--vault_count", dest="vault_count", action="store", type="int",
                    help="number of vaults to start")
  (options, args) = parser.parse_args()
  # print options
 
  # check options
  result = CheckOptions(options);
  if result != 0:
    return result
  
  if options.ip_address == None:
    vault_count = 10 if options.vault_count == None else options.vault_count
    result = SetupNetwork(vault_count)
    if result != 0:
      return result

  result = StartLifeStuffMgr(options.ip_address, options.ls_mgr_port)
  if result != 0:
    return result

  return 0

if __name__ == "__main__":
  sys.exit(main())

