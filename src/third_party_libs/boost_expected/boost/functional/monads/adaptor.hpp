// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
// (C) Copyright 2014 Vicente J. Botet Escriba

#ifndef BOOST_FUNCTIONAL_ADAPTOR_HPP
#define BOOST_FUNCTIONAL_ADAPTOR_HPP

#include <boost/move/move.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/functional/monads.hpp>
#include <boost/expected/expected.hpp>

namespace boost
{
namespace functional
{

  template <class H>
  class adaptor_holder
  {
  public:
    typedef H holder_type;
    typedef typename H::funct_type funct_type;
    template <class E>
    struct rebind_right
    {
      typedef typename H::template rebind_right<E>::type type;
    };

    explicit adaptor_holder(funct_type f) :
      fct_(f)
    {
    }

    template <class E>
    typename H::template rebind_right<decay_t<E>>::type::result_type operator()(E e)
    {
      return typename H::template rebind_right<decay_t<E>>::type(fct_)(std::forward<E>(e));
    }
  private:
    funct_type fct_;
  };

namespace detail
{

  template <class E, class F, class V>
  class if_valued0
  {
    F fct_;
  public:
    explicit if_valued0(F f) :
      fct_(f)
    {
    }

    typedef rebindable::value_type<E> value_type;
    typedef typename rebindable::rebind<E, typename std::result_of<F(value_type)>::type>::type result_type;

    result_type operator()(E e)
    {
      using namespace ::boost::functional::errored;
      if (has_value(e))
      {
        return result_type(fct_(deref(e)));
      }
      else
      {
        return result_type(get_errored(e));
      }
    }
  };

  template <class E, class F, class R>
  class if_valued2
  {
    F fct_;
  public:
    explicit if_valued2(F f) :
      fct_(f)
    {
    }

    typedef void value_type;
    typedef typename rebindable::rebind<E, R>::type result_type;

    result_type operator()(E e)
    {
      using namespace ::boost::functional::errored;
      if (has_value(e))
      {
        return result_type(fct_());
      }
      else
      {
        return result_type(get_errored(e));
      }
    }
  };

  template <class E, class F>
  class if_valued2<E, F, void>
  {
    F fct_;
  public:
    explicit if_valued2(F f) :
      fct_(f)
    {
    }

    typedef void value_type;
    typedef typename rebindable::rebind<E, void>::type result_type;

    result_type operator()(E e)
    {
      using namespace ::boost::functional::errored;
      if (has_value(e))
      {
        fct_();
        return result_type(in_place2);
      }
      else
      {
        return result_type(get_errored(e));
      }
    }
  };

  template <class E, class F>
  struct if_valued0<E, F, void> : if_valued2<E, F, typename std::result_of<F()>::type>
  {

    explicit if_valued0(F f) :
      if_valued2<E, F, typename std::result_of<F()>::type> (f)
    {
    }

  };

  template <class F>
  struct if_valued_adaptor
  {
    typedef F funct_type;
    template <class E>
    struct rebind_right
    {
      typedef if_valued0<E, funct_type, rebindable::value_type<E>> type;
    };
  };
}

  template <class F>
  inline adaptor_holder<detail::if_valued_adaptor<F> > if_valued(F f)
  {
    return adaptor_holder<detail::if_valued_adaptor<F> > (f);
  }

namespace detail
{

  template <class F>
  class ident_t
  {
    F fct_;
  public:

    explicit ident_t(F f) :
      fct_(f)
    {
    }

    typedef typename std::result_of<F()>::type result_type;

    template <class G>
    result_type operator()(G e)
    {
      return fct_();
    }
  };
}

  template <class F>
  inline detail::ident_t<F> ident(F f)
  {
    return detail::ident_t<F>(f);
  }

namespace detail
{

  template <class E, class F, class V>
  class if_unexpected
  {
    F fct_;
  public:
    typedef F funct_type;
    typedef E result_type;

    explicit if_unexpected(funct_type f) :
      fct_(f)
    {
    }

    decay_t<E> operator()(E e)
    {
      using namespace ::boost::functional::errored;
      if (!has_value(e))
      {
        return result_type(fct_(error(e)));
      }
      else
      {
        return std::move(e);
      }
    }
  };

  template <class F>
  struct if_unexpected_adaptor
  {
    typedef F funct_type;
    template <class E>
    struct rebind_right
    {
      typedef if_unexpected<E, funct_type, rebindable::value_type<E>> type;
    };
  };
}

  template <class F>
  inline adaptor_holder<detail::if_unexpected_adaptor<F> > if_unexpected(F f)
  {
    return adaptor_holder<detail::if_unexpected_adaptor<F> > (f);
  }

  namespace detail
  {

    template <class E, class F, class V>
    class catch_all
    {
      F fct_;
    public:
      typedef F funct_type;
      typedef E result_type;

      explicit catch_all(funct_type f) :
        fct_(f)
      {
      }

      decay_t<E> operator()(E e)
      {
        using namespace ::boost::functional::monad_exception;
        using namespace ::boost::functional::valued;

        try {
          std::cout << __FILE__ << __LINE__ << std::endl;
          return fct_(value(e));
        } catch (...) {
          std::cout << __FILE__ << __LINE__ << std::endl;
          return make_exception<type_constructor<E>>(std::current_exception());
        }
      }
    };

    template <class F>
    struct catch_all_adaptor
    {
      typedef F funct_type;
      template <class E>
      struct rebind_right
      {
        typedef catch_all<E, funct_type, rebindable::value_type<E>> type;
      };
    };
  }
  template <class F>
  inline adaptor_holder<detail::catch_all_adaptor<F> > catch_all(F f)
  {
    return adaptor_holder<detail::catch_all_adaptor<F> > (f);
  }

}
} // namespace boost

#endif // BOOST_FUNCTIONAL_ADAPTOR_HPP
