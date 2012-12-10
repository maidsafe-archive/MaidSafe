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

import sys
import os
import subprocess
import urllib2
import zipfile
import shutil
import time
import logging
import tempfile
import platform
try:
  import psutil
except ImportError:
  print("please install psutil 'pip install psutil'")
  print("or 'easy_install psutil'")
  print("Website : http://code.google.com/p/psutil/")
  sys.exit(1)

# MaidSafe imports
import utils

def timed_function(function):
  def wrapper(*arg):
    t1 = time.time()
    result = function(*arg)
    t2 = time.time()
    logging.info('%s took %0.3f ms', function.func_name, (t2-t1)*1000.0)
    return result
  return wrapper

@timed_function
def copy_tree(tree_from, tree_to):
  shutil.copytree(tree_from, tree_to)

@timed_function
def move_tree(tree_from, tree_to):
  shutil.move(tree_from, tree_to)

def download(url_string):
  file_name = url_string.split('/')[-1]
  url = urllib2.urlopen(url_string)

  file = open(os.path.join(tempfile.gettempdir(), file_name), 'wb')
  meta = url.info()
  file_size = int(meta.getheaders("Content-Length")[0])
  print "Downloading... %s, size=%sB" % (file_name, file_size)

  downloaded = 0
  block_sz = 65536
  while True:
      buffer = url.read(block_sz)
      if not buffer:
          break
      downloaded += len(buffer)
      file.write(buffer)
      status = r"%10d  [%3.2f%%]" % (downloaded, downloaded * 100. / file_size)
      status = status + chr(8)*(len(status)+1)
      print status,

  file.close()

def mount_drive(executable_name):
  print "Mounting drive..."
  sys.stdout.flush()
  process = subprocess.Popen([utils.GetProg(executable_name)]).pid
  time.sleep(3)

def drive_exists(executable_name):
  for proc in psutil.process_iter():
    if proc.name.find(executable_name) >= 0:
      print("Failed to kill process : " + proc.name)
      return -1;
    print("No drive processes running.")
    return 0

def unmount_drive(executable_name):
  print "Unmounting drive..."
  sys.stdout.flush()
  for proc in psutil.process_iter():
    if proc.name.find(executable_name) >= 0:
      try:
        if platform.system() == "Windows":
          proc.terminate()
        else:
          proc.kill()
      except:
        print("Could not kill all instances.")
  time.sleep(3)
  return drive_exists(executable_name)

@timed_function
def unzip_file(url):
  file_name = url.split('/')[-1]
  print "Unzipping file... %s" % (file_name)
  sys.stdout.flush()
  with zipfile.ZipFile(os.path.join(tempfile.gettempdir(), file_name), 'r') as file:
    file.extractall(os.path.join(tempfile.gettempdir(), os.path.splitext(file_name)[0]))

# Copy files to drive...
def copy_files_to_drive(url, folder):
  file_name = url.split('/')[-1]
  directory_name = os.path.splitext(file_name)[0] + folder
  print "Copying %s to drive..." % (directory_name)
  sys.stdout.flush()
  directory = os.path.join(tempfile.gettempdir(), directory_name)
  logging.info('Copying %s to drive...', directory_name)
  copy_tree(directory, "S:/" + directory_name)

# Copy files on drive...
def copy_files_on_drive(url, folder):
  print "Copying %s on drive..." % (folder)
  sys.stdout.flush()
  try:
    os.makedirs("S:/" + "Copied")
  except OSError:
    pass
  file_name = url.split('/')[-1]
  directory_name = os.path.splitext(file_name)[0] + folder
  directory = os.path.join(tempfile.gettempdir(), directory_name)
  logging.info('Copying %s on drive...', folder)
  copy_tree("S:/" + directory_name, "S:/" + "Copied" + folder)

# Move files on drive...
def move_files_on_drive(url, folder):
  print "Moving %s on drive..." % (folder)
  sys.stdout.flush()
  try:
    os.makedirs("S:/" + "Moved")
  except OSError:
    pass
  file_name = url.split('/')[-1]
  directory_name = os.path.splitext(file_name)[0] + folder
  directory = os.path.join(tempfile.gettempdir(), directory_name)
  logging.info('Moving %s on drive...', folder)
  move_tree("S:/" + directory_name, "S:/" + "Moved" + folder)

# Copy files on disk...
def copy_files_on_disk(url, folder):
  print "Copying %s on disk..." % (folder)
  sys.stdout.flush()
  try:
    os.makedirs(tempfile.gettempdir() + "/Copied")
  except OSError:
    pass
  file_name = url.split('/')[-1]
  directory_name = '/' + os.path.splitext(file_name)[0] + folder
  print "copy directory name = %s, tempdir = %s" % (directory_name, tempfile.gettempdir())
  directory = os.path.join(tempfile.gettempdir(), directory_name)
  logging.info('Copying %s on disk...', folder)
  copy_tree(tempfile.gettempdir() + directory_name, tempfile.gettempdir() + "/Copied" + folder)

# Move files on disk...
def move_files_on_disk(url, folder):
  print "Moving %s on disk..." % (folder)
  sys.stdout.flush()
  try:
    os.makedirs(tempfile.gettempdir() + "/Moved")
  except OSError:
    pass
  file_name = url.split('/')[-1]
  directory_name = os.path.splitext(file_name)[0] + folder
  print "move directory name = %s" % (directory_name)
  directory = os.path.join(tempfile.gettempdir(), directory_name)
  logging.info('Moving %s on disk...', folder)
  move_tree(directory, tempfile.gettempdir() + "/Moved" + folder)

def main():
  option = 'a'
  url = "http://dash.maidsafe.net/test_files.zip"
  executable_name = "drive_demo"
  log_file = os.path.join(tempfile.gettempdir(), 'drive_timing.log')
  if os.path.exists(log_file):
    os.remove(log_file)
  logging.basicConfig(filename = log_file, level = logging.INFO)
  utils.ClearScreen()
  while(option != 'm'):
    #utils.ClearScreen()
    print ("MaidSafe Quality Assurance Suite | Drive Actions")
    print ("================================================")
    print ("Setup.")
    print ("")
    print (" 1: Download zip file from http://dash.maidsafe.net.")
    print (" 2: Unzip downloaded file.")
    print (" 3: Mount drive.")
    print ("")
    print ("The following timed tests will be logged to...")
    print ("%s." % (log_file))
    print ("")
    print (" 4: Disk to Drive copy doc files.")
    print (" 5: Disk to Drive copy htm files.")
    print (" 6: Disk to Drive copy pdf files.")
    print (" 7: Disk to Drive copy mp4 files.")
    print (" 8: Disk to Drive copy wmv files.")
    print ("")
    print (" 9: Drive to Drive copy doc files.")
    print ("10: Drive to Drive copy htm files.")
    print ("11: Drive to Drive copy pdf files.")
    print ("12: Drive to Drive copy mp4 files.")
    print ("13: Drive to Drive copy wmv files.")
    print ("")
    print ("14: Drive to Drive move doc files.")
    print ("15: Drive to Drive move htm files.")
    print ("16: Drive to Drive move pdf files.")
    print ("17: Drive to Drive move mp4 files.")
    print ("18: Drive to Drive move wmv files.")
    print ("")
    print ("19: Disk to Disk copy doc files.")
    print ("20: Disk to Disk copy htm files.")
    print ("21: Disk to Disk copy pdf files.")
    print ("22: Disk to Disk copy mp4 files.")
    print ("23: Disk to Disk copy wmv files.")
    print ("")
    print ("24: Disk to Disk move doc files.")
    print ("25: Disk to Disk move htm files.")
    print ("26: Disk to Disk move pdf files.")
    print ("27: Disk to Disk move mp4 files.")
    print ("28: Disk to Disk move wmv files.")
    print ("")
    print ("Finish.")
    print ("")
    print ("29: Unmount drive.")
    print ("")
    option = raw_input("Please select an option (m for main QA menu): ").lower()
    if option == "1":
      download(url)
    elif option == "2":
      unzip_file(url)
    elif option == "3":
      mount_drive(executable_name)
    elif option == "4":
      copy_files_to_drive(url, "/5200 items/DOC")
    elif option == "5":
      copy_files_to_drive(url, "/5200 items/HTML")
    elif option == "6":
      copy_files_to_drive(url, "/5200 items/PDF")
    elif option == "7":
      copy_files_to_drive(url, "/5200 items/MP4")
    elif option == "8":
      copy_files_to_drive(url, "/5200 items/WMV")
    elif option == "9":
      copy_files_on_drive(url, "/5200 items/DOC")
    elif option == "10":
      copy_files_on_drive(url, "/5200 items/HTML")
    elif option == "11":
      copy_files_on_drive(url, "/5200 items/PDF")
    elif option == "12":
      copy_files_on_drive(url, "/5200 items/MP4")
    elif option == "13":
      copy_files_on_drive(url, "/5200 items/WMV")
    elif option == "14":
      move_files_on_drive(url, "/5200 items/DOC")
    elif option == "15":
      move_files_on_drive(url, "/5200 items/HTML")
    elif option == "16":
      move_files_on_drive(url, "/5200 items/PDF")
    elif option == "17":
      move_files_on_drive(url, "/5200 items/MP4")
    elif option == "18":
      move_files_on_drive(url, "/5200 items/WMV")
    elif option == "19":
      copy_files_on_disk(url, "/5200 items/DOC")
    elif option == "20":
      copy_files_on_disk(url, "/5200 items/HTML")
    elif option == "21":
      copy_files_on_disk(url, "/5200 items/PDF")
    elif option == "22":
      copy_files_on_disk(url, "/5200 items/MP4")
    elif option == "23":
      copy_files_on_disk(url, "/5200 items/WMV")
    elif option == "24":
      move_files_on_disk(url, "/5200 items/DOC")
    elif option == "25":
      move_files_on_disk(url, "/5200 items/HTML")
    elif option == "26":
      move_files_on_disk(url, "/5200 items/PDF")
    elif option == "27":
      move_files_on_disk(url, "/5200 items/MP4")
    elif option == "28":
      move_files_on_disk(url, "/5200 items/WMV")
    elif option == "29":
      unmount_drive(executable_name)
    else:
      print "That's not a valid option."
  #utils.ClearScreen()

if __name__ == "__main__":
  sys.exit(main())
