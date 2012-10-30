// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/windows/search_path.hpp
 *
 * Defines a function to search for an executable in path.
 */

#ifndef BOOST_PROCESS_WINDOWS_SEARCH_PATH_HPP
#define BOOST_PROCESS_WINDOWS_SEARCH_PATH_HPP

#include <boost/process/config.hpp>
#include <boost/filesystem.hpp>
#include <boost/tokenizer.hpp>
#include <boost/array.hpp>
#include <string>
#include <stdexcept>
#include <stdlib.h>
#include <Shellapi.h>

namespace boost { namespace process { namespace windows {

#if defined(_UNICODE) || defined(UNICODE)
inline std::wstring search_path(const std::wstring &filename,
    std::wstring path = L"")
{
    if (path.empty())
    {
        path = ::_wgetenv(L"PATH");
        if (path.empty())
            BOOST_PROCESS_THROW(std::runtime_error(
                "Environment variable PATH not found"));
    }

    std::wstring result;
    typedef boost::tokenizer<boost::char_separator<wchar_t>,
        std::wstring::const_iterator, std::wstring> tokenizer;
    boost::char_separator<wchar_t> sep(L";");
    tokenizer tok(path, sep);
    for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
    {
        boost::filesystem::path p = *it;
        p /= filename;
        boost::array<std::wstring, 4> extensions =
            { L"", L".exe", L".com", L".bat" };
        for (boost::array<std::wstring, 4>::iterator it2 = extensions.begin();
            it2 != extensions.end(); ++it2)
        {
            boost::filesystem::path p2 = p.stem();
            p2 += *it2;
            if (::SHGetFileInfoW(p2.c_str(), 0, 0, 0, SHGFI_EXETYPE))
            {
                result = p2.wstring();
                break;
            }
        }
    }
    return result;
}
#else
inline std::string search_path(const std::string &filename,
    std::string path = "")
{
    if (path.empty())
    {
        path = ::getenv("PATH");
        if (path.empty())
            BOOST_PROCESS_THROW(std::runtime_error(
                "Environment variable PATH not found"));
    }

    std::string result;
    typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
    boost::char_separator<char> sep(";");
    tokenizer tok(path, sep);
    for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it)
    {
        boost::filesystem::path p = *it;
        p /= filename;
        boost::array<std::string, 4> extensions =
            { "", ".exe", ".com", ".bat" };
        for (boost::array<std::string, 4>::iterator it2 = extensions.begin();
            it2 != extensions.end(); ++it2)
        {
            boost::filesystem::path p2 = p.stem();
            p2 += *it2;
            if (::SHGetFileInfoA(p2.string().c_str(), 0, 0, 0, SHGFI_EXETYPE))
            {
                result = p2.string();
                break;
            }
        }
    }
    return result;
}
#endif

}}}

#endif
