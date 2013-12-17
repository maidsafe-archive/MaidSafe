#!/bin/bash

exe=vault
vault_count=60

echo "Setting up $vault_count vaults..."
i=4
while [ "$i" -le "$vault_count" ]
do
	~/MaidSafe-Open/build/$exe --log_no_async true --log_private V --log_passport V --log_nfs V --log_vault V --log_* Error --identity_index $i &>vault_$i.txt &
	echo "Vault using key $i started."
	i=$(( $i + 1 ))
        sleep 1
done

