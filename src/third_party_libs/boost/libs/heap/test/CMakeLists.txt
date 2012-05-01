#----------------------------------------------------------------------------
# CMake framework maintained by Denis Arnaud on git://github.com/pocb/boost
#----------------------------------------------------------------------------
#
boost_additional_test_dependencies (heap BOOST_DEPENDS test)

#
set (TESTS
  binomial_heap_test
  d_ary_heap_test
  fibonacci_heap_test
  mutable_heap_test
  pairing_heap_tests
  priority_queue_test
  skew_heap_test)

foreach (_test ${TESTS})
  boost_test_run (${_test} DEPENDS boost_unit_test_framework)
endforeach (_test ${TESTS})

if (WIN32)
  message ("Need to turn on iterator debugging")
endif (WIN32)
