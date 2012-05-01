// Boost result_of library

//  Copyright Douglas Gregor 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#define BOOST_RESULT_OF_USE_DECLTYPE

// For more information, see http://www.boost.org/libs/utility
#include <boost/utility/result_of.hpp>
#include <utility>
#include <boost/static_assert.hpp>
#include <boost/type_traits/is_same.hpp>

struct int_result_type
{
  typedef int result_type;
  result_type operator()(float);
};

struct int_result_of
{
  template<typename F> struct result { typedef int type; };
  result<int_result_of(double)>::type operator()(double);
  result<const int_result_of(double)>::type operator()(double) const;
  result<int_result_of()>::type operator()();
  result<volatile int_result_of()>::type operator()() volatile;
};

struct int_result_type_and_float_result_of_and_char_return
{
  typedef int result_type;
  template<typename F> struct result { typedef float type; };
  char operator()(char);
};

template<typename T>
struct int_result_type_template
{
  typedef int result_type;
  result_type operator()(float);
};

template<typename T>
struct int_result_of_template
{
  template<typename F> struct result;
  template<typename This, typename That> struct result<This(That)> { typedef int type; };
  typename result<int_result_of_template<T>(double)>::type operator()(double);
  typename result<const int_result_of_template<T>(double)>::type operator()(double) const;
  typename result<int_result_of_template<T>(double)>::type operator()();
  typename result<volatile int_result_of_template<T>(double)>::type operator()() volatile;
};

template<typename T>
struct int_result_type_and_float_result_of_and_char_return_template
{
  typedef int result_type;
  template<typename F> struct result;
  template<typename This, typename That> struct result<This(That)> { typedef float type; };
  char operator()(char);
};

struct result_of_member_function_template
{
  template<typename F> struct result;

  template<typename This, typename That> struct result<This(That)> { typedef That type; };
  template<class T> typename result<result_of_member_function_template(T)>::type operator()(T);

  template<typename This, typename That> struct result<const This(That)> { typedef const That type; };
  template<class T> typename result<const result_of_member_function_template(T)>::type operator()(T) const;

  template<typename This, typename That> struct result<volatile This(That)> { typedef volatile That type; };
  template<class T> typename result<volatile result_of_member_function_template(T)>::type operator()(T) volatile;

  template<typename This, typename That> struct result<const volatile This(That)> { typedef const volatile That type; };
  template<class T> typename result<const volatile result_of_member_function_template(T)>::type operator()(T) const volatile;

  template<typename This, typename That> struct result<This(That &, That)> { typedef That & type; };
  template<class T> typename result<result_of_member_function_template(T &, T)>::type operator()(T &, T);

  template<typename This, typename That> struct result<This(That const &, That)> { typedef That const & type; };
  template<class T> typename result<result_of_member_function_template(T const &, T)>::type operator()(T const &, T);

  template<typename This, typename That> struct result<This(That volatile &, That)> { typedef That volatile & type; };
  template<class T> typename result<result_of_member_function_template(T volatile &, T)>::type operator()(T volatile &, T);

  template<typename This, typename That> struct result<This(That const volatile &, That)> { typedef That const volatile & type; };
  template<class T> typename result<result_of_member_function_template(T const volatile &, T)>::type operator()(T const volatile &, T);
};

struct no_result_type_or_result_of
{
  int operator()(double);
  short operator()(double) const;
  unsigned int operator()();
  unsigned short operator()() volatile;
  const unsigned short operator()() const volatile;
#if !defined(BOOST_NO_RVALUE_REFERENCES)
  short operator()(int&&);
  int operator()(int&);
  long operator()(int const&);
#endif
};

template<typename T>
struct no_result_type_or_result_of_template
{
  int operator()(double);
  short operator()(double) const;
  unsigned int operator()();
  unsigned short operator()() volatile;
  const unsigned short operator()() const volatile;
#if !defined(BOOST_NO_RVALUE_REFERENCES)
  short operator()(int&&);
  int operator()(int&);
  long operator()(int const&);
#endif
};

struct X {};

int main()
{
  using namespace boost;

  typedef int (*func_ptr)(float, double);
  typedef int (&func_ref)(float, double);
  typedef int (*func_ptr_0)();
  typedef int (&func_ref_0)();
  typedef int (X::*mem_func_ptr)(float);
  typedef int (X::*mem_func_ptr_c)(float) const;
  typedef int (X::*mem_func_ptr_v)(float) volatile;
  typedef int (X::*mem_func_ptr_cv)(float) const volatile;
  typedef int (X::*mem_func_ptr_0)();

  BOOST_STATIC_ASSERT((is_same<result_of<int_result_type(float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_of(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const int_result_of(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_type_template<void>(float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_of_template<void>(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const int_result_of_template<void>(double)>::type, int>::value));

  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_type(float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_of(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<const int_result_of(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_type_template<void>(float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_of_template<void>(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<const int_result_of_template<void>(double)>::type, int>::value));

  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_of(void)>::type, void>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<volatile int_result_of(void)>::type, void>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_of_template<void>(void)>::type, void>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<volatile int_result_of_template<void>(void)>::type, void>::value));

  // Prior to decltype, result_of could not deduce the return type
  // nullary function objects unless they exposed a result_type.
#if !defined(BOOST_NO_DECLTYPE)
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_of(void)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<volatile int_result_of(void)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_of_template<void>(void)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<volatile int_result_of_template<void>(void)>::type, int>::value));
#else
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_of(void)>::type, void>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<volatile int_result_of(void)>::type, void>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_of_template<void>(void)>::type, void>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<volatile int_result_of_template<void>(void)>::type, void>::value));
#endif

  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_type_and_float_result_of_and_char_return(char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<int_result_type_and_float_result_of_and_char_return_template<void>(char)>::type, int>::value));

  // Prior to decltype, result_of ignored a nested result<> if
  // result_type was defined. After decltype, result_of deduces the
  // actual return type of the function object, ignoring both
  // result<> and result_type.
#if !defined(BOOST_NO_DECLTYPE)
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_type_and_float_result_of_and_char_return(char)>::type, char>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_type_and_float_result_of_and_char_return_template<void>(char)>::type, char>::value));
#else
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_type_and_float_result_of_and_char_return(char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<int_result_type_and_float_result_of_and_char_return_template<void>(char)>::type, int>::value));
#endif

  BOOST_STATIC_ASSERT((is_same<result_of<func_ptr(char, float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<func_ref(char, float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<func_ptr_0()>::type, int>::value)); 
  BOOST_STATIC_ASSERT((is_same<result_of<func_ref_0()>::type, int>::value)); 
  BOOST_STATIC_ASSERT((is_same<result_of<mem_func_ptr(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<mem_func_ptr_c(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<mem_func_ptr_v(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<mem_func_ptr_cv(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<mem_func_ptr_0(X)>::type, int>::value)); 
  BOOST_STATIC_ASSERT((is_same<result_of<func_ptr(void)>::type, int>::value));

  BOOST_STATIC_ASSERT((is_same<tr1_result_of<func_ptr(char, float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<func_ref(char, float)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<func_ptr_0()>::type, int>::value)); 
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<func_ref_0()>::type, int>::value)); 
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<mem_func_ptr(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<mem_func_ptr_c(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<mem_func_ptr_v(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<mem_func_ptr_cv(X,char)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<mem_func_ptr_0(X)>::type, int>::value)); 
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<func_ptr(void)>::type, int>::value));

  BOOST_STATIC_ASSERT((is_same<result_of<result_of_member_function_template(double)>::type, double>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const result_of_member_function_template(double)>::type, const double>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<volatile result_of_member_function_template(double)>::type, volatile double>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const volatile result_of_member_function_template(double)>::type, const volatile double>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<result_of_member_function_template(int &, int)>::type, int &>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<result_of_member_function_template(int const &, int)>::type, int const &>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<result_of_member_function_template(int volatile &, int)>::type, int volatile &>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<result_of_member_function_template(int const volatile &, int)>::type, int const volatile &>::value));

  BOOST_STATIC_ASSERT((is_same<tr1_result_of<result_of_member_function_template(double)>::type, double>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<const result_of_member_function_template(double)>::type, const double>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<volatile result_of_member_function_template(double)>::type, volatile double>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<const volatile result_of_member_function_template(double)>::type, const volatile double>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<result_of_member_function_template(int &, int)>::type, int &>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<result_of_member_function_template(int const &, int)>::type, int const &>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<result_of_member_function_template(int volatile &, int)>::type, int volatile &>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<result_of_member_function_template(int const volatile &, int)>::type, int const volatile &>::value));

  typedef int (*pf_t)(int);
  BOOST_STATIC_ASSERT((is_same<result_of<pf_t(int)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<pf_t const(int)>::type,int>::value));

  BOOST_STATIC_ASSERT((is_same<tr1_result_of<pf_t(int)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<tr1_result_of<pf_t const(int)>::type,int>::value));

#if !defined(BOOST_NO_DECLTYPE)
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of(void)>::type, unsigned int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const no_result_type_or_result_of(double)>::type, short>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<volatile no_result_type_or_result_of(void)>::type, unsigned short>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const volatile no_result_type_or_result_of(void)>::type, const unsigned short>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of_template<void>(double)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of_template<void>(void)>::type, unsigned int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const no_result_type_or_result_of_template<void>(double)>::type, short>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<volatile no_result_type_or_result_of_template<void>(void)>::type, unsigned short>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<const volatile no_result_type_or_result_of_template<void>(void)>::type, const unsigned short>::value));
#if !defined(BOOST_NO_RVALUE_REFERENCES)
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of(int&&)>::type, short>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of(int&)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of(int const&)>::type, long>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of_template<void>(int&&)>::type, short>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of_template<void>(int&)>::type, int>::value));
  BOOST_STATIC_ASSERT((is_same<result_of<no_result_type_or_result_of_template<void>(int const&)>::type, long>::value));
#endif
#endif

  return 0;
}
