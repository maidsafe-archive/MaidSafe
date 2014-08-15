START "Vault 2" /D C:\Code\TestNet C:\Code\build_maidsafe\Debug\vault --log_routing I --log_vault I --log_vault_manager I --peer %1 --identity_index 2
START "Vault 3" /D C:\Code\TestNet C:\Code\build_maidsafe\Debug\vault --log_routing I --log_vault I --log_vault_manager I --peer %1 --identity_index 3
ECHO "Wait for the first two nodes to connect then close the Boot Strap vaults by using CTRL-C in those windows"
TIMEOUT 40
FOR /L %%i IN (4,1,%2) DO Start3.bat %%i
ECHO "Monitor the activity on one or two of the vault nodes and wait until everything stablizes before attempting to run tests"