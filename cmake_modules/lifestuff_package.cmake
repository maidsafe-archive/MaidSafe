#==================================================================================================#
#                                                                                                  #
#  Copyright (c) 2012 MaidSafe.net limited                                                         #
#                                                                                                  #
#  The following source code is property of MaidSafe.net limited and is not meant for external     #
#  use.  The use of this code is governed by the license file licence.txt found in the root        #
#  directory of this project and also on www.maidsafe.net.                                         #
#                                                                                                  #
#  You are not free to copy, amend or otherwise use this source code without the explicit written  #
#  permission of the board of directors of MaidSafe.net.                                           #
#                                                                                                  #
#==================================================================================================#


if(NOT MSVC)
  set(ErrorMessage "Package build must be in Release mode.\nrun:      cmake . -DCMAKE_BUILD_TYPE=Release -DPACKAGE_BUILD=ON\n\n")
  if(NOT CMAKE_BUILD_TYPE)
    message(FATAL_ERROR ${ErrorMessage})
  elseif(NOT ${CMAKE_BUILD_TYPE} STREQUAL "Release")
    message(FATAL_ERROR ${ErrorMessage})
  endif()
endif()

# NOTE : This variable must be always used to pick binaries for package build to avoid accidental debug build inclusion.
#set(PACKAGE_BINARY_DIR ${CMAKE_BINARY_DIR}/package/bin/Release)
if(MSVC)
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
  # set(CPACK_RESOURCE_FILE_README "${lifestuff_ui_qt_SOURCE_DIR}/installer/text/lifestuff/readme.txt")
  set(CPACK_RESOURCE_FILE_LICENSE "${lifestuff_ui_qt_SOURCE_DIR}/installer/text/lifestuff/eula.txt")
endif()
#set(CPACK_PACKAGE_EXECUTABLES "lifestuff" "LifeStuff")
#set(CPACK_STRIP_FILES "true")

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
  #install(TARGETS lifestuff_ui_qt_local EXPORT lifestuff_ui_qt_local
  #          BUNDLE DESTINATION /Applications/ COMPONENT Runtime
  #          RUNTIME DESTINATION /Applications/
  #          CONFIGURATIONS Release)
  #set(CPACK_SET_DESTDIR TRUE)
  #set(CPACK_GENERATOR PackageMaker)
elseif(WIN32)
  set(CPACK_PACKAGE_NAME "LifeStuff")
  # Set the CMAKE_MODULE_PATH to include the path to our modified NSIS.template.in file before the default CMake one.
  set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/package/Windows ${CMAKE_MODULE_PATH})
  install(FILES ${PACKAGE_BINARY_DIR}/lifestuff.exe DESTINATION .)
  install(FILES ${PACKAGE_BINARY_DIR}/cbfs_driver.exe DESTINATION .)
  install(FILES ${PACKAGE_BINARY_DIR}/lifestuff_mgr.exe DESTINATION .)
  install(FILES ${PACKAGE_BINARY_DIR}/lifestuff_vault.exe DESTINATION .)
  install(FILES ${drive_SOURCE_DIR}/drivers/windows/cbfs/cbfs.cab DESTINATION driver)
  if(CMAKE_CL_64)
    install(FILES ${drive_SOURCE_DIR}/drivers/windows/cbfs/64bit/x64/cbfsinst.dll DESTINATION driver)
  else()
    install(FILES ${drive_SOURCE_DIR}/drivers/windows/cbfs/32bit/cbfsinst.dll DESTINATION driver)
  endif()
  install(FILES ${lifestuff_ui_qt_SOURCE_DIR}/installer/text/lifestuff/eula.txt DESTINATION .)
  install(FILES ${OpenMP_DLL} DESTINATION .)
  install(FILES ${MSVCP110} DESTINATION .)
  install(FILES ${MSVCR110} DESTINATION .)
  install(FILES ${VCCORLIB110} DESTINATION .)
  install(FILES ${QtLibsRelease} DESTINATION .)
  install(FILES ${QtImageFormatsPluginsRelease} DESTINATION plugins/imageformats)
  # install(FILES ${CMAKE_BINARY_DIR}/Release/resources/lifestuff_tour.rcc DESTINATION resources)
  # install(FILES ${CMAKE_BINARY_DIR}/Release/sounds/launch_lifestuff_gui.wav DESTINATION sounds)
  # install(FILES ${CMAKE_BINARY_DIR}/Release/sounds/notification_alert.wav DESTINATION sounds)

  set(CPACK_PACKAGE_INSTALL_DIRECTORY "maidsafe\\\\lifestuff")
  set(CPACK_GENERATOR NSIS)
  set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "LifeStuff")
  set(CPACK_NSIS_MUI_ICON "${lifestuff_ui_qt_SOURCE_DIR}/installer/common/icons/WinLinux/install_icon.ico")
  set(CPACK_NSIS_MUI_UNIICON "${lifestuff_ui_qt_SOURCE_DIR}/installer/common/icons/WinLinux/uninstall_icon.ico")
  set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "
    !define MUI_HEADERIMAGE_BITMAP \\\"${lifestuff_ui_qt_SOURCE_DIR}\\\\installer\\\\common\\\\images\\\\top_left_image.bmp\\\"
    !define MUI_HEADERIMAGE_UNBITMAP \\\"${lifestuff_ui_qt_SOURCE_DIR}\\\\installer\\\\common\\\\images\\\\top_left_image.bmp\\\"
    !define MUI_WELCOMEFINISHPAGE_BITMAP \\\"${lifestuff_ui_qt_SOURCE_DIR}\\\\installer\\\\common\\\\images\\\\Installer-Image-Beta.bmp\\\"
    !define MUI_UNWELCOMEFINISHPAGE_BITMAP \\\"${lifestuff_ui_qt_SOURCE_DIR}\\\\installer\\\\common\\\\images\\\\Installer-Image-Beta.bmp\\\"
  ")
  set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "")
  set(CPACK_NSIS_MODIFY_PATH ON)
  set(CPACK_NSIS_DISPLAY_NAME "LifeStuff")
  set(CPACK_NSIS_PACKAGE_NAME "LifeStuff")
  set(CPACK_NSIS_INSTALLED_ICON_NAME "lifestuff.exe")
  set(CPACK_NSIS_EXECUTABLES_DIRECTORY ".")
  set(CPACK_NSIS_HELP_LINK "http://www.maidsafe.net")
  set(CPACK_NSIS_URL_INFO_ABOUT "http://maidsafe.net/lifestuff.html")
  set(CPACK_NSIS_CONTACT "support@maidsafe.net")
  set(CPACK_PACKAGE_RELOCATABLE ON)
  set(CPACK_NSIS_MENU_LINKS "http://maidsafe.net/lifestuff.html" "About LifeStuff")
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_NSIS_DISPLAY_NAME}_${CPACK_PACKAGE_VERSION}_${TargetPlatform}_${TargetArchitecture}")
endif()
set(CPACK_STRIP_FILES ON)
set(CPACK_PACKAGE_CONTACT "support@maidsafe.net")
set(CPACK_PACKAGE_EXECUTABLES "lifestuff" "LifeStuff")
