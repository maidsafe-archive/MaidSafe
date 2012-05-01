#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
boost_add_library(
  filesystem
  v2_operations.cpp v2_path.cpp v2_portability.cpp
  SHARED_COMPILE_FLAGS "-DBOOST_FILESYSTEM_DYN_LINK=1"
  STATIC_COMPILE_FLAGS "-DBOOST_FILESYSTEM_STATIC_LINK=1"
  DEPENDS boost_system
  )

