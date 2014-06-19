#!/bin/bash

### This shell script needs to be copied to the same directory as
### local_network_controller and vault executables

exe=local_network_controller
join_nodes=8
path=/tmp/test
listen_port=11111

cat > seed.conf << ESEEDOF
### Commands begin.
### Initial options.
1
### Path to VaultManager root directory.
$path/seed
### Create / Clear VaultManager root directory.
y
### Path to vault executable.

### VaultManager listening port.
$listen_port
### Number of Vaults to start.

### Commands end.
ESEEDOF
echo "Starting Seed Network."
x-terminal-emulator -e ./$exe seed.conf
#./$exe seed.conf &>seed_log.txt &
sleep 60

i=1

while [ "$i" -le "$join_nodes" ]
do
        listen_port=$(( $listen_port + 1 ))
	cat > join$i.conf << EJOINOF
### Commands begin.
### Initial options.
3
### Path to bootstrap file.
$path/seed/bootstrap.dat
### Path to VaultManager root directory.
$path/join$i
### Create / Clear VaultManager root directory.
y
### Path to vault executable.

### VaultManager listening port.
$listen_port
### Number of Vaults to start.

### Commands end.
EJOINOF
        x-terminal-emulator -e ./$exe join$i.conf
#	./$exe join$i.conf &>join_log_$i.txt &
	echo "Joining $i vault."
	i=$(( $i + 1 ))
        sleep 5
done

# killall -vw -s SIGINT $exe

