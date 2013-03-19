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
import platform
import smtplib
import subprocess
import sys
from optparse import OptionParser

# MaidSafe imports
import client_environment
import lifestuff_killer

# Globals
g_git_path = ""
g_base_dir = ""
g_build_dirname = "build"
g_build_dir = ""
g_levels_to_cmakelist = 1
if platform.system() != "Windows":
  g_levels_to_cmakelist = 2
g_name_of_target = "lifestuff_python_api"

def WindowsifyCommand(command):
  local_command = g_git_path + " -login -c '" + command + "'"
  print local_command
  return local_command

def PullNext():
  command = "git checkout next"
  if platform.system() == "Windows":
    command = WindowsifyCommand(command)
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed checking out super project next"
    return -1

  command = "git pull"
  if platform.system() == "Windows":
    command = WindowsifyCommand(command)
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed pulling super project next"
    return -1

  command = "git submodule foreach ''git checkout next''"
  if platform.system() == "Windows":
    command = WindowsifyCommand(command)
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed checking out submodules' next"
    return -1

  command = "git submodule foreach ''git pull''"
  if platform.system() == "Windows":
    command = WindowsifyCommand(command)
  return_code = subprocess.call(command.split(), cwd=g_base_dir)
  if return_code != 0:
    print "Failed pulling submodules' next"
    return -1

  return 0

def BuildTargets(build_type):
  command = "cmake "
  for i in range(g_levels_to_cmakelist):
    command = command + "../"
  if platform.system() != 'Windows':
    command = command + " -DCMAKE_BUILD_TYPE=" + build_type

  return_code = subprocess.call(command.split(), cwd=g_build_dir)
  if return_code != 0:
    print "Failed running cmake"
    return -1

  command = "cmake --build . --target "
  if platform.system() == 'Windows':
    command = command + "src/lifestuff/lifestuff_python_api --config " + build_type
  else:
    command = command + "lifestuff_python_api"
  print "\n\n", command, "\n\n"

  return_code = subprocess.call(command.split(), cwd=g_build_dir)
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
  # This import MUST be here to allow the script to copy the new lifestuff_python_api library
  # to the tools directory on Windows. Otherwise, the module is loaded and it cannot be
  # overwritten.
  try:
    import client
  except Exception as e:
    print "Failed to import client:", e
    return -1

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
    sender = "test.results@maidsafe.net"
    recipient = "dev@maidsafe.net"
    subject = "Lifestuff client test results " + str(datetime.datetime.now())

    body = "Results of the tests:<br><br>"
    for option in operations_results:
      body = body + option + ": " + operations_results[option] + "<br>"

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
    session.login(sender, "t5e3swt5")

    session.sendmail(sender, recipient, headers + "\r\n\r\n" + body)
    session.quit()
    return 0
  except Exception as e:
    print "Exception while sending results:" , e
    return -1

def StopEnvironment():
  return lifestuff_killer.main()

def RunTest(build_type):
  result = PullNext()
  if result != 0:
    return -1

  result = BuildTargets(build_type)
  if result != 0:
    return -1

  result = StartEnvironment(None, None)
  if result != 0:
    print "Failed starting environment:", result
    return -1

  operations_results = {}
  test_result = RunAllClientOptions(operations_results)

  result = AlertResults(operations_results)
  if result != 0:
    print "Failed reporting results:", result
#    return -1

  result = StopEnvironment()
  if result != 0:
    print "Failed stopping environment:", result

  return test_result
#  return 0

def HandleOptions(options):
  if options.build_type != "Debug" and options.build_type != "Release":
    print "Build type options only Debug or Release"
    return -1

  if not os.path.isdir(options.base_directory):
    print "Path given as base directory(", options.base_directory, ") is not directory."
    return -1


  if platform.system() == "Windows":
    if not os.path.isdir(str(options.git_path)) or\
       not os.path.exists(str(os.path.join(options.git_path, "bin", "sh.exe"))):
      print "Path given as git directory does not exist or does not contain bin\sh.exe."
      return -1
    else:
      global g_git_path
      g_git_path = os.path.join(options.git_path, "bin", "sh.exe")

  global g_base_dir, g_build_dir, g_build_dirname
  g_base_dir = options.base_directory
  g_build_dir = os.path.join(g_base_dir, g_build_dirname)
  if platform.system() != "Windows":
    g_build_dir = os.path.join(g_build_dir, options.build_type)

  if os.path.isdir(g_build_dir):
    if not os.path.exists(g_build_dir):
      try:
        os.makedirs(g_build_dir)
      except Exception as e:
        print "Failure creating the build directory:", e
        return -1
  else:
    print "Potential build directory path(", g_build_dir, ") already exixts and is not a directory."
    return -1

  return 0

def main():
  print "Script to run a lifestuff client suite test on next."
  parser = OptionParser()
  parser.add_option("-b", "--base_directory", dest="base_directory", action="store", type="string",
                    help="path to the base of the super project")
  parser.add_option("-t", "--build_type", dest="build_type", action="store", type="string",
                    help="Debug or Release")
  if platform.system() == "Windows":
    parser.add_option("-g", "--git_path", dest="git_path", action="store", type="string",
                      help="Path to the base of the git shell, e.g. C:\Program Files (x86)\Git")
  (options, args) = parser.parse_args()

  return_code = HandleOptions(options)
  if return_code != 0:
    return -1

  return RunTest(options.build_type)

if __name__ == "__main__":
  sys.exit(main())
