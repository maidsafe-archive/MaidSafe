// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Pierre Talbot

#ifndef BOOST_FUNCTIONAL_MONAD_DO_HPP
#define BOOST_FUNCTIONAL_MONAD_DO_HPP

#define BOOST_PP_VARIADICS 1
#include <boost/preprocessor.hpp>
#include <boost/functional/monads/monad.hpp>

// Macro helpers.

#define IS_EVEN(i) BOOST_PP_EQUAL(0, BOOST_PP_MOD(i, 2))
#define IS_LAST_ELEM(i, seq) BOOST_PP_EQUAL(i, BOOST_PP_DEC(BOOST_PP_SEQ_SIZE(seq)))

// Bind the couple (decl, monad_expr) to a suitable lambda and method call.

#define MONADIC_BIND_0_IV_0(mexpr, decl)
#define MONADIC_BIND_0_IV_1(mexpr, decl) boost::functional::monad::bind BOOST_PP_LPAREN() (mexpr), [&](decl){ return

#define MONADIC_BIND_0_III(bit, mexpr, decl) \
  MONADIC_BIND_0_IV_ ## bit (mexpr, decl)

#define MONADIC_BIND_0_II(cond, mexpr, decl) \
  MONADIC_BIND_0_III(cond, mexpr, decl)

#define MONADIC_BIND_0_I(cond, i, inc_i, seq) \
  MONADIC_BIND_0_II(cond, \
    BOOST_PP_SEQ_ELEM(inc_i, seq), \
    BOOST_PP_SEQ_ELEM(i, seq))

#define MONADIC_BIND_0(i, seq, elem) \
  MONADIC_BIND_0_I(IS_EVEN(i), i, BOOST_PP_INC(i), seq)

#define MONADIC_BIND_1(i, seq, elem) elem;

// For each couple (decl, monad_expr) we add a lambda.

#define DO_ELEM_III(bit, i, seq, elem) \
  MONADIC_BIND_ ## bit(i, seq, elem)

#define DO_ELEM_II(bit, i, seq, elem) \
  DO_ELEM_III(bit, i, seq, elem)

#define DO_ELEM_I(cond, i, seq, elem) \
  DO_ELEM_II(BOOST_PP_BOOL(cond), i, seq, elem)

#define DO_ELEM(r, seq, i, elem) \
  DO_ELEM_I(IS_LAST_ELEM(i, seq), i, seq, elem)

// Close the lambda we opened.

#define CLOSE_LAMBDA_IV } BOOST_PP_RPAREN() ;
#define CLOSE_LAMBDA_III(z,n,t) BOOST_PP_COMMA_IF(0) CLOSE_LAMBDA_IV
#define CLOSE_LAMBDA_II(n) \
  BOOST_PP_REPEAT(n, CLOSE_LAMBDA_III, _)
#define CLOSE_LAMBDA_I(n) \
  CLOSE_LAMBDA_II(BOOST_PP_DIV(n, 2))

/*
Syntactic transformation, example:

DO(
  int i, monad_expr,
YIELD(i+4))

===>

bind(monad_expr, [&](int i){
  return i + 4;
})

------------------

DO(
  int i, monad_expr_1,
  int j, monad_expr_2,
YIELD(i+j))

===>

bind(monad_expr_1, [&](int i){
bind(monad_expr_2, [&](int j){
  return i + j;
})})

-----------------

The monad expression must be evaluated to a value implementing the Monad concept.
*/

#define DO_SEQ(seq) \
  BOOST_PP_SEQ_FOR_EACH_I(DO_ELEM, seq, seq) \
  CLOSE_LAMBDA_I(BOOST_PP_SEQ_SIZE(seq))

#define DO(...) DO_SEQ(BOOST_PP_VARIADIC_TO_SEQ(__VA_ARGS__))

#endif // BOOST_FUNCTIONAL_MONAD_DO_HPP
