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
#                                                                              #
#  Module used to configure and build Qt.                                      #
#                                                                              #
#  Module will be invoked only if BUILD_QT or BUILD_QT_IN_SOURCE are set ON.   #
#                                                                              #
#==============================================================================#



unset(QtConfigureExe CACHE)
find_program(QtConfigureExe NAMES configure PATHS ${QT_SRC_DIR} NO_DEFAULT_PATH)
if(NOT QtConfigureExe)
  message(FATAL_ERROR "Failed to find configure exe.${QT_ERROR_MESSAGE}")
endif()

if(BUILD_QT)
  # Building to current project's build tree - create new directory for Qt binaries
  set(QT_BUILD_DIR ${PROJECT_BINARY_DIR}/build_qt)
  execute_process(COMMAND ${CMAKE_COMMAND} -E remove_directory ${QT_BUILD_DIR})
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${QT_BUILD_DIR})
elseif(BUILD_QT_IN_SOURCE)
  # Building inside Qt source tree - run "confclean" in case the source has been built to previously
  set(QT_BUILD_DIR ${QT_SRC_DIR})
  if(WIN32)
    execute_process(COMMAND cmd /c "nmake /S /NOLOGO confclean" WORKING_DIRECTORY ${QT_BUILD_DIR})
  else()
    execute_process(COMMAND make confclean WORKING_DIRECTORY ${QT_BUILD_DIR})
  endif()
else()
  message(FATAL_ERROR "This module should not be invoked.")
endif()

if(WIN32)
  # Modify the qmake.conf to identify VS11 as the version, and to build a 64-bit version if required.
  find_file(QmakeConf NAMES qmake.conf PATHS ${QT_SRC_DIR}/mkspecs/win32-msvc2010 NO_DEFAULT_PATH)
  file(READ ${QmakeConf} QmakeConfContents)
  string(REGEX REPLACE "_MSC_VER=[0-9]+ " "_MSC_VER=1700 " QmakeConfContents ${QmakeConfContents})
  if(CMAKE_CL_64)
    string(REGEX REPLACE " /MACHINE:X64" "" QmakeConfContents ${QmakeConfContents})
    string(REGEX REPLACE "QMAKE_LFLAGS ([^\n]+)" "QMAKE_LFLAGS \\1 /MACHINE:X64" QmakeConfContents ${QmakeConfContents})
  endif()
  file(WRITE ${QmakeConf} "${QmakeConfContents}")
  
  set(QtSetupCommand "set PATH=%PATH%;${QT_SRC_DIR}\\bin")
  if(CMAKE_CL_64)
    set(QtSetupCommand "${QtSetupCommand} && set TARGET_CPU=x64")
  endif()

  set(QtConfigureCommand "${QtConfigureExe}")
  set(QtConfigureCommand "${QtConfigureCommand} -opensource")
  set(QtConfigureCommand "${QtConfigureCommand} -confirm-license") 
  set(QtConfigureCommand "${QtConfigureCommand} -shared")
  set(QtConfigureCommand "${QtConfigureCommand} -no-fast")
  set(QtConfigureCommand "${QtConfigureCommand} -exceptions") 
  set(QtConfigureCommand "${QtConfigureCommand} -accessibility") 
  set(QtConfigureCommand "${QtConfigureCommand} -stl")
  set(QtConfigureCommand "${QtConfigureCommand} -no-qt3support")
  set(QtConfigureCommand "${QtConfigureCommand} -opengl desktop")
  set(QtConfigureCommand "${QtConfigureCommand} -no-openvg")
  set(QtConfigureCommand "${QtConfigureCommand} -platform win32-msvc2010")
  set(QtConfigureCommand "${QtConfigureCommand} -graphicssystem raster")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-zlib")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libpng")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libmng")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libtiff")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libjpeg")
  set(QtConfigureCommand "${QtConfigureCommand} -dsp")
  set(QtConfigureCommand "${QtConfigureCommand} -no-vcproj")
  set(QtConfigureCommand "${QtConfigureCommand} -incredibuild-xge")
  set(QtConfigureCommand "${QtConfigureCommand} -plugin-manifests")
  set(QtConfigureCommand "${QtConfigureCommand} -qmake")
  set(QtConfigureCommand "${QtConfigureCommand} -process")
  set(QtConfigureCommand "${QtConfigureCommand} -rtti")
  set(QtConfigureCommand "${QtConfigureCommand} -mmx")
  set(QtConfigureCommand "${QtConfigureCommand} -3dnow")
  set(QtConfigureCommand "${QtConfigureCommand} -sse")
  set(QtConfigureCommand "${QtConfigureCommand} -sse2")
  set(QtConfigureCommand "${QtConfigureCommand} -no-openssl")
  set(QtConfigureCommand "${QtConfigureCommand} -no-dbus")
  set(QtConfigureCommand "${QtConfigureCommand} -phonon")
  set(QtConfigureCommand "${QtConfigureCommand} -phonon-backend")
  set(QtConfigureCommand "${QtConfigureCommand} -multimedia")
  set(QtConfigureCommand "${QtConfigureCommand} -audio-backend")
  set(QtConfigureCommand "${QtConfigureCommand} -script")
  set(QtConfigureCommand "${QtConfigureCommand} -scripttools")
  set(QtConfigureCommand "${QtConfigureCommand} -no-directwrite")
  set(QtConfigureCommand "${QtConfigureCommand} -arch windows")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-style-windows")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-style-windowsxp")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-style-windowsvista")
  set(QtConfigureCommand "${QtConfigureCommand} -no-style-plastique")
  set(QtConfigureCommand "${QtConfigureCommand} -no-style-cleanlooks")
  set(QtConfigureCommand "${QtConfigureCommand} -no-style-motif")
  set(QtConfigureCommand "${QtConfigureCommand} -no-style-cde")
  set(QtConfigureCommand "${QtConfigureCommand} -no-style-windowsce")
  set(QtConfigureCommand "${QtConfigureCommand} -no-style-windowsmobile")
  set(QtConfigureCommand "${QtConfigureCommand} -no-style-s60")
  set(QtConfigureCommand "${QtConfigureCommand} -native-gestures")
  set(QtConfigureCommand "${QtConfigureCommand} -mp")
  set(QtConfigureCommand "${QtConfigureCommand} -nomake demos")
  set(QtConfigureCommand "${QtConfigureCommand} -nomake examples")

  set(QtReleaseConfigureCommand "${QtConfigureCommand} -release")
  set(QtReleaseConfigureCommand "${QtReleaseConfigureCommand} -ltcg")
  set(QtReleaseConfigureCommand "${QtReleaseConfigureCommand} -declarative")
  set(QtReleaseConfigureCommand "${QtReleaseConfigureCommand} -webkit")

  set(QtDebugConfigureCommand "${QtConfigureCommand} -debug")
  set(QtDebugConfigureCommand "${QtDebugConfigureCommand} -declarative-debug")
  set(QtDebugConfigureCommand "${QtDebugConfigureCommand} -webkit-debug")

  set(QtNmakeCommand "nmake /S /NOLOGO")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-tools-bootstrap")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-moc")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-rcc")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-uic")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-winmain")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-corelib")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-plugins")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-xml")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-network")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-sql")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-testlib")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-gui")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-xmlpatterns")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-svg")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-opengl")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-script")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-declarative")
#   set(QtNmakeCommand "${QtNmakeCommand} sub-webkit")


  message(STATUS "Configuring Qt for a Release build.")
  execute_process(COMMAND cmd /c "${QtSetupCommand} && ${QtReleaseConfigureCommand}"
                  WORKING_DIRECTORY ${QT_BUILD_DIR}
                  RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  if(NOT ResultVar EQUAL 0)
    message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
    message(FATAL_ERROR "\nConfiguring Qt for Release failed.  Command was:\n${QtSetupCommand} && ${QtReleaseConfigureCommand}")
  endif()

  message(STATUS "Building Release Qt.  Go get a cup of coffee.")
  execute_process(COMMAND cmd /c "${QtSetupCommand} && ${QtNmakeCommand}"
                  WORKING_DIRECTORY ${QT_BUILD_DIR}
                  RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  if(NOT ResultVar EQUAL 0)
    message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
    message(FATAL_ERROR "\nBuilding Release Qt failed.  Command was:\n${QtSetupCommand} && ${QtNmakeCommand}")
  endif()



  
  message(STATUS "Cleaning Qt configuration.")
  execute_process(COMMAND cmd /c "${QtSetupCommand} && nmake /S /NOLOGO confclean"
                  WORKING_DIRECTORY ${QT_BUILD_DIR}
                  RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  if(NOT ResultVar EQUAL 0)
    message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
    message(FATAL_ERROR "\nCleaning Qt configuration failed.  Command was:\n${QtSetupCommand} && nmake /S /NOLOGO confclean")
  endif()

  message(STATUS "Configuring Qt for a Debug build.")
  execute_process(COMMAND cmd /c "${QtSetupCommand} && ${QtDebugConfigureCommand}"
                  WORKING_DIRECTORY ${QT_BUILD_DIR}
                  RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  if(NOT ResultVar EQUAL 0)
    message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
    message(FATAL_ERROR "\nConfiguring Qt for Debug failed.  Command was:\n${QtSetupCommand} && ${QtDebugConfigureCommand}")
  endif()

  message(STATUS "Building Debug Qt.  Go get another cup of coffee.")
  execute_process(COMMAND cmd /c "${QtSetupCommand} && ${QtNmakeCommand}"
                  WORKING_DIRECTORY ${QT_BUILD_DIR}
                  RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  if(NOT ResultVar EQUAL 0)
    message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
    message(FATAL_ERROR "\nBuilding Debug Qt failed.  Command was:\n${QtSetupCommand} && ${QtNmakeCommand}")
  endif()

else()

  if(APPLE)
    set(QtConfigurePlatform "-platform")
    set(QtConfigurePlatformValue "unsupported/macx-clang")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
    set(QtConfigurePlatform "-platform")
    set(QtConfigurePlatformValue "unsupported/linux-clang")
  endif()
  
  message(STATUS "Configuring Qt for a Release build.")
  execute_process(COMMAND ${QtConfigureExe} -prefix ${QT_BUILD_DIR}
                                            -opensource
                                            -confirm-license
                                            -shared
                                            -no-qt3support
                                            -graphicssystem raster
                                            -qt-zlib
                                            -qt-libpng
                                            -qt-libmng
                                            -qt-libtiff
                                            -qt-libjpeg
                                            -webkit
                                            -no-multimedia
                                            -no-phonon
                                            -no-phonon-backend
                                            -no-openvg
                                            -no-openssl
                                            -no-dbus
                                            -optimized-qmake
                                            -no-gtkstyle
                                            -no-xinerama
                                            -make libs
                                            ${QtConfigurePlatform} ${QtConfigurePlatformValue}
                  WORKING_DIRECTORY ${QT_BUILD_DIR}
                  RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  if(NOT ResultVar EQUAL 0)
    message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
    message(FATAL_ERROR "\nConfiguring Qt for Release failed. --- ${ResultVar}")
  endif()
  
  message(STATUS "Building Release Qt.  Go get a cup of coffee.")
  execute_process(COMMAND make "-j7"
                  WORKING_DIRECTORY ${QT_BUILD_DIR}
                  RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
  if(NOT ResultVar EQUAL 0)
    message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
    message(FATAL_ERROR "\nBuilding Release Qt failed.  Command was:\nmake -j7 --- ${ResultVar}")
  endif()

endif()

set(QT_ROOT_DIR ${QT_BUILD_DIR} CACHE PATH "Path to built Qt libraries' root directory" FORCE)
unset(QT_SRC_DIR CACHE)
unset(BUILD_QT CACHE)
unset(BUILD_QT_IN_SOURCE CACHE)
