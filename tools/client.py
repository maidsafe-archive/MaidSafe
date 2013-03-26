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

import platform
import random
import string
from time import sleep
import utils
import sys

# MaidSafe imports
import client_environment
import lifestuff_killer
verified_exes = utils.CheckCurDirIsBuildDir()
if verified_exes == True:
  from lifestuff_python_api import *


# as per return_codes.h
kSuccess = 0

# as per LifeStuffState in lifestuff.h
kZeroth = 0
kInitialised = 1
kConnected = 2
kLoggedIn = 3

kChangePassword = 3
kChangePin = 4
kChangeKeyword = 5
kCreatePublicId = 6

kKeywordIndex = 0
kPinIndex = 1
kPasswordIndex = 2
kPublicIdIndex = 3
kHasPublicIdIndex = 4

use_local_network = 0

#-----Helpers-----

def GenerateWord():
  length = random.randrange(5, 31)
  return ''.join(random.choice(string.ascii_uppercase + string.ascii_lowercase + string.digits) for x in range(length))


def GenerateKeyword():
  return GenerateWord()


def GeneratePassword():
  return GenerateWord()


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


def DoFullCreateUser(life_stuff, credentials):
  keyword = credentials[kKeywordIndex]
  pin = credentials[kPinIndex]
  password = credentials[kPasswordIndex]
  result = life_stuff.CreateUser(keyword, pin, password)
  if result != kSuccess:
    print "\tFailed to create user."
    return result
  print "\tCreated user successfully."

  result = life_stuff.MountDrive()
  if result != kSuccess:
    print "\tFailed to mount drive."
    return result
  print "\tMounted drive successfully."
  return kSuccess


def DoFullLogIn(life_stuff, credentials):
  keyword = credentials[kKeywordIndex]
  pin = credentials[kPinIndex]
  password = credentials[kPasswordIndex]
  has_public_id = credentials[kHasPublicIdIndex]
  result = life_stuff.LogIn(keyword, pin, password)
  if result != kSuccess:
    print "\tFailed to log in."
    return result
  print "\tLogged in successfully."

  result = life_stuff.MountDrive()
  if result != kSuccess:
    print "\tFailed to mount drive."
    return result
  print "\tMounted drive successfully."

  if has_public_id:
    result = life_stuff.StartMessagesAndIntros()
    if result != kSuccess:
      print "\tFailed to start messages and intros."
      return result
    print "\tStarted messages and intros successfully."

  return kSuccess


def DoFullLogOut(life_stuff, credentials):
  has_public_id = credentials[kHasPublicIdIndex]
  if has_public_id:
    result = life_stuff.StopMessagesAndIntros()
    if result != kSuccess:
      print "\tFailed to stop messages and intros."
      return result
    print "\tStopped messages and intros successfully."

  result = life_stuff.UnMountDrive()
  if result != kSuccess:
    print "\tFailed to unmount drive."
    return result
  print "\tUnmounted drive successfully."

  result = life_stuff.LogOut()
  if result != kSuccess:
    print "\tFailed to log out."
    return result
  print "\tLogged out successfully."

  return kSuccess


def CheckLogInLogOut(credentials):
  print "Checking LogIn and LogOut..."
  life_stuff = LifeStuff({}, "")
  result = DoFullLogIn(life_stuff, credentials)
  if result != 0:
    print "Failed to log in with result: " + str(result)
    return result

  result = DoFullLogOut(life_stuff, credentials)
  if result != 0:
    print "Failed to log out with result: " + str(result)
    return result

  return kSuccess



#-----Options-----

def CreateUser(credentials):
  keyword = credentials[kKeywordIndex]
  pin = credentials[kPinIndex]
  password = credentials[kPasswordIndex]
  print "Creating user..."
  life_stuff = LifeStuff({}, "")
  result = DoFullCreateUser(life_stuff, credentials)
  if result != 0:
    print "Failed to create user with result: " + str(result)
    return result

  result = DoFullLogOut(life_stuff, credentials)
  if result != 0:
    print "Failed to log out with result: " + str(result)
    return result
  return kSuccess


def TestChangePassword(credentials):
  return TestCreateOrChangeCredential(credentials, kChangePassword)


def TestChangePin(credentials):
  return TestCreateOrChangeCredential(credentials, kChangePin)


def TestChangeKeyword(credentials):
  return TestCreateOrChangeCredential(credentials, kChangeKeyword)


def TestCreatePublicId(credentials):
  return TestCreateOrChangeCredential(credentials, kCreatePublicId)


def TestChangePublicId():
  # TODO
  print "Sorry, changing Public IDs isn't implemented yet."
  raw_input("Press enter to continue.")


def TestCreateOrChangeCredential(credentials, action):
  if action != kChangePassword and action != kChangePin and action != kChangeKeyword and action != kCreatePublicId:
    print "Action is not recognised."
    return -1

  keyword = credentials[kKeywordIndex]
  pin = credentials[kPinIndex]
  password = credentials[kPasswordIndex]
  public_id = credentials[kPublicIdIndex]

  print "Logging in..."
  life_stuff = LifeStuff({}, "")
  result = DoFullLogIn(life_stuff, credentials)
  if result != 0:
    print "Failed to log in with result: " + str(result)
    return result

  if action == kChangePassword:
    new_password = GenerateWord()
    print "Changing password from %s to %s..." % (password, new_password)
    result = life_stuff.ChangePassword(new_password, password)
    if result != 0:
      print "Failed to change password with result: " + str(result)
      return result
    credentials[kPasswordIndex] = new_password
    print "Changed password successfully."
  elif action == kChangePin:
    new_pin = GeneratePin()
    print "Changing PIN from %s to %s..." % (pin, new_pin)
    result = life_stuff.ChangePin(new_pin, password)
    if result != 0:
      print "Failed to change pin with result: " + str(result)
      return result
    credentials[kPinIndex] = new_pin
    print "Changed PIN successfully."
  elif action == kChangeKeyword:
    new_keyword = GenerateWord()
    print "Changing keyword from %s to %s..." % (keyword, new_keyword)
    result = life_stuff.ChangeKeyword(new_keyword, password)
    if result != 0:
      print "Failed to change keyword with result: " + str(result)
      return result
    credentials[kKeywordIndex] = new_keyword
    print "Changed keyword successfully."
  elif action == kCreatePublicId:
    print "Creating public id %s..." % public_id
    result = life_stuff.CreatePublicId(public_id)
    if result != 0:
      print "Failed to create public id with result: " + str(result)
      return result
    credentials[kHasPublicIdIndex] = True
    print "Created public ID successfully."

  print "Logging out..."
  result = DoFullLogOut(life_stuff, credentials)
  if result != 0:
    print "Failed to log out with result: " + str(result)
    return result

  if action != kCreatePublicId:
    result = CheckLogInLogOut(credentials)
    if result != 0:
      print "Failed to log in and out with result: " + str(result)
      return result

  return kSuccess


def TestMultipleInstances(credentials):
  # TODO - verify test. Currently get exception on LogIn of second lifestuff instance,
  # as in TESTlifestuff_api_user_credentials.FUNC_LogInFromTwoPlaces
  num_instances = int(raw_input("Please enter the number of instances to run: "))
  while num_instances > 10 or num_instances < 2:
    print "Currently the number of instances should be between 2 and 10."
    num_instances = int(raw_input("Please enter the number of instances to run: "))
  life_stuffs = []

  for i in range(num_instances):
    print "Starting instance %d..." % i
    life_stuff = LifeStuff({}, "")
    result = DoFullLogIn(life_stuff, credentials)
    if result != kSuccess:
      print "Failed to log instance %d in with result %d" % (i, result)
      return result
    life_stuffs.append(life_stuff)
    sleep(random.randrange(1, 6))

  if len(life_stuffs) != num_instances:
    print "Only managed %d of %d instances." % (len(life_stuffs), num_instances)

  num_logged_in = 0
  for life_stuff in lifestuffs:
    if life_stuff.state() == kLoggedIn:
      num_logged_in += 1

  if num_logged_in != 1:
    print "Wrong number of logged in instances: %d" % num_logged_in
    return -1

  return kSuccess


def ShowCredentials(credentials):
  print "\tKeyword:   " + credentials[kKeywordIndex]
  print "\tPIN:       " + credentials[kPinIndex]
  print "\tPassword:  " + credentials[kPasswordIndex]
  print "\tPublic ID: " + credentials[kPublicIdIndex]
  if credentials[kHasPublicIdIndex]:
    print "\tHas created public id."
  else :
    print "\tHasn't created public ID."


def RegenerateCredentials(credentials):
  print "Old credentials:"
  ShowCredentials(credentials)
  GenerateCredentials(credentials)
  print "New credentials:"
  ShowCredentials(credentials)
  raw_input("Press enter to continue.")


def GenerateCredentials(credentials):
  del credentials[:]
  credentials.append(GenerateKeyword())
  credentials.append(GeneratePin())
  credentials.append(GeneratePassword())
  credentials.append(GeneratePublicId())
  credentials.append(False)

def LifeStuffClientHeader():
  print ("MaidSafe Quality Assurance Suite | Client Actions")
  print ("================================")

def PrintMenu(procs):
  LifeStuffClientHeader()
  print(str(procs) + " Vaults running on this machine")
  print "1: Create user, logout"
  print "2: Login, change Password, logout; login, logout"
  print "3: Login, change PIN, logout; login, logout"
  print "4: Login, change Keyword, logout; login, logout"
  print "5: Login, create Public ID, logout"
 # print "6: Login, change Public ID, logout; login, logout"
  print "7: Login multiple instances (supply number), check only last one exists"
  print "8: Show credentials"   # TODO - remove
  print "9: Regenerate credentials (current credentials will be lost)"  # TODO - remove
  print "10: Login, logout"  # TODO - remove

g_functions_dictionary = {"1":CreateUser,
                          "2":TestChangePassword,
                          "3":TestChangePin,
                          "4":TestChangeKeyword,
                          "5":TestCreatePublicId}
g_options_dictionary = {"1":"Create User",
                        "2":"Change Password",
                        "3":"Change Pin",
                        "4":"Change Keyword",
                        "5":"Create Lifestuff Id"}
def ClientMenu():
  option = "a"
  utils.ClearScreen()
  credentials = []
  GenerateCredentials(credentials)
  while(option != "m"):
    utils.ClearScreen()
    procs = utils.CountProcs("lifestuff_vault")
    PrintMenu(procs)
    option = raw_input("Please select an option (m for main QA menu): ").lower()
    if option == "1" or  option == "2" or option == "3" or option == "4" or option == "5":
      g_functions_dictionary[option](credentials)
      raw_input("Press enter to continue.")
#    elif option == "6":
#      TestChangePublicId()
#      raw_input("Press enter to continue.")
#    elif option == "7":
#      TestMultipleInstances(credentials)
#      raw_input("Press enter to continue.")
    elif option == "8":  # TODO - remove
      ShowCredentials(credentials)
      raw_input("Press enter to continue.")
    elif option == "9":  # TODO - remove
      RegenerateCredentials(credentials)
    elif option == "10":  # TODO - remove
      CheckLogInLogOut(credentials)
  utils.ClearScreen()

def PrintErrorMessage(error_message):
  if error_message != "":
    print "================================"
    print "Error message from previous option: ", error_message
    print "================================"

def ProcessNetworkOption(option, error_message):
  user_id = ""
  if platform.system() != "Windows":
    user_id = raw_input("Vaults will use default user lifestuff to run unless another is given: ")
  if option == "1":
    vault_procs = utils.CountProcs("lifestuff_vault")
    mgr_procs = utils.CountProcs("lifestuff_mgr")
    if vault_procs > 0 or mgr_procs > 0:
      error_message = "Already running vaults/lifestuff_mgr in this computer. Try option 2."
      return -1
    else:
      vault_count = int(raw_input("How many vaults would you like to run (10-50): "))
      result = client_environment.ParameterBasedStartup(None, None, None, None, vault_count, user_id)
      if result != 0:
        return result
  elif option == "2":
    use_local_network = -1
    ip_address = raw_input("IP address of vault in the network (will use LIVE port): ")
    if client_environment.CheckPassedInIp(ip_address) != 0:
      error_message = "IP given is incomplete or incorrect."
    else:
      result = client_environment.ParameterBasedStartup(ip_address, None, None, None, None, user_id)
      if result != 0:
        return result

  return 0

def GeneralOptions():
  option = "a"
  error_message = ""
  while(True):
    utils.ClearScreen()
    PrintErrorMessage(error_message)
    LifeStuffClientHeader()
    print "1: Create local network"
    print "2: Use established network"
    option = raw_input("Please select an option (m for main QA menu): ").lower()
    if option == "m":
      if utils.YesNo("Would you like to clean up the lifestuff processes that are running?"):
        lifestuff_killer.KillLifeStuff()
      return 1

    result = ProcessNetworkOption(option, error_message)
    if result != 0:
      continue
    else:
      return 0


def main():
  utils.ClearScreen()
  LifeStuffClientHeader()
  if verified_exes == False:
    print "Build directory and executables not verified."
    return -1

  result = GeneralOptions()
  print result
  if result == 0:
    ClientMenu()

if __name__ == "__main__":
  sys.exit(main())

