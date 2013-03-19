#!/usr/bin/env python

import sys

key_exe_name = 'pd_key_helper'
vault_exe_name = 'lifestuff_vault'
port_number = 5483

def CheckExecutablesPresence():
  # Check key_exe_name and vault_exe_name are in current dir (don't use curdir)
  print('Not found in current directory:', key_exe_name)
  print('Not found in current directory:', vault_exe_name)
  return 0 #failure
  #return 1 #success


def CheckPortIsFree():
  # Check port_number is available (get own IP?)
  print('Port not available:', port_number)
  return 0
  #return 1 #success

def CheckNoBootstrapFile():
  # Check no file called 'bootstrap' is in current dir
  print('Found file \'bootstrap\' in current directory. Please delete it and try again.')
  return 0
  #return 1 # success

def CheckNoKeysFile():
  # Check no file called '??? - TODO' is in current dir
  print('Found file \'????\' in current directory. Please delete it and try again.')
  return 0
  #return 1 # success

def CheckNoChunkStoreFiles():
  # Check no file called 'cs*' is in current dir
  print('Found file \'cs???\' in current directory. Please delete it and try again.')
  return 0
  #return 1 # success

def PerformInitialChecks():
  print('Performing initial checks...')
  if CheckExecutablesPresence() == 0:
    return 0
  result = CheckPortIsFree()
  if CheckPortIsFree() == 0:
    return 0
  if CheckNoBootstrapFile() == 0:
    return 0
  if CheckNoKeysFile() == 0:
    return 0
  if CheckNoChunkStoreFile() == 0:
    return 0
  return 1


def GenerateKeysStartBootstraps(num_key_pairs):
  # spawn key_exe; write output to file
  # wait (time?) for file output to indicate bootstraps are running
  key_bootstrap_wait = 10
  print('Failed to generate keys and start bootstraps in %d seconds' % key_bootstrap_wait)
  return 0 #failure
  #return 1 # success
  
  
def StartFirstTwoVaults():
  # touch bootstrap
  return


def StopBootstraps():
  # interrupt key_exe (Ctrl+C)
  return











def StartNetwork(size):
  if size < 12:
    size = 50
  if PerformInitialChecks() == 0:
    print('Failed initial checks. Aborting network setup.')
    return
  
  print('Starting network of size', str(size))


def CleanUp():
  # remove files we've created
  return


def main():
  if len(sys.argv) > 1:
    StartNetwork(sys.argv[1])
  else:
    StartNetwork(50)

if __name__ == "__main__":
  main()

