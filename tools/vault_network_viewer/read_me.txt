Step 1 :
java -jar "VaultsNetworkViewer.jar" <BuildPath>
where BuildPath shall be your build directory(absolute path) holding :
vault, vault_key_helper and no_display_list.txt
such as : java -jar "VaultsNetworkViewer.jar" /home/maidsafe/MaidSafe-Open/build/

Step 2 :
if first time running on the machine, click the PreConditions button to generate keys and chunks

Step 3 :
click the Bootstrap Network button to setup a network from bootstrap

During the test :
following the test scenario in the document Visualized_Vaults_Network_Test_Scenario.xls
the viewer will visualize what's happening among the vaults during the procedure

Final :
click the Shutdown Network button to stop Vaults and clean up the resouce.
(better create a "logs" folder under build directory to host log files to be moved in)
