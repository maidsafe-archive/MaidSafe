/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#if !defined(BOOST_SPIRIT_ACTIONS_CLASS_HPP)
#define BOOST_SPIRIT_ACTIONS_CLASS_HPP

#include <boost/scoped_ptr.hpp>
#include "actions.hpp"
#include "parsers.hpp"
#include "values_parse.hpp"
#include "collector.hpp"

namespace quickbook
{
    namespace cl = boost::spirit::classic;
    namespace fs = boost::filesystem;

    struct actions
    {
        actions(fs::path const& filein_, fs::path const& xinclude_base, string_stream& out_,
                id_manager&);

    private:
        boost::scoped_ptr<quickbook_grammar> grammar_;

    public:
    ///////////////////////////////////////////////////////////////////////////
    // State
    ///////////////////////////////////////////////////////////////////////////

        typedef std::vector<std::string> string_list;

        static int const max_template_depth = 100;
        
    // global state
        fs::path                xinclude_base;
        template_stack          templates;
        int                     error_count;
        string_list             anchors;
        bool                    warned_about_breaks;
        bool                    conditional;
        id_manager&             ids;

    // state saved for files and templates.
        bool                    imported;
        string_symbols          macro;
        std::string             source_mode;
        file_ptr          current_file;
        fs::path                filename_relative;  // for the __FILENAME__ macro.
                                                    // (relative to the original file
                                                    //  or include path).

    // state saved for templates.
        int                     template_depth;
        int                     min_section_level;

    // output state - scoped by templates and grammar
        collector               out;            // main output stream
        collector               phrase;         // phrase output stream
        value_parser            values;         // parsed values

        quickbook_grammar& grammar() const;

    ///////////////////////////////////////////////////////////////////////////
    // actions
    ///////////////////////////////////////////////////////////////////////////

        void start_list(char mark);
        void end_list(char mark);
        void start_list_item();
        void end_list_item();

        scoped_parser<to_value_scoped_action>
                                to_value;
        scoped_parser<cond_phrase_push>
                                scoped_cond_phrase;

        element_action          element;
        error_action            error;

        code_action             code;
        code_action             code_block;
        code_action             inline_code;
        paragraph_action        paragraph;
        list_item_action        list_item;
        phrase_end_action       phrase_end;
        raw_char_action         raw_char;
        plain_char_action       plain_char;
        escape_unicode_action   escape_unicode;

        simple_phrase_action    simple_markup;

        break_action            break_;
        do_macro_action         do_macro;

        element_id_warning_action element_id_warning;
    };
}

#endif // BOOST_SPIRIT_ACTIONS_CLASS_HPP

