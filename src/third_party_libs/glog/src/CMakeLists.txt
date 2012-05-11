if(WIN32)
  include_directories(windows)
else()
  include_directories(.)
endif()

include(${PROJECT_SOURCE_DIR}/cmake_modules/maidsafe_standard_flags.cmake)

if(WIN32)
  add_library(glog
              base/commandlineflags.h
              windows/config.h
              base/googleinit.h
              windows/glog/log_severity.h
              logging.cc
              windows/glog/logging.h
              base/mutex.h
              windows/port.cc
              windows/port.h
              raw_logging.cc
              windows/glog/raw_logging.h
              windows/glog/stl_logging.h
              utilities.cc
              utilities.h
              vlog_is_on.cc
              windows/glog/vlog_is_on.h)
else()
  add_library(glog
              demangle.cc
              logging.cc
              raw_logging.cc
              signalhandler.cc
              symbolize.cc
              utilities.cc
              vlog_is_on.cc)
endif()

set_target_properties(glog PROPERTIES FOLDER "Third Party/Google Logging")

macro(glog_test NAME)
  add_executable(${NAME} ${NAME}.cc)
  target_link_libraries(${NAME} glog gflags pthread googletest)
  set_target_properties(${NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${LIBMV_TESTS_OUTPUT_DIR} FOLDER "Third Party/Google Logging")
  add_test(${NAME} ${LIBMV_TESTS_OUTPUT_DIR}/${NAME})
endmacro()

if(NOT WIN32)
#  glog_test(demangle_unittest)
#  # These require extra magic...
#  glog_test(logging_striptest10)
#  glog_test(logging_striptest2)
#  # I don't think logging_unittest can really be called a unittest, because it
#  # benchmarks the logging. Commented out to keep tests fast.
#  glog_test(logging_unittest)
#  glog_test(signalhandler_unittest)
#  glog_test(stacktrace_unittest)
#  glog_test(stl_logging_unittest)
#  glog_test(symbolize_unittest)
#  glog_test(utilities_unittest)
endif()
