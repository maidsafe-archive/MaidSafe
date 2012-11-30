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

import utils
from time import sleep

def DoOption1():
  PrintStuff("You just selected option 1!")


def DoOption2():
  PrintStuff("This is option 2!")


def DoOption3():
  PrintStuff("You just selected option 3!")


def DoOption4():
  PrintStuff("This is option 4!")


def DoOption5():
  PrintStuff("You just selected option 5!")


def DoOption6():
  PrintStuff("This is option 6!")


def DoOption7():
  PrintStuff("You just selected option 7!")


def DoOption8():
  PrintStuff("This is option 8!")


def PrintStuff(string):
  print string
  sleep(2)


def ClientMenu():
  option = 'a'
  utils.ClearScreen()
  while(option != 'm'):
    utils.ClearScreen()
    print ("MaidSafe Quality Assurance Suite | Client Actions")
    print ("================================")
    print "1: Create user local network (if vaults running > 12)"
    print "2: Create User (to connect to a net supplied via IP address)"
    print "3: Login change password logout / login"
    print "4: Login change pin logout / login"
    print "5: Login change username logout / login"
    print "6: Create public name"
    print "7: Login change public logout / login"
    print "8: login multiple instances (supply number), check only last one exists"
    option = raw_input("Please select an option (m for main Qa menu): ")
    if option == "1":
      DoOption1()
    elif option == "2":
      DoOption2()
    elif option == "3":
      DoOption3()
    elif option == "4":
      DoOption4()
    elif option == "5":
      DoOption5()
    elif option == "6":
      DoOption6()
    elif option == "7":
      DoOption7()
    elif option == "8":
      DoOption8()
    else:
      PrintStuff("That's not a valid option.")
  utils.ClearScreen()


def main():
  print("This is the suite of Qa anaysis info for clients")
  ClientMenu()
if __name__ == "__main__":
  sys.exit(main())
  
