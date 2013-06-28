#!/usr/bin/env python
import requests
import smtplib
import sys

def main():

  values = {'machine_type' : sys.argv[1],
            'build_type' : sys.argv[2],
            'is_valid' : sys.argv[3],
            'project' : sys.argv[4],
            'committer' : sys.argv[5]}
  requests.get("http://buildstatus.novinet.com/Incoming/Submit.aspx", params=values)

if __name__ == "__main__":
      main()
