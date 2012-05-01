/** -*- c++ -*- \file utils.hpp \brief Test utilities. */

#ifndef TEST_UTILS_HPP
#define TEST_UTILS_HPP


#include <iostream>


#define EXPAND_(x) x

#define STRINGIFY_(x) #x

#define JOIN_(x,y) x ## y

#ifndef NDEBUG
#       define BOOST_UBLAS_DEBUG_TRACE(x) std::cerr << "[Debug>> " << EXPAND_(x) << std::endl
#else
#       define BOOST_UBLAS_DEBUG_TRACE(x) /**/
#endif // NDEBUG

#define BOOST_UBLAS_TEST_BEGIN() unsigned int test_fails_(0)

#define BOOST_UBLAS_TEST_DEF(x) void EXPAND_(x)(unsigned int& test_fails_)

#define BOOST_UBLAS_TEST_DO(x) EXPAND_(x)(test_fails_)

#define BOOST_UBLAS_TEST_END() if (test_fails_ > 0) { std::cerr << "Number of failed tests: " << test_fails_ << std::endl; return -1; \
} else { std::cerr << "No failed test" << std::endl; return 0; }

#define BOOST_UBLAS_TEST_CHECK(x) if (!(x)) { std::cerr << "Failed assertion: " << STRINGIFY_(x) << std::endl; ++test_fails_; }


#endif // TEST_UTILS_HPP
