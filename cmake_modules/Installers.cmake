include(version)
################Farmer ###########################################################################
if(UNIX)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(farmer_install 
     
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND $(CMAKE_COMMAND) -E make_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vault_manager> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/vault_manager"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vault> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/vault"
      COMMAND ${CMAKE_COMMAND} -E copy "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/vault_manager" "${PROJECT_BINARY_DIR}/installer/etc/init.d/vault_manager"
      COMMAND ${CMAKE_COMMAND} -E copy "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install" "${PROJECT_BINARY_DIR}/post_install"
      COMMAND fpm -s dir -t deb -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-farmer --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'fuse' --description "MaidSafe Farmer"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" --after-install "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install" .
      # COMMAND fpm -s dir -t rpm -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-farmer --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'fuse' --description "MaidSafe Farmer"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" --after-install "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install" .
      DEPENDS vault vault_manager
      VERBATIM
     ) 

    add_custom_target(maidsafe-dev 
      ALL 
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND $(CMAKE_COMMAND) -E make_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/common/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/passport/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/rudp/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/routing/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/drive/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/encrypt/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/nfs/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/client/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/api/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/vault/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/vault_manager/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/third_party_libs/cryptopp" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/cryptopp"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/cryptopp/TestVectors"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/cryptopp/TestData"
      COMMAND rm -rf "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/cryptopp/*.cpp"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/sqlite/"
      COMMAND ${CMAKE_COMMAND} -E copy           "${PROJECT_SOURCE_DIR}/src/third_party_libs/sqlite/include/sqlite3.h" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/sqlite/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/third_party_libs/leveldb/include/" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/third_party_libs/boost_process/boost" "${PROJECT_BINARY_DIR}/installer/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/installer/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe> "${PROJECT_BINARY_DIR}/installer/usr/lib/"

      COMMAND fpm -s dir -t deb -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
      DEPENDS vault vault_manager
      VERBATIM
     ) 

  endif()
endif(UNIX)

