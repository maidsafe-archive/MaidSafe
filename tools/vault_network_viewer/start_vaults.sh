#!/bin/bash

exe=vault
ENTERED_VAULT_COUNT=""

if [ "$ENTERED_VAULT_COUNT" = "" ]; then
echo "==================================================="
echo "Enter total number of vaults you want to create :"
echo "==================================================="

    read vault_count
    ENTERED_VAULT_COUNT=$vault_count
fi

echo "Setting up $vault_count vaults..."

if [ "$ENTERED_KEY_START" = "" ]; then
echo "==================================================="
echo "Enter value the identity index should start from :"
echo "==================================================="

    read i
    ENTERED_KEY_START=$i
fi
#i=4
while [ "$i" -le "$vault_count" ]
do
#	~/MaidSafe-Open/build/$exe --log_no_async true --log_routing V --log_private V --log_passport V --log_nfs V --log_vault V --log_* E --identity_index $i &>vault_$i.txt &
	~/MaidSafe-Testing/Vault_Test/build/$exe --log_* G --identity_index $i &>vault_$i.txt &
	echo "Vault using key $i started."
#       x-terminal-emulator -e /home/maidsafe/MaidSafe-Open/build/$exe --identity_index $i
	i=$(( $i + 1 ))
        sleep 1
done

