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


set(CMAKE_DISABLE_SOURCE_CHANGES ON)
set(CMAKE_DISABLE_IN_SOURCE_BUILD ON)

cmake_minimum_required(VERSION 2.8.7 FATAL_ERROR)

project(glog)
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/src")
INCLUDE ( FindGTest )
INCLUDE ( CTest )

INCLUDE_DIRECTORIES ( src )

INCLUDE_DIRECTORIES ( ${GTEST_INCLUDE_DIR} )

ADD_LIBRARY ( glog
  src/logging.cc
  src/raw_logging.cc
  src/vlog_is_on.cc
  src/utilities.cc
  src/demangle.cc
  src/symbolize.cc
  src/signalhandler.cc
  )

ADD_EXECUTABLE ( logging_unittest src/logging_unittest.cc )
TARGET_LINK_LIBRARIES ( logging_unittest glog ${GTEST_LIBRARY} pthread )

ADD_EXECUTABLE ( demangle_unittest src/demangle_unittest.cc )
TARGET_LINK_LIBRARIES ( demangle_unittest glog ${GTEST_LIBRARY} pthread )

ADD_EXECUTABLE ( stacktrace_unittest src/stacktrace_unittest.cc )
TARGET_LINK_LIBRARIES ( stacktrace_unittest glog  ${GTEST_LIBRARY} pthread )

ADD_EXECUTABLE ( symbolize_unittest src/symbolize_unittest.cc )
TARGET_LINK_LIBRARIES ( symbolize_unittest glog ${GTEST_LIBRARY} pthread  )

ADD_EXECUTABLE ( stl_logging_unittest src/stl_logging_unittest.cc )
TARGET_LINK_LIBRARIES ( stl_logging_unittest glog ${GTEST_LIBRARY} pthread  )

ADD_EXECUTABLE ( utilities_unittest src/utilities_unittest.cc )
TARGET_LINK_LIBRARIES ( utilities_unittest glog ${GTEST_LIBRARY} pthread  )

ADD_EXECUTABLE ( logging_striptest0 src/logging_striptest_main.cc )
TARGET_LINK_LIBRARIES ( logging_striptest0 glog ${GTEST_LIBRARY} pthread  )

ADD_EXECUTABLE ( logging_striptest2 src/logging_striptest2.cc )
TARGET_LINK_LIBRARIES ( logging_striptest2 glog ${GTEST_LIBRARY} pthread  )

ADD_EXECUTABLE ( logging_striptest10 src/logging_striptest10.cc )
TARGET_LINK_LIBRARIES ( logging_striptest10 glog ${GTEST_LIBRARY} pthread  )

ADD_EXECUTABLE ( signalhandler_unittest src/signalhandler_unittest.cc )
TARGET_LINK_LIBRARIES ( signalhandler_unittest glog ${GTEST_LIBRARY} pthread  )

ADD_TEST ( LoggingUnittest logging_unittest )

ADD_TEST ( DemangleUnittest demangle_unittest )

ADD_TEST ( StacktraceUnittest stacktrace_unittest )

ADD_TEST ( SymbolizeUnittest symbolize_unittest )

ADD_TEST ( STLLoggingUnittest stl_logging_unittest )

ADD_TEST ( UtilitiesUnittest utilities_unittest )

ADD_TEST ( LoggingStriptest0 logging_striptest0 )

ADD_TEST ( LoggingStriptest2 logging_striptest2 )

ADD_TEST ( LoggingStriptest10 logging_striptest10 )

ADD_TEST ( SignalHandlerUnittest signalhandler_unittest )

# installation to glog subdirectory
# logging.h  log_severity.h  raw_logging.h  stl_logging.h  vlog_is_on.h
INSTALL ( TARGETS glog ARCHIVE DESTINATION ${CMAKE_INSTALL_PREFIX}/lib )
INSTALL ( FILES
  src/glog/logging.h
  src/glog/log_severity.h
  src/glog/raw_logging.h
  src/glog/stl_logging.h
  src/glog/vlog_is_on.h
  DESTINATION ${CMAKE_INSTALL_PREFIX}/glog )

