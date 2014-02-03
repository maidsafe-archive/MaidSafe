Step 1 :
./vault_key_helper -b
(copy one of the endpoint info printed on screen)

Step 2 :
./vault --log_* G --peer <paste endpoint info here> --identity_index 2 &>vault_2.txt
(such as ./vault --log_* G --identity_index 2 --peer 192.168.0.109:12427 &>vault_2.txt)

Step 3 :
./vault --log_* G --identity_index 3 &>vault_3.txt

Step 4 :
terminate the previous vault_key_help by pressing ctrl+C
then wait for about 5 seconds

Step 5 :
./start_vaults.sh
(remember to copy the script to your build directory and change the path.
Step 5 must be started no later than 30 seconds of Step 2 started)

Step 6 :
During the running of the script in Step 5, NetworkViewer.java can be started to visualize the test
(you will need key_list.txt and no_display_list.txt to be loaded to filter out redundant messages.
Make sure key_list.txt is the latest one being used.
You can use "./vault_key_helper -lp &>key_list.txt" to generate.
Ensure the path in NetworkViewer.java has been properly setup.)


Final :
./stop_vaults.sh
to stop the process and clean up the resouce.
(better create a "logs" folder under build directory to host log files to be moved in)
