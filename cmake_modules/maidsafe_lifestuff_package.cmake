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


if(LOCAL_TARGETS_ONLY)
  message(STATUS "Currently building the local only version of the GUI")
  message(STATUS "To build the full version, use:          cmake . -DLOCAL_TARGETS_ONLY=OFF")
else()
  message(STATUS "Currently building the full version of the GUI")
  message(STATUS "To build the local only version, use:    cmake . -DLOCAL_TARGETS_ONLY=ON")
endif()


#Removing the previous CPackConfig.cmake & CPackSourceConfig.cmake files
find_file(CPACK_CONFIG_CMAKE_FILES NAMES  CPackConfig.cmake CPackSourceConfig.cmake PATHS ${CMAKE_BINARY_DIR})
file(REMOVE ${CPACK_CONFIG_CMAKE_FILES})
if(${PACKAGE_BUILD})
  if(LOCAL_TARGETS_ONLY)
    message(FATAL_ERROR "You need to specify -DLOCAL_TARGETS_ONLY=OFF to make a package.")
  endif()
  set(CPACK_PACKAGE_VENDOR "MaidSafe")
  set(CPACK_PACKAGE_CONTACT "support@maidsafe.net")
  set(CPACK_PACKAGE_DESCRIPTION_SUMMARY " LifeStuff")
  if(NOT APPLE)
    set(CPACK_RESOURCE_FILE_README "${PROJECT_SOURCE_DIR}/README")
    set(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/licence.txt")
  endif()
  set(CPACK_PACKAGE_EXECUTABLES "lifestuff_gui", "LifeStuff")
  set(CPACK_STRIP_FILES "true")
  if(UNIX AND NOT APPLE)
    # Linux requires lower case package names
    set(CPACK_PACKAGE_NAME "LifeStuff")
    set(CMAKE_INSTALL_PREFIX /opt/maidsafe/lifestuff)
    set(CPACK_TOPLEVEL_TAG ${CMAKE_INSTALL_PREFIX})
    set(CPACK_PACKAGING_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX})
    install(TARGETS lifestuff_gui_local RUNTIME DESTINATION . CONFIGURATIONS Release)
    #install(TARGETS pd-vault RUNTIME DESTINATION ../vault CONFIGURATIONS Release)
    install(PROGRAMS ${CMAKE_BINARY_DIR}/pd-vault RUNTIME DESTINATION ../vault CONFIGURATIONS Release)
    install(FILES ${PROJECT_SOURCE_DIR}/installer/linux/scripts/lifestuff_client.desktop DESTINATION .)
    install(FILES ${PROJECT_SOURCE_DIR}/installer/common/icons/WinLinux/app_icon.ico DESTINATION .)
    install(PROGRAMS ${pd_SOURCE_DIR}/installer/linux/scripts/postinst RENAME vault_postinst DESTINATION ../vault)
    install(PROGRAMS ${pd_SOURCE_DIR}/installer/linux/scripts/prem RENAME vault_prem DESTINATION ../vault)
    install(PROGRAMS ${pd_SOURCE_DIR}/installer/linux/scripts/daemoniser RENAME vault_daemoniser DESTINATION ../vault)
    execute_process(
      COMMAND /usr/bin/dpkg --print-architecture
      OUTPUT_VARIABLE CPACK_DEBIAN_PACKAGE_ARCHITECTURE
      RESULT_VARIABLE EXECUTE_RESULT
      OUTPUT_STRIP_TRAILING_WHITESPACE
      ERROR_QUIET)
    if(EXECUTE_RESULT)
      message(STATUS "Unable to determine current dpkg architecture: ${EXECUTE_RESULT} - will try RPM")
      set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${PROJECT_SOURCE_DIR}/installer/linux/scripts/postinst)
      set(CPACK_RPM_PRE_UNINSTALL_SCRIPT_FILE ${PROJECT_SOURCE_DIR}/installer/linux/scripts/prerm)
      set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
      set(CPACK_GENERATOR RPM;TGZ)
    else()
      message("Debian package architecture: ${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
      set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${PROJECT_SOURCE_DIR}/installer/linux/scripts/postinst;${PROJECT_SOURCE_DIR}/installer/linux/scripts/prerm") # postinstall and before remove
      set(CPACK_DEBIAN_PACKAGE_SECTION "Network")
      set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}${CPACK_DEBIAN_PACKAGE_ARCHITECTURE}")
      set(CPACK_GENERATOR DEB;TGZ)
    endif()
    #message(STATUS "Package install directory is set to \"${CPACK_PACKAGE_INSTALL_DIRECTORY}\"")
  elseif(APPLE)
    install(TARGETS lifestuff_gui_local EXPORT lifestuff_gui_local
              BUNDLE DESTINATION /Applications/ COMPONENT Runtime
              RUNTIME DESTINATION /Applications/
              CONFIGURATIONS Release)
    set(CPACK_SET_DESTDIR TRUE)
    set(CPACK_GENERATOR PackageMaker)
  elseif(WIN32)
    set(CPACK_PACKAGE_NAME "LifeStuff")
    # Set the CMAKE_MODULE_PATH to include the path to our modified NSIS.template.in file before the default CMake one.
    set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/package/Windows ${CMAKE_MODULE_PATH})
    
    
    install(TARGETS lifestuff_gui_local RUNTIME DESTINATION bin CONFIGURATIONS Release)
    install(PROGRAMS ${CMAKE_BINARY_DIR}/Release/cbfs_installer RUNTIME DESTINATION bin CONFIGURATIONS Release)
    #install(TARGETS cbfs_installer RUNTIME DESTINATION bin CONFIGURATIONS Release)
    message("CMAKE_BINARY_DIR --- ${CMAKE_BINARY_DIR}")
    #install(PROGRAMS ${CMAKE_BINARY_DIR}/Release/pd-vault-service RUNTIME DESTINATION bin CONFIGURATIONS Release)
    install(TARGETS pd-vault-service RUNTIME DESTINATION bin CONFIGURATIONS Release)
    install(DIRECTORY ${drive_SOURCE_DIR}/drivers/cbfs/windows DESTINATION drivers)
    install(FILES ${OpenMP_DLL} DESTINATION bin)
    set(CPACK_PACKAGE_INSTALL_DIRECTORY "MaidSafe\\\\LifeStuff")
    set(CPACK_GENERATOR NSIS)
    set(CPACK_PACKAGE_INSTALL_REGISTRY_KEY "LifeStuff")
    set(CPACK_NSIS_MUI_ICON "${PROJECT_SOURCE_DIR}/installer/common/icons/WinLinux/installer.ico")
    set(CPACK_NSIS_MUI_UNIICON "${PROJECT_SOURCE_DIR}/installer/common/icons/WinLinux/uninstaller.ico")
    set(CPACK_NSIS_EXTRA_INSTALL_COMMANDS "")
    set(CPACK_NSIS_EXTRA_UNINSTALL_COMMANDS "")
    set(CPACK_NSIS_MODIFY_PATH ON)
    set(CPACK_NSIS_DISPLAY_NAME "LifeStuff")
    set(CPACK_NSIS_PACKAGE_NAME "LifeStuff")
    set(CPACK_NSIS_INSTALLED_ICON_NAME "bin\\\\lifestuff_gui_local.exe")
    set(CPACK_NSIS_EXECUTABLES_DIRECTORY "bin")
    set(CPACK_NSIS_HELP_LINK "http://www.maidsafe.net")
    set(CPACK_NSIS_URL_INFO_ABOUT "http://maidsafe.net/lifestuff.html")
    set(CPACK_NSIS_CONTACT "support@maidsafe.net")
    set(CPACK_PACKAGE_RELOCATABLE ON)
    set(CPACK_NSIS_MENU_LINKS "http://maidsafe.net/lifestuff.html" "About LifeStuff")
  endif()
  set(CPACK_STRIP_FILES ON)
  set(CPACK_PACKAGE_CONTACT "support@maidsafe.net")
  set(CPACK_PACKAGE_EXECUTABLES "lifestuff_gui_local" "LifeStuff")
  include(CPack)
endif()
















###################################################################################################
# Install details                                                                                 #
###################################################################################################
# install(TARGETS pd-vault DESTINATION bin CONFIGURATIONS Release)
# if(WIN32)
#   install(TARGETS pd-vault-service DESTINATION bin CONFIGURATIONS Release)
# else()
#   install(PROGRAMS ${PROJECT_SOURCE_DIR}/installer/linux/scripts/postinst DESTINATION script RENAME vault_postinst CONFIGURATIONS Release)
#   install(PROGRAMS ${PROJECT_SOURCE_DIR}/installer/linux/scripts/prem DESTINATION script RENAME vault_prem CONFIGURATIONS Release)
#   install(PROGRAMS ${PROJECT_SOURCE_DIR}/installer/linux/scripts/daemoniser DESTINATION script RENAME vault_daemoniser CONFIGURATIONS Release)
# endif()
