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

set(QtNeedsConfigured TRUE)
if(BUILD_QT)
  # Building to current project's build tree - create new directory for Qt binaries
  set(QT_BUILD_DIR ${PROJECT_BINARY_DIR}/build_qt)
  execute_process(COMMAND ${CMAKE_COMMAND} -E make_directory ${QT_BUILD_DIR})
  find_file(ConfigureCache NAMES configure.cache PATHS ${QT_BUILD_DIR} NO_DEFAULT_PATH)
  if(ConfigureCache)
    set(QtNeedsConfigured FALSE)
  endif()
  message(STATUS "About to build Qt to ${QT_BUILD_DIR}")
  message(STATUS "To build Qt in-source instead, run cmake . -DBUILD_QT_IN_SOURCE=ON")
elseif(BUILD_QT_IN_SOURCE)
  # Building inside Qt source tree - run "confclean" in case the source has been built to previously
  set(QT_BUILD_DIR ${QT_SRC_DIR})
  message(STATUS "About to build Qt in-source.")
  message(STATUS "To build Qt to ${QT_BUILD_DIR} instead, run cmake . -DBUILD_QT=ON")
  if(WIN32)
    execute_process(COMMAND cmd /c "nmake /S /NOLOGO confclean" WORKING_DIRECTORY ${QT_BUILD_DIR})
  else()
    execute_process(COMMAND make confclean WORKING_DIRECTORY ${QT_BUILD_DIR})
  endif()
else()
  message(FATAL_ERROR "This module should not be invoked.")
endif()

if(WIN32)
  # Modify the qmake.conf to identify Visual Studio 2012 as the version, to set
  # flag /Zm1000, and to build a 64-bit version if required.
  find_file(QmakeConf NAMES qmake.conf PATHS ${QT_SRC_DIR}/mkspecs/win32-msvc2010 NO_DEFAULT_PATH)
  file(READ ${QmakeConf} QmakeConfContents)
  string(REGEX REPLACE "_MSC_VER=[0-9]+ " "_MSC_VER=1700 " QmakeConfContents ${QmakeConfContents})
  string(REGEX REPLACE "-Zm[0-9]+ " "-Zm1000 " QmakeConfContents ${QmakeConfContents})

  # Modify HashSet.h in webkit to allow compilation with Visual Studio 2012 RC
  find_file(HashSetH NAMES hashset.h PATHS ${QT_SRC_DIR}/src/3rdparty/webkit/Source/JavaScriptCore/wtf NO_DEFAULT_PATH)
  file(READ ${HashSetH} HashSetContents)
  string(REPLACE "return m_impl.add(value);" "typedef typename HashSet<T, U, V>::iterator iter_type;\n        auto& temp = m_impl.add(value);\n        return make_pair((iter_type)temp.first, temp.second);" HashSetContents "${HashSetContents}")
  string(REPLACE "return m_impl.template addPassingHashCode<T, T, Adapter>(value, value);" "typedef typename HashSet<Value, HashFunctions, Traits>::iterator iter_type;\n        auto& temp = m_impl.template addPassingHashCode<T, T, Adapter>(value, value);\n        return make_pair((iter_type)temp.first, temp.second);" HashSetContents "${HashSetContents}")
  file(WRITE ${HashSetH} "${HashSetContents}")

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
  set(QtConfigureCommand "${QtConfigureCommand} -debug-and-release")
  set(QtConfigureCommand "${QtConfigureCommand} -opensource")
  set(QtConfigureCommand "${QtConfigureCommand} -confirm-license") 
  set(QtConfigureCommand "${QtConfigureCommand} -shared")
  set(QtConfigureCommand "${QtConfigureCommand} -ltcg")
  set(QtConfigureCommand "${QtConfigureCommand} -no-qt3support")
  set(QtConfigureCommand "${QtConfigureCommand} -platform win32-msvc2010")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-zlib")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libpng")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libmng")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libtiff")
  set(QtConfigureCommand "${QtConfigureCommand} -qt-libjpeg")
  set(QtConfigureCommand "${QtConfigureCommand} -nomake demos")
  set(QtConfigureCommand "${QtConfigureCommand} -nomake examples")

  if(QtNeedsConfigured)
    message(STATUS "Configuring Qt.")
    execute_process(COMMAND cmd /c "${QtSetupCommand} && ${QtConfigureCommand}"
                    WORKING_DIRECTORY ${QT_BUILD_DIR}
                    RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
    if(NOT ResultVar EQUAL 0)
      message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
      message(FATAL_ERROR "\nConfiguring Qt failed.  Command was:\n${QtSetupCommand} && ${QtConfigureCommand}")
    endif()
  else()
    message(STATUS "Qt has already been configured (found ${ConfigureCache})")
    message(STATUS "To force reconfiguration, delete ${QT_BUILD_DIR}")
  endif()

  set(QtNmakeCommand "nmake /S /NOLOGO")
  set(QtSubModules tools-bootstrap moc rcc uic winmain corelib gui sql svg network xml xmlpatterns opengl phonon script testlib declarative multimedia plugins webkit)
  foreach(QtSubModule ${QtSubModules})
    message(STATUS "Building Qt submodule ${QtSubModule}")
    execute_process(COMMAND cmd /c "${QtSetupCommand} && ${QtNmakeCommand} sub-${QtSubModule}"
                    WORKING_DIRECTORY ${QT_BUILD_DIR}
                    RESULT_VARIABLE ResultVar OUTPUT_VARIABLE OutputVar ERROR_VARIABLE ErrorVar)
    if(NOT ResultVar EQUAL 0)
      message("\n${HR}\n${ErrorVar}\n\n\n${HR}\n${OutputVar}\n\n\n${HR}\n")
      message(FATAL_ERROR "\nBuilding Qt failed.  Command was:\n${QtSetupCommand} && ${QtNmakeCommand} sub-${QtSubModule}")
    endif()
  endforeach()

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
