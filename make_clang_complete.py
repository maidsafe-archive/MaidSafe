#!/usr/bin/env python

import sys
import os.path

def main():
  file_in = open('clang_complete_template', "r")
  file_out = open('.clang_complete', "w")
  lines = []
  for line in file_in.readlines():
    # alter the include paths to be absolute
    if line.startswith("-I"):
      path = line[2:]
      line = '-I' + os.path.join(os.path.abspath(os.path.curdir), path)
      print line
    file_out.write(line)

  file_in.close()
  file_out.close()

if __name__ == "__main__":
    main()
