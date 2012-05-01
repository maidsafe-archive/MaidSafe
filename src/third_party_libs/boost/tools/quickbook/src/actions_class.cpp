/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    Copyright (c) 2005 Thomas Guest
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include "actions_class.hpp"
#include "actions_state.hpp"
#include "quickbook.hpp"
#include "grammar.hpp"
#include "input_path.hpp"

#if (defined(BOOST_MSVC) && (BOOST_MSVC <= 1310))
#pragma warning(disable:4355)
#endif

namespace quickbook
{
    actions::actions(fs::path const& filein_, fs::path const& xinclude_base_,
            string_stream& out_, id_manager& ids)
        : grammar_()

        , xinclude_base(xinclude_base_)

        , templates()
        , error_count(0)
        , anchors()
        , warned_about_breaks(false)
        , conditional(true)
        , ids(ids)

        , imported(false)
        , macro()
        , source_mode("c++")
        , current_file(0)
        , filename_relative(filein_.filename())

        , template_depth(0)
        , min_section_level(1)

        , out(out_)
        , phrase()
        , values(&current_file)

        , to_value(*this)
        , scoped_cond_phrase(*this)

        , element(*this)
        , error(*this)
        , code(code_action::block, *this)
        , code_block(code_action::inline_block, *this)
        , inline_code(code_action::inline_, *this)
        , paragraph(*this)
        , list_item(*this)
        , phrase_end(*this)
        , raw_char(phrase)
        , plain_char(phrase, *this)
        , escape_unicode(phrase, *this)

        , simple_markup(phrase, *this)

        , break_(phrase, *this)
        , do_macro(phrase, *this)

        , element_id_warning(*this)
    {
        // add the predefined macros
        macro.add
            ("__DATE__", std::string(quickbook_get_date))
            ("__TIME__", std::string(quickbook_get_time))
            ("__FILENAME__", detail::path_to_generic(filename_relative))
        ;
        
        boost::scoped_ptr<quickbook_grammar> g(
            new quickbook_grammar(*this));
        grammar_.swap(g);
    }

    quickbook_grammar& actions::grammar() const {
        return *grammar_;
    }

    file_state::file_state(actions& a, scope_flags scope)
        : a(a)
        , scope(scope)
        , qbk_version(qbk_version_n)
        , imported(a.imported)
        , current_file(a.current_file)
        , filename_relative(a.filename_relative)
        , xinclude_base(a.xinclude_base)
        , source_mode(a.source_mode)
        , macro()
    {
        if (scope & scope_macros) macro = a.macro;
        if (scope & scope_templates) a.templates.push();
        if (scope & scope_output) {
            a.out.push();
            a.phrase.push();
        }
        a.values.builder.save();
    }

    file_state::~file_state()
    {
        a.values.builder.restore();
        boost::swap(qbk_version_n, qbk_version);
        boost::swap(a.imported, imported);
        boost::swap(a.current_file, current_file);
        boost::swap(a.filename_relative, filename_relative);
        boost::swap(a.xinclude_base, xinclude_base);
        boost::swap(a.source_mode, source_mode);
        if (scope & scope_output) {
            a.out.pop();
            a.phrase.pop();
        }
        if (scope & scope_templates) a.templates.pop();
        if (scope & scope_macros) a.macro = macro;
    }
    
    template_state::template_state(actions& a)
        : file_state(a, file_state::scope_all)
        , template_depth(a.template_depth)
        , min_section_level(a.min_section_level)
    {
    }

    template_state::~template_state()
    {
        boost::swap(a.template_depth, template_depth);
        boost::swap(a.min_section_level, min_section_level);
    }
}
