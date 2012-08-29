#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
#----------------------------------------------------------------------------
# This file was automatically generated from the original CMakeLists.txt file
# Add a variable to hold the headers for the library
set (lib_headers
    filesystem.hpp
    filesystem
	)

# Add a library target to the build system
boost_library_project(
  filesystem
  # SRCDIRS  v3/src
  TESTDIRS  test
  HEADERS ${lib_headers}
  #  DOCDIRS 
  DESCRIPTION  "Provides portable facilities to query and manipulate paths, files, and directories. Note: the default version is now v3: v2 is deprecated."
  MODULARIZED
  AUTHORS  "Beman Dawes <bdawes -at- acm.org>"
  #  MAINTAINERS 
  )

boost_add_library(
  filesystem

  src/operations.cpp
  src/path.cpp
  src/portability.cpp
  src/utf8_codecvt_facet.cpp
  src/codecvt_error_category.cpp
  src/path_traits.cpp
  src/unique_path.cpp
  src/windows_file_codecvt.cpp

  SHARED_COMPILE_FLAGS "-DBOOST_FILESYSTEM_DYN_LINK=1"
  STATIC_COMPILE_FLAGS "-DBOOST_FILESYSTEM_STATIC_LINK=1"
  DEPENDS boost_system
  )
