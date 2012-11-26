#!/usr/bin/python
import os
import subprocess
import sys

def main():
  # check all parameters for the test
  if len(sys.argv) != 5:
    print("Wrong number of parameters for the test")
    return -1

  test_exe = sys.argv[1]
  test_filter = "--gtest_filter=" + sys.argv[2]
  catch_exceptions = "--gtest_catch_exceptions=" + sys.argv[3]
  root_path = sys.argv[4]

  # check if lifestuff_killer.py is needed an run it
  retcode = subprocess.call([sys.executable, os.path.join(root_path, "tools", "lifestuff_killer.py")])
  if retcode != 0:
    print("Failed executing lifestuff_killer.py")
    return retcode

  # print ("Percieved command: ", test_exe, test_filter, catch_exceptions)
  retcode = subprocess.call([test_exe, test_filter, catch_exceptions])
  return retcode

if __name__ == "__main__":
  sys.exit(main())
