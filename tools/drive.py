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

def WriteTestFile(path):
  content = "test" + str(random.randint(1000000, 9999999))
  print "Writing test file..."
  try:
    f = open(path, "w")
    f.write(content)
  except:
    print "Failed to write to", path
    content = ""
  finally:
    f.close()
  return content

def CheckTestFile(path, expected_content):
  print "Reading test file..."
  result = False
  try:
    f = open(path, "r")
    content = f.read()
    if content != expected_content:
      raise Exception("Contents don't match. ('%s' should be '%s')." % (content, expected_content))
  except Exception as ex:
    print "Failed to read test file:", ex
  else:
    result = True
  finally:
    f.close()
  return result

def ReportedProgress(action, status):
  print "reporting progress : action " % action % " is in status of " % status


def Test():
  life_stuff = LifeStuff({})

  user_name = "testuser" + str(random.randint(10000, 99999))

  print "Creating user %s..." % user_name

  result = life_stuff.CreateUser(user_name, ReportedProgress)
  if result != kSuccess:
    print "Failed to create user '%s': %s" % (user_name, result)
    return result

  print "Mounting Drive..."

  result = life_stuff.MountDrive()
  if result != kSuccess:
    print "Failed to mount Drive:", result
    return result

  print "Drive mounted:", life_stuff.mount_path()

  content = WriteTestFile(life_stuff.mount_path() + "/test.txt")
  if not CheckTestFile(life_stuff.mount_path() + "/test.txt", content):
    life_stuff.UnMountDrive()
    life_stuff.LogOut()
    return -1

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

  if not CheckTestFile(life_stuff.mount_path() + "/test.txt", content):
    life_stuff.UnMountDrive()
    life_stuff.LogOut()
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
