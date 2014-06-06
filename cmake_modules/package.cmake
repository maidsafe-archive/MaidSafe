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
#                                                                                                  #
#  Generates a package from all exported targets.                                                  #
#                                                                                                  #
#==================================================================================================#

find_file(MaidSafeExport NAMES maidsafe_export.cmake
                         PATHS ${MAIDSAFE_BINARY_DIR} ../build ../../build
                         NO_DEFAULT_PATH)

if(NOT MaidSafeExport)
  set(ErrorMessage "\n\nCan't find maidsafe_export.cmake in MAIDSAFE_BINARY_DIR.  ")
  set(ErrorMessage "${ErrorMessage}Currently MAIDSAFE_BINARY_DIR is set to ")
  set(ErrorMessage "${ErrorMessage}\"${MAIDSAFE_BINARY_DIR}\"  It must be set to the MaidSafe ")
  set(ErrorMessage "${ErrorMessage}super-project's build root.\nTo set it, run:\n")
  set(ErrorMessage "${ErrorMessage}    cmake . -DMAIDSAFE_BINARY_DIR=\"<path to build root>\"\n\n")
  message(FATAL_ERROR "${ErrorMessage}")
else()
  get_filename_component(ExportDir "${MaidSafeExport}" PATH)
  get_filename_component(MAIDSAFE_BINARY_DIR "${ExportDir}" PATH CACHE)
  file(TO_CMAKE_PATH ${MAIDSAFE_BINARY_DIR} MAIDSAFE_BINARY_DIR)
endif()

include(${MaidSafeExport})

if(MSVC)
  set(PACKAGE_BINARY_RELEASE_DIR ${CMAKE_BINARY_DIR}/Release)
  set(PACKAGE_BINARY_DEBUG_DIR ${CMAKE_BINARY_DIR}/Debug)
else()
  set(PACKAGE_BINARY_DIR ${CMAKE_BINARY_DIR})
endif()

if(MSVC)
  if(CMAKE_CL_64)
    set(VC_RUNTIME_DIR "$ENV{VCInstallDir}/redist/x64/Microsoft.VC120.CRT")
  else()
    set(VC_RUNTIME_DIR "$ENV{VCInstallDir}/redist/x86/Microsoft.VC120.CRT")
  endif()
  find_file(MSVCP120 NAMES msvcp120.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
  find_file(MSVCR120 NAMES msvcr120.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
  find_file(VCCORLIB120 NAMES vccorlib120.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
  if(NOT MSVCP120)
    set(ERROR_MESSAGE "\nCould not find library msvcp120.dll.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}\nRun cmake from a Visual Studio Command Prompt.")
    message(FATAL_ERROR "${ERROR_MESSAGE}")
  endif()
endif()

#Removing the previous CPackConfig.cmake & CPackSourceConfig.cmake files
find_file(CPACK_CONFIG_CMAKE_FILES NAMES  CPackConfig.cmake CPackSourceConfig.cmake PATHS ${CMAKE_BINARY_DIR})
file(REMOVE ${CPACK_CONFIG_CMAKE_FILES})

set(CPACK_PACKAGE_VENDOR "MaidSafe")
set(CPACK_PACKAGE_CONTACT "support@maidsafe.net")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MaidSafe Platform Binaries")
#if(NOT APPLE)
#  set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/installer/shared/eula.txt")
#endif()

if(UNIX AND NOT APPLE)
  # Linux requires lower case package names
  set(CPACK_PACKAGE_NAME "LifeStuff")
  set(CMAKE_INSTALL_PREFIX /opt/maidsafe/lifestuff)
  set(CPACK_TOPLEVEL_TAG ${CMAKE_INSTALL_PREFIX})
  set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  install(PROGRAMS ${PACKAGE_BINARY_DIR}/lifestuff DESTINATION /usr/bin)
  install(PROGRAMS ${PACKAGE_BINARY_DIR}/lifestuff-vault DESTINATION /usr/bin)
  install(PROGRAMS ${PACKAGE_BINARY_DIR}/lifestuff-mgr DESTINATION /usr/bin)
  install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/installer/linux/scripts/lifestuff_client.desktop RENAME lifestuff.desktop DESTINATION /usr/share/applications/)
  execute_process(
    COMMAND /usr/bin/dpkg --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    RESULT_VARIABLE EXECUTE_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
  if(EXECUTE_RESULT)
    message(STATUS "Unable to determine current dpkg architecture: ${EXECUTE_RESULT} - will try RPM")
#    set(CPACK_RPM_PACKAGE_REQUIRES("libqtcore4 >= 4:4.7.2-0, libqtgui4 >= 4:4.7.2-0, libfuse2 >= 2.8.4-1")
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/fedora/lifestuff-mgr.service RENAME lifestuff-mgr.service DESTINATION /etc/systemd/user/)
    install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/src/maidsafe/lifestuff/qt_ui/resources/icons/app_icon_linux.png RENAME lifestuff.png DESTINATION /usr/share/maidsafe/lifestuff/resources/icons/)
    install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/src/maidsafe/lifestuff/qt_ui/resources/sounds/launch_lifestuff_gui.wav DESTINATION /usr/share/maidsafe/lifestuff/resources/sounds/)
    install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/src/maidsafe/lifestuff/qt_ui/resources/sounds/notification_alert.wav DESTINATION /usr/share/maidsafe/lifestuff/resources/sounds/)
    install(FILES ${CPACK_RESOURCE_FILE_README} DESTINATION /usr/share/doc/lifestuff/)
    install(FILES ${CPACK_RESOURCE_FILE_LICENSE} DESTINATION /usr/share/doc/lifestuff/)
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${lifestuff_ui_qt_SOURCE_DIR}/installer/linux/scripts/fedora/postinst)
    set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${lifestuff_ui_qt_SOURCE_DIR}/installer/linux/scripts/fedora/prerm)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_GENERATOR RPM)
  else()
    message("Debian package architecture: ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
#    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqtcore4 (>= 4:4.7.2-0), libqtgui4 (>= 4:4.7.2-0), libfuse2 (>= 2.8.4-1)")
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/ubuntu/daemoniser RENAME lifestuff-mgr DESTINATION /etc/init.d/)
    install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/src/maidsafe/lifestuff/qt_ui/resources/icons/app_icon_linux.png RENAME lifestuff.png DESTINATION /usr/share/maidsafe/lifestuff/resources/icons/)
    install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/src/maidsafe/lifestuff/qt_ui/resources/sounds/launch_lifestuff_gui.wav DESTINATION /usr/share/maidsafe/lifestuff/resources/sounds/)
    install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/src/maidsafe/lifestuff/qt_ui/resources/sounds/notification_alert.wav DESTINATION /usr/share/maidsafe/lifestuff/resources/sounds/)
    install(FILES ${CPACK_RESOURCE_FILE_README} DESTINATION /usr/share/doc/lifestuff/)
    install(FILES ${CPACK_RESOURCE_FILE_LICENSE} DESTINATION /usr/share/doc/lifestuff/)
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${lifestuff_ui_qt_SOURCE_DIR}/installer/linux/scripts/ubuntu/postinst;${lifestuff_ui_qt_SOURCE_DIR}/installer/linux/scripts/ubuntu/prerm") # postinstall and before remove
    set(CPACK_DEBIAN_PACKAGE_SECTION "Network")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    set(CPACK_GENERATOR DEB)
  endif()
  #message(STATUS "Package install directory is set to \"${CPACK_PACKAGE_INSTALL_DIRECTORY}\"")
elseif(APPLE)#TODO
elseif(WIN32)
  set(CPACK_GENERATOR ZIP)
  set(CPACK_PACKAGE_NAME "MaidSafe")

  install(FILES ${OpenMP_DLL} DESTINATION bin)
  install(FILES ${MSVCP120} DESTINATION bin)
  install(FILES ${MSVCR120} DESTINATION bin)
  install(FILES ${VCCORLIB120} DESTINATION bin)
 
  set(CPACK_PACKAGE_RELOCATABLE ON)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}_${CPACK_PACKAGE_VERSION}_${TargetPlatform}_${TargetArchitecture}")
endif()
set(CPACK_STRIP_FILES ON)
