#!/usr/bin/python
import sys
import os

hook_root = os.path.dirname(os.path.dirname(__file__))
module_root = os.path.join(os.path.dirname(os.getcwd()), ".git/modules")
hook_list = []

for hook in os.listdir(hook_root) :
  filename,ext = os.path.splitext(hook)
  if ext == '' :
    if os.path.isfile(os.path.join(hook_root, hook)) :
      hook_list.append(hook)

for dir_name, sub_dir, files in os.walk(module_root) :
  if os.path.basename(dir_name) == 'hooks' :
    for hook in hook_list : 
      if hook in files :
        os.remove(os.path.join(dir_name, hook))
      os.symlink(os.path.join(hook_root, hook), os.path.join(dir_name, hook))
