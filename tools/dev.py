#!/usr/bin/env python
import os
import subprocess

all = { "Common" : 'common', "Rudp" : 'rudp', "Routing" : 'routing',
        "Private" : 'private', "Pd" : 'pd', "Encrypt" : 'encrypt',
        "Drive" : 'drive', "Lifestuff" : 'lifestuff' }

# style check 
def StyleCheck():
  print ("Libraries available")
  print ("-------------------")
  for library in all:
    print (all[library])
  print ("-------------------")
  option = raw_input("please type library name to check: ")
  if option not in all.values():
    print("Invalid choice please choose again")
    StyleCheck()
  directory = os.path.join(os.path.curdir, '..', 'src', all[library])
  for r,d,f in os.walk(directory):
    for files in f:
      if (files.endswith(".cc") or files.endswith(".h")) and not \
         (files.endswith(".pb.cc") or files.endswith(".pb.h")):
        style_check = os.path.join(r,files)
        subprocess.Popen(['python', 'cpplint.py', style_check])


# coverage (linux only)

# memcheck

# etc.

def main():
  option = 'a'
  os.system( [ 'clear', 'cls' ][ os.name == 'nt' ] )
  while(option != 'q'):
    print ("MaidSafe Development helper tool")
    print ("================================")
    print ("1: Style Check ")
    option = raw_input("Please select an option (q to quit): ")
    if (option == "1"):
      StyleCheck()

if __name__ == "__main__":
      main()
