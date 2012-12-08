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
import platform
import subprocess
import socket
import sys
import tempfile
from optparse import OptionParser

# MaidSafe imports
import utils
import vault

ls_mgr_exe = utils.GetProg("lifestuff_mgr")
ls_vault_exe = utils.GetProg("lifestuff_vault")

def CheckPassedInIp(ip_address):
  try:
    socket.inet_aton(ip_address)
  except socket.error:
    print "Not a legal IP address"
    return -1

  if len(ip_address.split('.')) != 4:
    print "Not a fully formed IP address"
    return -1

  return 0

def CheckOptions(ip_address, ls_mgr_port, vault_count, config_path):
  start_network = 0
  if ip_address != None:
    result = CheckPassedInIp(ip_address)
    if result != 0:
      return {"result":result}
    else:
      start_network = -1

  if ls_mgr_port != None:
    if ls_mgr_port < 1024 and ls_mgr_port > 65535:
      print "Port outside of 1024-65535 range: ", ls_mgr_port
      return {"result":-1}

  if vault_count != None:
    if vault_count > 50 and vault_count < 10:
      print "Vault count for local network outside of 10-50 range: ", vault_count
      return {"result":-1}

  if vault_count != None and ip_address != None:
    print "Conflicting parameters -i and -n: Use only one or none."
    return {"result":-1}

  return {"result":0, "start_network":start_network}

def SetupNetwork(ip_address, vault_count, user_id):
  try:
    vault.RemoveChunkStores(vault_count)

    bool_result = vault.SanityCheck(vault_count + 2, user_id)
    if bool_result == False:
      print "Failed in SanityCheck"
      return -1

    peer = ""
    if ip_address != None:
      peer = ip_address
    else:
      peer = utils.GetIp()
    int_result = vault.SaveKeys(peer)
    if int_result != 0:
      print "Failed in SaveKeys"
      return -1

    return 0
  except Exception as e:
    print "Failed starting network: ", e
    return -1

def CheckExecutablesExist(cwd, full_ls_mgr_exe):
  if os.path.exists(full_ls_mgr_exe) != True:
    print "lifestuff_mgr not found in ", cwd
    return -1

  full_ls_vault_exe = os.path.join(cwd, ls_vault_exe)
  if os.path.exists(full_ls_vault_exe) != True:
    print "lifestuff_vault not found in ", cwd
    return -1

  return 0

def PopulateLifeStuffMgrParamters(cwd, parameter_list, ip_address, port, config_path, logging):
  if logging != None and logging != 0:
    parameter_list.append("--log_private")
    parameter_list.append("I")

  # lifestuff_vault executable
  parameter_list.append("--vault_path")
  parameter_list.append(cwd)

  # root of config path
  parameter_list.append("--root_dir")
  if config_path == None:
    parameter_list.append(tempfile.mkdtemp(prefix="Maidsafe_QA_Test"))
  else:
    parameter_list.append(config_path)

  # port
  if port != None:
    string_port = "" + port
    parameter_list.append("--port")
    parameter_list.append(string_port)

  # ip adress to give to components that register as bootstrap
  if ip_address != None:
    parameter_list.append("--bootstrap_ips")
    parameter_list.append(ip_address)

def StartLifeStuffMgr(ip_address, port, config_path, logging):
  cwd = os.getcwd()
  full_ls_mgr_exe = os.path.join(cwd, ls_mgr_exe)
  result = CheckExecutablesExist(cwd, full_ls_mgr_exe)
  if result != 0:
    return result

  parameter_list = [full_ls_mgr_exe]
  PopulateLifeStuffMgrParamters(cwd, parameter_list, ip_address, port, config_path, str(logging))
  # print parameter_list

  try:
    subprocess.Popen(parameter_list, shell=False, stdout=None, stderr=None)
    return 0
  except Exception as e:
    print "Failed executing lifestuff_mgr: ", e
    return -1

def ParameterBasedStartup(ip_address, ls_mgr_port, config_path, logging, vault_count, user_id):
  dictionary_result = CheckOptions(ip_address, ls_mgr_port, vault_count, config_path)
  if dictionary_result["result"] != 0:
    return dictionary_result["result"]

  if dictionary_result["start_network"] == 0:
    vault_count = 10 if vault_count == None else vault_count
    result = SetupNetwork(ip_address, vault_count, user_id)
    if result != 0:
      return result

  result = StartLifeStuffMgr(ip_address, ls_mgr_port, config_path, logging)
  if result != 0:
    return result

  return 0

def main():
  parser = OptionParser()
  # lifestuff_vault options
  parser.add_option("-n", "--vault_count", dest="vault_count", action="store", type="int",
                    help="number of vaults to start")
  if platform.system() != "Windows":
    parser.add_option("-u", "--user_id", dest="user_id", action="store", type="string",
                      help="user id to execute processes")

  # lifestuff_mgr options
  parser.add_option("-i", "--ip", dest="ip_address", action="store", type="string",
                    help="IP address of bootstrap node. If this option is not specified, a network will be set up.")
  parser.add_option("-p", "--ls_mgr_port", dest="ls_mgr_port", action="store", type="string",
                    help="Port to start lifestuff_mrg on")
  parser.add_option("-c", "--config_path", dest="config_path", action="store", type="int",
                    help="desired path for configuration file and vault chunkstores")
  parser.add_option("-l", "--logging", dest="logging", action="store", type="int",
                    help="0 for no logging, any other value for logging")
  (options, args) = parser.parse_args()

  # check options
  user_id = ""
  if platform.system() != "Windows":
    user_id = options.user_id

  print "Before ParameterBasedStartup"
  result = ParameterBasedStartup(options.ip_address,
                                 options.ls_mgr_port,
                                 options.config_path,
                                 options.logging,
                                 options.vault_count,
                                 user_id);
  print "After ParameterBasedStartup:", result
  if result != 0:
    return result

  print "About to return: ", result
  return 0

if __name__ == "__main__":
  sys.exit(main())

