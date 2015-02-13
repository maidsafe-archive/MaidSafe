#==================================================================================================#
#                                                                                                  #
#  Copyright 2014 MaidSafe.net limited                                                             #
#                                                                                                  #
#  This MaidSafe Software is licensed to you under (1) the MaidSafe.net Commercial License,        #
#  version 1.0 or later, or (2) The General Public License (GPL), version 3, depending on which    #
#  licence you accepted on initial access to the Software (the "Licences").                        #
#                                                                                                  #
#  By contributing code to the MaidSafe Software, or to this project generally, you agree to be    #
#  bound by the terms of the MaidSafe Contributor Agreement, version 1.0, found in the root        #
#  directory of this project at LICENSE, COPYING and CONTRIBUTOR respectively and also available   #
#  at: http://www.maidsafe.net/licenses                                                            #
#                                                                                                  #
#  Unless required by applicable law or agreed to in writing, the MaidSafe Software distributed    #
#  under the GPL Licence is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF   #
#  ANY KIND, either express or implied.                                                            #
#                                                                                                  #
#  See the Licences for the specific language governing permissions and limitations relating to    #
#  use of the MaidSafe Software.                                                                   #
#                                                                                                  #
#==================================================================================================#


# Assert that configuration is Release for all but DevDebug target
if(NOT Config STREQUAL "Release" AND NOT TargetType STREQUAL "DevDebug")
  message(FATAL_ERROR "Invalid build configuration.  ${TargetName} is only availale for Release builds.")
endif()

# Set up various common values and flags
set(InstallerDir "${SUPER_PROJECT_BINARY_DIR}/installer")
set(Maintainer "dev@maidsafe.net")
set(Url "http://maidsafe.net")
set(Vendor "maidsafe.net limited")

set(FpmTarFlags -s dir -t tar -C "${InstallerDir}" -f --name ${TargetName}-${Version} --version "${Version}")
set(FpmDebFlags -s dir -t deb -C "${InstallerDir}" -f --name ${TargetName} --version "${Version}" --iteration 1           --maintainer "${Maintainer}" --url '${Url}' --license 'GPL' --vendor "${Vendor}")
set(FpmRpmFlags -s dir -t rpm -C "${InstallerDir}" -f --name ${TargetName} --version "${Version}" --iteration 1 --epoch 1 --maintainer "${Maintainer}" --url '${Url}' --license 'GPL' --vendor "${Vendor}")

separate_arguments(TargetLibs UNIX_COMMAND "${TargetLibs}")
separate_arguments(TargetHeaders UNIX_COMMAND "${TargetHeaders}")
separate_arguments(TargetExes UNIX_COMMAND "${TargetExes}")

# Clear installer folder and execute relevant file/folder commands
file(REMOVE_RECURSE "${InstallerDir}")
if(TargetType STREQUAL "Farmer")
  set(PostInstall "${SUPER_PROJECT_SOURCE_DIR}/src/vault_manager/installer/linux/scripts/ubuntu/post_install")
  file(COPY ${TargetExes} DESTINATION "${InstallerDir}/opt/maidsafe/sbin")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/vault_manager/installer/linux/scripts/ubuntu/vault_manager" DESTINATION "${InstallerDir}/etc/init.d/")
  file(COPY "${PostInstall}" DESTINATION "${SUPER_PROJECT_BINARY_DIR}/post_install")

  set(Description "MaidSafe Farmer")
  list(APPEND FpmDebFlags --description "${Description}" --after-install "${PostInstall}")
  list(APPEND FpmRpmFlags --description "${Description}" --after-install "${PostInstall}")

elseif(TargetType STREQUAL "Dev")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/common/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/passport/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/crux/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/routing/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/drive/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/encrypt/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/nfs/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/api/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/launcher/include/maidsafe" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(COPY "${BoostSourceDir}/boost" DESTINATION "${InstallerDir}/opt/maidsafe/include/")
  file(GLOB CryptoHeaders "${SUPER_PROJECT_SOURCE_DIR}/src/third_party_libs/cryptopp/*.h")
  file(COPY ${CryptoHeaders} DESTINATION "${InstallerDir}/opt/maidsafe/include/cryptopp")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/third_party_libs/header_only/asio.hpp" DESTINATION "${InstallerDir}/opt/maidsafe/include")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/third_party_libs/header_only/asio" DESTINATION "${InstallerDir}/opt/maidsafe/include/asio")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/third_party_libs/header_only/cereal" DESTINATION "${InstallerDir}/opt/maidsafe/include/cereal")
  file(COPY "${SUPER_PROJECT_SOURCE_DIR}/src/third_party_libs/sqlite/include/sqlite3.h" DESTINATION "${InstallerDir}/opt/maidsafe/include/sqlite/")
  file(COPY ${TargetLibs} DESTINATION "${InstallerDir}/opt/maidsafe/lib/")

  set(Description "MaidSafe Developer Environment")
  list(APPEND FpmDebFlags -d build-essential -d libfuse-dev -d libicu-dev --description "${Description}")
  list(APPEND FpmRpmFlags -d build-essential -d libfuse-dev -d libicu-dev --description "${Description}")

elseif(TargetType STREQUAL "Utilities")
  file(COPY ${TargetExes} DESTINATION "${InstallerDir}/opt/maidsafe/tests")
  set(Description "MaidSafe Core system tests and utilities")
  list(APPEND FpmDebFlags --description "${Description}")
  list(APPEND FpmRpmFlags --description "${Description}")

elseif(TargetType STREQUAL "DevDebug")
  file(COPY ${TargetLibs} DESTINATION "${InstallerDir}/opt/maidsafe/usr/lib/")
  set(Description "MaidSafe Developer Environment (Debug symbols included)")
  list(APPEND FpmDebFlags -d build-essential -d libfuse-dev -d libicu-dev --description "${Description}")
  list(APPEND FpmRpmFlags -d build-essential -d libfuse-dev -d libicu-dev --description "${Description}")
endif()

# Create the installers
execute_process(COMMAND fpm ${FpmTarFlags} .)
execute_process(COMMAND fpm ${FpmDebFlags} .)
execute_process(COMMAND fpm ${FpmRpmFlags} .)


####When we get the cmake generators fixed we can config apple installer fromm this position as well.!!!!!!!!!!!!!!!!!!!!! otherwise its a huge copy and paste :-(
# if(APPLE)
#       COMMAND fpm -s dir -t osxpkg -C ${InstallerDir} --name ${TargetName} --version "${Version}" --iteration 1 -d 'build-essential'  -d 'libfuse-dev' -d'git-all' -d 'libicu-dev' --description "MaidSafe developer Environment"  --maintainer "dev@maidsafe.net" --url 'http://www.maidsafe.net' --license 'GPL' --vendor "maidsafe.net limited" --verbose --debug .
#
# else()
