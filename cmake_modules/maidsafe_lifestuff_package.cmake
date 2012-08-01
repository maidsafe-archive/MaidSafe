#==============================================================================#
#                                                                              #
#  Copyright (c) 2012 MaidSafe.net limited                                     #
#                                                                              #
#  The following source code is property of MaidSafe.net limited and is not    #
#  meant for external use.  The use of this code is governed by the license    #
#  file licence.txt found in the root directory of this project and also on    #
#  www.maidsafe.net.                                                           #
#                                                                              #
#  You are not free to copy, amend or otherwise use this source code without   #
#  the explicit written permission of the board of directors of MaidSafe.net.  #
#                                                                              #
#==============================================================================#

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  message(FATAL_ERROR "\n Package build is not allowed in Debug build type !!!
          \n-- To build the package, use: \n cmake . -DCMAKE_BUILD_TYPE=Release -DPACKAGE_BUILD=ON \n")
endif()

# NOTE : This variable must be always used to pick binaries for package build to avoid accidental debug build inclusion.
#set(PACKAGE_BINARY_DIR ${CMAKE_BINARY_DIR}/package/bin/Release)
if (MSVC)
  set(PACKAGE_BINARY_DIR ${CMAKE_BINARY_DIR}/Release)
else()
  set(PACKAGE_BINARY_DIR ${CMAKE_BINARY_DIR})
endif()

if(MSVC)
  if(CMAKE_CL_64)
    set(VC_RUNTIME_DIR "$ENV{VCInstallDir}/redist/x64/Microsoft.VC110.CRT")
  else()
    set(VC_RUNTIME_DIR "$ENV{VCInstallDir}/redist/x86/Microsoft.VC110.CRT")
  endif()
  find_file(MSVCP110 NAMES msvcp110.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
  find_file(MSVCR110 NAMES msvcr110.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
  find_file(VCCORLIB110 NAMES vccorlib110.dll PATHS ${VC_RUNTIME_DIR} NO_DEFAULT_PATH)
  if(NOT MSVCP110)
    set(ERROR_MESSAGE "\nCould not find library msvcp110.dll.")
    set(ERROR_MESSAGE "${ERROR_MESSAGE}\nRun cmake from a Visual Studio Command Prompt.")
    message(FATAL_ERROR "${ERROR_MESSAGE}")
  endif()
endif()

#Removing the previous CPackConfig.cmake & CPackSourceConfig.cmake files
find_file(CPACK_CONFIG_CMAKE_FILES NAMES  CPackConfig.cmake CPackSourceConfig.cmake PATHS ${CMAKE_BINARY_DIR})
file(REMOVE ${CPACK_CONFIG_CMAKE_FILES})

if(LOCAL_TARGETS_ONLY)
  message(FATAL_ERROR "You need to specify -DLOCAL_TARGETS_ONLY=OFF to make a package.")
endif()
set(CPACK_PACKAGE_VENDOR "MaidSafe")
set(CPACK_PACKAGE_CONTACT "support@maidsafe.net")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY " LifeStuff")
if(NOT APPLE)
  set(CPACK_RESOURCE_FILE_README "${lifestuff_gui_SOURCE_DIR}/README")
  set(CPACK_RESOURCE_FILE_LICENSE "${lifestuff_gui_SOURCE_DIR}/licence.txt")
endif()
#set(CPACK_PACKAGE_EXECUTABLES "lifestuff" "LifeStuff")
#set(CPACK_STRIP_FILES "true")

if(UNIX AND NOT APPLE)
  # Linux requires lower case package names
  set(CPACK_PACKAGE_NAME "LifeStuff")
  set(CMAKE_INSTALL_PREFIX /opt/maidsafe/lifestuff)
  set(CPACK_TOPLEVEL_TAG ${CMAKE_INSTALL_PREFIX})
  set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
  install(PROGRAMS ${PACKAGE_BINARY_DIR}/lifestuff_local DESTINATION .)
  install(PROGRAMS ${PACKAGE_BINARY_DIR}/pd-vault DESTINATION ../vault)
  install(PROGRAMS ${PACKAGE_BINARY_DIR}/vault-manager DESTINATION ../vault)
  install(FILES ${lifestuff_gui_SOURCE_DIR}/installer/linux/scripts/lifestuff_client.desktop DESTINATION .)
  install(FILES ${lifestuff_gui_SOURCE_DIR}/installer/common/icons/WinLinux/app_icon.ico DESTINATION .)
  install(FILES ${lifestuff_gui_SOURCE_DIR}/installer/common/bootstrap DESTINATION $ENV{HOME}/.config/maidsafe/lifestuff)
  install(FILES ${lifestuff_gui_SOURCE_DIR}/installer/common/bootstrap RENAME bootstrap.vault_manager DESTINATION /usr/share/maidsafe/lifestuff)
  execute_process(
    COMMAND /usr/bin/dpkg --print-architecture
    OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
    RESULT_VARIABLE EXECUTE_RESULT
    OUTPUT_STRIP_TRAILING_WHITESPACE
    ERROR_QUIET)
  if(EXECUTE_RESULT)
    message(STATUS "Unable to determine current dpkg architecture: ${EXECUTE_RESULT} - will try RPM")
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/fedora/postinst RENAME vault_manager_postinst DESTINATION ../vault)
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/fedora/prerm RENAME vault_manager_prerm DESTINATION ../vault)
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/fedora/vault_manager.service DESTINATION ../vault)
    set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${lifestuff_gui_SOURCE_DIR}/installer/linux/scripts/fedora/postinst)
    set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${lifestuff_gui_SOURCE_DIR}/installer/linux/scripts/fedora/prerm)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
    set(CPACK_GENERATOR RPM)
  else()
    message("Debian package architecture: ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    install(PROGRAMS ${pd_SOURCE_DIR}/installer/linux/scripts/postinst RENAME vault_postinst DESTINATION ../vault)
    install(PROGRAMS ${pd_SOURCE_DIR}/installer/linux/scripts/prerm RENAME vault_prerm DESTINATION ../vault)
    install(PROGRAMS ${pd_SOURCE_DIR}/installer/linux/scripts/daemoniser RENAME vault_daemoniser DESTINATION ../vault)
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/ubuntu/postinst RENAME vault_manager_postinst DESTINATION ../vault)
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/ubuntu/prerm RENAME vault_manager_prerm DESTINATION ../vault)
    install(PROGRAMS ${private_SOURCE_DIR}/installer/linux/scripts/ubuntu/daemoniser RENAME vault_manager_daemoniser DESTINATION ../vault)
    set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${lifestuff_gui_SOURCE_DIR}/installer/linux/scripts/postinst;${lifestuff_gui_SOURCE_DIR}/installer/linux/scripts/prerm") # postinstall and before remove
    set(CPACK_DEBIAN_PACKAGE_SECTION "Network")
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
    set(CPACK_GENERATOR DEB)
  endif()
  #message(STATUS "Package install directory is set to \"${CPACK_PACKAGE_INSTALL_DIRECTORY}\"")
elseif(APPLE)#TODO
  #install(TARGETS lifestuff_gui_local EXPORT lifestuff_gui_local
  #          BUNDLE DESTINATION /Applications/ COMPONENT Runtime
  #          RUNTIME DESTINATION /Applications/
  #          CONFIGURATIONS Release)
  #set(CPACK_SET_DESTDIR TRUE)
  #set(CPACK_GENERATOR PackageMaker)
elseif(WIN32)
  set(CPACK_PACKAGE_NAME "LifeStuff")
  # Set the CMAKE_MODULE_PATH to include the path to our modified NSIS.template.in file before the default CMake one.
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/package/Windows ${CMAKE_MODULE_PATH})
  install(FILES ${PACKAGE_BINARY_DIR}/lifestuff_local.exe DESTINATION bin)
  install(FILES ${PACKAGE_BINARY_DIR}/cbfs_installer.exe DESTINATION bin)
#  install(FILES ${PACKAGE_BINARY_DIR}/pd-vault-service.exe DESTINATION bin)
  install(FILES ${drive_SOURCE_DIR}/drivers/windows/cbfs/cbfs.cab DESTINATION drivers/cbfs)
  if(CMAKE_CL_64)
    install(FILES ${drive_SOURCE_DIR}/drivers/windows/cbfs/64bit/x64/cbfsinst.dll DESTINATION drivers/cbfs)
  else()
    install(FILES ${drive_SOURCE_DIR}/drivers/windows/cbfs/32bit/cbfsinst.dll DESTINATION drivers/cbfs)
  endif()
  install(FILES ${lifestuff_gui_SOURCE_DIR}/installer/common/bootstrap DESTINATION .)
  install(FILES ${OpenMP_DLL} DESTINATION bin)
  install(FILES ${QtLibsRelease} DESTINATION bin)
  install(FILES ${QtImageFormatsPluginsRelease} DESTINATION bin/plugins/imageformats)
  install(FILES ${MSVCP110} DESTINATION bin)
  install(FILES ${MSVCR110} DESTINATION bin)
  install(FILES ${VCCORLIB110} DESTINATION bin)
  install(FILES ${CMAKE_BINARY_DIR}/Release/resources/connect_welcome_screen.rcc DESTINATION bin/resources)
  install(FILES ${CMAKE_BINARY_DIR}/Release/resources/lifestuff_tour.rcc DESTINATION bin/resources)

  set(CPACK_PACKAGE_INSTALL_DIRECTORY "MaidSafe\\\\LifeStuff")
  set(CPACK_GENERATOR NSIS)
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "LifeStuff")
  set(CPACK_NSIS_MUI_ICON "${lifestuff_gui_SOURCE_DIR}/installer/common/icons/WinLinux/installer.ico")
  set(CPACK_NSIS_MUI_UNIICON "${lifestuff_gui_SOURCE_DIR}/installer/common/icons/WinLinux/uninstaller.ico")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "")
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_NSIS_DISPLAY_NAME "LifeStuff")
  set(CPACK_NSIS_PACKAGE_NAME "LifeStuff")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\lifestuff_local.exe")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
  set(CPACK_NSIS_HELP_LINK "http://www.maidsafe.net")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://maidsafe.net/lifestuff.html")
  set(CPACK_NSIS_CONTACT "support@maidsafe.net")
  set(CPACK_PACKAGE_RELOCATABLE ON)
  set(CPACK_NSIS_MENU_LINKS "http://maidsafe.net/lifestuff.html" "About LifeStuff")
endif()
set(CPACK_STRIP_FILES ON)
set(CPACK_PACKAGE_CONTACT "support@maidsafe.net")
set(CPACK_PACKAGE_EXECUTABLES "lifestuff" "LifeStuff")
