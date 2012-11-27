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
import datetime
import subprocess
import utils
import random
from subprocess import Popen, PIPE, STDOUT
from time import sleep


def SetupKeys(num):
  print("Setting up keys ... ")
  return subprocess.call(['./routing_key_helper', '-c', '-p', '-n', str(num)], stdout=PIPE)


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
  print("Setup boostrap nodes ...")
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

def AddTypedRoutingObject(peer, index, node_type):
  if node_type == 'Client':
    node_type = '-c'
  p_v = Popen('./routing_node -s ' + node_type + ' -i ' + str(index) + ' -p ' + peer,
              shell = True, stdout=PIPE, stdin=PIPE, stderr=STDOUT)

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

#  print 'added node # ' + str(index)
  return p_v


def AddRoutingObjects(peer, start, quantity, node_type):
   p_s = []
   for i in range(start, start + quantity):
     result = AddTypedRoutingObject(peer, i, node_type)
     if result == -1:
       return -1
     p_s.append(result)

   return p_s

def StopRoutingObjects(p_vs):
  for p in p_vs:
    p.stdin.write('exit\n')
    sleep(2)
    if p.poll() == None:
      print "Failed to stop node!"

def SendToDirect(p_c, index):
  print 'Sending to ...'
  p_c.stdin.write('senddirect ' + str(index) + '\n')
  i = 0
  line_limit = 20
  t_start = datetime.datetime.now()
  time_delta = datetime.datetime.now() - t_start
  timeout = 20
  while i < line_limit and time_delta < datetime.timedelta(seconds=timeout):
    next_line = p_c.stdout.readline()
    if next_line.find('Response received in') != -1:
      if ((next_line.split(' ')[-1]).split(':')[2]).find('20.') != -1:
        return -1
      return 0
    i = i + 1
    time_delta = datetime.datetime.now() - t_start

  return -1

def CheckClientNodeIsNotInRoutingTable(p_c, to_index):
  for index in range(0, to_index):
    p_c.stdin.write('rrt ' + str(index) + '\n')
    sleep(1)
  p_c.stdin.write('help\n')
  node_id = ''
  count = 0
  stop = False
  while stop == False:
    next_line = p_c.stdout.readline()
    if next_line.find('Sending a msg from ') != -1:
      node_id = ((next_line.split(':')[1]).split(' ')[1])
 #     print 'node_id ' + node_id
      count = count -1
    if node_id != '':
      if next_line.find(node_id) != -1:
        count = count + 1
    if next_line.find('exit Exit application.') != -1:
      stop = True
  return count

def SendGroup(p, target):
#  print 'Send group message to ' + str(target)
  p.stdin.write('sendgroup ' + str(target) + '\n')
  sleep(1)
  p.stdin.write('help\n')
  stop = False
  while stop == False:
    next_line = p.stdout.readline()
    if next_line.find('Failure') != -1:
      return -1
    if next_line.find('exit Exit application.') != -1:
      stop = True
  return 0

def JAV1():
  print 'Testing JAV1'
  if not SetupKeys(20) == 0:
    return -1
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
  print 'PASSED'
  return 0

def JAC1():
  print 'Tesing JAC1'
  if not SetupKeys(20) == 0:
    return -1
  items = SetupBootstraps()
  if items == -1:
    return -1
  peer = items[0]
  p_b0 = items[1]
  p_b1 = items[2]

  p_vs = AddRoutingObjects(peer, 2, 6, '')
  if p_vs == -1:
    print 'At lease one node among 6 nodes failed to start'
    StopBootstrap(p_b0, p_b1)
    return -1

  p_c = AddTypedRoutingObject(peer, 11, 'Client')
  if p_c == -1:
    print 'Failed to start client node'
    StopBootstraps(p_b0, p_b1)
    StopRoutingNodes(p_vs)
    return -1
  p_vs.append(p_c)

  if SendToDirect(p_c, 11) != -1:
#    print 'Client failed to send to self'
    StopBootstraps(p_b0, p_b1)
    StopRoutingObjects(p_vs)
    return -1

  if SendToDirect(p_c, 5) == -1:
    print 'Client failed to send to vault'
    StopBootstraps(p_b0, p_b1)
    StopRoutingObjects(p_vs)
    return -1

  if CheckClientNodeIsNotInRoutingTable(p_c, 6) != 0:
    StopBootstraps(p_b0, p_b1)
    StopRoutingObjects(p_vs)
    return -1

  print ''
  print 'Removing nodes ...'
  StopBootstraps(p_b0, p_b1)
  StopRoutingObjects(p_vs)
  print 'PASSED'
  return 0

def JAC2():
  print 'Testing JAC2'
  if not SetupKeys(20) == 0:
    return -1
  items = SetupBootstraps()
  if items == -1:
    return -1
  peer = items[0]
  p_b0 = items[1]
  p_b1 = items[2]

  p_vs = AddRoutingObjects(peer, 2, 6, '')
  if p_vs == -1:
    print 'At lease one node among 6 nodes failed to start'
    StopBootstrap(p_b0, p_b1)
    return -1

  p_cs = AddRoutingObjects(peer, 11, 6, 'Client')
  if p_cs == -1:
    print 'At lease one node among 6 nodes failed to start'
    StopBootstraps(p_b0, p_b1)
    StopRoutingNodes(p_vs)
    return -1
  p_vs = p_vs + p_cs

  p_v = AddTypedRoutingObject(peer, 8, '')
  if p_v == -1:
    print 'Failed to join the network'
    StopBootstraps(p_b0, p_b1)
    StopRoutingObjects(p_vs)
    return -1

  p_vs.append(p_v)
  print 'Validate routing table',
  for p_c in p_cs:
    print '...',
    if CheckClientNodeIsNotInRoutingTable(p_c, 8) != 0:
      print "Routing tables are not valid"
      StopBootstraps(p_b0, p_b1)
      StopRoutingObjects(p_vs)
      return -1
  print ''
  print 'Removing nodes ...'
  StopBootstraps(p_b0, p_b1)
  StopRoutingObjects(p_vs)
  print 'PASSED'
  return 0

def  SGM1():
  print 'Testing SGM1'
  if not SetupKeys(20) == 0:
    return -1
  items = SetupBootstraps()
  if items == -1:
    return -1
  peer = items[0]
  p_b0 = items[1]
  p_b1 = items[2]

  p_vs = items[1:3]
  p_vs = p_vs + AddRoutingObjects(peer, 2, 6, '')
  if p_vs == -1:
    print 'At lease one node among 6 nodes failed to start'
    StopRoutingObjects(p_vs)
    return -1

  p_cs = AddRoutingObjects(peer, 11, 6, 'Client')
  if p_cs == -1:
    print 'At lease one node among 6 nodes failed to start'
#    StopBootstraps(p_b0, p_b1)
    StopRoutingObjects(p_vs)
    return -1
  p_vs = p_vs + p_cs

  print 'Send group messages to self [',
  for i in range(1, 20):
    rnd = random.randint(0, 13)
    target = rnd;
    if rnd > 7:
      target = rnd + 3
#    print 'rnd, dest: ' + str(rnd) + ', ' + str(target)
    print str(target),
    if SendGroup(p_vs[rnd], target) != 0:
      print "Failed to send group message"
#      StopBootstraps(p_b0, p_b1)
      StopRoutingNodes(p_vs)
      return -1
  print ']'

  print 'Send group messages to random destinations from [',
  for i in range(1, 20):
    rnd = random.randint(0, 13)
    target = random.randint(0, 19)
    if rnd > 7:
      print str(rnd + 3),
    else:
      print str(rnd),
    if SendGroup(p_vs[rnd], target) != 0:
      print "Failed to send group message"
#      StopBootstraps(p_b0, p_b1)
      StopRoutingNodes(p_vs)
      return -1
  print ']'

  StopRoutingObjects(p_vs)
  return 0


def SanityCheck():
  print("Running Routing Sanity Check, please wait ....")
  if (JAC1() == 0) and (JAV1() == 0) and (JAC2() == 0) and (SGM1() == 0):
    print 'Routing Sanity Check    : PASSED'
  else:
    print 'Routing Sanity Check    : FAILED'

def main():
  print("This is the suite for lifestuff Qa analysis")
  SanityCheck()

if __name__ == "__main__":
  sys.exit(main())


