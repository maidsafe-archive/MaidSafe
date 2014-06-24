#!/bin/bash

### This shell script needs to be copied to the same directory as
### local_network_controller and vault executables

exe=local_network_controller
vault_exe=vault
join_nodes=(32 52)
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

cp $path/seed/bootstrap.dat $path/bootstrap.dat

### Record the process info of the seed local_network_controller and vaults
seed_controller=`ps -ef | grep local_network_controller`
seed_vaults=`ps -ef | grep vault`


### iteration : 0 - initial join, kill seeding vaults once completed 
###             1 - later on join, process finished once completed
declare -i iteration=0

#### Join individual vaults one by one ####
i=1
while [ "$iteration" -le 1 ]
do
	while [ "$i" -le "${join_nodes[$iteration]}" ]
	do
		listen_port=$(( $listen_port + 1 ))
		cat > join$i.conf << EJOINOF
### Commands begin.
### Initial options.
3
### Path to bootstrap file.
$path/bootstrap.dat
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
               #./$exe join$i.conf &>join_log_$i.txt &
		echo "Joining $i vault."
		i=$(( $i + 1 ))
		sleep 5
	done

	if [ "$iteration" -eq 0 ]; then
		echo "Please join one vault from other machine"
		read temp
		cp $path/other/bootstrap.dat $path/bootstrap.dat
		#### Stop the Seed Network ####
		echo "stop seed network"
		readarray -t y <<< "$seed_controller"
		for proc in "${y[@]}"
		do
			z=(${proc//$' '/ })
                        if [ ${z[7]} == "./$exe" ]; then
				kill ${z[1]}
			fi
		done
		sleep 10
        fi
	iteration=$(( $iteration + 1 ))
done

echo "setup seeding network completed"

# killall -vw -s SIGINT $exe

