#!/usr/bin/python
import sys
import os

root_hook_dir = os.path.dirname(os.path.dirname(__file__))
start_dir = os.path.join(os.path.dirname(os.getcwd()), ".git/modules")
hook_list = []

os.chdir(root_hook_dir)

for hook in os.listdir(root_hook_dir) :
   filename,ext = os.path.splitext(hook)
   if ext == '' :
      hook_list.append(hook)

for dir_name, sub_dir, files in os.walk(start_dir) :
   if os.path.basename(dir_name) == 'hooks' :
      for hook in hook_list : 
         if hook not in files and os.path.isdir(hook) == False :
            os.symlink(os.path.join(root_hook_dir, hook), os.path.join(dir_name, hook))
