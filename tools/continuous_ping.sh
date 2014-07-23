#!/bin/bash

# start with './continuous_ping.sh node_list.txt'
# with node_list.txt in a following format :
# <host_name>;<IP>

while read line
do
    IFS=';' read -a arr <<< "$line"
    echo "starts pinging to host name : ${arr[0]}  with IP : ${arr[1]}"
    ping -D -i 10 ${arr[1]} &>ping_${arr[0]}_result.txt &
done < $1

# use 'sudo killall -SIGINT ping' to stop ping process
