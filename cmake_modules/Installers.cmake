################Farmer ###########################################################################
if(UNIX)
add_custom_target(TARGET farmer_install 
  # install(TARGETS vault_manager COMPONENT Development DESTINATION /opt/maidsafe/vault)
  # install(FILES ${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/vault_manager COMPONENT Development DESTINATION /etc/init.d/vault_manager)
  COMMAND ${CMAKE_COMMAND} -E copy_if_different ${PROJECT_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install "${PROJECT_BINARY_DIR}/post_install")
endif(UNIX)

# if(MaidsafeTesting AND NOT $ENV{DESTDIR})
#   install(TARGETS maidsafe_vault_manager COMPONENT Development CONFIGURATIONS Debug Release ARCHIVE DESTINATION lib)
# install(DIRECTORY ${PROJECT_SOURCE_DIR}/include/ COMPONENT Development DESTINATION include)
#   install(TARGETS local_network_controller network_test_helper COMPONENT Tools CONFIGURATIONS Debug RUNTIME DESTINATION bin/debug)
#   install(TARGETS local_network_controller network_test_helper COMPONENT Tools CONFIGURATIONS Release RUNTIME DESTINATION bin)
#   install(TARGETS test_vault_manager dummy_vault COMPONENT Tests CONFIGURATIONS Debug RUNTIME DESTINATION bin/debug)
#   install(TARGETS test_vault_manager dummy_vault COMPONENT Tests CONFIGURATIONS Release RUNTIME DESTINATION bin)
# endif()
################Dev ###########################################################################
################Dev with symbols ##############################################################
################Tests #########################################################################
