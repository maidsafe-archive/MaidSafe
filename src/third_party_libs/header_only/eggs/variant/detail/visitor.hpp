//! \file eggs/variant/detail/visitor.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_DETAIL_VISITOR_HPP
#define EGGS_VARIANT_DETAIL_VISITOR_HPP

#include <eggs/variant/detail/pack.hpp>

#include <eggs/variant/bad_variant_access.hpp>

#include <cassert>
#include <cstddef>
#include <exception>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants
{
    template <typename ...Ts>
    class variant;
}}

namespace eggs { namespace variants { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    template <typename F, typename Sig>
    struct visitor;

    template <typename F, typename R, typename ...Args>
    struct visitor<F, R(Args...)>
    {
        template <typename ...Ts>
        R operator()(pack<Ts...>, std::size_t which, Args&&... args) const
        {
            using function_ptr = R(*)(Args...);
            EGGS_CXX11_STATIC_CONSTEXPR function_ptr table[] = {
                &F::template call<Ts>...};

            assert(which < sizeof...(Ts) && "discriminator out of bounds");
            return table[which](std::forward<Args>(args)...);
        }

        EGGS_CXX11_NORETURN R operator()(pack<>, std::size_t, Args&&...) const
        {
            std::terminate();
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    struct copy_construct
      : visitor<copy_construct, void(void*, void const*)>
    {
        template <typename T>
        static void call(void* ptr, void const* other)
        {
            ::new (ptr) T(*static_cast<T const*>(other));
        }
    };

    struct move_construct
      : visitor<move_construct, void(void*, void*)>
    {
        template <typename T>
        static void call(void* ptr, void* other)
        {
            ::new (ptr) T(std::move(*static_cast<T*>(other)));
        }
    };

    struct copy_assign
      : visitor<copy_assign, void(void*, void const*)>
    {
        template <typename T>
        static void call(void* ptr, void const* other)
        {
            *static_cast<T*>(ptr) = *static_cast<T const*>(other);
        }
    };

    struct move_assign
      : visitor<move_assign, void(void*, void*)>
    {
        template <typename T>
        static void call(void* ptr, void* other)
        {
            *static_cast<T*>(ptr) = std::move(*static_cast<T*>(other));
        }
    };

    struct destroy
      : visitor<destroy, void(void*)>
    {
        template <typename T>
        static void call(void* ptr)
        {
            static_cast<T*>(ptr)->~T();
        }
    };

    struct swap
      : visitor<swap, void(void*, void*)>
    {
        template <typename T>
        static void call(void* ptr, void* other)
        {
            using std::swap;
            swap(*static_cast<T*>(ptr), *static_cast<T*>(other));
        }
    };

    struct equal_to
      : visitor<equal_to, bool(void const*, void const*)>
    {
        template <typename T>
        static bool call(void const* ptr, void const* other)
        {
            return *static_cast<T const*>(ptr) == *static_cast<T const*>(other);
        }
    };

    struct less
      : visitor<less, bool(void const*, void const*)>
    {
        template <typename T>
        static bool call(void const* ptr, void const* other)
        {
            return *static_cast<T const*>(ptr) < *static_cast<T const*>(other);
        }
    };

    struct type_id
      : visitor<type_id, std::type_info const&()>
    {
        template <typename T>
        static std::type_info const& call()
        {
            return typeid(T);
        }
    };

    ///////////////////////////////////////////////////////////////////////////
    template <typename F, typename ...Args>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Args&&... args)
        EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
            std::forward<F>(f)(std::forward<Args>(args)...)))
     -> decltype(std::forward<F>(f)(std::forward<Args>(args)...))
    {
        return std::forward<F>(f)(std::forward<Args>(args)...);
    }

#if EGGS_CXX11_HAS_SFINAE_FOR_EXPRESSIONS
    template <typename F, typename Arg0, typename ...Args>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0, Args&&... args)
        EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
            (arg0.*f)(std::forward<Args>(args)...)))
     -> decltype((arg0.*f)(std::forward<Args>(args)...))
    {
        return (arg0.*f)(std::forward<Args>(args)...);
    }

    template <typename F, typename Arg0, typename ...Args>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0, Args&&... args)
        EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
            ((*arg0).*f)(std::forward<Args>(args)...)))
     -> decltype(((*arg0).*f)(std::forward<Args>(args)...))
    {
        return ((*arg0).*f)(std::forward<Args>(args)...);
    }

    template <typename F, typename Arg0>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0) EGGS_CXX11_NOEXCEPT
     -> decltype(arg0.*f)
    {
        return arg0.*f;
    }

    template <typename F, typename Arg0>
    EGGS_CXX11_CONSTEXPR auto _invoke(F&& f, Arg0&& arg0) EGGS_CXX11_NOEXCEPT
     -> decltype((*arg0).*f)
    {
        return (*arg0).*f;
    }
#endif

    template <typename R>
    struct _invoke_guard
    {
        template <typename ...Ts>
        EGGS_CXX11_CONSTEXPR R operator()(Ts&&... vs) const
            EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
                _invoke(std::forward<Ts>(vs)...)))
        {
            return _invoke(std::forward<Ts>(vs)...);
        }
    };

    template <>
    struct _invoke_guard<void>
    {
        template <typename ...Ts>
        EGGS_CXX14_CONSTEXPR void operator()(Ts&&... vs) const
            EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(
                _invoke(std::forward<Ts>(vs)...)))
        {
            _invoke(std::forward<Ts>(vs)...);
        }
    };

    template <typename Variant>
    struct _qualified_pack;

    template <typename ...Ts>
    struct _qualified_pack<variant<Ts...>&>
      : pack<Ts&...>
    {};

    template <typename ...Ts>
    struct _qualified_pack<variant<Ts...> const&>
      : pack<Ts const&...>
    {};

    template <typename ...Ts>
    struct _qualified_pack<variant<Ts...>&&>
      : pack<Ts&&...>
    {};

    template <
        typename R, typename F, typename Ms
      , typename IsConst, typename Vs
    >
    struct _apply;

    template <typename R, typename F, typename ...Ms>
    struct _apply<R, F, pack<Ms...>, std::false_type, pack<>>
      : visitor<
            _apply<R, F, pack<Ms...>, std::false_type, pack<>>
          , R(F, Ms..., void*)
        >
    {
        template <
            typename T
          , typename U = typename std::remove_cv<
                typename std::remove_reference<T>::type>::type
        >
        static R call(F f, Ms... ms, void* ptr)
        {
            return _invoke_guard<R>{}(
                std::forward<F>(f), std::forward<Ms>(ms)...
              , std::forward<T>(*static_cast<U*>(ptr)));
        }
    };

    template <typename R, typename F, typename ...Ms>
    struct _apply<R, F, pack<Ms...>, std::true_type, pack<>>
      : visitor<
            _apply<R, F, pack<Ms...>, std::true_type, pack<>>
          , R(F, Ms..., void const*)
        >
    {
        template <
            typename T
          , typename U = typename std::remove_cv<
                typename std::remove_reference<T>::type>::type const
        >
        static R call(F f, Ms... ms, void const* ptr)
        {
            return _invoke_guard<R>{}(
                std::forward<F>(f), std::forward<Ms>(ms)...
              , std::forward<T>(*static_cast<U*>(ptr)));
        }
    };

    template <typename R, typename F, typename ...Ms, typename V, typename ...Vs>
    struct _apply<R, F, pack<Ms...>, std::false_type, pack<V, Vs...>>
      : visitor<
            _apply<R, F, pack<Ms...>, std::false_type, pack<V, Vs...>>
          , R(F, Ms..., void*, V, Vs...)
        >
    {
        template <
            typename T
          , typename U = typename std::remove_cv<
                typename std::remove_reference<T>::type>::type
          , typename IsConst = typename std::is_const<
                typename std::remove_reference<V>::type>::type
        >
        static R call(F f, Ms... ms, void* ptr, V v, Vs... vs)
        {
            return bool(v)
              ? _apply<R, F, pack<Ms..., T>, IsConst, pack<Vs...>>{}(
                    _qualified_pack<V>{}, v.which()
                  , std::forward<F>(f), std::forward<Ms>(ms)...
                  , std::forward<T>(*static_cast<U*>(ptr))
                  , v.target(), std::forward<Vs>(vs)...
                )
              : throw bad_variant_access{};
        }
    };

    template <typename R, typename F, typename ...Ms, typename V, typename ...Vs>
    struct _apply<R, F, pack<Ms...>, std::true_type, pack<V, Vs...>>
      : visitor<
            _apply<R, F, pack<Ms...>, std::true_type, pack<V, Vs...>>
          , R(F, Ms..., void const*, V, Vs...)
        >
    {
        template <
            typename T
          , typename U = typename std::remove_cv<
                typename std::remove_reference<T>::type>::type const
          , typename IsConst = typename std::is_const<
                typename std::remove_reference<V>::type>::type
        >
        static R call(F f, Ms... ms, void const* ptr, V v, Vs... vs)
        {
            return bool(v)
              ? _apply<R, F, pack<Ms..., T>, IsConst, pack<Vs...>>{}(
                    _qualified_pack<V>{}, v.which()
                  , std::forward<F>(f), std::forward<Ms>(ms)...
                  , std::forward<T>(*static_cast<U*>(ptr))
                  , v.target(), std::forward<Vs>(vs)...
                )
              : throw bad_variant_access{};
        }
    };

    template <
        typename R, typename F, typename V, typename ...Vs
      , typename IsConst = typename std::is_const<
            typename std::remove_reference<V>::type>::type
    >
    R apply(F&& f, V&& v, Vs&&... vs)
    {
        return bool(v)
          ? _apply<R, F&&, pack<>, IsConst, pack<Vs&&...>>{}(
                _qualified_pack<V&&>{}, v.which()
              , std::forward<F>(f), v.target(), std::forward<Vs>(vs)...
            )
          : throw bad_variant_access{};
    }

    ///////////////////////////////////////////////////////////////////////////
    template <typename ...Ts>
    struct _always_void
    {
        using type = void;
    };

    template <typename T, typename Enable = void>
    struct _weak_result
    {};

    template <typename R, typename ...Args>
    struct _weak_result<R(*)(Args...)>
    {
        using type = R;
    };

    template <typename R, typename ...Args>
    struct _weak_result<R(*)(Args..., ...)>
    {
        using type = R;
    };

    template <typename C>
    struct _weak_result<C, typename _always_void<typename C::result_type>::type>
    {
        using type = typename C::result_type;
    };

    template <typename T>
    using weak_result = typename _weak_result<T>::type;
}}}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_DETAIL_VISITOR_HPP*/
