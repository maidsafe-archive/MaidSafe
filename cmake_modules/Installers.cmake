include(version)
################Farmer ###########################################################################
if(UNIX)
  if(CMAKE_BUILD_TYPE STREQUAL "Release")
    add_custom_target(maidsafe-farmer 
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND $(CMAKE_COMMAND) -E make_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vault_manager> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/vault_manager"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:vault> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/vault"
      COMMAND ${CMAKE_COMMAND} -E copy "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/vault_manager" "${PROJECT_BINARY_DIR}/installer/etc/init.d/vault_manager"
      COMMAND ${CMAKE_COMMAND} -E copy "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install" "${PROJECT_BINARY_DIR}/post_install"
      COMMAND fpm -s dir -t deb -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-farmer --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 --description "MaidSafe Farmer"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" --after-install "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install" .
      COMMAND fpm -s dir -t tar -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-farmer --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 --description "MaidSafe Farmer"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" --after-install "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install" .
      COMMAND fpm -s dir -t rpm -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-farmer --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 --epoch 1 --description "MaidSafe Farmer"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" --after-install "${vault_manager_SOURCE_DIR}/installer/linux/scripts/ubuntu/post_install" .
      DEPENDS vault vault_manager
      VERBATIM
     ) 

    add_custom_target(maidsafe-dev 
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND $(CMAKE_COMMAND) -E make_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/common/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/passport/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/rudp/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/routing/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/drive/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/encrypt/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/nfs/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/api/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/vault/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/vault_manager/include/maidsafe" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/third_party_libs/cryptopp" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/cryptopp"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/cryptopp/TestVectors"
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/cryptopp/TestData"
      COMMAND rm -rf "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/cryptopp/*.cpp"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/sqlite/"
      COMMAND ${CMAKE_COMMAND} -E copy           "${PROJECT_SOURCE_DIR}/src/third_party_libs/sqlite/include/sqlite3.h" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/sqlite/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/third_party_libs/leveldb/include/" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E copy_directory "${PROJECT_SOURCE_DIR}/src/third_party_libs/boost_process/boost" "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/include/maidsafe/"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_common> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_passport> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_rudp> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_routing> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_encrypt> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_api> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_nfs_core> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_nfs_client> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostSystem> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostChrono> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostDateTime> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostFilesystem> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostLocale> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostProgramOptions> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostRegex> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostThread> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostSerialization> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cryptopp> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:protobuf_lite> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sqlite> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
####wnen we get the cmake generators fixed we can config apple installer fromm this position as well.!!!!!!!!!!!!!!!!!!!!! otherwise its a huge copy and paste :-(
# if(APPLE)
#       COMMAND fpm -s dir -t osxpkg -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
#
# else()
      COMMAND fpm -s dir -t deb -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d 'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
      COMMAND fpm -s dir -t tar -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d 'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
      COMMAND fpm -s dir -t rpm -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 --epoch 1 -d 'build-essential' -d 'libfuse-dev' -d 'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
# endif()
      VERBATIM
     ) 
    add_custom_target(maidsafe-utilities 
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND $(CMAKE_COMMAND) -E make_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND $(CMAKE_COMMAND) -E make_directory "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_common> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_rudp> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_routing> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_routing_api> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_drive> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_passport> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_nfs> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_encrypt> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_filesystem> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:test_api> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/tests/"
      
      COMMAND fpm -s dir -t deb -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-utilities --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 --description "MaidSafe Core system tests and utilities"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
      COMMAND fpm -s dir -t rpm -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-utilities --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 --epoch 1 --description "MaidSafe Core system tests and utlities"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
      COMMAND fpm -s dir -t tar -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-utilities --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" .
      DEPENDS vault vault_manager
      VERBATIM
     ) 

endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_custom_target(maidsafe-dev-dbg 
      ALL 
      COMMAND ${CMAKE_COMMAND} -E remove_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND $(CMAKE_COMMAND) -E make_directory "${PROJECT_BINARY_DIR}/installer/"
      COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_common> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_passport> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_rudp> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_routing> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_encrypt> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_api> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_nfs_core> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:maidsafe_nfs_client> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostSystem> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostChrono> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostDateTime> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostFilesystem> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostLocale> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostProgramOptions> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostRegex> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostThread> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:BoostSerialization> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:cryptopp> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:protobuf_lite> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
      COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:sqlite> "${PROJECT_BINARY_DIR}/installer/opt/maidsafe/usr/lib/"
####wnen we get the cmake generators fixed we can config apple installer fromm this position as well.!!!!!!!!!!!!!!!!!!!!! otherwise its a huge copy and paste :-(
# if(APPLE)
#       COMMAND fpm -s dir -t osxpkg -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
#
# else()
      COMMAND fpm -s dir -t deb -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev-dbg --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d 'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment (debug symbols included)"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
      COMMAND fpm -s dir -t tar -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev-dbg --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d 'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment (debug symbols included)"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
      COMMAND fpm -s dir -t rpm -C ${PROJECT_BINARY_DIR}/installer --name maidsafe-dev-dbg --version "${ApplicationVersionMajor}.${ApplicationVersionMinor}.${ApplicationVersionPatch}" --epoch 1  --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d 'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment (debug symbols included)"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" .
# endif()
      VERBATIM
     ) 

endif()
endif(UNIX)

