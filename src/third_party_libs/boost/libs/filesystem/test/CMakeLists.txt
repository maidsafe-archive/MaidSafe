#
# Copyright Troy D. Straszheim
#
# Distributed under the Boost Software License, Version 1.0.
# See http://www.boost.org/LICENSE_1_0.txt
#
boost_additional_test_dependencies(filesystem BOOST_DEPENDS test bind)

ADD_DEFINITIONS(-DBOOST_ALL_NO_LIB) # -DBOOST_FILESYSTEM_STATIC_LINK

boost_test_compile(mbcopy ../v3/example/mbcopy.cpp)
boost_test_compile(mbpath ../v3/example/mbpath.cpp)
boost_test_compile(tchar ../v3/example/tchar.cpp)
boost_test_compile(file_size ../v3/example/file_size.cpp)
boost_test_compile(path_info ../v3/example/path_info.cpp)
boost_test_compile(error_demo ../v3/example/error_demo.cpp)
boost_test_compile(tut0 ../v3/example/tut0.cpp)
boost_test_compile(tut1 ../v3/example/tut1.cpp)
boost_test_compile(tut2 ../v3/example/tut2.cpp)
boost_test_compile(tut3 ../v3/example/tut3.cpp)
boost_test_compile(tut4 ../v3/example/tut4.cpp)
boost_test_compile(tut5 ../v3/example/tut5.cpp)
