/*=============================================================================
    Copyright (c) 2009 Daniel James

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(BOOST_QUICKBOOK_DETAIL_INPUT_PATH_HPP)
#define BOOST_QUICKBOOK_DETAIL_INPUT_PATH_HPP

#include <boost/config.hpp>
#include <boost/filesystem/v3/path.hpp>
#include <string>
#include <stdexcept>
#include <iostream>
#include "fwd.hpp"

#if defined(__cygwin__) || defined(__CYGWIN__)
#   define QUICKBOOK_CYGWIN_PATHS 1
#elif defined(_WIN32)
#   define QUICKBOOK_WIDE_PATHS 1
#   if defined(BOOST_MSVC) && BOOST_MSVC >= 1400
#       define QUICKBOOK_WIDE_STREAMS 1
#   endif
#endif

#if !defined(QUICKBOOK_WIDE_PATHS)
#define QUICKBOOK_WIDE_PATHS 0
#endif

#if !defined(QUICKBOOK_WIDE_STREAMS)
#define QUICKBOOK_WIDE_STREAMS 0
#endif

#if !defined(QUICKBOOK_CYGWIN_PATHS)
#define QUICKBOOK_CYGWIN_PATHS 0
#endif

namespace quickbook
{
    namespace fs = boost::filesystem;

    namespace detail
    {
        struct conversion_error : std::runtime_error
        {
            conversion_error(char const* m) : std::runtime_error(m) {}
        };

        // 'generic':   Paths in quickbook source and the generated boostbook.
        //              Always UTF-8.
        // 'input':     Paths (or other parameters) from the command line and
        //              possibly other sources in the future. Wide strings on
        //              normal windows, UTF-8 for cygwin and other platforms
        //              (hopefully).
        // 'stream':    Strings to be written to a stream.
        // 'path':      Stored as a boost::filesystem::path. Since
        //              Boost.Filesystem doesn't support cygwin, this
        //              is always wide on windows. UTF-8 on other
        //              platforms (again, hopefully).
    
#if QUICKBOOK_WIDE_PATHS
        typedef std::wstring input_string;
#else
        typedef std::string input_string;
#endif

#if QUICKBOOK_WIDE_STREAMS
        typedef std::wostream ostream;
        typedef std::wstring stream_string;
#else
        typedef std::ostream ostream;
        typedef std::string stream_string;
#endif

        std::string input_to_utf8(input_string const&);
        fs::path input_to_path(input_string const&);
        stream_string path_to_stream(fs::path const&);
    
        std::string path_to_generic(fs::path const&);
        fs::path generic_to_path(std::string const&);

        void initialise_output();
        
        ostream& out();

        // Preformats an error/warning message so that it can be parsed by
        // common IDEs. Uses the ms_errors global to determine if VS format
        // or GCC format. Returns the stream to continue ouput of the verbose
        // error message.
        ostream& outerr();
        ostream& outerr(fs::path const& file, int line = -1);
        ostream& outwarn(fs::path const& file, int line = -1);
        ostream& outerr(file_ptr const&, string_iterator);
        ostream& outwarn(file_ptr const&, string_iterator);
        
        struct utf8_proxy
        {
            std::string value;
            
            explicit utf8_proxy(std::string const& v) : value(v) {}
        };

        void write_utf8(ostream& out, std::string const&);
        
        inline ostream& operator<<(ostream& out, utf8_proxy const& p) {
            write_utf8(out, p.value);
            return out;
        }

        inline utf8_proxy utf8(std::string const& value) {
            return utf8_proxy(value);
        }

        template <typename It>
        inline utf8_proxy utf8(It begin, It end) {
            return utf8_proxy(std::string(begin, end));
        }
    }
}

#endif
