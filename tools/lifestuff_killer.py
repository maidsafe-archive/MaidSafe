#!/usr/bin/env python
import sys
try:
    import psutil
except ImportError:
    print("please install psutil 'pip install psutil'")
    sys.exit(1)

PROC = "lifestuff_"

def KillLifeStuff():
  for proc in psutil.process_iter():
    if proc.name.find(PROC) >= 0:
      print("Killing process : ", proc.name)
      proc.kill()

def main():
  KillLifeStuff()
  for proc in psutil.process_iter():
    if proc.name.find(PROC) >= 0:
      print("Failed to kill process : ", proc.name)
      return -1;
    print("No lifestuff processes running (now)")
    return 0


if __name__ == "__main__":
  sys.exit(main())

