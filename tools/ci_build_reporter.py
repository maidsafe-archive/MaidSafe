#!/usr/bin/env python
import requests
import smtplib
import sys

def main():

  values = {'os' : sys.argv[1],
            'build_type' : sys.argv[2],
            'status' : sys.argv[3],
            'project' : sys.argv[4],
            'commiter' : sys.argv[5]}
  requests.get("http://109.228.30.58/Submit.aspx", params=values)

  # Send Mail Alert for Build Failure to dev@maidsafe.net
  # server=smtplib.SMTP('smtp.gmail.com:587')
  # server.starttls()
  # server.login("ci@maidsafe.net","cipassw0rd")
  # server.sendmail("ci@maidsafe.net", "dev@maidsafe.net", "Broken Project Details: {} && Broken by Details: {}")
  # server.quit()

if __name__ == "__main__":
      main()
