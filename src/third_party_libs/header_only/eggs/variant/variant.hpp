//! \file eggs/variant/variant.hpp
// Eggs.Variant
//
// Copyright Agustin K-ballo Berge, Fusion Fenix 2014
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef EGGS_VARIANT_VARIANT_HPP
#define EGGS_VARIANT_VARIANT_HPP

#include <eggs/variant/detail/pack.hpp>
#include <eggs/variant/detail/visitor.hpp>

#include <eggs/variant/bad_variant_access.hpp>
#include <eggs/variant/in_place.hpp>
#include <eggs/variant/nullvariant.hpp>

#include <cstddef>
#include <functional>
#include <initializer_list>
#include <new>
#include <type_traits>
#include <typeinfo>
#include <utility>

#include <eggs/variant/detail/config/prefix.hpp>

namespace eggs { namespace variants
{
    template <typename ...Ts>
    class variant;

    namespace detail
    {
        ///////////////////////////////////////////////////////////////////////
        template <typename T>
        struct is_variant
          : std::false_type
        {};

        template <typename ...Ts>
        struct is_variant<variant<Ts...>>
          : std::true_type
        {};

        template <typename ...Ts>
        struct is_variant<variant<Ts...> const>
          : std::true_type
        {};

        template <typename ...Ts>
        struct is_variant<variant<Ts...> volatile>
          : std::true_type
        {};

        template <typename ...Ts>
        struct is_variant<variant<Ts...> const volatile>
          : std::true_type
        {};

        ///////////////////////////////////////////////////////////////////////
        namespace swap_adl
        {
            using std::swap;

            template <typename T>
            struct is_nothrow_swappable
              : std::integral_constant<
                    bool
                  , EGGS_CXX11_NOEXCEPT_EXPR(swap(std::declval<T&>(), std::declval<T&>()))
                >
            {};
        }

        template <typename T>
        struct is_nothrow_swappable
          : swap_adl::is_nothrow_swappable<T>
        {};

        ///////////////////////////////////////////////////////////////////////
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

        ///////////////////////////////////////////////////////////////////////
        template <typename Ts, bool TriviallyCopyable, bool TriviallyDestructible>
        struct _storage;

        template <typename ...Ts>
        struct _storage<pack<Ts...>, true, true>
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
                ::new (&_buffer) typename detail::at_index<
                    I, detail::pack<empty, Ts...>
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

        ///////////////////////////////////////////////////////////////////////
        namespace conditionally_deleted_adl
        {
            template <bool CopyCnstr, bool MoveCnstr = CopyCnstr>
            struct conditionally_deleted_cnstr
            {};

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS && EGGS_CXX11_HAS_DELETED_FUNCTIONS
            template <>
            struct conditionally_deleted_cnstr<true, false>
            {
                conditionally_deleted_cnstr() = default;
                conditionally_deleted_cnstr(conditionally_deleted_cnstr const&) = delete;
                conditionally_deleted_cnstr(conditionally_deleted_cnstr&&) = default;
                conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr const&) = default;
                conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr&&) = default;
            };

            template <>
            struct conditionally_deleted_cnstr<false, true>
            {
                conditionally_deleted_cnstr() = default;
                conditionally_deleted_cnstr(conditionally_deleted_cnstr const&) = default;
                conditionally_deleted_cnstr(conditionally_deleted_cnstr&&) = delete;
                conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr const&) = default;
                conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr&&) = default;
            };

            template <>
            struct conditionally_deleted_cnstr<true, true>
            {
                conditionally_deleted_cnstr() = default;
                conditionally_deleted_cnstr(conditionally_deleted_cnstr const&) = delete;
                conditionally_deleted_cnstr(conditionally_deleted_cnstr&&) = delete;
                conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr const&) = default;
                conditionally_deleted_cnstr& operator=(conditionally_deleted_cnstr&&) = default;
            };
#endif

            template <bool CopyAssign, bool MoveAssign = CopyAssign>
            struct conditionally_deleted_assign
            {};

#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS && EGGS_CXX11_HAS_DELETED_FUNCTIONS
            template <>
            struct conditionally_deleted_assign<true, false>
            {
                conditionally_deleted_assign() = default;
                conditionally_deleted_assign& operator=(conditionally_deleted_assign const&) = delete;
                conditionally_deleted_assign& operator=(conditionally_deleted_assign&&) = default;
            };

            template <>
            struct conditionally_deleted_assign<false, true>
            {
                conditionally_deleted_assign() = default;
                conditionally_deleted_assign& operator=(conditionally_deleted_assign const&) = default;
                conditionally_deleted_assign& operator=(conditionally_deleted_assign&&) = delete;
            };

            template <>
            struct conditionally_deleted_assign<true, true>
            {
                conditionally_deleted_assign() = default;
                conditionally_deleted_assign& operator=(conditionally_deleted_assign const&) = delete;
                conditionally_deleted_assign& operator=(conditionally_deleted_assign&&) = delete;
            };
#endif
        }
        using conditionally_deleted_adl::conditionally_deleted_cnstr;
        using conditionally_deleted_adl::conditionally_deleted_assign;

        ///////////////////////////////////////////////////////////////////////
        struct hash
        {
            using result_type = std::size_t;

            template <typename T>
            std::size_t operator()(T const& v) const
            {
                return std::hash<T>{}(v);
            }
        };
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class ...Ts> class variant;
    //!
    //! In a `variant`, at most one of the members can be active at any time,
    //! that is, the value of at most one of the members can be stored in a
    //! `variant` at any time.  Implementations are not permitted to use
    //! additional storage, such as dynamic memory, to allocate its contained
    //! value. The contained value shall be allocated in a region of the
    //! `variant<Ts...>` storage suitably aligned for the types `Ts...`.
    //!
    //! All `T` in `Ts...` shall be object types and shall satisfy the
    //! requirements of `Destructible`.
    template <typename ...Ts>
    class variant
      : detail::conditionally_deleted_cnstr<
            !detail::all_of<detail::pack<std::is_copy_constructible<Ts>...>>::value
          , !detail::all_of<detail::pack<std::is_move_constructible<Ts>...>>::value
        >
      , detail::conditionally_deleted_assign<
            !detail::all_of<detail::pack<std::is_copy_assignable<Ts>...>>::value
          , !detail::all_of<detail::pack<std::is_move_assignable<Ts>...>>::value
        >
    {
        static_assert(
            !detail::any_of<detail::pack<
                std::is_function<Ts>...>>::value
          , "variant member has function type");

        static_assert(
            !detail::any_of<detail::pack<
                std::is_reference<Ts>...>>::value
          , "variant member has reference type");

        static_assert(
            !detail::any_of<detail::pack<
                std::is_void<Ts>...>>::value
          , "variant member has void type");

        static_assert(
            !detail::any_of<detail::pack<
                detail::is_null_variant<Ts>...>>::value
          , "variant member has nullvariant_t type");

    public:
        //! static constexpr std::size_t npos = std::size_t(-1);
        EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = std::size_t(-1);

    public:
        //! variant() noexcept;
        //!
        //! \postconditions `*this` does not have an active member.
        //!
        //! \remarks No member is initialized.
        variant() EGGS_CXX11_NOEXCEPT
          : _storage{}
        {}

        //! variant(nullvariant_t) noexcept;
        //!
        //! \postconditions `*this` does not have an active member.
        //!
        //! \remarks No member is initialized.
        variant(nullvariant_t) EGGS_CXX11_NOEXCEPT
          : _storage{}
        {}

        //! variant(variant const& rhs);
        //!
        //! \requires `std::is_copy_constructible_v<T>` is `true` for all `T`
        //!  in `Ts...`.
        //!
        //! \effects If `rhs` has an active member of type `T`, initializes
        //!  the active member as if direct-non-list-initializing an object of
        //!  type `T` with the expression `*rhs.target<T>()`.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks If `std::is_trivially_copyable_v<T>` is `true` for all
        //!  `T` in `Ts...`, then this copy constructor shall be trivial.
#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        variant(variant const& rhs) = default;
#endif

        //! variant(variant&& rhs) noexcept(see below);
        //!
        //! \requires `std::is_move_constructible_v<T>` is `true` for all `T`
        //!  in `Ts...`.
        //!
        //! \effects If `rhs` has an active member of type `T`, initializes
        //!  the active member as if direct-non-list-initializing an object of
        //!  type `T` with the expression `std::move(*rhs.target<T>())`.
        //!  `bool(rhs)` is unchanged.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks The expression inside `noexcept` is equivalent to the
        //!  logical AND of `std::is_nothrow_move_constructible_v<Ts>...`. If
        //!  `std::is_trivially_copyable_v<T>` is `true` for all `T` in
        //!  `Ts...`, then this move constructor shall be trivial.
#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        variant(variant&& rhs) = default;
#endif

        //! template <class U>
        //! variant(U&& v);
        //!
        //! Let `T` be `std::remove_cv_t<std::remove_reference_t<U>>`
        //!
        //! \requires `std::is_constructible_v<T, U&&>` is `true`.
        //!
        //! \effects Initializes the active member as if direct-non-list-
        //!  initializing an object of type `T` with the expression
        //!  `std::forward<U>(v)`.
        //!
        //! \postconditions `*this` has an active member.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks This constructor shall not participate in overload
        //!  resolution unless `T` occurs exactly once in
        //!  `std::remove_cv_t<Ts>...`.
        template <
            typename U
          , typename T = typename std::remove_cv<
                typename std::remove_reference<U>::type>::type
          , typename Enable = typename std::enable_if<detail::contains<
                T, detail::pack<typename std::remove_cv<Ts>::type...>
            >::value>::type
        > variant(U&& v)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(
                std::is_nothrow_constructible<T, U&&>::value)
#endif
          : _storage{detail::index_of<T, detail::pack<
                    detail::empty, typename std::remove_cv<Ts>::type...
                >>{}, std::forward<U>(v)}
        {}

        //! template <std::size_t I, class ...Args>
        //! explicit variant(unspecified<I>, Args&&... args);
        //!
        //! Let `T` be the `I`th element in `Ts...`, where indexing is
        //! zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and `std::is_constructible_v<T,
        //!  Args&&...>` is `true`.
        //!
        //! \effects Initializes the active member as if direct-non-list-
        //!  initializing an object of type `T` with the arguments
        //!  `std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks The first argument shall be the expression `in_place<I>`.
        template <
            std::size_t I, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
        >
        explicit variant(
            in_place_t(detail::pack_c<std::size_t, I>)
          , Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(
                std::is_nothrow_constructible<T, Args&&...>::value)
#endif
          : _storage{std::integral_constant<std::size_t, I + 1>{},
                std::forward<Args>(args)...}
        {}

        //! template <std::size_t I, class U, class ...Args>
        //! explicit variant(unspecified<I>, std::initializer_list<U> il, Args&&... args);
        //!
        //! Let `T` be the `I`th element in `Ts...`, where indexing is
        //! zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and  `std::is_constructible_v<T,
        //!  initializer_list<U>&, Args&&...>` is `true`.
        //!
        //! \effects Initializes the active member as if direct-non-list-
        //!  initializing an object of type `T` with the arguments `il,
        //!  std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \remarks The first argument shall be the expression `in_place<I>`.
        //!  This function shall not participate in overload resolution unless
        //!  `std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>`
        //!  is `true`.
        template <
            std::size_t I, typename U, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
          , typename Enable = typename std::enable_if<std::is_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value>::type
        >
        explicit variant(
            in_place_t(detail::pack_c<std::size_t, I>)
          , std::initializer_list<U> il, Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(std::is_nothrow_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value)
#endif
          : _storage{std::integral_constant<std::size_t, I + 1>{},
                il, std::forward<Args>(args)...}
        {}

        //! template <class T, class ...Args>
        //! explicit variant(unspecified<T>, Args&&... args);
        //!
        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `variant(in_place<I>,
        //!  std::forward<Args>(args)...)` where `I` is the zero-based index
        //!  of `T` in `Ts...`.
        //!
        //! \remarks The first argument shall be the expression `in_place<T>`.
        template <typename T, typename ...Args>
        explicit variant(
            in_place_t(detail::pack<T>)
          , Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(
                std::is_nothrow_constructible<T, Args&&...>::value)
#endif
          : _storage{detail::index_of<T, detail::pack<
                    detail::empty, typename std::remove_cv<Ts>::type...
                >>{}, std::forward<Args>(args)...}
        {}

        //! template <class T, class U, class ...Args>
        //! explicit variant(unspecified<T>, std::initializer_list<U> il, Args&&... args);
        //!
        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `variant(in_place<I>, il,
        //!  std::forward<Args>(args)...)` where `I` is the zero-based index
        //!  of `T` in `Ts...`.
        //!
        //! \remarks The first argument shall be the expression `in_place<T>`.
        //!  This function shall not participate in overload resolution unless
        //!  `std::is_constructible_v<T, std::initializer_list<U>&, Args&&...>`
        //!  is `true`.
        template <
            typename T, typename U, typename ...Args
          , typename Enable = typename std::enable_if<std::is_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value>::type
        >
        explicit variant(
            in_place_t(detail::pack<T>)
          , std::initializer_list<U> il, Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(std::is_nothrow_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value)
#endif
          : _storage{detail::index_of<T, detail::pack<
                    detail::empty, typename std::remove_cv<Ts>::type...
                >>{}, il, std::forward<Args>(args)...}
        {}

        //! ~variant();
        //!
        //! \effects If `*this` has an active member of type `T`, destroys the
        //!  active member as if by calling `target<T>()->~T()`.
        //!
        //! \remarks If `std::is_trivially_destructible_v<T>` is `true` for all
        //!  `T` in `Ts...`, then this destructor shall be trivial.
#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        ~variant() = default;
#endif

        //! variant& operator=(nullvariant_t) noexcept;
        //!
        //! \effects If `*this` has an active member of type `T`, destroys the
        //!  active member by calling `T::~T()`.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `*this` does not have an active member.
        //!
        variant& operator=(nullvariant_t) EGGS_CXX11_NOEXCEPT
        {
            _storage.emplace(std::integral_constant<std::size_t, 0>{});
            return *this;
        }

        //! variant& operator=(variant const& rhs);
        //!
        //! \requires `std::is_copy_constructible_v<T>` and
        //!  `std::is_copy_assignable_v<T>` is `true` for all `T` in `Ts...`.
        //!
        //! \effects
        //!  - If both `*this` and `rhs` have an active member of type `T`,
        //!    assigns to the active member the expression `*rhs.target<T>()`;
        //!
        //!  - otherwise, calls `*this = nullvariant`. Then, if `rhs` has an
        //!    active member of type `T`, initializes the active member as if
        //!    direct-non-list-initializing an object of type `T` with the
        //!    expression `*rhs.target<T>()`.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s copy assignment, the state of the active member is as
        //!  defined by the exception safety guarantee of `T`'s copy
        //!  assignment. If an exception is thrown during the call to `T`'s
        //!  copy constructor, `*this` has no active member, and the previous
        //!  active member (if any) has been destroyed.
        //!
        //! \remarks If `std::is_trivially_copyable_v<T>` is `true` for all
        //!  `T` in `Ts...`, then this copy assignment operator shall be
        //!  trivial.
#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        variant& operator=(variant const& rhs) = default;
#endif

        //! variant& operator=(variant&& rhs) noexcept(see below);
        //!
        //! \requires `std::is_move_constructible_v<T>` and
        //!  `std::is_move_assignable_v<T>` is `true` for all `T` in `Ts...`.
        //!
        //! \effects
        //!  - If both `*this` and `rhs` have an active member of type `T`,
        //!    assigns to the active member the expression
        //!    `std::move(*rhs.target<T>())`;
        //!
        //!  - otherwise, calls `*this = nullvariant`. Then, if `rhs` has an
        //!    active member of type `T`, initializes the active member as if
        //!    direct-non-list-initializing an object of type `T` with the
        //!    expression `std::move(*rhs.target<Tn>())`.
        //!
        //!  `bool(rhs)` is unchanged.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `rhs.which() == this->which()`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s move assignment, the state of both active members is
        //!  determined by the exception safety guarantee of `T`'s move
        //!  assignment. If an exception is thrown during the call to `T`'s
        //!  move constructor, `*this` has no active member, the previous
        //!  active member (if any) has been destroyed, and the state of the
        //!  active member of `rhs` is determined by the exception safety
        //!  guarantee of `T`'s move constructor.
        //!
        //! \remarks The expression inside `noexcept` is equivalent to the
        //!  logical AND of `std::is_nothrow_move_assignable_v<Ts>...` and
        //!  `std::is_nothrow_move_constructible_v<Ts>...`. If
        //!  `std::is_trivially_copyable_v<T>` is `true` for all `T` in
        //!  `Ts...`, then this move assignment operator shall be trivial.
#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        variant& operator=(variant&& rhs) = default;
#endif

        //! template <class U>
        //! variant& operator=(U&& v);
        //!
        //! Let `T` be `std::remove_cv_t<std::remove_reference_t<U>>`
        //!
        //! \requires `std::is_constructible_v<T, U&&>` and
        //!  `std::is_assignable_v<T, U&&>` are `true`.
        //!
        //! \effects
        //!  - If `*this` has an active member of type `T`, assigns to the
        //!    active member the expression `std::forward<U>(v)`;
        //!
        //!  - otherwise, calls `*this = nullvariant`. Then, initializes the
        //!    active member as if direct-non-list-initializing an object of
        //!    type `T` with the expression `std::forward<U>(v)`.
        //!
        //! \returns `*this`.
        //!
        //! \postconditions `*this` has an active member.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s assignment, the state of the active member is as defined
        //!  by the exception safety guarantee of `T`'s copy assignment. If
        //!  an exception is thrown during the call to `T`'s constructor,
        //!  `*this` has no active member, and the previous active member
        //!  (if any) has been destroyed.
        //!
        //! \remarks This operator shall not participate in overload
        //!  resolution unless `T` occurs exactly once in
        //!  `std::remove_cv_t<Ts>...`.
        template <
            typename U
          , typename T = typename std::remove_cv<
                typename std::remove_reference<U>::type>::type
          , typename Enable = typename std::enable_if<detail::contains<
                T, detail::pack<typename std::remove_cv<Ts>::type...>
            >::value>::type
        >
        variant& operator=(U&& v)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(std::is_nothrow_assignable<T, U&&>::value
                  && std::is_nothrow_constructible<T, U&&>::value)
#endif
        {
            using t_which = detail::index_of<T, detail::pack<
                    detail::empty, typename std::remove_cv<Ts>::type...
                >>;

            if (_storage.which() == t_which{})
            {
                T* active_member_ptr = static_cast<T*>(_storage.target());

                *active_member_ptr = std::forward<U>(v);
            } else {
                _storage.emplace(t_which{}, std::forward<U>(v));
            }
            return *this;
        }

        //! template <std::size_t I, class ...Args>
        //! void emplace(Args&&... args);
        //!
        //! Let `T` be the `I`th element in `Ts...`, where indexing is
        //! zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and `std::is_constructible_v<T,
        //!  Args&&...>` is `true`.
        //!
        //! \effects Calls `*this = nullvariant`. Then, initializes the active
        //!  member as if direct-non-list-initializing  an object of type `T`
        //!  with the arguments `std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s constructor, `*this` has no active member, and the previous
        //!  active member (if any) has been destroyed.
        template <
            std::size_t I, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
        >
        void emplace(Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(
                std::is_nothrow_constructible<T, Args&&...>::value)
#endif
        {
            using t_which = std::integral_constant<std::size_t, I + 1>;

            _storage.emplace(t_which{}, std::forward<Args>(args)...);
        }

        //! template <std::size_t I, class U, class ...Args>
        //! void emplace(std::initializer_list<U> il, Args&&... args);
        //!
        //! Let `T` be the `I`th element in `Ts...`, where indexing is
        //! zero-based.
        //!
        //! \requires `I < sizeof...(Ts)` and  `std::is_constructible_v<T,
        //!  initializer_list<U>&, Args&&...>` is `true`.
        //!
        //! \effects Calls `*this = nullvariant`. Then, initializes the active
        //!  member as if direct-non-list-initializing an object of type `T`
        //!  with the arguments `il, std::forward<Args>(args)...`.
        //!
        //! \postconditions `*this` has an active member of type `T`.
        //!
        //! \throws Any exception thrown by the selected constructor of `T`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  `T`'s constructor, `*this` has no active member, and the previous
        //!  active member (if any) has been destroyed.
        //!
        //! \remarks This function shall not participate in overload resolution
        //!  unless `std::is_constructible_v<T, std::initializer_list<U>&,
        //!  Args&&...>` is `true`.
        template <
            std::size_t I, typename U, typename ...Args
          , typename T = typename detail::at_index<
                I, detail::pack<Ts...>>::type
          , typename Enable = typename std::enable_if<std::is_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value>::type
        >
        void emplace(std::initializer_list<U> il, Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(std::is_nothrow_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value)
#endif
        {
            using t_which = std::integral_constant<std::size_t, I + 1>;

            _storage.emplace(t_which{}, il, std::forward<Args>(args)...);
        }

        //! template <class T, class ...Args>
        //! void emplace(Args&&... args);
        //!
        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `emplace<I>(std::forward<Args>(args)...)`
        //!  where `I` is the zero-based index of `T` in `Ts...`.
        template <typename T, typename ...Args>
        void emplace(Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(
                std::is_nothrow_constructible<T, Args&&...>::value)
#endif
        {
            using t_which = detail::index_of<T, detail::pack<detail::empty, Ts...>>;

            _storage.emplace(t_which{}, std::forward<Args>(args)...);
        }

        //! template <class T, class U, class ...Args>
        //! void emplace(std::initializer_list<U> il, Args&&... args);
        //!
        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \effects Equivalent to `emplace<I>(il, std::forward<Args>(args)...)`
        //!  where `I` is the zero-based index of `T` in `Ts...`.
        //!
        //! \remarks This function shall not participate in overload resolution
        //!  unless `std::is_constructible_v<T, std::initializer_list<U>&,
        //!  Args&&...>` is `true`.
        template <
            typename T, typename U, typename ...Args
          , typename Enable = typename std::enable_if<std::is_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value>::type
        >
        void emplace(std::initializer_list<U> il, Args&&... args)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(std::is_nothrow_constructible<
                T, std::initializer_list<U>&, Args&&...
            >::value)
#endif
        {
            using t_which = detail::index_of<T, detail::pack<detail::empty, Ts...>>;

            _storage.emplace(t_which{}, il, std::forward<Args>(args)...);
        }

        //! void swap(variant& rhs) noexcept(see below);
        //!
        //! \requires Lvalues of `T` shall be swappable and
        //!  `std::is_move_constructible_v<T>` is `true` for all `T` in
        //!  `Ts...`.
        //!
        //! \effects
        //!  - If both `*this` and `rhs` have an active member of type `T`,
        //!    calls `swap(*this->target<T>(), *rhs.target<T>())`;
        //!
        //!  - otherwise, calls `std::swap(*this, rhs)`.
        //!
        //! \exceptionsafety If an exception is thrown during the call to
        //!  function `swap` the state of the active members of type `T` is
        //!  determined by the exception safety guarantee of `swap` for
        //!  lvalues of `T`. If an exception is thrown during the call to
        //!  a move constructor, the state of `*this` and `rhs` is
        //!  unspecified.
        //!
        //! \remarks The expression inside `noexcept` is equivalent to the
        //!  logical AND of `noexcept(swap(std::declval<Ts&>>(),
        //!  std::declval<Ts&>()))...` where `std::swap` is in scope and
        //!  `std::is_nothrow_move_constructible_v<Ts>...`.
        void swap(variant& rhs)
#if EGGS_CXX11_STD_HAS_IS_NOTHROW_TRAITS
            EGGS_CXX11_NOEXCEPT_IF(detail::all_of<detail::pack<
                detail::is_nothrow_swappable<Ts>...
              , std::is_nothrow_move_constructible<Ts>...
            >>::value)
#endif
        {
            _storage.swap(rhs._storage);
        }

        //! explicit operator bool() const noexcept;
        //!
        //! \returns `true` if and only if `*this` has an active member.
        explicit operator bool() const EGGS_CXX11_NOEXCEPT
        {
            return _storage.which() != 0;
        }

        //! std::size_t which() const noexcept;
        //!
        //! \returns The zero-based index of the active member if `*this` has
        //!  one. Otherwise, returns `npos`.
        std::size_t which() const EGGS_CXX11_NOEXCEPT
        {
            return _storage.which() != 0 ? _storage.which() - 1 : npos;
        }

        //! std::type_info const& target_type() const noexcept;
        //!
        //! \returns If `*this` has an active member of type `T`, `typeid(T)`;
        //!  otherwise `typeid(void)`.
        std::type_info const& target_type() const EGGS_CXX11_NOEXCEPT
        {
            return _storage.which() != 0
              ? detail::type_id{}(
                    detail::pack<Ts...>{}, _storage.which() - 1
                )
              : typeid(void);
        }

        //! void* target() noexcept;
        //!
        //! \returns If `*this` has an active member, a pointer to the active
        //!  member; otherwise a null pointer.
        void* target() EGGS_CXX11_NOEXCEPT
        {
            return _storage.which() != 0 ? _storage.target() : nullptr;
        }

        //! void const* target() const noexcept;
        //!
        //! \returns If `*this` has an active member, a pointer to the active
        //!  member; otherwise a null pointer.
        void const* target() const EGGS_CXX11_NOEXCEPT
        {
            return _storage.which() != 0 ? _storage.target() : nullptr;
        }

        //! template <class T>
        //! T* target() noexcept;
        //!
        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \returns If `*this` has an active member of type `T`, a pointer to
        //!  the active member; otherwise a null pointer.
        template <typename T>
        T* target() EGGS_CXX11_NOEXCEPT
        {
            EGGS_CXX11_CONSTEXPR std::size_t t_which = detail::index_of<
                T, detail::pack<Ts...>>::value + 1;

            return _storage.which() == t_which
              ? static_cast<T*>(_storage.target())
              : nullptr;
        }

        //! template <class T>
        //! T const* target() const noexcept;
        //!
        //! \requires `T` shall occur exactly once in `Ts...`.
        //!
        //! \returns If `*this` has an active member of type `T`, a pointer to
        //!  the active member; otherwise a null pointer.
        template <typename T>
        T const* target() const EGGS_CXX11_NOEXCEPT
        {
            EGGS_CXX11_CONSTEXPR std::size_t t_which = detail::index_of<
                T, detail::pack<Ts...>>::value + 1;

            return _storage.which() == t_which
              ? static_cast<T const*>(_storage.target())
              : nullptr;
        }

    private:
        detail::storage<Ts...> _storage;
    };

    template <>
    class variant<>
    {
    public:
        EGGS_CXX11_STATIC_CONSTEXPR std::size_t npos = std::size_t(-1);

    public:
        variant() EGGS_CXX11_NOEXCEPT {}
        variant(nullvariant_t) EGGS_CXX11_NOEXCEPT {}
#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        variant(variant const&) EGGS_CXX11_NOEXCEPT = default;
        variant(variant&&) EGGS_CXX11_NOEXCEPT = default;
#endif

        variant& operator=(nullvariant_t) EGGS_CXX11_NOEXCEPT { return *this; }
#if EGGS_CXX11_HAS_DEFAULTED_FUNCTIONS
        variant& operator=(variant const&) EGGS_CXX11_NOEXCEPT = default;
        variant& operator=(variant&&) EGGS_CXX11_NOEXCEPT = default;
#endif

        void swap(variant&) EGGS_CXX11_NOEXCEPT {}

        explicit operator bool() const EGGS_CXX11_NOEXCEPT { return false; }
        std::size_t which() const EGGS_CXX11_NOEXCEPT { return npos; }
        std::type_info const& target_type() const EGGS_CXX11_NOEXCEPT { return typeid(void); }
        void* target() EGGS_CXX11_NOEXCEPT { return nullptr; }
        void const* target() const EGGS_CXX11_NOEXCEPT { return nullptr; }
    };

    ///////////////////////////////////////////////////////////////////////////
    //! template <class T>
    //! struct variant_size; // undefined
    //!
    //! \remarks All specializations of `variant_size<T>` shall meet the
    //!  `UnaryTypeTrait` requirements with a `BaseCharacteristic` of
    //!  `std::integral_constant<std::size_t, N>` for some `N`.
    template <typename T>
    struct variant_size; // undefined

    //! template <class ...Ts>
    //! struct variant_size<variant<Ts...>>;
    //!
    //! \remarks Has a `BaseCharacteristic` of `std::integral_constant<
    //!  std::size_t, sizeof...(Ts)>`.
    template <typename ...Ts>
    struct variant_size<variant<Ts...>>
      : std::integral_constant<std::size_t, sizeof...(Ts)>
    {};

    //! template <class T>
    //! struct variant_size<T const>;
    //!
    //! \remarks Let `VS` denote `variant_size<T>` of the cv-unqualified type
    //!  `T`. Has a `BaseCharacteristic` of `std::integral_constant<
    //!  std::size_t, VS::value>`
    template <typename T>
    struct variant_size<T const>
      : variant_size<T>
    {};

#if EGGS_CXX14_HAS_VARIABLE_TEMPLATES
    //! template <class T>
    //! constexpr std::size_t variant_size_v = variant_size<T>::value;
    template <typename T>
    EGGS_CXX11_CONSTEXPR std::size_t variant_size_v = variant_size<T>::value;
#endif

    //! template <std::size_t I, class T>
    //! struct variant_element; // undefined
    //!
    //! \remarks All specializations of `variant_element<I, T>` shall meet the
    //!  `TransformationTrait` requirements with a member typedef `type` that
    //!  names the `I`th member of `T`, where indexing is zero-based.
    template <std::size_t I, typename T>
    struct variant_element; // undefined

    //! template <std::size_t I, class ...Ts>
    //! struct variant_element<I, variant<Ts...>>;
    //!
    //! \requires `I < sizeof...(Ts)`.
    //!
    //! \remarks The member typedef `type` shall name the type of the `I`th
    //!  element of `Ts...`, where indexing is zero-based.
    template <std::size_t I, typename ...Ts>
    struct variant_element<I, variant<Ts...>>
      : detail::at_index<I, detail::pack<Ts...>>
    {};

    //! template <std::size_t I, class T>
    //! struct variant_element<I, T const>;
    //!
    //! \remarks Let `VE` denote `variant_element<I, T>` of the cv-unqualified
    //!  type `T`. The member typedef `type` names `std::add_const_t<
    //!  typename VE::type>`.
    template <std::size_t I, typename T>
    struct variant_element<I, T const>
      : std::add_const<typename variant_element<I, T>::type>
    {};

    //! template <std::size_t I, class T>
    //! using variant_element_t = class variant_element<I, T>::type;
    template <std::size_t I, typename T>
    using variant_element_t = typename variant_element<I, T>::type;

    ///////////////////////////////////////////////////////////////////////////
    //! template <std::size_t I, class ...Ts>
    //! variant_element_t<I, variant<Ts...>>& get(variant<Ts...>& v);
    //!
    //! \requires `I < sizeof...(Ts)`.
    //!
    //! \returns A reference to the `I`th member of `v` if it is active, where
    //!  indexing is zero-based.
    //!
    //! \throws `bad_variant_access` if the `I`th member of `v` is not active.
    template <std::size_t I, typename ...Ts>
    variant_element_t<I, variant<Ts...>>& get(variant<Ts...>& v)
    {
        using value_type = variant_element_t<I, variant<Ts...>>;
        if (value_type* value = v.template target<value_type>())
            return *value;
        throw bad_variant_access{};
    }

    //! template <std::size_t I, class ...Ts>
    //! variant_element_t<I, variant<Ts...>> const& get(variant<Ts...> const& v);
    //!
    //! \requires `I < sizeof...(Ts)`.
    //!
    //! \returns A const reference to the `I`th member of `v` if it is active,
    //!  where indexing is zero-based.
    //!
    //! \throws `bad_variant_access` if the `I`th member of `v` is not active.
    template <std::size_t I, typename ...Ts>
    variant_element_t<I, variant<Ts...>> const& get(variant<Ts...> const& v)
    {
        using value_type = variant_element_t<I, variant<Ts...>>;
        if (value_type const* value = v.template target<value_type>())
            return *value;
        throw bad_variant_access{};
    }

    //! template <std::size_t I, class ...Ts>
    //! variant_element_t<I, variant<Ts...>>&& get(variant<Ts...>&& v);
    //!
    //! \effects Equivalent to return `std::forward<variant_element_t<I,
    //!  variant<Ts...>>&&>(get<I>(v))`.
    template <std::size_t I, typename ...Ts>
    variant_element_t<I, variant<Ts...>>&& get(variant<Ts...>&& v)
    {
        using value_type = variant_element_t<I, variant<Ts...>>;
        return std::forward<value_type&&>(get<I>(v));
    }

    //! template <class T, class ...Ts>
    //! T& get(variant<Ts...>& v);
    //!
    //! \requires The type `T` occurs exactly once in `Ts...`.
    //!
    //! \returns A reference to the active member of `v` if it is of type `T`.
    //!
    //! \throws `bad_variant_access` if the active member of `v` is not of
    //!  type `T`.
    template <typename T, typename ...Ts>
    T& get(variant<Ts...>& v)
    {
        if (T* value = v.template target<T>())
            return *value;
        throw bad_variant_access{};
    }

    //! template <class T, class ...Ts>
    //! T const& get(variant<Ts...> const& v);
    //!
    //! \requires The type `T` occurs exactly once in `Ts...`.
    //!
    //! \returns A const reference to the active member of `v` if it is of
    //!  type `T`.
    //!
    //! \throws `bad_variant_access` if the active member of `v` is not of
    //!  type `T`.
    template <typename T, typename ...Ts>
    T const& get(variant<Ts...> const& v)
    {
        if (T const* value = v.template target<T>())
            return *value;
        throw bad_variant_access{};
    }

    //! template <class T, class ...Ts>
    //! T&& get(variant<Ts...>&& v);
    //!
    //! \effects Equivalent to return `std::forward<T&&>(get<T>(v))`.
    template <typename T, typename ...Ts>
    T&& get(variant<Ts...>&& v)
    {
        return std::forward<T&&>(get<T>(v));
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class ...Ts>
    //! bool operator==(variant<Ts...> const& lhs, variant<Ts...> const& rhs);
    //!
    //! \requires All `T` in `Ts...` shall meet the requirements of
    //!  `EqualityComparable`.
    //!
    //! \returns If both `lhs` and `rhs` have an active member of type `T`,
    //!  `*lhs.target<T>() == *rhs.target<T>()`; otherwise, if
    //!  `bool(lhs) == bool(rhs)`, `true`; otherwise, `false`.
    template <typename ...Ts>
    bool operator==(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return lhs.which() == rhs.which()
          ? !bool(lhs) || detail::equal_to{}(
                detail::pack<Ts...>{}, lhs.which()
              , lhs.target(), rhs.target()
            )
          : false;
    }

    //! template <class ...Ts>
    //! bool operator!=(variant<Ts...> const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `!(lhs == rhs)`.
    template <typename ...Ts>
    bool operator!=(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs == rhs);
    }

    //! template <class ...Ts>
    //! bool operator<(variant<Ts...> const& lhs, variant<Ts...> const& rhs);
    //!
    //! \requires All `T` in `Ts...` shall meet the requirements of
    //!  `LessThanComparable`.
    //!
    //! \returns If both `lhs` and `rhs` have an active member of type `T`,
    //!  `*lhs.target<T>() < *rhs.target<T>()`; otherwise, if
    //!  `!bool(rhs)`, `false`; otherwise, if `!bool(lhs)`, `true`; otherwise,
    //!  `lhs.which() < rhs.which()`.
    template <typename ...Ts>
    bool operator<(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return lhs.which() == rhs.which()
          ? bool(lhs) && detail::less{}(
                detail::pack<Ts...>{}, lhs.which()
              , lhs.target(), rhs.target()
            )
          : bool(lhs) == bool(rhs) ? lhs.which() < rhs.which() : bool(rhs);
    }

    //! template <class ...Ts>
    //! bool operator>(variant<Ts...> const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `rhs < lhs`.
    template <typename ...Ts>
    bool operator>(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return rhs < lhs;
    }

    //! template <class ...Ts>
    //! bool operator<=(variant<Ts...> const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `!(rhs < lhs)`.
    template <typename ...Ts>
    bool operator<=(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return !(rhs < lhs);
    }

    //! template <class ...Ts>
    //! bool operator>=(variant<Ts...> const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `!(lhs < rhs)`.
    template <typename ...Ts>
    bool operator>=(variant<Ts...> const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs < rhs);
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class ...Ts>
    //! bool operator==(variant<Ts...> const& x, nullvariant_t) noexcept;
    //!
    //! \returns `!x`
    template <typename ...Ts>
    bool operator==(variant<Ts...> const& x, nullvariant_t) EGGS_CXX11_NOEXCEPT
    {
        return !x;
    }

    //! template <class ...Ts>
    //! bool operator==(nullvariant_t, variant<Ts...> const& x) noexcept;
    //!
    //! \returns `!x`
    template <typename ...Ts>
    bool operator==(nullvariant_t, variant<Ts...> const& x) EGGS_CXX11_NOEXCEPT
    {
        return !x;
    }

    //! template <class ...Ts>
    //! bool operator!=(variant<Ts...> const& x, nullvariant_t) noexcept;
    //!
    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator!=(variant<Ts...> const& x, nullvariant_t) EGGS_CXX11_NOEXCEPT
    {
        return bool(x);
    }

    //! template <class ...Ts>
    //! bool operator!=(nullvariant_t, variant<Ts...> const& x) noexcept;
    //!
    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator!=(nullvariant_t, variant<Ts...> const& x) EGGS_CXX11_NOEXCEPT
    {
        return bool(x);
    }

    //! template <class ...Ts>
    //! bool operator<(variant<Ts...> const& x, nullvariant_t) noexcept;
    //!
    //! \returns `false`
    template <typename ...Ts>
    bool operator<(variant<Ts...> const& /*x*/, nullvariant_t) EGGS_CXX11_NOEXCEPT
    {
        return false;
    }

    //! template <class ...Ts>
    //! bool operator<(nullvariant_t, variant<Ts...> const& x) noexcept;
    //!
    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator<(nullvariant_t, variant<Ts...> const& x) EGGS_CXX11_NOEXCEPT
    {
        return bool(x);
    }

    //! template <class ...Ts>
    //! bool operator>(variant<Ts...> const& x, nullvariant_t) noexcept;
    //!
    //! \returns `bool(x)`
    template <typename ...Ts>
    bool operator>(variant<Ts...> const& x, nullvariant_t) EGGS_CXX11_NOEXCEPT
    {
        return bool(x);
    }

    //! template <class ...Ts>
    //! bool operator>(nullvariant_t, variant<Ts...> const& x) noexcept;
    //!
    //! \returns `false`
    template <typename ...Ts>
    bool operator>(nullvariant_t, variant<Ts...> const& /*x*/) EGGS_CXX11_NOEXCEPT
    {
        return false;
    }

    //! template <class ...Ts>
    //! bool operator<=(variant<Ts...> const& x, nullvariant_t) noexcept;
    //!
    //! \returns `!x`
    template <typename ...Ts>
    bool operator<=(variant<Ts...> const& x, nullvariant_t) EGGS_CXX11_NOEXCEPT
    {
        return !x;
    }

    //! template <class ...Ts>
    //! bool operator<=(nullvariant_t, variant<Ts...> const& x) noexcept;
    //!
    //! \returns `true`
    template <typename ...Ts>
    bool operator<=(nullvariant_t, variant<Ts...> const& /*x*/) EGGS_CXX11_NOEXCEPT
    {
        return true;
    }

    //! template <class ...Ts>
    //! bool operator>=(variant<Ts...> const& x, nullvariant_t) noexcept;
    //!
    //! \returns `true`
    template <typename ...Ts>
    bool operator>=(variant<Ts...> const& /*x*/, nullvariant_t) EGGS_CXX11_NOEXCEPT
    {
        return true;
    }

    //! template <class ...Ts>
    //! bool operator>=(nullvariant_t, variant<Ts...> const& x) noexcept;
    //!
    //! \returns `!x`
    template <typename ...Ts>
    bool operator>=(nullvariant_t, variant<Ts...> const& x) EGGS_CXX11_NOEXCEPT
    {
        return !x;
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class ...Ts, class T>
    //! bool operator==(variant<Ts...> const& lhs, T const& rhs);
    //!
    //! \requires `T` shall meet the requirements of `EqualityComparable`.
    //!
    //! \returns If `lhs` has an active member of type `T`,
    //!  `*lhs.target<T>() == rhs`; otherwise, `false`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator==(variant<Ts...> const& lhs, T const& rhs)
    {
        EGGS_CXX11_CONSTEXPR std::size_t rhs_which = detail::index_of<
            T, detail::pack<typename std::remove_cv<Ts>::type...>>::value;

        return lhs.which() == rhs_which
          ? *lhs.template target<T>() == rhs
          : false;
    }

    //! template <class T, class ...Ts>
    //! bool operator==(T const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `rhs == lhs`
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator==(T const& lhs, variant<Ts...> const& rhs)
    {
        return rhs == lhs;
    }

    //! template <class ...Ts, class T>
    //! bool operator!=(variant<Ts...> const& lhs, T const& rhs);
    //!
    //! \returns `!(lhs == rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator!=(variant<Ts...> const& lhs, T const& rhs)
    {
        return !(lhs == rhs);
    }

    //! template <class T, class ...Ts>
    //! bool operator!=(T const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `!(lhs == rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator!=(T const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs == rhs);
    }

    //! template <class ...Ts, class T>
    //! bool operator<(variant<Ts...> const& lhs, T const& rhs);
    //!
    //! \requires `T` shall meet the requirements of `LessThanComparable`.
    //!
    //! \returns If `lhs` has an active member of type `T`,
    //!  `*lhs->target<T>() < rhs`; otherwise, if `lhs` has no active member
    //!  or if `lhs` has an active member of type `Td` and `Td` occurs in
    //!  `Ts...` before `T`, `true`; otherwise, `false`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator<(variant<Ts...> const& lhs, T const& rhs)
    {
        EGGS_CXX11_CONSTEXPR std::size_t rhs_which = detail::index_of<
            T, detail::pack<typename std::remove_cv<Ts>::type...>>::value;

        return lhs.which() == rhs_which
          ? *lhs.template target<T>() < rhs
          : bool(lhs) ? lhs.which() < rhs_which : true;
    }

    //! template <class T, class ...Ts>
    //! bool operator<(T const& lhs, variant<Ts...> const& rhs);
    //!
    //! \requires `T` shall meet the requirements of `LessThanComparable`.
    //!
    //! \returns If `rhs` has an active member of type `T`,
    //!  `lhs < *rhs->target<T>()`; otherwise, if `rhs` has an active member
    //!  of type `Td` and `Td` occurs in `Ts...` after `T`, `true`;
    //!  otherwise, `false`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator<(T const& lhs, variant<Ts...> const& rhs)
    {
        EGGS_CXX11_CONSTEXPR std::size_t lhs_which = detail::index_of<
            T, detail::pack<typename std::remove_cv<Ts>::type...>>::value;

        return lhs_which == rhs.which()
          ? lhs < *rhs.template target<T>()
          : bool(rhs) ? lhs_which < rhs.which() : false;
    }

    //! template <class ...Ts, class T>
    //! bool operator>(variant<Ts...> const& lhs, T const& rhs);
    //!
    //! \returns `rhs < lhs`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator>(variant<Ts...> const& lhs, T const& rhs)
    {
        return rhs < lhs;
    }

    //! template <class T, class ...Ts>
    //! bool operator>(T const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `rhs < lhs`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator>(T const& lhs, variant<Ts...> const& rhs)
    {
        return rhs < lhs;
    }

    //! template <class ...Ts, class T>
    //! bool operator<=(variant<Ts...> const& lhs, T const& rhs);
    //!
    //! \returns `!(rhs < lhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator<=(variant<Ts...> const& lhs, T const& rhs)
    {
        return !(rhs < lhs);
    }

    //! template <class T, class ...Ts>
    //! bool operator<=(T const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `!(rhs < lhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator<=(T const& lhs, variant<Ts...> const& rhs)
    {
        return !(rhs < lhs);
    }

    //! template <class ...Ts, class T>
    //! bool operator>=(variant<Ts...> const& lhs, T const& rhs);
    //!
    //! \returns `!(lhs < rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename ...Ts, typename T
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator>=(variant<Ts...> const& lhs, T const& rhs)
    {
        return !(lhs < rhs);
    }

    //! template <class T, class ...Ts>
    //! bool operator>=(T const& lhs, variant<Ts...> const& rhs);
    //!
    //! \returns `!(lhs < rhs)`.
    //!
    //! \remarks This operator shall not participate in overload resolution
    //!  unless `T` occurs exactly once in `Ts...`.
    template <
        typename T, typename ...Ts
      , typename Enable = typename std::enable_if<detail::contains<
            T, detail::pack<typename std::remove_cv<Ts>::type...>
        >::value>::type
    >
    bool operator>=(T const& lhs, variant<Ts...> const& rhs)
    {
        return !(lhs < rhs);
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class R, class F, class ...Vs>
    //! R apply(F&& f, Vs&&... vs);
    //!
    //! Let `Vi` be the `i`-th type in `Vs...`, `Ui` be `std::decay_t<Vi>`,
    //! where all indexing is zero-based.
    //!
    //! \requires `sizeof...(Vs) != 0` shall be `true`. For all `i`, `Ui`
    //!  shall be the type `variant<Tsi...>` where `Tsi` is the parameter
    //!  pack representing the element types in `Ui`. `INVOKE(
    //!  std::forward<F>(f), get<Is>(std::forward<Vs>(vs))..., R)` shall be a
    //!  valid expression for all `Is...` in the range `[0u, sizeof...
    //!  (Tsi))...`.
    //!
    //! \effects Equivalent to `INVOKE(std::forward<F>(f), get<Is>(
    //!  std::forward<Vs>(vs))...), R)` where `Is...` are the zero-based
    //!  indices of the active members of `vs...`.
    //!
    //! \throws `bad_variant_access` if any of `vs...` has no active member.
    template <
        typename R
      , typename F, typename ...Vs
      , typename Enable = typename std::enable_if<
            sizeof...(Vs) != 0 && detail::all_of<detail::pack<
                detail::is_variant<typename std::remove_reference<Vs>::type>...
            >>::value
        >::type
    >
    R apply(F&& f, Vs&&... vs)
    {
        return detail::apply<R>(std::forward<F>(f), std::forward<Vs>(vs)...);
    }

    //! template <class F, class ...Vs>
    //! R apply(F&& f, Vs&&... vs);
    //!
    //! Let `FD` be `std::decay_t<F>`, `R` be the strong result type of `FD`:
    //! - if `FD` is a pointer to function type, `R` shall be the return type
    //!   of `FD`;
    //! - if `FD` is a pointer to member function type, `R` shall be the
    //!   return type of `FD`;
    //! - if `FD` is a class type with a member type `result_type`, `R` shall
    //!   be `FD::result_type`;
    //!
    //! \effects Equivalent to `apply<R>(std::forward<F>(f),
    //!  std::forward<Vs>(vs)...)`.
    //!
    //! \remarks This function shall not participate in overload resolution
    //!  unless `FD` has a strong result type.
    template <
        typename F, typename ...Vs
      , typename R = detail::weak_result<typename std::decay<F>::type>
      , typename Enable = typename std::enable_if<
            sizeof...(Vs) != 0 && detail::all_of<detail::pack<
                detail::is_variant<typename std::remove_reference<Vs>::type>...
            >>::value
        >::type
    >
    R apply(F&& f, Vs&&... vs)
    {
        return apply<R>(std::forward<F>(f), std::forward<Vs>(vs)...);
    }

    ///////////////////////////////////////////////////////////////////////////
    //! template <class ...Ts>
    //! void swap(variant<Ts...>& x, variant<Ts...>& y)
    //!   noexcept(noexcept(x.swap(y))
    //!
    //! \effects Calls `x.swap(y)`.
    template <typename ...Ts>
    void swap(variant<Ts...>& x, variant<Ts...>& y)
        EGGS_CXX11_NOEXCEPT_IF(EGGS_CXX11_NOEXCEPT_EXPR(x.swap(y)))
    {
        x.swap(y);
    }
}}

namespace std
{
    //! template <class ...Ts>
    //! struct hash<::eggs::variants::variant<Ts...>>;
    //!
    //! \requires The template specialization `std::hash<T>` shall meet the
    //!  requirements of class template `std::hash` for all `T` in `Ts...`.
    //!  The template specialization `std::hash<variant<Ts...>>` shall meet
    //!  the requirements of class template `std::hash`. For an object `v` of
    //!  type `variant<Ts...>`, if `v` has an active member of type `T`,
    //!  `std::hash<variant<Ts...>>()(v)` shall evaluate to the same value as
    //!  `std::hash<T>()(*v.target<T>())`; otherwise it evaluates to an
    //!  unspecified value.
    template <typename ...Ts>
    struct hash< ::eggs::variants::variant<Ts...>>
    {
        using argument_type = ::eggs::variants::variant<Ts...>;
        using result_type = std::size_t;

        std::size_t operator()(::eggs::variants::variant<Ts...> const& v) const
        {
            return bool(v)
              ? ::eggs::variants::apply<std::size_t>(
                    ::eggs::variants::detail::hash{}, v)
              : 0u;
        }
    };
}

#include <eggs/variant/detail/config/suffix.hpp>

#endif /*EGGS_VARIANT_VARIANT_HPP*/
