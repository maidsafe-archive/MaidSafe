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

from lifestuff_python_api import *
import random
import sys
import time

kSuccess = 0

def Test():
  life_stuff = LifeStuff({}, "")

  user_name = "testuser" + str(random.randint(10000, 99999))

  print "Creating user %s..." % user_name

  result = life_stuff.CreateUser(user_name, "1234", "password")
  if result != kSuccess:
    print "Failed to create user '%s': %s" % (user_name, result)
    return result

  print "Mounting Drive..."

  result = life_stuff.MountDrive()
  if result != kSuccess:
    print "Failed to mount Drive:", result
    return result

  print "Drive mounted:", life_stuff.mount_path()

  print "Writing test file..."

  with open(life_stuff.mount_path() + "/test.txt", "w") as f:
    f.write(user_name)
  f.close()

  print "Unmounting Drive..."

  result = life_stuff.UnMountDrive()
  if result != kSuccess:
    print "Failed to unmount Drive:", result
    return result

  time.sleep(3)

  print "Re-mounting Drive..."

  result = life_stuff.MountDrive()
  if result != kSuccess:
    print "Failed to re-mount Drive:", result
    return result

  print "Drive mounted:", life_stuff.mount_path()

  print "Reading test file..."

  with open(life_stuff.mount_path() + "/test.txt", "r") as f:
    contents = f.read()
  f.close()

  if contents != user_name:
    print "Failed to read test file contents."
    life_stuff.UnMountDrive()
    return -1

  print "Unmounting Drive..."

  result = life_stuff.UnMountDrive()
  if result != kSuccess:
    print "Failed to unmount Drive:", result
    return result

  print "Logging out..."

  result = life_stuff.LogOut()
  if result != kSuccess:
    print "Failed to log out:", result
    return result

  print "Test successful."
  return kSuccess

def main():
  print("This script is for QA analysis of LifeStuff/Drive.")
  return Test()

if __name__ == "__main__":
  sys.exit(main())
