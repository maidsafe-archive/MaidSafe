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
import sys
import subprocess
import multiprocessing
import utils
import lifestuff_killer
import routing
import lifestuff
import vault

# all = { "Common" : 'common', "Rudp" : 'rudp', "Routing" : 'routing',
#         "Private" : 'private', "Pd" : 'pd', "Encrypt" : 'encrypt',
#         "Drive" : 'drive', "Lifestuff" : 'lifestuff' }

def CppLint():
  encoding = sys.getfilesystemencoding()
  script_path = os.path.dirname(unicode(__file__, encoding))
  return os.path.join(script_path, 'cpplint.py')

def StyleCheck():
  option = utils.GetLib()
  if option == 'q':
    utils.ClearScreen()
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
  for key, value in utils.all.iteritems():
    if os.name == 'nt':
      subprocess.call(['cl', "Exper" + key + ".exe"])
    else:
      subprocess.call(['make', "Exper" + key])

def CppCheck():
  checker = utils.FindFile('cppcheck')
  if checker == None:
    return
  option = utils.GetLib()
  if option == 'q':
    utils.ClearScreen()
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
  pool.map(work, [utils.FindFile('TESTcommon', os.curdir)] * number_of_vaults)


def RunQaCheck():
  print("Not yet implemented")
  RunNetwork(50)


def MainMenu():
  option = 'a'
  utils.ClearScreen()
  if utils.BuildType() == None:
    print ("Not a build dir please cd to build dir i.e. the location of CMakeCache.txt")
    return -1;
  while(option != 'q'):
    print ("MaidSafe Development helper tool")
    print ("Using " +  utils.BuildType() + " build")
    print ("================================")
    print ("1: Style Check ")
    print ("2: All Experimentals ")
    if utils.FindFile('cppcheck') == None:
      print ("No Cppcheck installed, please install cppcheck")
      print ("this will allow basic checks on c++ code")
    else:
      print ("3: cppcheck (found)")
    print ("4: Qa Menu")
    option = raw_input("Please select an option (q to quit): ")
    if (option == "1"):
      StyleCheck()
    if (option == "2"):
      RunAllExperimentals()
    if (option == "3"):
      CppCheck()
    if (option == "4"):
      QaMenu()


def QaMenu():
  option = 'a'
  utils.ClearScreen()
  while(option != 'm'):
    print ("MaidSafe Quality Assurance Suite")
    print ("================================")
    print ("1: Routing:   sanity check")
    print ("2: Vault:     feature checks")
    print ("3: Vault:     performance checks")
    print ("4: Client:    feature checks")
    print ("5: Client:    performance checks")
    print ("4: LifeStuff: feature checks (includes manager)")
    print ("5: LifeStuff: performance (includes manager)")
    option = raw_input("Please select an option (m for main menu): ")
    if (option == "1"):
      routing.SanityCheck()
    if (option == "2"):
      vault.SanityCheck(10)
  utils.ClearScreen()

def main():
  utils.ClearScreen()
  build_dir = utils.GetBuildDir()
  if build_dir == None:
    return -1;
  MainMenu()


if __name__ == "__main__":
      sys.exit(main())
