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

kSuccess = 0

def Chat(s1, s2, s3, s4):
    pass

def FileTransferSuccess(s1, s2, s3, s4, s5):
    pass

def FileTransferFailure(s1, s2, s3):
    pass

def NewContact(s1, s2, s3, s4):
    pass

def ContactConfirmation(s1, s2, s3):
    pass

def ContactProfilePicture(s1, s2, s3):
    pass

def ContactPresence(s1, s2, s3, contact_presense):
    pass

def ContactDeletion(s1, s2, s3, s4):
    pass

def LifestuffCardUpdate(s1, s2, s3):
    pass

def NetworkHealth(i):
    pass

def ImmediateQuitRequired():
    pass

def UpdateAvailable(s):
    pass

def OperationProgress(operation, sub_task):
    pass

call_backs = [Chat, FileTransferSuccess, FileTransferFailure, NewContact, ContactConfirmation, ContactProfilePicture, ContactPresence, ContactDeletion, LifestuffCardUpdate, NetworkHealth, ImmediateQuitRequired, UpdateAvailable, OperationProgress]

def Test():
  life_stuff = LifeStuff(call_backs, "/tmp/ls_py_test/")
  user_name = "testuser" + str(random.randint(10000, 99999))
  result = life_stuff.CreateUser(user_name, "1234", "password")
  if result != kSuccess:
    print "Failed to create user '%s': %s" % (user_name, result)
    return result
  print "Test successful."
  return kSuccess

def main():
  print("This is the suite for QA analysis of LifeStuff")
  Test()

if __name__ == "__main__":
  sys.exit(main())

