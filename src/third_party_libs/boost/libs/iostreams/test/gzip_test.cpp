// (C) Copyright 2008 CodeRage, LLC (turkanis at coderage dot com)
// (C) Copyright 2004-2007 Jonathan Turkanis
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt.)

// See http://www.boost.org/libs/iostreams for documentation.

#include <string>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/device/array.hpp>
#include <boost/iostreams/device/back_inserter.hpp>
#include <boost/iostreams/filter/gzip.hpp>
#include <boost/iostreams/filter/test.hpp>
#include <boost/iostreams/filtering_stream.hpp>
#include <boost/ref.hpp>
#include <boost/range/iterator_range.hpp>
#include <boost/test/test_tools.hpp>
#include <boost/test/unit_test.hpp>
#include "detail/sequence.hpp"
#include "detail/verification.hpp"

using namespace boost;
using namespace boost::iostreams;
using namespace boost::iostreams::test;
namespace io = boost::iostreams;
using boost::unit_test::test_suite;     

struct gzip_alloc : std::allocator<char> { };

void compression_test()
{
    text_sequence      data;

    // Test compression and decompression with metadata
    for (int i = 0; i < 4; ++i) {
        gzip_params params;
        if (i & 1) {
            params.file_name = "original file name";
        }
        if (i & 2) {
            params.comment = "detailed file description";
        }
        gzip_compressor    out(params);
        gzip_decompressor  in;
        BOOST_CHECK(
            test_filter_pair( boost::ref(out), 
                              boost::ref(in), 
                              std::string(data.begin(), data.end()) )
        );
        BOOST_CHECK(in.file_name() == params.file_name);
        BOOST_CHECK(in.comment() == params.comment);
    }

    // Test compression and decompression with custom allocator
    BOOST_CHECK(
        test_filter_pair( basic_gzip_compressor<gzip_alloc>(), 
                          basic_gzip_decompressor<gzip_alloc>(), 
                          std::string(data.begin(), data.end()) )
    );
}

void multiple_member_test()
{
    text_sequence      data;
    std::vector<char>  temp, dest;

    // Write compressed data to temp, twice in succession
    filtering_ostream out;
    out.push(gzip_compressor());
    out.push(io::back_inserter(temp));
    io::copy(make_iterator_range(data), out);
    out.push(io::back_inserter(temp));
    io::copy(make_iterator_range(data), out);

    // Read compressed data from temp into dest
    filtering_istream in;
    in.push(gzip_decompressor());
    in.push(array_source(&temp[0], temp.size()));
    io::copy(in, io::back_inserter(dest));

    // Check that dest consists of two copies of data
    BOOST_REQUIRE_EQUAL(data.size() * 2, dest.size());
    BOOST_CHECK(std::equal(data.begin(), data.end(), dest.begin()));
    BOOST_CHECK(std::equal(data.begin(), data.end(), dest.begin() + dest.size() / 2));

    dest.clear();
    io::copy(
        array_source(&temp[0], temp.size()),
        io::compose(gzip_decompressor(), io::back_inserter(dest)));

    // Check that dest consists of two copies of data
    BOOST_REQUIRE_EQUAL(data.size() * 2, dest.size());
    BOOST_CHECK(std::equal(data.begin(), data.end(), dest.begin()));
    BOOST_CHECK(std::equal(data.begin(), data.end(), dest.begin() + dest.size() / 2));
}

void array_source_test()
{
    std::string data = "simple test string.";
    std::string encoded;

    filtering_ostream out;
    out.push(gzip_compressor());
    out.push(io::back_inserter(encoded));
    io::copy(make_iterator_range(data), out);

    std::string res;
    io::array_source src(encoded.data(),encoded.length());
    io::copy(io::compose(io::gzip_decompressor(), src), io::back_inserter(res));
    
    BOOST_CHECK_EQUAL(data, res);
}

test_suite* init_unit_test_suite(int, char* []) 
{
    test_suite* test = BOOST_TEST_SUITE("gzip test");
    test->add(BOOST_TEST_CASE(&compression_test));
    test->add(BOOST_TEST_CASE(&multiple_member_test));
    test->add(BOOST_TEST_CASE(&array_source_test));
    return test;
}
