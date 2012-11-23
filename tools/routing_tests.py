#!/usr/bin/env python
import datetime
import subprocess
from subprocess import Popen, PIPE, STDOUT
from time import sleep


def SetupKeys(num):
  p = subprocess.check_output('./routing_key_helper -c -p -n' + str(num), shell = True)
#  print "-----Keys helper output---------"
#  print p
#  print "-----END Keys helper output-----\n"


def ExtractNodeEndpoint(p_b):
  i = 0
  line_limit = 50
  t_start = datetime.datetime.now()
  time_delta = datetime.datetime.now() - t_start
  timeout = 30
  while i < line_limit and time_delta < datetime.timedelta(seconds=timeout):
    next_line = p_b.stdout.readline()
    if next_line.find('Current BootStrap node endpoint info') != -1:
      peer = next_line.split()[7]
      return peer
    i = i + 1
    time_delta = datetime.datetime.now() - t_start

  if i == line_limit or time_delta >= datetime.timedelta(seconds=timeout):
    print "Failed to get endpoint"
    return -1


def SetupBootstraps():
  p_b0 = Popen('./routing_node -s -b -i 0', shell = True, stdout=PIPE, stdin=PIPE)
  p_b1 = Popen('./routing_node -s -b -i 1', shell = True, stdout=PIPE, stdin=PIPE)

  peer_0 = ExtractNodeEndpoint(p_b0)
  if peer_0 == -1:
    KillBootstraps(p_b0, p_b1)
    return -1

  peer_1 = ExtractNodeEndpoint(p_b1)
  if peer_1 == -1:
    KillBootstraps(p_b0, p_b1)
    return -1
    
  sleep(1)

  p_b0.stdin.write('peer ' + peer_1 + '\n')
  p_b1.stdin.write('peer ' + peer_0 + '\n')

  p_b0.stdin.write('zerostatejoin\n')
  p_b1.stdin.write('zerostatejoin\n')
  sleep(1)

  return [peer_0, p_b0, p_b1]


def StopBootstraps(p_b0, p_b1):
  p_b0.stdin.write('exit\n')
  p_b1.stdin.write('exit\n')


def KillBootstraps(p_b0, p_b1):
  # TODO - needs to kill all 4 processes. Currently only kills 2.
  p_b0.kill()
  p_b1.kill()
  
  
def AddRoutingObject(peer):
  p_v = Popen('./routing_node -s -i 2 -p ' + peer, shell = True, stdout=PIPE, stdin=PIPE, stderr=STDOUT)

  i = 0
  line_limit = 100
  t_start = datetime.datetime.now()
  time_delta = datetime.datetime.now() - t_start
  timeout = 3
  while i < line_limit and time_delta < datetime.timedelta(seconds=timeout):
    next_line = p_v.stdout.readline()
#    print 'p_v\t' + next_line.rstrip()
    if next_line.find('Current Node joined') != -1:
      break
    i = i + 1
    time_delta = datetime.datetime.now() - t_start

  if i == line_limit or time_delta >= datetime.timedelta(seconds=timeout):
    return -1
    
  return p_v
  

def JAV1():
  SetupKeys(20)
  items = SetupBootstraps()
  if items == -1:
    return -1
  peer = items[0]
  p_b0 = items[1]
  p_b1 = items[2]

  p_v = AddRoutingObject(peer)
  if p_v == -1:
    print 'Failed to add routing object!'
    StopBootstraps(p_b0, p_b1)
    return -1

  p_v.stdin.write('exit\n')
  sleep(2)
  if p_v.poll() == None:
    print "Failed to stop node 2!"
    StopBootstraps(p_b0, p_b1)
    return -1

  StopBootstraps(p_b0, p_b1)
  return 0


def main():
  if JAV1() == 0:
    print 'TEST JAV1    : PASSED'
  else:
    print 'TEST JAV1    : FAILED'


if __name__ == "__main__":
  main()
  
