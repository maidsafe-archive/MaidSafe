/*=============================================================================
    Copyright (c) 2011 Daniel James

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(BOOST_QUICKBOOK_INTRUSIVE_BASE_HPP)
#define BOOST_QUICKBOOK_INTRUSIVE_BASE_HPP

namespace quickbook
{
    //
    // instructive_base
    //

    template <typename T>
    struct intrusive_base
    {
        intrusive_base() : ref_count_(0) {}
        intrusive_base(intrusive_base const&) : ref_count_(0) {}
        intrusive_base& operator=(intrusive_base const&) { return *this; }
        ~intrusive_base() { assert(!ref_count_); }

        friend void intrusive_ptr_add_ref(T* ptr)
            { ++ptr->ref_count_; }

        friend void intrusive_ptr_release(T* ptr)
            { if(--ptr->ref_count_ == 0) delete ptr; }
    private:
        unsigned ref_count_;
    };
}

#endif
