/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_QUICKBOOK_ACTIONS_HPP)
#define BOOST_SPIRIT_QUICKBOOK_ACTIONS_HPP

#include <string>
#include <vector>
#include "fwd.hpp"
#include "template_stack.hpp"
#include "utils.hpp"
#include "values.hpp"
#include "scoped.hpp"
#include "symbols.hpp"
#include <boost/spirit/include/classic_parser.hpp>

namespace quickbook
{
    namespace cl = boost::spirit::classic;

    extern unsigned qbk_version_n; // qbk_major_version * 100 + qbk_minor_version

    struct quickbook_range : cl::parser<quickbook_range> {
        quickbook_range(unsigned min_, unsigned max_)
            : min_(min_), max_(max_) {}
        
        template <typename ScannerT>
        typename cl::parser_result<quickbook_range, ScannerT>::type
        parse(ScannerT const& scan) const
        {
            if (qbk_version_n >= min_ && qbk_version_n < max_)
            {
                return scan.empty_match();
            }
            else
            {
                return scan.no_match();
            }
        }

        unsigned min_, max_;
    };
    
    inline quickbook_range qbk_since(unsigned min_) {
        return quickbook_range(min_, 999);
    }
    
    inline quickbook_range qbk_before(unsigned max_) {
        return quickbook_range(0, max_);
    }

    // Throws load_error
    int load_snippets(fs::path const& file, std::vector<template_symbol>& storage,
        std::string const& extension, value::tag_type load_type);

    std::string syntax_highlight(
        parse_iterator first, parse_iterator last,
        actions& escape_actions,
        std::string const& source_mode);        

    struct xinclude_path {
        xinclude_path(fs::path& path, std::string const& uri) :
            path(path), uri(uri) {}

        fs::path path;
        std::string uri;
    };

    xinclude_path calculate_xinclude_path(value const&, quickbook::actions&);

    struct error_message_action
    {
        // Prints an error message to std::cerr

        error_message_action(quickbook::actions& actions, std::string const& m)
            : actions(actions)
            , message(m)
        {}

        void operator()(parse_iterator, parse_iterator) const;

        quickbook::actions& actions;
        std::string message;
    };

    struct error_action
    {
        // Prints an error message to std::cerr

        error_action(quickbook::actions& actions)
        : actions(actions) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        error_message_action operator()(std::string const& message)
        {
            return error_message_action(actions, message);
        }

        quickbook::actions& actions;
    };

    struct element_action
    {
        element_action(quickbook::actions& actions)
            : actions(actions) {}

        void operator()(parse_iterator, parse_iterator) const;

        quickbook::actions& actions;
    };

    struct paragraph_action
    {
        //  implicit paragraphs
        //  doesn't output the paragraph if it's only whitespace.

        paragraph_action(
            quickbook::actions& actions)
        : actions(actions) {}

        void operator()() const;
        void operator()(parse_iterator, parse_iterator) const { (*this)(); }

        quickbook::actions& actions;
    };

    struct list_item_action
    {
        //  implicit paragraphs
        //  doesn't output the paragraph if it's only whitespace.

        list_item_action(
            quickbook::actions& actions)
        : actions(actions) {}

        void operator()() const;
        void operator()(parse_iterator, parse_iterator) const { (*this)(); }

        quickbook::actions& actions;
    };

    struct phrase_end_action
    {
        phrase_end_action(quickbook::actions& actions) :
            actions(actions) {}

        void operator()() const;
        void operator()(parse_iterator, parse_iterator) const { (*this)(); }

        quickbook::actions& actions;
    };

    struct simple_phrase_action
    {
        //  Handles simple text formats

        simple_phrase_action(
            collector& out
          , quickbook::actions& actions)
        : out(out)
        , actions(actions) {}

        void operator()(char) const;

        collector& out;
        quickbook::actions& actions;
    };

    struct cond_phrase_push : scoped_action_base
    {
        cond_phrase_push(quickbook::actions& x)
            : actions(x) {}

        bool start();
        void cleanup();

        quickbook::actions& actions;
        bool saved_conditional;
        std::vector<std::string> anchors;
    };

    extern char const* quickbook_get_date;
    extern char const* quickbook_get_time;

    struct do_macro_action
    {
        // Handles macro substitutions

        do_macro_action(collector& phrase, quickbook::actions& actions)
            : phrase(phrase)
            , actions(actions) {}

        void operator()(std::string const& str) const;
        collector& phrase;
        quickbook::actions& actions;
    };

    struct raw_char_action
    {
        // Prints a space

        raw_char_action(collector& out)
            : out(out) {}

        void operator()(char ch) const;
        void operator()(parse_iterator first, parse_iterator last) const;

        collector& out;
    };

    struct plain_char_action
    {
        // Prints a single plain char.
        // Converts '<' to "&lt;"... etc See utils.hpp

        plain_char_action(collector& phrase, quickbook::actions& actions)
        : phrase(phrase)
        , actions(actions) {}

        void operator()(char ch) const;
        void operator()(parse_iterator first, parse_iterator last) const;

        collector& phrase;
        quickbook::actions& actions;
    };
    
    struct escape_unicode_action
    {
        escape_unicode_action(collector& phrase, quickbook::actions& actions)
        : phrase(phrase)
        , actions(actions) {}
        void operator()(parse_iterator first, parse_iterator last) const;

        collector& phrase;
        quickbook::actions& actions;
    };

    struct code_action
    {
        enum code_type { block, inline_block, inline_ };
    
        // Does the actual syntax highlighing of code

        code_action(
            code_type type
          , quickbook::actions& actions)
        : type(type)
        , actions(actions)
        {
        }

        void operator()(parse_iterator first, parse_iterator last) const;

        code_type type;
        quickbook::actions& actions;
    };

    struct break_action
    {
        break_action(collector& phrase, quickbook::actions& actions)
        : phrase(phrase), actions(actions) {}

        void operator()(parse_iterator f, parse_iterator) const;

        collector& phrase;
        quickbook::actions& actions;
    };

   struct element_id_warning_action
   {
        element_id_warning_action(quickbook::actions& actions_)
            : actions(actions_) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        quickbook::actions& actions;
   };

    // Returns the doc_type, or an empty string if there isn't one.
    std::string pre(quickbook::actions& actions, parse_iterator pos, value include_doc_id, bool nested_file);
    void post(quickbook::actions& actions, std::string const& doc_type);

    struct to_value_scoped_action : scoped_action_base
    {
        to_value_scoped_action(quickbook::actions& actions)
            : actions(actions) {}

        bool start(value::tag_type = value::default_tag);
        void success(parse_iterator, parse_iterator);
        void cleanup();

        quickbook::actions& actions;
        std::vector<std::string> saved_anchors;
        value::tag_type tag;
    };
}

#endif // BOOST_SPIRIT_QUICKBOOK_ACTIONS_HPP
