/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_chset.hpp>
#include <boost/spirit/include/classic_symbols.hpp>
#include <boost/spirit/include/classic_loops.hpp>
#include "grammar.hpp"
#include "grammar_impl.hpp" // Just for context stuff. Should move?
#include "actions_class.hpp"
#include "files.hpp"
#include "input_path.hpp"

namespace quickbook
{    
    namespace cl = boost::spirit::classic;

    // quickbook::actions is used in a few places here, as 'escape_actions'.
    // It's named differently to distinguish it from the syntax highlighting
    // actions, declared below.

    // Syntax Highlight Actions

    struct span
    {
        // Decorates c++ code fragments

        span(char const* name, collector& out)
        : name(name), out(out) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        char const* name;
        collector& out;
    };

    struct span_start
    {
        span_start(char const* name, collector& out)
        : name(name), out(out) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        char const* name;
        collector& out;
    };

    struct span_end
    {
        span_end(collector& out)
        : out(out) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        collector& out;
    };

    struct unexpected_char
    {
        // Handles unexpected chars in c++ syntax

        unexpected_char(
            collector& out
          , quickbook::actions& escape_actions)
        : out(out)
        , escape_actions(escape_actions) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        collector& out;
        quickbook::actions& escape_actions;
    };

    struct plain_char
    {
        // Prints a single plain char.
        // Converts '<' to "&lt;"... etc See utils.hpp

        plain_char(collector& out)
        : out(out) {}

        void operator()(char ch) const;
        void operator()(parse_iterator first, parse_iterator last) const;

        collector& out;
    };

    struct pre_escape_back
    {
        // Escapes back from code to quickbook (Pre)

        pre_escape_back(actions& escape_actions)
            : escape_actions(escape_actions) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        actions& escape_actions;
    };

    struct post_escape_back
    {
        // Escapes back from code to quickbook (Post)

        post_escape_back(collector& out, actions& escape_actions)
            : out(out), escape_actions(escape_actions) {}

        void operator()(parse_iterator first, parse_iterator last) const;

        collector& out;
        actions& escape_actions;
    };

    void span::operator()(parse_iterator first, parse_iterator last) const
    {
        out << "<phrase role=\"" << name << "\">";
        while (first != last)
            detail::print_char(*first++, out.get());
        out << "</phrase>";
    }

    void span_start::operator()(parse_iterator first, parse_iterator last) const
    {
        out << "<phrase role=\"" << name << "\">";
        while (first != last)
            detail::print_char(*first++, out.get());
    }

    void span_end::operator()(parse_iterator first, parse_iterator last) const
    {
        while (first != last)
            detail::print_char(*first++, out.get());
        out << "</phrase>";
    }

    void unexpected_char::operator()(parse_iterator first, parse_iterator last) const
    {
        file_position const pos = escape_actions.current_file->position_of(first.base());

        detail::outwarn(escape_actions.current_file->path, pos.line)
            << "in column:" << pos.column
            << ", unexpected character: " << detail::utf8(first, last)
            << "\n";

        // print out an unexpected character
        out << "<phrase role=\"error\">";
        while (first != last)
            detail::print_char(*first++, out.get());
        out << "</phrase>";
    }

    void plain_char::operator()(char ch) const
    {
        detail::print_char(ch, out.get());
    }

    void plain_char::operator()(parse_iterator first, parse_iterator last) const
    {
        while (first != last)
            detail::print_char(*first++, out.get());
    }

    void pre_escape_back::operator()(parse_iterator, parse_iterator) const
    {
        escape_actions.phrase.push(); // save the stream
    }

    void post_escape_back::operator()(parse_iterator, parse_iterator) const
    {
        out << escape_actions.phrase.str();
        escape_actions.phrase.pop(); // restore the stream
    }

    // Syntax

    struct keywords_holder
    {
        cl::symbols<> cpp, python;

        keywords_holder()
        {
            cpp
                    =   "and_eq", "and", "asm", "auto", "bitand", "bitor",
                        "bool", "break", "case", "catch", "char", "class",
                        "compl", "const_cast", "const", "continue", "default",
                        "delete", "do", "double", "dynamic_cast",  "else",
                        "enum", "explicit", "export", "extern", "false",
                        "float", "for", "friend", "goto", "if", "inline",
                        "int", "long", "mutable", "namespace", "new", "not_eq",
                        "not", "operator", "or_eq", "or", "private",
                        "protected", "public", "register", "reinterpret_cast",
                        "return", "short", "signed", "sizeof", "static",
                        "static_cast", "struct", "switch", "template", "this",
                        "throw", "true", "try", "typedef", "typeid",
                        "typename", "union", "unsigned", "using", "virtual",
                        "void", "volatile", "wchar_t", "while", "xor_eq", "xor"
                    ;

            python
                    =
                    "and",       "del",       "for",       "is",        "raise",
                    "assert",    "elif",      "from",      "lambda",    "return",
                    "break",     "else",      "global",    "not",       "try",
                    "class",     "except",    "if",        "or",        "while",
                    "continue",  "exec",      "import",    "pass",      "yield",
                    "def",       "finally",   "in",        "print",

                    // Technically "as" and "None" are not yet keywords (at Python
                    // 2.4). They are destined to become keywords, and we treat them
                    // as such for syntax highlighting purposes.

                    "as", "None"
                    ;
        }
    };

    namespace {
        keywords_holder keywords;
    }

    // Grammar for C++ highlighting
    struct cpp_highlight
    : public cl::grammar<cpp_highlight>
    {
        cpp_highlight(collector& out, actions& escape_actions)
        : out(out), escape_actions(escape_actions) {}

        template <typename Scanner>
        struct definition
        {
            definition(cpp_highlight const& self)
                : g(self.escape_actions.grammar())
            {
                program
                    =
                    *(  (+cl::space_p)  [plain_char(self.out)]
                    |   macro
                    |   escape
                    |   preprocessor    [span("preprocessor", self.out)]
                    |   comment
                    |   keyword         [span("keyword", self.out)]
                    |   identifier      [span("identifier", self.out)]
                    |   special         [span("special", self.out)]
                    |   string_         [span("string", self.out)]
                    |   char_           [span("char", self.out)]
                    |   number          [span("number", self.out)]
                    |   cl::repeat_p(1)[cl::anychar_p]
                                        [unexpected_char(self.out, self.escape_actions)]
                    )
                    ;

                macro =
                    // must not be followed by alpha or underscore
                    cl::eps_p(self.escape_actions.macro
                        >> (cl::eps_p - (cl::alpha_p | '_')))
                    >> self.escape_actions.macro        [do_macro_action(self.out, self.escape_actions)]
                    ;

                escape =
                    cl::str_p("``")     [pre_escape_back(self.escape_actions)]
                    >>
                    (
                        (
                            (
                                (+(cl::anychar_p - "``") >> cl::eps_p("``"))
                                & g.phrase
                            )
                            >>  cl::str_p("``")
                        )
                        |
                        (
                            cl::eps_p   [self.escape_actions.error]
                            >> *cl::anychar_p
                        )
                    )                   [post_escape_back(self.out, self.escape_actions)]
                    ;

                preprocessor
                    =   '#' >> *cl::space_p >> ((cl::alpha_p | '_') >> *(cl::alnum_p | '_'))
                    ;

                comment
                    =   cl::str_p("//")         [span_start("comment", self.out)]
                    >>  *(  escape
                        |   (+(cl::anychar_p - (cl::eol_p | "``")))
                                                [plain_char(self.out)]
                        )
                    >>  cl::eps_p               [span_end(self.out)]
                    |   cl::str_p("/*")         [span_start("comment", self.out)]
                    >>  *(  escape
                        |   (+(cl::anychar_p - (cl::str_p("*/") | "``")))
                                                [plain_char(self.out)]
                        )
                    >>  (!cl::str_p("*/"))      [span_end(self.out)]
                    ;

                keyword
                    =   keywords.cpp >> (cl::eps_p - (cl::alnum_p | '_'))
                    ;   // make sure we recognize whole words only

                special
                    =   +cl::chset_p("~!%^&*()+={[}]:;,<.>?/|\\-")
                    ;

                string_char = ('\\' >> cl::anychar_p) | (cl::anychar_p - '\\');

                string_
                    =   !cl::as_lower_d['l'] >> cl::confix_p('"', *string_char, '"')
                    ;

                char_
                    =   !cl::as_lower_d['l'] >> cl::confix_p('\'', *string_char, '\'')
                    ;

                number
                    =   (
                            cl::as_lower_d["0x"] >> cl::hex_p
                        |   '0' >> cl::oct_p
                        |   cl::real_p
                        )
                        >>  *cl::as_lower_d[cl::chset_p("ldfu")]
                    ;

                identifier
                    =   (cl::alpha_p | '_') >> *(cl::alnum_p | '_')
                    ;
            }

            cl::rule<Scanner>
                            program, macro, preprocessor, comment, special, string_, 
                            char_, number, identifier, keyword, escape,
                            string_char;

            quickbook_grammar& g;

            cl::rule<Scanner> const&
            start() const { return program; }
        };

        collector& out;
        actions& escape_actions;
    };

    // Grammar for Python highlighting
    // See also: The Python Reference Manual
    // http://docs.python.org/ref/ref.html
    struct python_highlight
    : public cl::grammar<python_highlight>
    {
        python_highlight(collector& out, actions& escape_actions)
        : out(out), escape_actions(escape_actions) {}

        template <typename Scanner>
        struct definition
        {
            definition(python_highlight const& self)
                : g(self.escape_actions.grammar())
            {
                program
                    =
                    *(  (+cl::space_p)  [plain_char(self.out)]
                    |   macro
                    |   escape          
                    |   comment
                    |   keyword         [span("keyword", self.out)]
                    |   identifier      [span("identifier", self.out)]
                    |   special         [span("special", self.out)]
                    |   string_         [span("string", self.out)]
                    |   number          [span("number", self.out)]
                    |   cl::repeat_p(1)[cl::anychar_p]
                                        [unexpected_char(self.out, self.escape_actions)]
                    )
                    ;

                macro = 
                    // must not be followed by alpha or underscore
                    cl::eps_p(self.escape_actions.macro
                        >> (cl::eps_p - (cl::alpha_p | '_')))
                    >> self.escape_actions.macro        [do_macro_action(self.out, self.escape_actions)]
                    ;

                escape =
                    cl::str_p("``")     [pre_escape_back(self.escape_actions)]
                    >>
                    (
                        (
                            (
                                (+(cl::anychar_p - "``") >> cl::eps_p("``"))
                                & g.phrase
                            )
                            >>  cl::str_p("``")
                        )
                        |
                        (
                            cl::eps_p   [self.escape_actions.error]
                            >> *cl::anychar_p
                        )
                    )                   [post_escape_back(self.out, self.escape_actions)]
                    ;

                comment
                    =   cl::str_p("#")          [span_start("comment", self.out)]
                    >>  *(  escape
                        |   (+(cl::anychar_p - (cl::eol_p | "``")))
                                                [plain_char(self.out)]
                        )
                    >>  cl::eps_p               [span_end(self.out)]
                    ;

                keyword
                    =   keywords.python >> (cl::eps_p - (cl::alnum_p | '_'))
                    ;   // make sure we recognize whole words only

                special
                    =   +cl::chset_p("~!%^&*()+={[}]:;,<.>/|\\-")
                    ;

                string_prefix
                    =    cl::as_lower_d[cl::str_p("u") >> ! cl::str_p("r")]
                    ;
                
                string_
                    =   ! string_prefix >> (long_string | short_string)
                    ;

                string_char = ('\\' >> cl::anychar_p) | (cl::anychar_p - '\\');
            
                short_string
                    =   cl::confix_p('\'', * string_char, '\'') |
                        cl::confix_p('"', * string_char, '"')
                    ;
            
                long_string
                    // Note: the "cl::str_p" on the next two lines work around
                    // an INTERNAL COMPILER ERROR when using VC7.1
                    =   cl::confix_p(cl::str_p("'''"), * string_char, "'''") |
                        cl::confix_p(cl::str_p("\"\"\""), * string_char, "\"\"\"")
                    ;
                
                number
                    =   (
                            cl::as_lower_d["0x"] >> cl::hex_p
                        |   '0' >> cl::oct_p
                        |   cl::real_p
                        )
                        >>  *cl::as_lower_d[cl::chset_p("lj")]
                    ;

                identifier
                    =   (cl::alpha_p | '_') >> *(cl::alnum_p | '_')
                    ;
            }

            cl::rule<Scanner>
                            program, macro, comment, special, string_, string_prefix, 
                            short_string, long_string, number, identifier, keyword, 
                            escape, string_char;

            quickbook_grammar& g;

            cl::rule<Scanner> const&
            start() const { return program; }
        };

        collector& out;
        actions& escape_actions;
    };

    // Grammar for plain text (no actual highlighting)
    struct teletype_highlight
    : public cl::grammar<teletype_highlight>
    {
        teletype_highlight(collector& out, actions& escape_actions)
        : out(out), escape_actions(escape_actions) {}

        template <typename Scanner>
        struct definition
        {
            definition(teletype_highlight const& self)
                : g(self.escape_actions.grammar())
            {
                program
                    =
                    *(  macro
                    |   escape          
                    |   cl::repeat_p(1)[cl::anychar_p]  [plain_char(self.out)]
                    )
                    ;

                macro =
                    // must not be followed by alpha or underscore
                    cl::eps_p(self.escape_actions.macro
                        >> (cl::eps_p - (cl::alpha_p | '_')))
                    >> self.escape_actions.macro        [do_macro_action(self.out, self.escape_actions)]
                    ;

                escape =
                    cl::str_p("``")     [pre_escape_back(self.escape_actions)]
                    >>
                    (
                        (
                            (
                                (+(cl::anychar_p - "``") >> cl::eps_p("``"))
                                & g.phrase
                            )
                            >>  cl::str_p("``")
                        )
                        |
                        (
                            cl::eps_p   [self.escape_actions.error]
                            >> *cl::anychar_p
                        )
                    )                   [post_escape_back(self.out, self.escape_actions)]
                    ;
            }

            cl::rule<Scanner> program, macro, escape;

            quickbook_grammar& g;

            cl::rule<Scanner> const&
            start() const { return program; }
        };

        collector& out;
        actions& escape_actions;
    };

    std::string syntax_highlight(
        parse_iterator first,
        parse_iterator last,
        actions& escape_actions,
        std::string const& source_mode)
    {
        quickbook::collector temp;

        // print the code with syntax coloring
        if (source_mode == "c++")
        {
            cpp_highlight cpp_p(temp, escape_actions);
            boost::spirit::classic::parse(first, last, cpp_p);
        }
        else if (source_mode == "python")
        {
            python_highlight python_p(temp, escape_actions);
            boost::spirit::classic::parse(first, last, python_p);
        }
        else if (source_mode == "teletype")
        {
            teletype_highlight teletype_p(temp, escape_actions);
            boost::spirit::classic::parse(first, last, teletype_p);
        }
        else
        {
            BOOST_ASSERT(0);
        }

        std::string str;
        temp.swap(str);
        
        return str;
    }
}
