#!/usr/bin/python
import os
import subprocess
import sys

style_check = "../../tools/cpplint.py"

# necessary check for initial commit
git_check = subprocess.call(["git", "rev-parse", "--verify",  "HEAD"], stdout=open(os.devnull, 'w'))
if git_check == 0 :
  against = "HEAD"
else :
  # Initial commit: diff against an empty tree object
  against = "4b825dc642cb6eb9a060e54bf8d69288fbee4904"

error = 0

# get a list of staged files
changed_files = subprocess.Popen(["git", "diff-index", "--cached", "--full-index", against], stdout=subprocess.PIPE)

for entry in iter(changed_files.stdout.readline, '') :
  parts = entry.split()
  status = parts[4]
  filename = parts[5]

  # file extension
  name,ext = os.path.splitext(filename)

  # only check files with .cc  or .h extension
  if ext != ".cc" :
    if ext != ".h" :
      continue

  # do not check deleted files
  if status == "D" :
    continue

  result = subprocess.call([style_check, filename])
  if result != 0 :
    error = 1
    subprocess.call(["git", "reset", "HEAD", filename])

if error == 1 :
  print "Style check failed."
  sys.exit(1)
