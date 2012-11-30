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
from lifestuff_python_api import *
import random
import string

kSuccess = 0

kChangePassword = 3
kChangePin = 4
kChangeKeyword = 5
kCreatePublicId = 6

#-----Helpers-----

def GenerateWord():
  length = random.randrange(5, 31)
  return ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for x in range(length))

def GeneratePin():
  pin = '0000'
  while pin == '0000':
    pin = ''.join(random.choice(string.digits) for x in range(4))
  return pin


def GeneratePublicId():
  #TODO: include spaces?
  length = random.randrange(1, 31)
  return ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for x in range(length))


def PrintStuff(string):
  print string
  sleep(2)


def DoFullCreateUser(life_stuff, keyword, pin, password):
  result = life_stuff.CreateUser(keyword, pin, password)
  if result != kSuccess:
    print "Failed to create user."
    return result
  print "Created user successfully."

  result = life_stuff.MountDrive()
  if result != kSuccess:
    print "Failed to mount drive."
    return result
  print "Mounted drive successfully."
  return kSuccess


def DoFullLogIn(life_stuff, keyword, pin, password, public_id=''):
  result = life_stuff.LogIn(keyword, pin, password)
  if result != kSuccess:
    print "Failed to log in."
    return result
  print "Logged in successfully."

  result = life_stuff.MountDrive()
  if result != kSuccess:
    print "Failed to mount drive."
    return result
  print "Mounted drive successfully."

  if public_id != '':
    result = life_stuff.StartMessagesAndIntros()
    if result != kSuccess:
      print "Failed to start messages and intros."
      return result
    print "Started messages and intros successfully."

  return kSuccess



def DoFullLogOut(life_stuff, public_id = ''):
  if public_id != '':
    result = life_stuff.StopMessagesAndIntros()
    if result != kSuccess:
      print "Failed to stop messages and intros."
      return result
    print "Stopped messages and intros successfully."

  result = life_stuff.UnMountDrive()
  if result != kSuccess:
    print "Failed to unmount drive."
    return result
  print "Unmounted drive successfully."

  result = life_stuff.LogOut()
  if result != kSuccess:
    print "Failed to log out."
    return result
  print "Logged out successfully."

  return kSuccess


def CheckLogInLogOut(keyword, pin, password):
  print "Checking LogIn and Logout..."
  life_stuff = LifeStuff({}, "")
  result = DoFullLogIn(life_stuff, keyword, pin, password)
  if result != 0:
    print "Failed to log in with result: " + str(result)
    return result

  result = DoFullLogOut(life_stuff)
  if result != 0:
    print "Failed to log out with result: " + str(result)
    return result

  return kSuccess



#-----Options-----

def CreateUserLocal(keyword, pin, password):
  print "Creating user..."
  life_stuff = LifeStuff({}, "")
  result = DoFullCreateUser(life_stuff, keyword, pin, password)
  if result != 0:
    print "Failed to create user with result: " + str(result)
    return result

  result = DoFullLogOut(life_stuff)
  if result != 0:
    print "Failed to log out with result: " + str(result)
    return result
  return kSuccess


def CreateUserIp():
  print "Sorry, joining a network by providing an IP address isn't implemented yet."
  raw_input("Press any key to continue.")
  #peer = raw_input("Please enter IP address: ")


def TestChangePassword(keyword, pin, password):
  return TestCreateOrChangeCredential(keyword, pin, password, kChangePassword)


def TestChangePin(keyword, pin, password):
  return TestCreateOrChangeCredential(keyword, pin, password, kChangePin)


def TestChangeKeyword(keyword, pin, password):
  return TestCreateOrChangeCredential(keyword, pin, password, kChangeKeyword)


def TestCreatePublicId(keyword, pin, password, public_id):
  return TestCreateOrChangeCredential(keyword, pin, password, kCreatePublicId, public_id)


def TestChangePublicId():
  print "Sorry, changing Public IDs isn't implemented yet."
  raw_input("Press any key to continue.")
#  return TestCreateOrChangeCredential(keyword, pin, password, kChangePublicId)

def TestCreateOrChangeCredential(keyword, pin, password, action, public_id=''):
  if action != kChangePassword and action != kChangePin and action != kChangeKeyword and action != kCreatePublicId:
    print "Action is not recognised."
    return -1

  print "Logging in..."
  life_stuff = LifeStuff({}, "")
  result = DoFullLogIn(life_stuff, keyword, pin, password)
  if result != 0:
    print "Failed to log in with result: " + str(result)
    return result

  if action == kChangePassword:
    new_password = GenerateWord()
    result = life_stuff.ChangePassword(new_password, password)
    if result != 0:
      print "Failed to change password with result: " + str(result)
      return result
    password = new_password
  elif action == kChangePin:
    new_pin = GeneratePin()
    result = life_stuff.ChangePin(new_pin, password)
    if result != 0:
      print "Failed to change pin with result: " + str(result)
      return result
    pin = new_pin
  elif action == kChangeKeyword:
    new_keyword = GenerateWord()
    result = life_stuff.ChangeKeyword(new_keyword, password)
    if result != 0:
      print "Failed to change keyword with result: " + str(result)
      return result
    keyword = new_keyword
  elif action == kCreatePublicId:
    if public_id == '':
      print "Can't create empty public ID!"
      return -1
    result = life_stuff.CreatePublicId(public_id)
    if result != 0:
      print "Failed to create public id with result: " + str(result)
      return result

  result = DoFullLogOut(life_stuff)
  if result != 0:
    print "Failed to log out with result: " + str(result)
    return result

  result = CheckLogInLogout(life_stuff, keyword, pin, password)
  if result != 0:
    print "Failed to log in and out with result: " + str(result)
    return result

  return kSuccess

def TestMultipleInstances():
  num_instances = raw_input("Please enter the number of instances to run: ")
  #TODO


def ShowCredentials(keyword, pin, password, public_id):
  print "Keyword: " + keyword
  print "PIN: " + pin
  print "Password: " + password
  print "Public ID: " + public_id
  raw_input("Press any key to continue.")


def ClientMenu():
  keyword = GenerateWord()
  pin = GeneratePin()
  password = GenerateWord()
  public_id = GeneratePublicId()
  option = 'a'
  utils.ClearScreen()
  while(option != 'm'):
    utils.ClearScreen()
    print ("MaidSafe Quality Assurance Suite | Client Actions")
    print ("================================")
    print "1: Create user, logout (use local network)"
    print "2: Create user, logout (join other network using IP address)"
    print "3: Login, change Password, logout; login, logout"
    print "4: Login, change PIN, logout; login, logout"
    print "5: Login, change Keyword, logout; login, logout"
    print "6: Login, create Public ID, logout"
    print "7: Login, change Public ID, logout; login, logout"
    print "8: Login multiple instances (supply number), check only last one exists"
    print "9: Show credentials"
    option = raw_input("Please select an option (m for main Qa menu): ")
    if option == "1":
      CreateUserLocal(keyword, pin, password)
    elif option == "2":
      CreateUserIp()
    elif option == "3":
      TestChangePassword()
    elif option == "4":
      TestChangePin()
    elif option == "5":
      TestChangeKeyword()
    elif option == "6":
      TestCreatePublicId()
    elif option == "7":
      TestChangePublicId()
    elif option == "8":
      TestMultipleInstances()
    elif option == "9":
      ShowCredentials(keyword, pin, password, public_id)
  utils.ClearScreen()


def main():
  print("This is the suite of Qa anaysis info for clients")
  ClientMenu()
if __name__ == "__main__":
  sys.exit(main())
  
