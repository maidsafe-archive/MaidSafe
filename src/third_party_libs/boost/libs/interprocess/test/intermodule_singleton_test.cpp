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
#include <boost/interprocess/detail/intermodule_singleton.hpp>
#include <iostream>

using namespace boost::interprocess;

class MyClass
{
   public:
   MyClass()
   {
      std::cout << "Constructor\n";
   }

   void shout() const
   {
      std::cout << "Shout\n";
   }

   ~MyClass()
   {
      std::cout << "Destructor\n";
   }
};

class MyDerivedClass
   : public MyClass
{};

class MyThrowingClass
{
   public:
   MyThrowingClass()
   {
      throw int(0);
   }
};


template < template<class, bool = false> class IntermoduleType >
int intermodule_singleton_test()
{
   bool exception_thrown = false;
   bool exception_2_thrown = false;

   try{
      IntermoduleType<MyThrowingClass, true>::get();
   }
   catch(int &){
      exception_thrown = true;
      //Second try
      try{
         IntermoduleType<MyThrowingClass, true>::get();
      }
      catch(interprocess_exception &){
         exception_2_thrown = true;
      }
   }

   if(!exception_thrown || !exception_2_thrown){
      return 1;
   }

   MyClass & mc = IntermoduleType<MyClass>::get();
   mc.shout();
   IntermoduleType<MyClass>::get().shout();
   IntermoduleType<MyDerivedClass>::get().shout();

   //Second try
   exception_2_thrown = false;
   try{
      IntermoduleType<MyThrowingClass, true>::get();
   }
   catch(interprocess_exception &){
      exception_2_thrown = true;
   }
   if(!exception_2_thrown){
      return 1;
   }

   return 0;   
}

int main ()
{
   if(0 != intermodule_singleton_test<ipcdetail::portable_intermodule_singleton>()){
      return 1;
   }

   #ifdef BOOST_INTERPROCESS_WINDOWS
   if(0 != intermodule_singleton_test<ipcdetail::windows_intermodule_singleton>()){
      return 1;
   }
   #endif

   return 0;
}

#include <boost/interprocess/detail/config_end.hpp>

