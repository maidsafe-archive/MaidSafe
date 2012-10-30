// Copyright (c) 2006, 2007 Julio M. Merino Vidal
// Copyright (c) 2008 Ilya Sokolov, Boris Schaeling
// Copyright (c) 2009 Boris Schaeling
// Copyright (c) 2010 Felipe Tanus, Boris Schaeling
// Copyright (c) 2011, 2012 Jeff Flinn, Boris Schaeling
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

/**
 * \file boost/process/windows/initializers/set_args.hpp
 *
 * Defines an initializer to set the command line arguments.
 */

#ifndef BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ARGS_HPP
#define BOOST_PROCESS_WINDOWS_INITIALIZERS_SET_ARGS_HPP

#include <boost/process/windows/initializers/initializer_base.hpp>
#include <boost/range/algorithm/for_each.hpp>
#include <boost/range/algorithm/copy.hpp>
#include <boost/bind.hpp>
#include <boost/ref.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/io/detail/quoted_manip.hpp>
#include <boost/shared_array.hpp>
#include <sstream>

namespace boost { namespace process { namespace windows { namespace initializers {

template <class Range>
class set_args_ : public initializer_base
{
private:
    typedef typename Range::value_type String;
    typedef typename String::value_type Char;
    typedef std::basic_ostringstream<Char> OStringStream;

    void add(const String &s, OStringStream &os)
    {
        if (boost::algorithm::contains(s, String(1, static_cast<Char>(' '))))
            os << boost::io::quoted(s) << static_cast<Char>(' ');
        else
            os << s << static_cast<Char>(' ');
    }

public:
    explicit set_args_(const Range &args)
    {
        OStringStream os;
        boost::for_each(args, boost::bind(&set_args_::add, this,
            _1, boost::ref(os)));
        String s = os.str();
        cmd_line_.reset(new Char[s.size() + 1]);
        boost::copy(s, cmd_line_.get());
        cmd_line_[s.size()] = 0;
    }

    template <class WindowsExecutor>
    void on_CreateProcess_setup(WindowsExecutor &e) const
    {
        e.cmd_line = cmd_line_.get();
    }

private:
    boost::shared_array<Char> cmd_line_;
};

template <class Range>
set_args_<Range> set_args(const Range &range)
{
    return set_args_<Range>(range);
}

}}}}

#endif
