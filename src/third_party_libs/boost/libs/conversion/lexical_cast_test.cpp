//  Unit test for boost::lexical_cast.
//
//  See http://www.boost.org for most recent version, including documentation.
//
//  Copyright Terje Sletteb and Kevlin Henney, 2005.
//  Copyright Alexander Nasonov, 2006.
//  Copyright Antony Polukhin, 2011.
//
//  Distributed under the Boost
//  Software License, Version 1.0. (See accompanying file
//  LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt).
//
// Note: The unit test no longer compile on MSVC 6, but lexical_cast itself works for it.

//
// We need this #define before any #includes: otherwise msvc will emit warnings
// deep within std::string, resulting from our (perfectly legal) use of basic_string
// with a custom traits class:
//
#define _SCL_SECURE_NO_WARNINGS

#include <boost/config.hpp>

#if defined(__INTEL_COMPILER)
#pragma warning(disable: 193 383 488 981 1418 1419)
#elif defined(BOOST_MSVC)
#pragma warning(disable: 4097 4100 4121 4127 4146 4244 4245 4511 4512 4701 4800)
#endif

#include <boost/lexical_cast.hpp>

#include <boost/cstdint.hpp>
#include <boost/test/unit_test.hpp>
#include <boost/test/floating_point_comparison.hpp>

#include <boost/type_traits/integral_promotion.hpp>
#include <string>
#include <vector>
#include <memory>

#if (defined(BOOST_HAS_LONG_LONG) || defined(BOOST_HAS_MS_INT64)) \
    && !(defined(BOOST_MSVC) && BOOST_MSVC < 1300)
#define LCAST_TEST_LONGLONG
#endif

#if defined(BOOST_NO_STRINGSTREAM) || defined(BOOST_NO_STD_WSTRING)
#define BOOST_LCAST_NO_WCHAR_T
#endif

template<class CharT>
struct my_traits : std::char_traits<CharT>
{
};

template<class CharT>
struct my_allocator : std::allocator<CharT>
{
};

// Test all 65536 values if true:
bool const lcast_test_small_integral_types_completely = false;

// lcast_integral_test_counter: use when testing all values of an integral
// types is not possible. Max. portable value is 32767.
int const lcast_integral_test_counter=1000;

using namespace boost;

void test_conversion_to_char();
void test_conversion_to_int();
void test_conversion_to_double();
void test_conversion_to_bool();
void test_conversion_with_nonconst_char();
void test_conversion_to_string();
void test_conversion_from_to_wchar_t_alias();
void test_conversion_to_pointer();
void test_conversion_from_wchar_t();
void test_conversion_to_wchar_t();
void test_conversion_from_wstring();
void test_conversion_to_wstring();
void test_bad_lexical_cast();
void test_no_whitespace_stripping();
void test_conversion_from_to_short();
void test_conversion_from_to_ushort();
void test_conversion_from_to_int();
void test_conversion_from_to_uint();
void test_conversion_from_to_long();
void test_conversion_from_to_ulong();
void test_conversion_from_to_intmax_t();
void test_conversion_from_to_uintmax_t();
#ifdef LCAST_TEST_LONGLONG
void test_conversion_from_to_longlong();
void test_conversion_from_to_ulonglong();
#endif
#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
void test_traits();
void test_wtraits();
void test_allocator();
void test_wallocator();
#endif
void test_char_types_conversions();
void operators_overload_test();
#ifndef BOOST_NO_CHAR16_T
void test_char16_conversions();
#endif
#ifndef BOOST_NO_CHAR32_T
void test_char32_conversions();
#endif


unit_test::test_suite *init_unit_test_suite(int, char *[])
{
    unit_test::test_suite *suite =
        BOOST_TEST_SUITE("lexical_cast unit test");
    suite->add(BOOST_TEST_CASE(test_conversion_to_char));
    suite->add(BOOST_TEST_CASE(test_conversion_to_int));
    suite->add(BOOST_TEST_CASE(test_conversion_to_double));
    suite->add(BOOST_TEST_CASE(test_conversion_to_bool));
    suite->add(BOOST_TEST_CASE(test_conversion_from_to_wchar_t_alias));
    suite->add(BOOST_TEST_CASE(test_conversion_to_pointer));
    suite->add(BOOST_TEST_CASE(test_conversion_to_string));
    suite->add(BOOST_TEST_CASE(test_conversion_with_nonconst_char));
#ifndef BOOST_LCAST_NO_WCHAR_T
    suite->add(BOOST_TEST_CASE(test_conversion_from_wchar_t));
    suite->add(BOOST_TEST_CASE(test_conversion_to_wchar_t));
    suite->add(BOOST_TEST_CASE(test_conversion_from_wstring));
    suite->add(BOOST_TEST_CASE(test_conversion_to_wstring));
#endif
    suite->add(BOOST_TEST_CASE(test_bad_lexical_cast));
    suite->add(BOOST_TEST_CASE(test_no_whitespace_stripping));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_short));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_ushort));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_int));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_uint));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_long));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_ulong));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_intmax_t));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_uintmax_t));
#ifdef LCAST_TEST_LONGLONG
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_longlong));
    suite->add(BOOST_TEST_CASE(&test_conversion_from_to_ulonglong));
#endif
#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
    suite->add(BOOST_TEST_CASE(&test_traits));
    suite->add(BOOST_TEST_CASE(&test_wtraits));
    suite->add(BOOST_TEST_CASE(&test_allocator));
    suite->add(BOOST_TEST_CASE(&test_wallocator));
#endif

    suite->add(BOOST_TEST_CASE(&test_char_types_conversions));
    suite->add(BOOST_TEST_CASE(&operators_overload_test));
#ifndef BOOST_NO_CHAR16_T
    suite->add(BOOST_TEST_CASE(&test_char16_conversions));
#endif
#ifndef BOOST_NO_CHAR32_T
    suite->add(BOOST_TEST_CASE(&test_char32_conversions));
#endif

    return suite;
}

void test_conversion_to_char()
{
    BOOST_CHECK_EQUAL('A', lexical_cast<char>('A'));
    BOOST_CHECK_EQUAL(' ', lexical_cast<char>(' '));
    BOOST_CHECK_EQUAL('1', lexical_cast<char>(1));
    BOOST_CHECK_EQUAL('0', lexical_cast<char>(0));
    BOOST_CHECK_THROW(lexical_cast<char>(123), bad_lexical_cast);
    BOOST_CHECK_EQUAL('1', lexical_cast<char>(1.0));
    BOOST_CHECK_EQUAL('1', lexical_cast<char>(true));
    BOOST_CHECK_EQUAL('0', lexical_cast<char>(false));
    BOOST_CHECK_EQUAL('A', lexical_cast<char>("A"));
    BOOST_CHECK_EQUAL(' ', lexical_cast<char>(" "));
    BOOST_CHECK_THROW(lexical_cast<char>(""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<char>("Test"), bad_lexical_cast);
    BOOST_CHECK_EQUAL('A', lexical_cast<char>(std::string("A")));
    BOOST_CHECK_EQUAL(' ', lexical_cast<char>(std::string(" ")));
    BOOST_CHECK_THROW(
        lexical_cast<char>(std::string("")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<char>(std::string("Test")), bad_lexical_cast);
}

void test_conversion_to_int()
{
    BOOST_CHECK_EQUAL(1, lexical_cast<int>('1'));
    BOOST_CHECK_EQUAL(0, lexical_cast<int>('0'));
    BOOST_CHECK_THROW(lexical_cast<int>('A'), bad_lexical_cast);
    BOOST_CHECK_EQUAL(1, lexical_cast<int>(1));
    BOOST_CHECK_EQUAL(1, lexical_cast<int>(1.0));

    BOOST_CHECK_EQUAL(
        (std::numeric_limits<int>::max)(),
        lexical_cast<int>((std::numeric_limits<int>::max)()));

    BOOST_CHECK_EQUAL(
        (std::numeric_limits<int>::min)(),
        lexical_cast<int>((std::numeric_limits<int>::min)()));

    BOOST_CHECK_THROW(lexical_cast<int>(1.23), bad_lexical_cast);

    BOOST_CHECK_THROW(lexical_cast<int>(1e20), bad_lexical_cast);
    BOOST_CHECK_EQUAL(1, lexical_cast<int>(true));
    BOOST_CHECK_EQUAL(0, lexical_cast<int>(false));
    BOOST_CHECK_EQUAL(123, lexical_cast<int>("123"));
    BOOST_CHECK_THROW(
        lexical_cast<int>(" 123"), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<int>(""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<int>("Test"), bad_lexical_cast);
    BOOST_CHECK_EQUAL(123, lexical_cast<int>("123"));
    BOOST_CHECK_EQUAL(123, lexical_cast<int>(std::string("123")));
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::string(" 123")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::string("")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::string("Test")), bad_lexical_cast);
}

void test_conversion_with_nonconst_char()
{
    std::vector<char> buffer;
    buffer.push_back('1');
    buffer.push_back('\0');
    BOOST_CHECK_EQUAL(boost::lexical_cast<int>(&buffer[0]), 1);

    std::vector<unsigned char> buffer2;
    buffer2.push_back('1');
    buffer2.push_back('\0');
    BOOST_CHECK_EQUAL(boost::lexical_cast<int>(&buffer2[0]), 1);

    std::vector<unsigned char> buffer3;
    buffer3.push_back('1');
    buffer3.push_back('\0');
    BOOST_CHECK_EQUAL(boost::lexical_cast<int>(&buffer3[0]), 1);

#ifndef BOOST_LCAST_NO_WCHAR_T
    std::vector<wchar_t> buffer4;
    buffer4.push_back(L'1');
    buffer4.push_back(L'\0');
    BOOST_CHECK_EQUAL(boost::lexical_cast<int>(&buffer4[0]), 1);
#endif
}

void test_conversion_to_double()
{
    BOOST_CHECK_CLOSE_FRACTION(1.0, lexical_cast<double>('1'), (std::numeric_limits<double>::epsilon()));
    BOOST_CHECK_THROW(lexical_cast<double>('A'), bad_lexical_cast);
    BOOST_CHECK_CLOSE_FRACTION(1.0, lexical_cast<double>(1), (std::numeric_limits<double>::epsilon()));
    BOOST_CHECK_CLOSE_FRACTION(1.23, lexical_cast<double>(1.23), (std::numeric_limits<double>::epsilon()));
    BOOST_CHECK_CLOSE_FRACTION(1.234567890, lexical_cast<double>(1.234567890), std::numeric_limits<double>::epsilon());
    BOOST_CHECK_CLOSE_FRACTION(1.234567890, lexical_cast<double>("1.234567890"), std::numeric_limits<double>::epsilon());
    BOOST_CHECK_CLOSE_FRACTION(1.0, lexical_cast<double>(true), (std::numeric_limits<double>::epsilon()));
    BOOST_CHECK_CLOSE_FRACTION(0.0, lexical_cast<double>(false), (std::numeric_limits<double>::epsilon()));
    BOOST_CHECK_CLOSE_FRACTION(1.23, lexical_cast<double>("1.23"), (std::numeric_limits<double>::epsilon()));
    BOOST_CHECK_THROW(lexical_cast<double>(""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<double>("Test"), bad_lexical_cast);
    BOOST_CHECK_CLOSE_FRACTION(1.23, lexical_cast<double>(std::string("1.23")), (std::numeric_limits<double>::epsilon()));
    BOOST_CHECK_THROW(
        lexical_cast<double>(std::string("")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<double>(std::string("Test")), bad_lexical_cast);
}

void test_conversion_to_bool()
{
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>('1'));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>('0'));
    BOOST_CHECK_THROW(lexical_cast<bool>('A'), bad_lexical_cast);
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(1));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(0));
    BOOST_CHECK_THROW(lexical_cast<bool>(123), bad_lexical_cast);
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(1.0));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(0.0));
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(true));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(false));
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>("1"));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>("0"));
    BOOST_CHECK_THROW(lexical_cast<bool>(""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>("Test"), bad_lexical_cast);
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>("1"));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>("0"));
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(std::string("1")));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(std::string("0")));

    BOOST_CHECK_THROW(lexical_cast<bool>(1.0001L), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>(2), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>(2u), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>(-1), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>(-2), bad_lexical_cast);


    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::string("")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::string("Test")), bad_lexical_cast);

    BOOST_CHECK(lexical_cast<bool>("+1") == true );
    BOOST_CHECK(lexical_cast<bool>("+0") == false );
    BOOST_CHECK(lexical_cast<bool>("-0") == false );
    BOOST_CHECK_THROW(lexical_cast<bool>("--0"), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>("-+-0"), bad_lexical_cast);
}

void test_conversion_to_string()
{
    char buf[] = "hello";
    char* str = buf;
    BOOST_CHECK_EQUAL(str, lexical_cast<std::string>(str));
    BOOST_CHECK_EQUAL("A", lexical_cast<std::string>('A'));
    BOOST_CHECK_EQUAL(" ", lexical_cast<std::string>(' '));
    BOOST_CHECK_EQUAL("123", lexical_cast<std::string>(123));
    BOOST_CHECK_EQUAL("1.23", lexical_cast<std::string>(1.23));
    BOOST_CHECK_EQUAL("1.111111111", lexical_cast<std::string>(1.111111111));
    BOOST_CHECK_EQUAL("1", lexical_cast<std::string>(true));
    BOOST_CHECK_EQUAL("0", lexical_cast<std::string>(false));
    BOOST_CHECK_EQUAL("Test", lexical_cast<std::string>("Test"));
    BOOST_CHECK_EQUAL(" ", lexical_cast<std::string>(" "));
    BOOST_CHECK_EQUAL("", lexical_cast<std::string>(""));
    BOOST_CHECK_EQUAL("Test", lexical_cast<std::string>(std::string("Test")));
    BOOST_CHECK_EQUAL(" ", lexical_cast<std::string>(std::string(" ")));
    BOOST_CHECK_EQUAL("", lexical_cast<std::string>(std::string("")));
}

void test_conversion_from_to_wchar_t_alias()
{
    BOOST_CHECK_EQUAL(123u, lexical_cast<unsigned short>("123"));
    BOOST_CHECK_EQUAL(123u, lexical_cast<unsigned int>("123"));
    BOOST_CHECK_EQUAL(123u, lexical_cast<unsigned long>("123"));
    BOOST_CHECK_EQUAL(std::string("123"),
        lexical_cast<std::string>(static_cast<unsigned short>(123)));
    BOOST_CHECK_EQUAL(std::string("123"), lexical_cast<std::string>(123u));
    BOOST_CHECK_EQUAL(std::string("123"), lexical_cast<std::string>(123ul));
}

void test_conversion_to_pointer()
{
    BOOST_CHECK_THROW(lexical_cast<char *>("Test"), bad_lexical_cast);
#ifndef BOOST_LCAST_NO_WCHAR_T
    BOOST_CHECK_THROW(lexical_cast<wchar_t *>("Test"), bad_lexical_cast);
#endif
}

void test_conversion_from_wchar_t()
{
#ifndef BOOST_LCAST_NO_WCHAR_T
#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_CHECK_EQUAL(1, lexical_cast<int>(L'1'));
    BOOST_CHECK_THROW(lexical_cast<int>(L'A'), bad_lexical_cast);
#endif

    BOOST_CHECK_EQUAL(123, lexical_cast<int>(L"123"));
    BOOST_CHECK_THROW(lexical_cast<int>(L""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<int>(L"Test"), bad_lexical_cast);

#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_CHECK_EQUAL(1.0, lexical_cast<double>(L'1'));
    BOOST_CHECK_THROW(lexical_cast<double>(L'A'), bad_lexical_cast);
#endif

    BOOST_CHECK_EQUAL(1.23, lexical_cast<double>(L"1.23"));
    BOOST_CHECK_THROW(lexical_cast<double>(L""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<double>(L"Test"), bad_lexical_cast);

#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(L'1'));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(L'0'));
    BOOST_CHECK_THROW(lexical_cast<bool>(L'A'), bad_lexical_cast);
#endif
    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(L"1"));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(L"0"));
    BOOST_CHECK_THROW(lexical_cast<bool>(L""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<bool>(L"Test"), bad_lexical_cast);
#endif
}

void test_conversion_to_wchar_t()
{
#if !defined(BOOST_LCAST_NO_WCHAR_T) && !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_CHECK_EQUAL(L'1', lexical_cast<wchar_t>(1));
    BOOST_CHECK_EQUAL(L'0', lexical_cast<wchar_t>(0));
    BOOST_CHECK_EQUAL(L'1', lexical_cast<wchar_t>('1'));
    BOOST_CHECK_EQUAL(L'0', lexical_cast<wchar_t>('0'));
    BOOST_CHECK_THROW(lexical_cast<wchar_t>(123), bad_lexical_cast);
    BOOST_CHECK_EQUAL(L'1', lexical_cast<wchar_t>(1.0));
    BOOST_CHECK_EQUAL(L'0', lexical_cast<wchar_t>(0.0));
    BOOST_CHECK_EQUAL(L'1', lexical_cast<wchar_t>(true));
    BOOST_CHECK_EQUAL(L'0', lexical_cast<wchar_t>(false));
    BOOST_CHECK_EQUAL(L'A', lexical_cast<wchar_t>(L'A'));
    BOOST_CHECK_EQUAL(L' ', lexical_cast<wchar_t>(L' '));
    BOOST_CHECK_EQUAL(L'A', lexical_cast<wchar_t>(L"A"));
    BOOST_CHECK_EQUAL(L' ', lexical_cast<wchar_t>(L" "));
    BOOST_CHECK_THROW(lexical_cast<wchar_t>(L""), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<wchar_t>(L"Test"), bad_lexical_cast);
    BOOST_CHECK_EQUAL(L'A', lexical_cast<wchar_t>(std::wstring(L"A")));
    BOOST_CHECK_EQUAL(L' ', lexical_cast<wchar_t>(std::wstring(L" ")));
    BOOST_CHECK_THROW(
        lexical_cast<wchar_t>(std::wstring(L"")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<wchar_t>(std::wstring(L"Test")), bad_lexical_cast);
#endif
}

void test_conversion_from_wstring()
{
#ifndef BOOST_LCAST_NO_WCHAR_T
    BOOST_CHECK_EQUAL(123, lexical_cast<int>(std::wstring(L"123")));
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::wstring(L"")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<int>(std::wstring(L"Test")), bad_lexical_cast);

    BOOST_CHECK_EQUAL(true, lexical_cast<bool>(std::wstring(L"1")));
    BOOST_CHECK_EQUAL(false, lexical_cast<bool>(std::wstring(L"0")));
    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::wstring(L"")), bad_lexical_cast);
    BOOST_CHECK_THROW(
        lexical_cast<bool>(std::wstring(L"Test")), bad_lexical_cast);
#endif
}

void test_conversion_to_wstring()
{
#ifndef BOOST_LCAST_NO_WCHAR_T
    wchar_t buf[] = L"hello";
    wchar_t* str = buf;
    BOOST_CHECK(str == lexical_cast<std::wstring>(str));
    BOOST_CHECK(L"123" == lexical_cast<std::wstring>(123));
    BOOST_CHECK(L"1.23" == lexical_cast<std::wstring>(1.23));
    BOOST_CHECK(L"1.111111111" == lexical_cast<std::wstring>(1.111111111));
    BOOST_CHECK(L"1" == lexical_cast<std::wstring>(true));
    BOOST_CHECK(L"0" == lexical_cast<std::wstring>(false));
#if !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    BOOST_CHECK(L"A" == lexical_cast<std::wstring>(L'A'));
    BOOST_CHECK(L" " == lexical_cast<std::wstring>(L' '));
    BOOST_CHECK(L"A" == lexical_cast<std::wstring>('A'));
#endif
    BOOST_CHECK(L"Test" == lexical_cast<std::wstring>(L"Test"));
    BOOST_CHECK(L" " == lexical_cast<std::wstring>(L" "));
    BOOST_CHECK(L"" == lexical_cast<std::wstring>(L""));
    BOOST_CHECK(L"Test" == lexical_cast<std::wstring>(std::wstring(L"Test")));
    BOOST_CHECK(L" " == lexical_cast<std::wstring>(std::wstring(L" ")));
    BOOST_CHECK(L"" == lexical_cast<std::wstring>(std::wstring(L"")));
#endif
}

void test_bad_lexical_cast()
{
    try
    {
        lexical_cast<int>(std::string("Test"));

        BOOST_CHECK(false); // Exception expected
    }
    catch(const bad_lexical_cast &e)
    {
        BOOST_CHECK(e.source_type() == typeid(std::string));
        BOOST_CHECK(e.target_type() == typeid(int));
    }
}

void test_no_whitespace_stripping()
{
    BOOST_CHECK_THROW(lexical_cast<int>(" 123"), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<int>("123 "), bad_lexical_cast);
}

// Replace "-,999" with "-999".
template<class CharT>
std::basic_string<CharT> to_str_gcc_workaround(std::basic_string<CharT> str)
{
    std::locale loc;
    std::numpunct<CharT> const& np = BOOST_USE_FACET(std::numpunct<CharT>, loc);
    std::ctype<CharT> const& ct = BOOST_USE_FACET(std::ctype<CharT>, loc);

    if(np.grouping().empty())
        return str;

    CharT prefix[3] = { ct.widen('-'), np.thousands_sep(), CharT() };

    if(str.find(prefix) != 0)
        return str;

    prefix[1] = CharT();
    str.replace(0, 2, prefix);
    return str;
}

template<class CharT, class T>
std::basic_string<CharT> to_str(T t)
{
    std::basic_ostringstream<CharT> o;
    o << t;
    return to_str_gcc_workaround(o.str());
}

template<class T, class CharT>
void test_conversion_from_integral_to_char(CharT zero)
{
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(0)) == zero + 0);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(1)) == zero + 1);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(2)) == zero + 2);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(3)) == zero + 3);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(4)) == zero + 4);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(5)) == zero + 5);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(6)) == zero + 6);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(7)) == zero + 7);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(8)) == zero + 8);
    BOOST_CHECK(lexical_cast<CharT>(static_cast<T>(9)) == zero + 9);

    BOOST_CHECK_THROW(lexical_cast<CharT>(static_cast<T>(10)), bad_lexical_cast);

    T t = (std::numeric_limits<T>::max)();
    BOOST_CHECK_THROW(lexical_cast<CharT>(t), bad_lexical_cast);
}

template<class T, class CharT>
void test_conversion_from_char_to_integral(CharT zero)
{
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 0)) == static_cast<T>(0) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 1)) == static_cast<T>(1) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 2)) == static_cast<T>(2) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 3)) == static_cast<T>(3) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 4)) == static_cast<T>(4) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 5)) == static_cast<T>(5) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 6)) == static_cast<T>(6) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 7)) == static_cast<T>(7) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 8)) == static_cast<T>(8) );
    BOOST_CHECK(lexical_cast<T>( static_cast<CharT>(zero + 9)) == static_cast<T>(9) );

    BOOST_CHECK_THROW(lexical_cast<T>( static_cast<CharT>(zero + 10)), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>( static_cast<CharT>(zero - 1)), bad_lexical_cast);
}

template<class T>
void test_conversion_from_integral_to_integral()
{
    T t = 0;
    BOOST_CHECK(lexical_cast<T>(t) == t);

    // Next two variables are used to supress warnings.
    int st = 32767; unsigned int ut = st;
    t = st;
    BOOST_CHECK(lexical_cast<short>(t) == st);
    BOOST_CHECK(lexical_cast<unsigned short>(t) == ut);
    BOOST_CHECK(lexical_cast<int>(t) == st);
    BOOST_CHECK(lexical_cast<unsigned int>(t) == ut);
    BOOST_CHECK(lexical_cast<long>(t) == st);
    BOOST_CHECK(lexical_cast<unsigned long>(t) == ut);

    t = (std::numeric_limits<T>::max)();
    BOOST_CHECK(lexical_cast<T>(t) == t);

    t = (std::numeric_limits<T>::min)();
    BOOST_CHECK(lexical_cast<T>(t) == t);
}

template<class T, class CharT>
void test_conversion_from_integral_to_string(CharT)
{
    typedef std::numeric_limits<T> limits;
    typedef std::basic_string<CharT> string_type;

    T t;

    t = (limits::min)();
    BOOST_CHECK(lexical_cast<string_type>(t) == to_str<CharT>(t));

    t = (limits::max)();
    BOOST_CHECK(lexical_cast<string_type>(t) == to_str<CharT>(t));

    if(limits::digits <= 16 && lcast_test_small_integral_types_completely)
        // min and max have already been tested.
        for(t = 1 + (limits::min)(); t != (limits::max)(); ++t)
            BOOST_CHECK(lexical_cast<string_type>(t) == to_str<CharT>(t));
    else
    {
        T const min_val = (limits::min)();
        T const max_val = (limits::max)();
        T const half_max_val = max_val / 2;
        T const cnt = lcast_integral_test_counter; // to supress warnings
        unsigned int const counter = cnt < half_max_val ? cnt : half_max_val;

        unsigned int i;

        // Test values around min:
        t = min_val;
        for(i = 0; i < counter; ++i, ++t)
            BOOST_CHECK(lexical_cast<string_type>(t) == to_str<CharT>(t));

        // Test values around max:
        t = max_val;
        for(i = 0; i < counter; ++i, --t)
            BOOST_CHECK(lexical_cast<string_type>(t) == to_str<CharT>(t));

        // Test values around zero:
        if(limits::is_signed)
            for(t = static_cast<T>(-counter); t < static_cast<T>(counter); ++t)
                BOOST_CHECK(lexical_cast<string_type>(t) == to_str<CharT>(t));

        // Test values around 100, 1000, 10000, ...
        T ten_power = 100;
        for(int e = 2; e <= limits::digits10; ++e, ten_power *= 10)
        {
            // ten_power + 100 probably never overflows
            for(t = ten_power - 100; t != ten_power + 100; ++t)
                BOOST_CHECK(lexical_cast<string_type>(t) == to_str<CharT>(t));
        }
    }
}

template<class T, class CharT>
void test_conversion_from_string_to_integral(CharT)
{
    typedef std::numeric_limits<T> limits;
    typedef std::basic_string<CharT> string_type;

    string_type s;
    string_type const zero = to_str<CharT>(0);
    string_type const nine = to_str<CharT>(9);
    T const min_val = (limits::min)();
    T const max_val = (limits::max)();

    s = to_str<CharT>(min_val);
    BOOST_CHECK_EQUAL(lexical_cast<T>(s), min_val);
    if(limits::is_signed)
    {
        BOOST_CHECK_THROW(lexical_cast<T>(s + zero), bad_lexical_cast);
        BOOST_CHECK_THROW(lexical_cast<T>(s + nine), bad_lexical_cast);
    }

    s = to_str<CharT>(max_val);
    BOOST_CHECK_EQUAL(lexical_cast<T>(s), max_val);
    {
        BOOST_CHECK_THROW(lexical_cast<T>(s + zero), bad_lexical_cast);
        BOOST_CHECK_THROW(lexical_cast<T>(s + nine), bad_lexical_cast);

        s = to_str<CharT>(max_val);
        for (int i =1; i <=10; ++i) {
            s[s.size()-1] += 1;
            BOOST_CHECK_THROW(lexical_cast<T>( s ), bad_lexical_cast);
        }

        s = to_str<CharT>(max_val);
        std::locale loc;
        typedef std::numpunct<char> numpunct;
        if ( BOOST_USE_FACET(numpunct, loc).grouping().empty() ) {
            // Following tests work well for locale C
            BOOST_CHECK_EQUAL(lexical_cast<T>(to_str<CharT>(0)+s), max_val);
            BOOST_CHECK_EQUAL(lexical_cast<T>(to_str<CharT>(0)+to_str<CharT>(0)+s), max_val);
            BOOST_CHECK_EQUAL(lexical_cast<T>(to_str<CharT>(0)+to_str<CharT>(0)+to_str<CharT>(0)+s), max_val);
        }

        for (int i =1; i <=256; ++i) {
            BOOST_CHECK_THROW(lexical_cast<T>( to_str<CharT>(i)+s ), bad_lexical_cast);
        }

        typedef BOOST_DEDUCED_TYPENAME boost::integral_promotion<T>::type promoted;
        if ( !(boost::is_same<T, promoted>::value) )
        {
            promoted prom = max_val;
            s = to_str<CharT>(max_val);
            for (int i =1; i <=256; ++i) {
                BOOST_CHECK_THROW(lexical_cast<T>( to_str<CharT>(prom+i) ), bad_lexical_cast);
                BOOST_CHECK_THROW(lexical_cast<T>( to_str<CharT>(i)+s ), bad_lexical_cast);
            }
        }
    }

    if(limits::digits <= 16 && lcast_test_small_integral_types_completely)
        // min and max have already been tested.
        for(T t = 1 + min_val; t != max_val; ++t)
            BOOST_CHECK(lexical_cast<T>(to_str<CharT>(t)) == t);
    else
    {
        T const half_max_val = max_val / 2;
        T const cnt = lcast_integral_test_counter; // to supress warnings
        unsigned int const counter = cnt < half_max_val ? cnt : half_max_val;

        T t;
        unsigned int i;

        // Test values around min:
        t = min_val;
        for(i = 0; i < counter; ++i, ++t)
            BOOST_CHECK(lexical_cast<T>(to_str<CharT>(t)) == t);

        // Test values around max:
        t = max_val;
        for(i = 0; i < counter; ++i, --t)
            BOOST_CHECK(lexical_cast<T>(to_str<CharT>(t)) == t);

        // Test values around zero:
        if(limits::is_signed)
            for(t = static_cast<T>(-counter); t < static_cast<T>(counter); ++t)
                BOOST_CHECK(lexical_cast<T>(to_str<CharT>(t)) == t);

        // Test values around 100, 1000, 10000, ...
        T ten_power = 100;
        for(int e = 2; e <= limits::digits10; ++e, ten_power *= 10)
        {
            // ten_power + 100 probably never overflows
            for(t = ten_power - 100; t != ten_power + 100; ++t)
                BOOST_CHECK(lexical_cast<T>(to_str<CharT>(t)) == t);
        }
    }
}

template<class T>
void test_conversion_from_to_integral_for_locale()
{
    std::locale current_locale;
    typedef std::numpunct<char> numpunct;
    numpunct const& np = BOOST_USE_FACET(numpunct, current_locale);
    if ( !np.grouping().empty() )
    {
        BOOST_CHECK_THROW(
                lexical_cast<T>( std::string("100") + np.thousands_sep() + np.thousands_sep() + "0" )
                , bad_lexical_cast);
        BOOST_CHECK_THROW(lexical_cast<T>( std::string("100") + np.thousands_sep() ), bad_lexical_cast);
        BOOST_CHECK_THROW(lexical_cast<T>( np.thousands_sep() + std::string("100") ), bad_lexical_cast);

        // Exception must not be thrown, when we are using no separators at all
        BOOST_CHECK( lexical_cast<T>("30000") == static_cast<T>(30000) );
    }

    test_conversion_from_integral_to_integral<T>();
    test_conversion_from_integral_to_string<T>('0');
    test_conversion_from_string_to_integral<T>('0');
#if !defined(BOOST_LCAST_NO_WCHAR_T)
    test_conversion_from_integral_to_string<T>(L'0');
    test_conversion_from_string_to_integral<T>(L'0');
#endif
}

struct restore_oldloc
{
    std::locale oldloc;
    ~restore_oldloc() { std::locale::global(oldloc); }
};

template<class T>
void test_conversion_from_to_integral()
{
    char const zero = '0';
    signed char const szero = '0';
    unsigned char const uzero = '0';
    test_conversion_from_integral_to_char<T>(zero);
    test_conversion_from_char_to_integral<T>(zero);
    test_conversion_from_integral_to_char<T>(szero);
    test_conversion_from_char_to_integral<T>(szero);
    test_conversion_from_integral_to_char<T>(uzero);
    test_conversion_from_char_to_integral<T>(uzero);
#if !defined(BOOST_LCAST_NO_WCHAR_T) && !defined(BOOST_NO_INTRINSIC_WCHAR_T)
    wchar_t const wzero = L'0';
    test_conversion_from_integral_to_char<T>(wzero);
    test_conversion_from_char_to_integral<T>(wzero);
#endif
#ifndef BOOST_NO_CHAR16_T
    char16_t const u16zero = u'0';
    test_conversion_from_integral_to_char<T>(u16zero);
    test_conversion_from_char_to_integral<T>(u16zero);
#endif
#ifndef BOOST_NO_CHAR32_T
    char32_t const u32zero = u'0';
    test_conversion_from_integral_to_char<T>(u32zero);
    test_conversion_from_char_to_integral<T>(u32zero);
#endif

    BOOST_CHECK(lexical_cast<T>("-1") == static_cast<T>(-1));
    BOOST_CHECK(lexical_cast<T>("-9") == static_cast<T>(-9));
    BOOST_CHECK(lexical_cast<T>(-1) == static_cast<T>(-1));
    BOOST_CHECK(lexical_cast<T>(-9) == static_cast<T>(-9));

    BOOST_CHECK_THROW(lexical_cast<T>("-1.0"), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>("-9.0"), bad_lexical_cast);
    BOOST_CHECK(lexical_cast<T>(-1.0) == static_cast<T>(-1));
    BOOST_CHECK(lexical_cast<T>(-9.0) == static_cast<T>(-9));

    BOOST_CHECK(lexical_cast<T>(static_cast<T>(1)) == static_cast<T>(1));
    BOOST_CHECK(lexical_cast<T>(static_cast<T>(9)) == static_cast<T>(9));
    BOOST_CHECK_THROW(lexical_cast<T>(1.1f), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>(1.1), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>(1.1L), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>(1.0001f), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>(1.0001), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>(1.0001L), bad_lexical_cast);

    BOOST_CHECK(lexical_cast<T>("+1") == static_cast<T>(1) );
    BOOST_CHECK(lexical_cast<T>("+9") == static_cast<T>(9) );
    BOOST_CHECK(lexical_cast<T>("+10") == static_cast<T>(10) );
    BOOST_CHECK(lexical_cast<T>("+90") == static_cast<T>(90) );
    BOOST_CHECK_THROW(lexical_cast<T>("++1"), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>("-+9"), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>("--1"), bad_lexical_cast);
    BOOST_CHECK_THROW(lexical_cast<T>("+-9"), bad_lexical_cast);
    // test_conversion_from_to_integral_for_locale

    // Overflow test case from David W. Birdsall
    std::string must_owerflow_str = "160000000000000000000";
    std::string must_owerflow_negative_str = "-160000000000000000000";
    for (int i = 0; i < 15; ++i) {
        BOOST_CHECK_THROW(lexical_cast<T>(must_owerflow_str), bad_lexical_cast);
        BOOST_CHECK_THROW(lexical_cast<T>(must_owerflow_negative_str), bad_lexical_cast);

        must_owerflow_str += '0';
        must_owerflow_negative_str += '0';
    }

    typedef std::numpunct<char> numpunct;

    restore_oldloc guard;
    std::locale const& oldloc = guard.oldloc;

    std::string grouping1 = BOOST_USE_FACET(numpunct, oldloc).grouping();
    std::string grouping2(grouping1);

    test_conversion_from_to_integral_for_locale<T>();

    try
    {
        std::locale newloc("");
        std::locale::global(newloc);

        grouping2 = BOOST_USE_FACET(numpunct, newloc).grouping();
    }
    catch(std::exception const& ex)
    {
        std::string msg("Failed to set system locale: ");
        msg += ex.what();
        BOOST_TEST_MESSAGE(msg);
    }

    if(grouping1 != grouping2)
        test_conversion_from_to_integral_for_locale<T>();

    if(grouping1.empty() && grouping2.empty())
        BOOST_TEST_MESSAGE("Formatting with thousands_sep has not been tested");
}

void test_conversion_from_to_short()
{
    test_conversion_from_to_integral<short>();
}

void test_conversion_from_to_ushort()
{
    test_conversion_from_to_integral<unsigned short>();
}

void test_conversion_from_to_int()
{
    test_conversion_from_to_integral<int>();
}

void test_conversion_from_to_uint()
{
    test_conversion_from_to_integral<unsigned int>();
}

void test_conversion_from_to_long()
{
    test_conversion_from_to_integral<long>();
}

void test_conversion_from_to_ulong()
{
    test_conversion_from_to_integral<unsigned long>();
}

void test_conversion_from_to_intmax_t()
{
    test_conversion_from_to_integral<boost::intmax_t>();
}

void test_conversion_from_to_uintmax_t()
{
    test_conversion_from_to_integral<boost::uintmax_t>();
}

#if defined(BOOST_HAS_LONG_LONG)

void test_conversion_from_to_longlong()
{
    test_conversion_from_to_integral<boost::long_long_type>();
}

void test_conversion_from_to_ulonglong()
{
    test_conversion_from_to_integral<boost::ulong_long_type>();
}

#elif defined(BOOST_HAS_MS_INT64)

void test_conversion_from_to_longlong()
{
    test_conversion_from_to_integral<__int64>();
}

void test_conversion_from_to_ulonglong()
{
    test_conversion_from_to_integral<unsigned __int64>();
}

#endif

#ifndef BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION
void test_traits()
{
    typedef std::basic_string<char, my_traits<char> > my_string;

    my_string const s("s");
    BOOST_CHECK(boost::lexical_cast<char>(s) == s[0]);
    BOOST_CHECK(boost::lexical_cast<my_string>(s) == s);
    BOOST_CHECK(boost::lexical_cast<my_string>(-1) == "-1");
}

void test_wtraits()
{
    typedef std::basic_string<wchar_t, my_traits<wchar_t> > my_string;

    my_string const s(L"s");
    BOOST_CHECK(boost::lexical_cast<wchar_t>(s) == s[0]);
    BOOST_CHECK(boost::lexical_cast<my_string>(s) == s);
    //BOOST_CHECK(boost::lexical_cast<my_string>(-1) == L"-1");
    // Commented out because gcc 3.3 doesn't support this:
    // basic_ostream<wchar_t, my_traits<wchar_t> > o; o << -1;
}

void test_allocator()
{
    typedef std::basic_string< char
                             , std::char_traits<char>
                             , my_allocator<char>
                             > my_string;

    my_string s("s");
    BOOST_CHECK(boost::lexical_cast<char>(s) == s[0]);
    BOOST_CHECK(boost::lexical_cast<std::string>(s) == "s");
    BOOST_CHECK(boost::lexical_cast<my_string>(s) == s);
    BOOST_CHECK(boost::lexical_cast<my_string>(1) == "1");
    BOOST_CHECK(boost::lexical_cast<my_string>("s") == s);
    BOOST_CHECK(boost::lexical_cast<my_string>(std::string("s")) == s);
}

void test_wallocator()
{
    typedef std::basic_string< wchar_t
                             , std::char_traits<wchar_t>
                             , my_allocator<wchar_t>
                             > my_string;

    my_string s(L"s");
    BOOST_CHECK(boost::lexical_cast<wchar_t>(s) == s[0]);
    BOOST_CHECK(boost::lexical_cast<std::wstring>(s) == L"s");
    BOOST_CHECK(boost::lexical_cast<my_string>(s) == s);
    BOOST_CHECK(boost::lexical_cast<my_string>(1) == L"1");
    BOOST_CHECK(boost::lexical_cast<my_string>(L"s") == s);
    BOOST_CHECK(boost::lexical_cast<my_string>(std::wstring(L"s")) == s);
}

#endif

void test_char_types_conversions()
{
    const char c_arr[]            = "Test array of chars";
    const unsigned char uc_arr[]  = "Test array of chars";
    const signed char sc_arr[]    = "Test array of chars";

    BOOST_CHECK(boost::lexical_cast<std::string>(c_arr) == std::string(c_arr));
    BOOST_CHECK(boost::lexical_cast<std::string>(uc_arr) == std::string(c_arr));
    BOOST_CHECK(boost::lexical_cast<std::string>(sc_arr) == std::string(c_arr));

    BOOST_CHECK(boost::lexical_cast<char>(c_arr[0]) == c_arr[0]);
    BOOST_CHECK(boost::lexical_cast<char>(uc_arr[0]) == c_arr[0]);
    BOOST_CHECK(boost::lexical_cast<char>(sc_arr[0]) == c_arr[0]);

    BOOST_CHECK(boost::lexical_cast<unsigned char>(c_arr[0]) == uc_arr[0]);
    BOOST_CHECK(boost::lexical_cast<unsigned char>(uc_arr[0]) == uc_arr[0]);
    BOOST_CHECK(boost::lexical_cast<unsigned char>(sc_arr[0]) == uc_arr[0]);

    BOOST_CHECK(boost::lexical_cast<signed char>(c_arr[0]) == sc_arr[0]);
    BOOST_CHECK(boost::lexical_cast<signed char>(uc_arr[0]) == sc_arr[0]);
    BOOST_CHECK(boost::lexical_cast<signed char>(sc_arr[0]) == sc_arr[0]);

#ifndef BOOST_LCAST_NO_WCHAR_T
    const wchar_t wc_arr[]=L"Test array of chars";

    BOOST_CHECK(boost::lexical_cast<std::wstring>(wc_arr) == std::wstring(wc_arr));
    BOOST_CHECK(boost::lexical_cast<wchar_t>(wc_arr[0]) == wc_arr[0]);

#endif
}



struct foo_operators_test
{
  foo_operators_test() : f(2) {}
  int f;
};

template <typename OStream>
OStream& operator<<(OStream& ostr, const foo_operators_test& foo)
{
  ostr << foo.f;
  return ostr;
}

template <typename IStream>
IStream& operator>>(IStream& istr, foo_operators_test& foo)
{
  istr >> foo.f;
  return istr;
}

void operators_overload_test()
{
    foo_operators_test foo;
    BOOST_CHECK_EQUAL(boost::lexical_cast<std::string>(foo), "2");
    BOOST_CHECK_EQUAL((boost::lexical_cast<foo_operators_test>("2")).f, 2);

    // Must compile
    (void)boost::lexical_cast<foo_operators_test>(foo);
}


#ifndef BOOST_NO_CHAR16_T
void test_char16_conversions()
{
    BOOST_CHECK(u"100" == lexical_cast<std::u16string>(u"100"));
    BOOST_CHECK(u"1" == lexical_cast<std::u16string>(u'1'));
}
#endif

#ifndef BOOST_NO_CHAR32_T
void test_char32_conversions()
{
    BOOST_CHECK(U"100" == lexical_cast<std::u32string>(U"100"));
    BOOST_CHECK(U"1" == lexical_cast<std::u32string>(U'1'));
}
#endif

