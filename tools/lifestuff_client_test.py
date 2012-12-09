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

import datetime
import os
import smtplib
import subprocess
import sys

# MaidSafe imports
import client
import client_environment
import lifestuff_killer

# Globals
g_base_dir = "/home/smer/qa"
g_build_dir = "/home/smer/qa/build"
g_levels_to_cmakelist = 2
g_build_type = "Debug"
g_name_of_target = "lifestuff_python_api"

def PullNext():
  command = "git checkout next"
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed checking out super project next"
    return -1

  command = "git pull"
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed pulling super project next"
    return -1

  command = "git submodule foreach ''git checkout next''"
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed checking out submodules' next"
    return -1

  command = "git submodule foreach ''git pull''"
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed pulling submodules' next"
    return -1

  return 0

def BuildTargets():
  build_path = os.path.join(g_build_dir, g_build_type)

  command = "cmake "
  for i in range(g_levels_to_cmakelist):
    command = command + "../"
  command = command + " -DCMAKE_BUILD_TYPE=" + g_build_type

  return_code = subprocess.call(command.split(), cwd=build_path)
  if return_code != 0:
    print "Failed running cmake"
    return -1

  command = "make -j6 " + g_name_of_target
  return_code = subprocess.call(command.split(), cwd=build_path)
  if return_code != 0:
    print "Failed compiling", g_name_of_target
    return -1

  return 0

def StartEnvironment(ip_address, user_id):
  result = client_environment.ParameterBasedStartup(ip_address, None, None, None, None, user_id)
  if result != 0:
    return -1

  return 0

def RunAllClientOptions(operations_results):
  credentials = []
  client.GenerateCredentials(credentials)

  for option in client.g_options_dictionary:
    operations_results.update({client.g_options_dictionary[option]:"Not run"})

  for function in client.g_functions_dictionary:
    return_code = client.g_functions_dictionary[function](credentials)
    if return_code != 0:
      operations_results[client.g_options_dictionary[function]] = "Failed"
      print "Failure in function:", client.g_options_dictionary[function]
      return -1
    else:
      operations_results[client.g_options_dictionary[function]] = "Success"

  return 0

def AlertResults(operations_results):
  # Sends an email usign gmail server
  try:
    smtp_server = "smtp.gmail.com"
    smtp_port = 587

    # TODO: replace with an account to email basecamp or dev or change
    #       to send report to dashboard?
    sender = "dan.schmidt@maidsafe.net"
    recipient = "dan.schmidt@maidsafe.net"
    subject = "Lifestuff client test results " + str(datetime.datetime.now())

    body = "Results of the tests:\n\n"
    for option in operations_results:
      body = body + option + ": " + operations_results[option] + "\n"

    headers = ["From: " + sender,
               "Subject: " + subject,
               "To: " + recipient,
               "MIME-Version: 1.0",
               "Content-Type: text/html"]
    headers = "\r\n".join(headers)

    session = smtplib.SMTP(smtp_server, smtp_port)

    session.ehlo()
    session.starttls()
    session.ehlo
    session.login(sender, "password")

    session.sendmail(sender, recipient, headers + "\r\n\r\n" + body)
    session.quit()
    return 0
  except Exception as e:
    print "Exception while sending results:" , e
    return -1

def StopEnvironment():
  return lifestuff_killer.main()

def RunTest():
  result = PullNext()
  if result != 0:
    return -1

  result = BuildTargets()
  if result != 0:
    return -1

  result = StartEnvironment(None, None)
  if result != 0:
    print "Failed starting environment:", result
    return -1

  operations_results = {}
  test_result = RunAllClientOptions(operations_results)

#  result = AlertResults(operations_results)
#  if result != 0:
#    print "Failed reporting results:", result
#    return -1

  result = StopEnvironment()
  if result != 0:
    print "Failed stopping environment:", result

  return test_result

def main():
  print "Script to run a lifestuff client suite test on next."
  return RunTest()

if __name__ == "__main__":
  sys.exit(main())
