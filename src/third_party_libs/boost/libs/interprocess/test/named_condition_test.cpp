//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2004-2011. Distributed under the Boost
// Software License, Version 1.0. (See accompanying file
// LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/interprocess for documentation.
//
//////////////////////////////////////////////////////////////////////////////

#include <boost/interprocess/detail/config_begin.hpp>
#include <boost/interprocess/detail/workaround.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/interprocess/sync/named_condition.hpp>
#include "condition_test_template.hpp"
#include "named_creation_template.hpp"
#include <string>
#include <sstream>
#include "get_process_id_name.hpp"

using namespace boost::interprocess;

struct condition_deleter
{
   std::string name;

   ~condition_deleter()
   { 
      if(name.empty())
         named_condition::remove(test::add_to_process_id_name("named_condition"));
      else
         named_condition::remove(name.c_str());
   }
};

inline std::string num_to_string(int n)
{  std::stringstream s; s << n; return s.str(); }

//This wrapper is necessary to have a default constructor
//in generic mutex_test_template functions
class named_condition_test_wrapper
   : public condition_deleter, public named_condition
{
   public:

   named_condition_test_wrapper()
      :  named_condition(open_or_create,
             (test::add_to_process_id_name("test_cond") + num_to_string(count)).c_str())
   {
      condition_deleter::name += test::add_to_process_id_name("test_cond");
      condition_deleter::name += num_to_string(count);
      ++count;
   }

   ~named_condition_test_wrapper()
   {  --count; }


   template<class Lock>
   class lock_wrapper
   {
      typedef void (lock_wrapper::*unspecified_bool_type)();
      public:

      typedef named_mutex mutex_type;

      lock_wrapper(Lock &l)
         : l_(l)
      {}

      mutex_type* mutex() const
      {  return l_.mutex();  }

      void lock()    { l_.lock(); }

      void unlock()  { l_.unlock(); }

      operator unspecified_bool_type() const
      {  return l_ ? &lock_wrapper::lock : 0;  }

      private:
      Lock &l_;
   };
/*
   template<class Lock>
   class lock_wrapper
   {
      public:

      typedef named_mutex mutex_type;

      lock_wrapper(Lock &l)
        : l_(l)
      {}

      mutex_type* mutex() const
      {  return l_.mutex();  }

      private:
      Lock &l_;
   };
*/
   template <typename L>
   void wait(L& lock)
   {
      lock_wrapper<L> newlock(lock);
      named_condition::wait(newlock);
   }

   template <typename L, typename Pr>
   void wait(L& lock, Pr pred)
   {
      lock_wrapper<L> newlock(lock);
      named_condition::wait(newlock, pred);
   }

   template <typename L>
   bool timed_wait(L& lock, const boost::posix_time::ptime &abs_time)
   {
      lock_wrapper<L> newlock(lock);
      return named_condition::timed_wait(newlock, abs_time);
   }

   template <typename L, typename Pr>
   bool timed_wait(L& lock, const boost::posix_time::ptime &abs_time, Pr pred)
   {
      lock_wrapper<L> newlock(lock);
      return named_condition::timed_wait(newlock, abs_time, pred);
   }

   static int count;
};

int named_condition_test_wrapper::count = 0;

//This wrapper is necessary to have a common constructor
//in generic named_creation_template functions
class named_condition_creation_test_wrapper
   : public condition_deleter, public named_condition
{
   public:
   named_condition_creation_test_wrapper(create_only_t)
      :  named_condition(create_only, test::add_to_process_id_name("named_condition"))
   {  ++count_;   }

   named_condition_creation_test_wrapper(open_only_t)
      :  named_condition(open_only, test::add_to_process_id_name("named_condition"))
   {  ++count_;   }

   named_condition_creation_test_wrapper(open_or_create_t)
      :  named_condition(open_or_create, test::add_to_process_id_name("named_condition"))
   {  ++count_;   }

   ~named_condition_creation_test_wrapper()   {
      if(--count_){
         ipcdetail::interprocess_tester::
            dont_close_on_destruction(static_cast<named_condition&>(*this));
      }
   }
   static int count_;
};

int named_condition_creation_test_wrapper::count_ = 0;

struct mutex_deleter
{
   std::string name;

   ~mutex_deleter()
   { 
      if(name.empty())
         named_mutex::remove(test::add_to_process_id_name("named_mutex"));
      else
         named_mutex::remove(name.c_str());
   }
};

//This wrapper is necessary to have a default constructor
//in generic mutex_test_template functions
class named_mutex_test_wrapper
   : public mutex_deleter, public named_mutex
{
   public:
   named_mutex_test_wrapper()
      :  named_mutex(open_or_create,
             (test::add_to_process_id_name("test_mutex") + num_to_string(count)).c_str())
   {
      mutex_deleter::name += test::add_to_process_id_name("test_mutex");
      mutex_deleter::name += num_to_string(count);
      ++count;
   }

   ~named_mutex_test_wrapper()
   {  --count; }

   static int count;
};

int named_mutex_test_wrapper::count = 0;

int main ()
{
   try{
      //Remove previous mutexes and conditions
      named_mutex::remove(test::add_to_process_id_name("test_mutex0"));
      named_condition::remove(test::add_to_process_id_name("test_cond0"));
      named_condition::remove(test::add_to_process_id_name("test_cond1"));
      named_condition::remove(test::add_to_process_id_name("named_condition"));
      named_mutex::remove(test::add_to_process_id_name("named_mutex"));

      test::test_named_creation<named_condition_creation_test_wrapper>();
      test::do_test_condition<named_condition_test_wrapper
                             ,named_mutex_test_wrapper>();
   }
   catch(std::exception &ex){
      std::cout << ex.what() << std::endl;
      return 1;
   }
   named_mutex::remove(test::add_to_process_id_name("test_mutex0"));
   named_condition::remove(test::add_to_process_id_name("test_cond0"));
   named_condition::remove(test::add_to_process_id_name("test_cond1"));
   named_condition::remove(test::add_to_process_id_name("named_condition"));
   named_mutex::remove(test::add_to_process_id_name("named_mutex"));
   return 0;
}

#include <boost/interprocess/detail/config_end.hpp>
