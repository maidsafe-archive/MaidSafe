Until ticket https://svn.boost.org/trac/boost/ticket/2839 is fixed, we need to
apply apply a patch to boost/boost/variant/detail\move.hpp.  Since Windows
doesn't have a native patch command, and we're not pulling boost via svn/git, we
need to manually replace the modified file with a patched version.

To create the patched version, rename this file's parent folder to the version
of boost being patched.  Then replace boost_variant_detail_move_patched.hpp in
this folder with a patched copy of boost/boost/variant/detail\move.hpp.

To patch the file, immediately after:




namespace detail { namespace move_swap {

template <typename T>
inline void swap(T& lhs, T& rhs)
{
    T tmp( boost::detail::variant::move(lhs) );
    lhs = boost::detail::variant::move(rhs);
    rhs = boost::detail::variant::move(tmp);
}




add the following:




// Rationale for the following fix can be found at https://svn.boost.org/trac/boost/ticket/2839.
template <typename T, typename U>
inline void swap(T& lhs, U& rhs)
{
    T tmp( boost::detail::variant::move(lhs) );
    lhs = boost::detail::variant::move(rhs);
    rhs = boost::detail::variant::move(tmp);
}
