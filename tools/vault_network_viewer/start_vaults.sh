#!/bin/bash

exe=vault
vault_count=37

echo "Setting up $vault_count vaults..."
i=4
while [ "$i" -le "$vault_count" ]
do
#	~/MaidSafe-Open/build/$exe --log_no_async true --log_routing V --log_private V --log_passport V --log_nfs V --log_vault V --log_* E --identity_index $i &>vault_$i.txt &
	~/MaidSafe-Open/build/$exe --log_* G --identity_index $i &>vault_$i.txt &
	echo "Vault using key $i started."
#       x-terminal-emulator -e /home/maidsafe/MaidSafe-Open/build/$exe --identity_index $i
	i=$(( $i + 1 ))
        sleep 1
done

