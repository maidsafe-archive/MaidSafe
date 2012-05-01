/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    Copyright (c) 2011 Daniel James

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/

#if !defined(BOOST_QUICKBOOK_FILES_HPP)
#define BOOST_QUICKBOOK_FILES_HPP

#include <string>
#include <boost/filesystem/v3/path.hpp>
#include <boost/intrusive_ptr.hpp>
#include <stdexcept>
#include <cassert>
#include "intrusive_base.hpp"

namespace quickbook {

    namespace fs = boost::filesystem;

    struct file;
    typedef boost::intrusive_ptr<file> file_ptr;

    struct file_position
    {
        file_position() : line(1), column(1) {}
        file_position(int l, int c) : line(l), column(c) {}

        int line;
        int column;
    };

    struct file : intrusive_base<file>
    {
        fs::path const path;
        std::string source;
    private:
        unsigned qbk_version;
    public:

        file(fs::path const& path, std::string const& source,
                unsigned qbk_version) :
            path(path), source(source), qbk_version(qbk_version)
        {}

        virtual ~file() {}

        unsigned version() const {
            assert(qbk_version);
            return qbk_version;
        }

        void version(unsigned v) {
            // Check that either version hasn't been set, or it was
            // previously set to the same version (because the same
            // file has been loaded twice).
            assert(!qbk_version || qbk_version == v);
            qbk_version = v;
        }

        virtual file_position position_of(std::string::const_iterator) const;
    };

    // If version isn't supplied then it must be set later.
    file_ptr load(fs::path const& filename,
        unsigned qbk_version = 0);

    struct load_error : std::runtime_error
    {
        explicit load_error(std::string const& arg)
            : std::runtime_error(arg) {}
    };

    // Interface for creating fake files which are mapped to
    // real files, so that the position can be found later.

    struct mapped_file_builder_data;

    struct mapped_file_builder
    {
        typedef std::string::const_iterator iterator;
        typedef std::string::size_type pos;

        mapped_file_builder();
        ~mapped_file_builder();

        void start(file_ptr);
        file_ptr release();
        void clear();

        bool empty() const;
        pos get_pos() const;

        void add(char const*, iterator);
        void add(std::string const&, iterator);
        void add(iterator, iterator);
        void add(mapped_file_builder const&);
        void add(mapped_file_builder const&, pos, pos);
        void unindent_and_add(iterator, iterator);
    private:
        mapped_file_builder_data* data;

        mapped_file_builder(mapped_file_builder const&);
        mapped_file_builder& operator=(mapped_file_builder const&);
    };
}

#endif // BOOST_QUICKBOOK_FILES_HPP
