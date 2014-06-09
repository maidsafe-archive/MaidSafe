FOR /L %%i IN (%1,1,%2) DO Start3.bat %%i
ECHO "Monitor the activity on one or two of the vault nodes and wait until everything stablizes before attempting to run tests"