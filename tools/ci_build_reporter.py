#!/usr/bin/env python
#import urllib2
#import urllib
import requests
import smtplib

def main():
  #url = 'http://109.228.30.58/Submit.html'  
  # half_url = u'/Submit.html?os=win7&build_type=x86&status=fail&project=MaidSafe-Common&commiter=Viv-Rajkumar'
  # req = urllib2.Request(url + half_url.encode('utf-8'))
  # response = urllib2.urlopen(req)
  # print response.read()
  # requests.get('http://109.228.30.58/Submit.html?os=win7&build_type=x86&status=fail&project=MaidSafe-Common&commiter=Viv-Rajkumar')
  # data = urllib.urlencode(values)
  # url = 'http://localhost:2740/OAuthService/Submit.html?' + data
  # urllib2.urlopen(url)
  # print req.get_method()
  # the_page = response.read()
  # value = urllib.quote_plus("http://109.228.30.58/Submit.html?os=win7&build_type=x86&status=fail&project=MaidSafe-Common&commiter=Viv-Rajkumar")
  #print(the_page)
  # urllib2.urlopen(value)
  
  values = {'os' : 'win7',
            'build_type' : 'x86',
            'status' : 'fail',
            'project' : 'MaidSafe-Common',
            'commiter' : 'Viv-Rajkumar'}
  headers = {'content-type': 'application/json'}
  r = requests.get("http://109.228.30.58/Submit.html?os=win7&build_type=x86&status=fail&project=MaidSafe-Common&commiter=Viv", headers=headers)
  print r.url
  print r.status_code
  print r.encoding
  print r.headers['content-type']
  
  
  # Send Mail Alert for Build Failure to dev@maidsafe.net
  # server=smtplib.SMTP('smtp.gmail.com:587')
  # server.starttls()
  # server.login("ci@maidsafe.net","cipassw0rd")
  # server.sendmail("ci@maidsafe.net", "dev@maidsafe.net", "Broken Project Details: {} && Broken by Details: {}")
  # server.quit()


if __name__ == "__main__":
      main()
