

#include <boost/thread.hpp>
#include <boost/config.hpp>

#ifndef BOOST_NO_RVALUE_REFERENCES
struct MovableButNonCopyable {
#ifndef   BOOST_NO_DEFAULTED_FUNCTIONS
      MovableButNonCopyable() = default;
      MovableButNonCopyable(MovableButNonCopyable const&) = delete;
      MovableButNonCopyable& operator=(MovableButNonCopyable const&) = delete;
      MovableButNonCopyable(MovableButNonCopyable&&) = default;
      MovableButNonCopyable& operator=(MovableButNonCopyable&&) = default;
#else
  MovableButNonCopyable() {};
  MovableButNonCopyable(MovableButNonCopyable&&) {};
  MovableButNonCopyable& operator=(MovableButNonCopyable&&) {
    return *this;
  };
private:
    MovableButNonCopyable(MovableButNonCopyable const&);
    MovableButNonCopyable& operator=(MovableButNonCopyable const&);
#endif
};
int main()
{
    boost::packaged_task<MovableButNonCopyable>(MovableButNonCopyable());
    return 0;
}
#else
int main()
{
    return 0;
}
#endif
