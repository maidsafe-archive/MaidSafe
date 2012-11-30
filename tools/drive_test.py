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

def download(url_string): 
  file_name = url_string.split('/')[-1]
  url = urllib2.urlopen(url_string)

  file = open(os.path.join(os.curdir, file_name), 'wb')
  meta = url.info()
  file_size = int(meta.getheaders("Content-Length")[0])
  print "Downloading: %s size (bytes): %s" % (file_name, file_size)

  downloaded = 0
  block_sz = 8192
  while True:
      buffer = url.read(block_sz)
      if not buffer:
          break
      downloaded += len(buffer)
      file.write(buffer)
      status = r"%10d  [%3.2f%%]" % (file_size_dl, downloaded * 100. / file_size)
      status = status + chr(8)*(len(status)+1)
      print status,

  file.close()


def run_drive():
  process = subprocess.Popen(["../build/Release/drive_demo.exe"]).pid


def unzip_file(url): 
  file_name = url.split('/')[-1]
  with zipfile.ZipFile(os.path.join(os.curdir, file_name), 'r') as file:
    file.extractall(os.path.join(os.curdir , os.path.splitext(file_name)[0]))

def copy_unzipped_file(url): 
  file_name = url.split('/')[-1]
  directory_name = os.path.splitext(file_name)[0]
  directory = os.path.join(os.curdir, directory_name)
  shutil.copytree(directory, "S:" + directory_name)


# run from tools directory in repository...
def main():
  print("This script is for QA user analysis of Drive.")
  url = "http://dash.maidsafe.net/test_files.zip"
  print "Downloading... %s" % (url)
  #download(url)
  print "Running drive..."
  run_drive()
  print "Unzipping... %s" % (url)
  #unzip_file(url)
  print "Copying unzipped files to drive..."
  copy_unzipped_file(url)
  print "Finished..."
  return 0

if __name__ == "__main__":
  sys.exit(main())