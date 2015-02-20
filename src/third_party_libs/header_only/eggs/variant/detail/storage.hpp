//! \file eggs/variant/detail/storage.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_DETAIL_STORAGE_HPP
#define EGGS_VARIANT_DETAIL_STORAGE_HPP

#include <eggs/variant/detail/pack.hpp>
#include <eggs/variant/detail/visitor.hpp>

#include <cstddef>
#include <new>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants { namespace detail
{
    ///////////////////////////////////////////////////////////////////////////
    namespace conditionally_deleted_adl
    {
        template <bool CopyCnstr, bool MoveCnstr = CopyCnstr>
        struct conditionally_deleted_cnstr
        {};

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS && EGGS_CXX11_HAS_DELETED_FUNCTIONS
        template <>
        struct conditionally_deleted_cnstr<true, false>
        {
            conditionally_deleted_cnstr() EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr(conditionally_deleted_cnstr const&) EGGS_CXX11_NOEXCEPT = delete;
            conditionally_deleted_cnstr(conditionally_deleted_cnstr&&) EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr const&) EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr&&) EGGS_CXX11_NOEXCEPT = default;
        };

        template <>
        struct conditionally_deleted_cnstr<false, true>
        {
            conditionally_deleted_cnstr() EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr(conditionally_deleted_cnstr const&) EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr(conditionally_deleted_cnstr&&) EGGS_CXX11_NOEXCEPT = delete;
            conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr const&) EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr&&) EGGS_CXX11_NOEXCEPT = default;
        };

        template <>
        struct conditionally_deleted_cnstr<true, true>
        {
            conditionally_deleted_cnstr() EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr(conditionally_deleted_cnstr const&) EGGS_CXX11_NOEXCEPT = delete;
            conditionally_deleted_cnstr(conditionally_deleted_cnstr&&) EGGS_CXX11_NOEXCEPT = delete;
            conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr const&) EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr&&) EGGS_CXX11_NOEXCEPT = default;
        };
#endif

        template <bool CopyAssign, bool MoveAssign = CopyAssign>
        struct conditionally_deleted_assign
        {};

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS && EGGS_CXX11_HAS_DELETED_FUNCTIONS
        template <>
        struct conditionally_deleted_assign<true, false>
        {
            conditionally_deleted_assign() EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_assign& operator=(conditionally_deleted_assign const&) EGGS_CXX11_NOEXCEPT = delete;
            conditionally_deleted_assign& operator=(conditionally_deleted_assign&&) EGGS_CXX11_NOEXCEPT = default;
        };

        template <>
        struct conditionally_deleted_assign<false, true>
        {
            conditionally_deleted_assign() EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_assign& operator=(conditionally_deleted_assign const&) EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_assign& operator=(conditionally_deleted_assign&&) EGGS_CXX11_NOEXCEPT = delete;
        };

        template <>
        struct conditionally_deleted_assign<true, true>
        {
            conditionally_deleted_assign() EGGS_CXX11_NOEXCEPT = default;
            conditionally_deleted_assign& operator=(conditionally_deleted_assign const&) EGGS_CXX11_NOEXCEPT = delete;
            conditionally_deleted_assign& operator=(conditionally_deleted_assign&&) EGGS_CXX11_NOEXCEPT = delete;
        };
#endif
    }

    using conditionally_deleted_adl::conditionally_deleted_cnstr;
    using conditionally_deleted_adl::conditionally_deleted_assign;

    ///////////////////////////////////////////////////////////////////////////
#if EGGS_CXX11_STD_HAS_ALIGNED_UNION
    using std::aligned_union;
#else
    template <std::size_t ...Vs>
    struct _static_max;

    template <std::size_t V0>
    struct _static_max<V0>
      : std::integral_constant<std::size_t, V0>
    {};

    template <std::size_t V0, std::size_t V1, std::size_t ...Vs>
    struct _static_max<V0, V1, Vs...>
      : _static_max<V0 < V1 ? V1 : V0, Vs...>
    {};

    template <std::size_t Len, typename ...Types>
    struct aligned_union
      : std::aligned_storage<
            _static_max<Len, sizeof(Types)...>::value
          , _static_max<std::alignment_of<Types>::value...>::value
        >
    {};
#endif

    ///////////////////////////////////////////////////////////////////////////
    template <typename Ts, bool TriviallyCopyable, bool TriviallyDestructible>
    struct _storage;

    template <typename ...Ts>
    struct _storage<pack<Ts...>, true, true>
      : conditionally_deleted_cnstr<
            !all_of<pack<std::is_copy_constructible<Ts>...>>::value
          , !all_of<pack<std::is_move_constructible<Ts>...>>::value
        >
      , conditionally_deleted_assign<
            !all_of<pack<std::is_copy_assignable<Ts>...>>::value
          , !all_of<pack<std::is_move_assignable<Ts>...>>::value
        >
    {
    public:
        _storage() EGGS_CXX11_NOEXCEPT
          : _which{0}
        {}

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        _storage(_storage const& rhs) EGGS_CXX11_NOEXCEPT = default;
        _storage(_storage&& rhs) EGGS_CXX11_NOEXCEPT = default;
#endif

        template <std::size_t I, typename ...Args>
        _storage(std::integral_constant<std::size_t, I> which, Args&&... args)
        {
            emplace(which, std::forward<Args>(args)...);
        }

        template <std::size_t I, typename ...Args>
        void emplace(std::integral_constant<std::size_t, I> which, Args&&... args)
        {
            ::new (&_buffer) typename at_index<
                I, pack<empty, Ts...>
            >::type(std::forward<Args>(args)...);
            _which = which;
        }

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        _storage& operator=(_storage const& rhs) EGGS_CXX11_NOEXCEPT = default;
        _storage& operator=(_storage&& rhs) EGGS_CXX11_NOEXCEPT = default;
#endif

        void swap(_storage& rhs)
        {
            std::swap(*this, rhs);
        }

        std::size_t which() const
        {
            return _which;
        }

        void* target()
        {
            return &_buffer;
        }

        void const* target() const
        {
            return &_buffer;
        }

    protected:
        std::size_t _which;
        typename aligned_union<0, Ts...>::type _buffer;
    };

    template <typename ...Ts>
    struct _storage<pack<Ts...>, false, true>
      : _storage<pack<Ts...>, true, true>
    {
        using base_type = _storage<pack<Ts...>, true, true>;

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        _storage() EGGS_CXX11_NOEXCEPT = default;
#else
        _storage() EGGS_CXX11_NOEXCEPT
          : base_type{}
        {}
#endif

        _storage(_storage const& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(all_of<pack<
                std::is_nothrow_copy_constructible<Ts>...
            >>::value)
#endif
          : base_type{}
        {
            detail::copy_construct{}(
                pack<empty, Ts...>{}, rhs._which
              , &_buffer, &rhs._buffer
            );
            _which = rhs._which;
        }

        _storage(_storage&& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(all_of<pack<
                std::is_nothrow_move_constructible<Ts>...
            >>::value)
#endif
          : base_type{}
        {
            detail::move_construct{}(
                pack<empty, Ts...>{}, rhs._which
              , &_buffer, &rhs._buffer
            );
            _which = rhs._which;
        }

        template <std::size_t I, typename ...Args>
        _storage(std::integral_constant<std::size_t, I> which, Args&&... args)
        {
            emplace(which, std::forward<Args>(args)...);
        }

        template <std::size_t I, typename ...Args>
        void emplace(std::integral_constant<std::size_t, I> which, Args&&... args)
        {
            _which = 0;

            base_type::emplace(which, std::forward<Args>(args)...);
        }

        _storage& operator=(_storage const& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(all_of<pack<
                std::is_nothrow_copy_assignable<Ts>...
              , std::is_nothrow_copy_constructible<Ts>...
            >>::value)
#endif
        {
            if (_which == rhs._which)
            {
                detail::copy_assign{}(
                    pack<empty, Ts...>{}, _which
                  , &_buffer, &rhs._buffer
                );
            } else {
                _which = 0;

                detail::copy_construct{}(
                    pack<empty, Ts...>{}, rhs._which
                  , &_buffer, &rhs._buffer
                );
                _which = rhs._which;
            }
            return *this;
        }

        _storage& operator=(_storage&& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(all_of<pack<
                std::is_nothrow_move_assignable<Ts>...
              , std::is_nothrow_move_constructible<Ts>...
            >>::value)
#endif
        {
            if (_which == rhs._which)
            {
                detail::move_assign{}(
                    pack<empty, Ts...>{}, _which
                  , &_buffer, &rhs._buffer
                );
            } else {
                _which = 0;

                detail::move_construct{}(
                    pack<empty, Ts...>{}, rhs._which
                  , &_buffer, &rhs._buffer
                );
                _which = rhs._which;
            }
            return *this;
        }

        void swap(_storage& rhs)
        {
            if (_which == rhs._which)
            {
                detail::swap{}(
                    pack<empty, Ts...>{}, _which
                  , &_buffer, &rhs._buffer
                );
            } else if (_which == 0) {
                *this = std::move(rhs);
                rhs._which = 0;
            } else if (rhs._which == 0) {
                rhs = std::move(*this);
                _which = 0;
            } else {
                std::swap(*this, rhs);
            }
        }

    protected:
        using base_type::_which;
        using base_type::_buffer;
    };

    template <typename ...Ts>
    struct _storage<pack<Ts...>, false, false>
      : _storage<pack<Ts...>, false, true>
    {
        using base_type = _storage<pack<Ts...>, false, true>;

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        _storage() EGGS_CXX11_NOEXCEPT = default;
        _storage(_storage const& rhs) = default;
        _storage(_storage&& rhs) = default;
#else
        _storage() EGGS_CXX11_NOEXCEPT
          : base_type{}
        {}

        _storage(_storage const& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(
                std::is_nothrow_copy_constructible<base_type>::value
            )
#endif
          : base_type{static_cast<base_type const&>(rhs)}
        {}

        _storage(_storage&& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(
                std::is_nothrow_move_constructible<base_type>::value
            )
#endif
          : base_type{static_cast<base_type&&>(rhs)}
        {}
#endif

        template <std::size_t I, typename ...Args>
        _storage(std::integral_constant<std::size_t, I> which, Args&&... args)
        {
            emplace(which, std::forward<Args>(args)...);
        }

        ~_storage()
        {
            _destroy();
        }

        template <std::size_t I, typename ...Args>
        void emplace(std::integral_constant<std::size_t, I> which, Args&&... args)
        {
            _destroy();
            base_type::emplace(which, std::forward<Args>(args)...);
        }

        _storage& operator=(_storage const& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(all_of<pack<
                std::is_nothrow_copy_assignable<Ts>...
              , std::is_nothrow_copy_constructible<Ts>...
            >>::value)
#endif
        {
            if (_which != rhs._which)
            {
                _destroy();
            }
            base_type::operator=(rhs);
            return *this;
        }

        _storage& operator=(_storage&& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(all_of<pack<
                std::is_nothrow_move_assignable<Ts>...
              , std::is_nothrow_move_constructible<Ts>...
            >>::value)
#endif
        {
            if (_which != rhs._which)
            {
                _destroy();
            }
            base_type::operator=(std::move(rhs));
            return *this;
        }

        void swap(_storage& rhs)
        {
            if (_which == 0)
            {
                base_type::swap(rhs);
                rhs._destroy();
            } else if (rhs._which == 0) {
                base_type::swap(rhs);
                _destroy();
            } else {
                base_type::swap(rhs);
            }
        }

    protected:
        void _destroy()
        {
            detail::destroy{}(
                pack<empty, Ts...>{}, _which
              , &_buffer
            );
        }

    protected:
        using base_type::_which;
        using base_type::_buffer;
    };

    template <typename ...Ts>
    using storage = _storage<
        pack<Ts...>
#if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_DESTRUCTIBLE
#  if EGGS_CXX11_STD_HAS_IS_TRIVIALLY_COPYABLE
      , all_of<pack<std::is_trivially_copyable<Ts>...>>::value
      , all_of<pack<std::is_trivially_destructible<Ts>...>>::value
#  else
      , all_of<pack<std::is_pod<Ts>...>>::value
      , all_of<pack<std::is_trivially_destructible<Ts>...>>::value
#  endif
#else
      , all_of<pack<std::is_pod<Ts>...>>::value
      , all_of<pack<std::is_pod<Ts>...>>::value
#endif
    >;
}}}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_DETAIL_STORAGE_HPP*/
