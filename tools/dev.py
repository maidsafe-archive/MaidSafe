#!/usr/bin/env python
import os
import sys
import subprocess
from subprocess import Popen, PIPE, STDOUT
import multiprocessing
from time import sleep

all = { "Common" : 'common', "Rudp" : 'rudp', "Routing" : 'routing',
        "Private" : 'private', "Pd" : 'pd', "Encrypt" : 'encrypt',
        "Drive" : 'drive', "Lifestuff" : 'lifestuff' }

def CppLint():
  encoding = sys.getfilesystemencoding()
  script_path = os.path.dirname(unicode(__file__, encoding))
  return os.path.join(script_path, 'cpplint.py')

def ClearScreen():
  os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )

def GetLib():
  option = ''
  while option.lower() not in all.values() and option != 'q':
    print ("Libraries available")
    print ("-------------------")
    for key, value in all.iteritems():
      print (key)
    print ("-------------------")
    option = raw_input("please type library name (q to exit): ")
  return option

# style check 
def StyleCheck():
  option = GetLib()
  if option == 'q':
    ClearScreen()
    return
  directory = os.path.join(os.path.curdir, '..', 'src', option)
  style_check = ""
  for r,d,f in os.walk(directory):
    for files in f:
      if (files.endswith(".cc") or files.endswith(".h")) and not \
         (files.endswith(".pb.cc") or files.endswith(".pb.h")):
        style_check = os.path.join(r,files)
        subprocess.call(['python', CppLint() , style_check])

def RunAllExperimentals():
  for key, value in all.iteritems():
    if os.name == 'nt':
      subprocess.call(['cl', "Exper" + key + ".exe"])
    else:
      subprocess.call(['make', "Exper" + key])

def FindFile(name, path=None, exts=('','.exe')):
  path = path or os.environ['PATH']
  for dir in path.split(os.pathsep):
    for ext in exts:
      binpath = os.path.join(dir, name) + ext
      if os.path.exists(binpath):
        return os.path.abspath(binpath)
  return None

def CppCheck():
  checker = FindFile('cppcheck')
  if checker == None:
    return
  option = GetLib()
  if option == 'q':
    ClearScreen()
    return
  directory = os.path.join(os.path.curdir, '..', 'src', option)
  style_check = ""
  for r,d,f in os.walk(directory):
    for files in f:
      if (files.endswith(".cc") or files.endswith(".h")):
        file_to_check = os.path.join(r,files)
        subprocess.call([checker, file_to_check])
  return checker

def work(cmd):
    return subprocess.call(cmd, shell=False)

def RunNetwork(number_of_vaults):
  pool = multiprocessing.Pool(processes=number_of_vaults)
  pool.map(work, [FindFile('TESTcommon', os.curdir)] * number_of_vaults)

def RoutingSetupKeys(num):
  p = subprocess.check_output('./routing_key_helper -c -p -n' + str(num), shell = True)
  print "-----Keys helper output---------"
  print p
  print "-----END Keys helper output-----\n"


def RoutingSetupBootstrap():
  p_b0 = Popen('./routing_node -s -b -i 0', shell = True, stdout=PIPE, stdin=PIPE)
  p_b1 = Popen('./routing_node -s -b -i 1', shell = True, stdout=PIPE, stdin=PIPE)

  i = 0
  while i < 50:
    i = i + 1
    next_line = p_b0.stdout.readline()
    if next_line.find('Current BootStrap node endpoint info') != -1:
      peer_0 = next_line.split()[7]
      print('FOUND PEER', peer_0)
      break

  if i == 50:
    print "Failed to set up bootstrap 1!"
    return -1

  i = 0
  while i < 50:
    i = i + 1
    next_line = p_b1.stdout.readline()
    if next_line.find('Current BootStrap node endpoint info') != -1:
      peer_1 = next_line.split()[7]
      print('FOUND PEER', peer_1)
      break

  if i == 50:
    print "Failed to set up bootstrap 1!"
    return -1

  sleep(1)

  p_b0.stdin.write('peer ' + peer_1 + '\n')
  p_b1.stdin.write('peer ' + peer_0 + '\n')
  p_b0.stdin.write('zerostatejoin\n')
  p_b1.stdin.write('zerostatejoin\n')
  sleep(1)

  return [peer_0, p_b0, p_b1]


def Routing_JAV1():
  RoutingSetupKeys(20)
  items = RoutingSetupBootstrap()
  peer = items[0]
  p_b0 = items[1]
  p_b1 = items[2]

  p_v = Popen('./routing_node -s -i 2 -p ' + peer, shell = True, stdout=PIPE, stdin=PIPE, stderr=STDOUT)

  i = 0
  while i < 100:
    next_line = p_v.stdout.readline()
    print 'p_v\t' + next_line.rstrip()
    if next_line.find('Current Node joined') != -1:
      print('NODE JOINED')
      break
    i = i + 1

  if i == 100:
    print "Node 2 failed to join!"
    p_b0.stdin.write('exit\n')
    p_b1.stdin.write('exit\n')
    return -1

  p_v.stdin.write('exit\n')
  sleep(2)
  if p_v.poll() == None:
    print "Failed to stop node 2!"
  else:
    print "Node 2 stopped successfully"

  p_b0.stdin.write('exit\n')
  p_b1.stdin.write('exit\n')

def RunQaCheck():
  Routing_JAV1()
#  f = open('./temp_log.txt','w')
#  p = Popen('./routing_node -s -i 2 -p 192.168.0.131:14378', shell = True, stdout=PIPE, stdin=PIPE)
#  s = Popen('./routing_node -s -i 3 -p 192.168.0.131:14378', shell = True, stdout=PIPE, stdin=PIPE)
#  while True:
#    next_line = p.stdout.readline()
#    if not next_line:
#      break
#    print next_line.rstrip()

#  p.stdin.write('help\n')

#  while True:
#    next_line = p.stdout.readline()
#    if not next_line:
#      break
#    print next_line.rstrip()

#  p.stdin.write('exit\n')
#  s.stdin.write('exit\n')

#time.sleep(2)
#  p = Popen(['./routing_node', '-s -i 4 -p 192.168.0.129:61922'], shell=False, stdout=STDOUT, stdin=PIPE, stderr=STDOUT)
#  print(PIPE)

#  p.communicate(input='help\n')
#  print(grep_stdout)
# coverage (linux only)

# memcheck



# etc.

def main():
  option = 'a'
  ClearScreen()
  if FindFile('TESTcommon', os.curdir) == None:
    print("This does not look like a build directory, please run this from your build dir")
    print("or build the project targets before running this tool")
    return
  while(option != 'q'):
    print ("MaidSafe Development helper tool")
    print ("================================")
    print ("1: Style Check ")
    print ("2: All Experimentals ")
    if FindFile('cppcheck') == None:
      print ("No Cppcheck installed, please install cppcheck")
      print ("this will allow basic checks on c++ code")
    else:
      print ("3: cppcheck (found)")
    print ("4: Qa current state")
    option = raw_input("Please select an option (q to quit): ")
    if (option == "1"):
      StyleCheck()
    if (option == "2"):
      RunAllExperimentals()
    if (option == "3"):
      CppCheck()
    if (option == "4"):
      RunQaCheck()

if __name__ == "__main__":
      main()
