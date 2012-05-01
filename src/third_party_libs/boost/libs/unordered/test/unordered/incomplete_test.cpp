
// Copyright 2009 Daniel James.
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#include "../helpers/prefix.hpp"

#include <utility>
#include <boost/unordered_map.hpp>
#include <boost/unordered_set.hpp>

namespace x
{
    struct D { boost::unordered_map<D, D> x; };
}

namespace test
{
    // Declare, but don't define some types.

    struct value;
    struct hash;
    struct equals;
    template <class T>
    struct malloc_allocator;

    // Declare some instances
    
    typedef boost::unordered_map<value, value, hash, equals,
        malloc_allocator<std::pair<value const, value> > > map;
    typedef boost::unordered_multimap<value, value, hash, equals,
        malloc_allocator<std::pair<value const, value> > > multimap;
    typedef boost::unordered_set<value, hash, equals,
        malloc_allocator<value> > set;
    typedef boost::unordered_multiset<value, hash, equals,
        malloc_allocator<value> > multiset;
    
    // Now define the types which are stored as members, as they are needed for
    // declaring struct members.

    struct hash { 
        template <typename T>
        std::size_t operator()(T const&) const { return 0; }
    };

    struct equals {
        template <typename T>
        bool operator()(T const&, T const&) const { return true; }
    };

}

#include "../helpers/allocator.hpp"

namespace test
{
    // Declare some members of a structs.
    //
    // Incomplete hash, equals and allocator aren't here supported at the
    // moment.
    
    struct struct1 {
        boost::unordered_map<struct1, struct1, hash, equals,
            malloc_allocator<std::pair<struct1 const, struct1> > > x;
    };
    struct struct2 {
        boost::unordered_multimap<struct2, struct2, hash, equals,
            malloc_allocator<std::pair<struct2 const, struct2> > > x;
    };
    struct struct3 {
        boost::unordered_set<struct3, hash, equals,
            malloc_allocator<struct3> > x;
    };
    struct struct4 {
        boost::unordered_multiset<struct4, hash, equals,
            malloc_allocator<struct4> > x;
    };
    
    // Now define the value type.

    struct value {};

    // Create some instances.
    
    test::map m1;
    test::multimap m2;
    test::set s1;
    test::multiset s2;

    test::struct1 c1;
    test::struct2 c2;
    test::struct3 c3;
    test::struct4 c4;

    // Now declare, but don't define, the operators required for comparing
    // elements.

    std::size_t hash_value(value const&);
    bool operator==(value const&, value const&);

    std::size_t hash_value(struct1 const&);
    std::size_t hash_value(struct2 const&);
    std::size_t hash_value(struct3 const&);
    std::size_t hash_value(struct4 const&);
    
    bool operator==(struct1 const&, struct1 const&);
    bool operator==(struct2 const&, struct2 const&);
    bool operator==(struct3 const&, struct3 const&);
    bool operator==(struct4 const&, struct4 const&);
    
    // And finally use these
    
    void use_types()
    {
        test::value x;
        m1[x] = x;
        m2.insert(std::make_pair(x, x));
        s1.insert(x);
        s2.insert(x);

        c1.x.insert(std::make_pair(c1, c1));
        c2.x.insert(std::make_pair(c2, c2));
        c3.x.insert(c3);
        c4.x.insert(c4);
    }

    // And finally define the operators required for comparing elements.

    std::size_t hash_value(value const&) { return 0; }
    bool operator==(value const&, value const&) { return true; }

    std::size_t hash_value(struct1 const&) { return 0; }
    std::size_t hash_value(struct2 const&) { return 0; }
    std::size_t hash_value(struct3 const&) { return 0; }
    std::size_t hash_value(struct4 const&) { return 0; }
    
    bool operator==(struct1 const&, struct1 const&) { return true; }
    bool operator==(struct2 const&, struct2 const&) { return true; }
    bool operator==(struct3 const&, struct3 const&) { return true; }
    bool operator==(struct4 const&, struct4 const&) { return true; }
}

int main() {
    // This could just be a compile test, but I like to be able to run these
    // things. It's probably irrational, but I find it reassuring.

    test::use_types();
}
