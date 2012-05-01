/*=============================================================================
    Copyright (c) 2002 2004 2006 Joel de Guzman
    Copyright (c) 2004 Eric Niebler
    Copyright (c) 2005 Thomas Guest
    http://spirit.sourceforge.net/

    Use, modification and distribution is subject to the Boost Software
    License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt)
=============================================================================*/
#include <numeric>
#include <functional>
#include <vector>
#include <map>
#include <boost/filesystem/v3/convenience.hpp>
#include <boost/filesystem/v3/fstream.hpp>
#include <boost/range/distance.hpp>
#include <boost/range/algorithm/replace.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/replace.hpp>
#include <boost/next_prior.hpp>
#include <boost/foreach.hpp>
#include "quickbook.hpp"
#include "actions.hpp"
#include "utils.hpp"
#include "files.hpp"
#include "markups.hpp"
#include "actions_class.hpp"
#include "actions_state.hpp"
#include "grammar.hpp"
#include "input_path.hpp"
#include "block_tags.hpp"
#include "phrase_tags.hpp"
#include "id_manager.hpp"

namespace quickbook
{
    char const* quickbook_get_date = "__quickbook_get_date__";
    char const* quickbook_get_time = "__quickbook_get_time__";

    unsigned qbk_version_n = 0; // qbk_major_version * 100 + qbk_minor_version

    namespace {
        void write_anchors(quickbook::actions& actions, collector& tgt)
        {
            for(quickbook::actions::string_list::iterator
                it = actions.anchors.begin(),
                end = actions.anchors.end();
                it != end; ++it)
            {
                tgt << "<anchor id=\"";
                detail::print_string(*it, tgt.get());
                tgt << "\"/>";
            }
            
            actions.anchors.clear();
        }
        
        std::string add_anchor(quickbook::actions& actions,
                std::string const& id,
                id_category::categories category =
                    id_category::explicit_anchor_id)
        {
            std::string placeholder = actions.ids.add_anchor(id, category);
            actions.anchors.push_back(placeholder);
            return placeholder;
        }
    }

    void explicit_list_action(quickbook::actions&, value);
    void header_action(quickbook::actions&, value);
    void begin_section_action(quickbook::actions&, value);
    void end_section_action(quickbook::actions&, value, string_iterator);
    void block_action(quickbook::actions&, value);
    void block_empty_action(quickbook::actions&, value);
    void macro_definition_action(quickbook::actions&, value);
    void template_body_action(quickbook::actions&, value);
    void variable_list_action(quickbook::actions&, value);
    void table_action(quickbook::actions&, value);
    void xinclude_action(quickbook::actions&, value);
    void include_action(quickbook::actions&, value, string_iterator);
    void image_action(quickbook::actions&, value);
    void anchor_action(quickbook::actions&, value);
    void link_action(quickbook::actions&, value);
    void phrase_action(quickbook::actions&, value);
    void role_action(quickbook::actions&, value);
    void footnote_action(quickbook::actions&, value);
    void raw_phrase_action(quickbook::actions&, value);
    void source_mode_action(quickbook::actions&, value);
    void do_template_action(quickbook::actions&, value, string_iterator);
    
    void element_action::operator()(parse_iterator first, parse_iterator) const
    {
        value_consumer values = actions.values.release();
        if(!values.check() || !actions.conditional) return;
        value v = values.consume();
        values.finish();
        
        switch(v.get_tag())
        {
        case block_tags::ordered_list:
        case block_tags::itemized_list:
            return explicit_list_action(actions, v);
        case block_tags::generic_heading:
        case block_tags::heading1:
        case block_tags::heading2:
        case block_tags::heading3:
        case block_tags::heading4:
        case block_tags::heading5:
        case block_tags::heading6:
            return header_action(actions, v);
        case block_tags::begin_section:
            return begin_section_action(actions, v);
        case block_tags::end_section:
            return end_section_action(actions, v, first.base());
        case block_tags::blurb:
        case block_tags::preformatted:
        case block_tags::blockquote:
        case block_tags::warning:
        case block_tags::caution:
        case block_tags::important:
        case block_tags::note:
        case block_tags::tip:
        case block_tags::block:
            return block_action(actions,v);
        case block_tags::hr:
            return block_empty_action(actions,v);
        case block_tags::macro_definition:
            return macro_definition_action(actions,v);
        case block_tags::template_definition:
            return template_body_action(actions,v);
        case block_tags::variable_list:
            return variable_list_action(actions, v);
        case block_tags::table:
            return table_action(actions, v);
        case block_tags::xinclude:
            return xinclude_action(actions, v);
        case block_tags::import:
        case block_tags::include:
            return include_action(actions, v, first.base());
        case phrase_tags::image:
            return image_action(actions, v);
        case phrase_tags::anchor:
            return anchor_action(actions, v);
        case phrase_tags::url:
        case phrase_tags::link:
        case phrase_tags::funcref:
        case phrase_tags::classref:
        case phrase_tags::memberref:
        case phrase_tags::enumref:
        case phrase_tags::macroref:
        case phrase_tags::headerref:
        case phrase_tags::conceptref:
        case phrase_tags::globalref:
            return link_action(actions, v);
        case phrase_tags::bold:
        case phrase_tags::italic:
        case phrase_tags::underline:
        case phrase_tags::teletype:
        case phrase_tags::strikethrough:
        case phrase_tags::quote:
        case phrase_tags::replaceable:
            return phrase_action(actions, v);
        case phrase_tags::footnote:
            return footnote_action(actions, v);
        case phrase_tags::escape:
            return raw_phrase_action(actions, v);
        case phrase_tags::role:
            return role_action(actions, v);
        case source_mode_tags::cpp:
        case source_mode_tags::python:
        case source_mode_tags::teletype:
            return source_mode_action(actions, v);
        case template_tags::template_:
            return do_template_action(actions, v, first.base());
        default:
            break;
        }
    }

    void break_action::operator()(parse_iterator first, parse_iterator) const
    {
        write_anchors(actions, phrase);

        if(*first == '\\')
        {
            detail::outwarn(actions.current_file, first.base())
                //<< "in column:" << pos.column << ", "
                << "'\\n' is deprecated, pleases use '[br]' instead" << ".\n";
        }

        if(!actions.warned_about_breaks)
        {
            detail::outwarn(actions.current_file, first.base())
                << "line breaks generate invalid boostbook "
                   "(will only note first occurrence).\n";

            actions.warned_about_breaks = true;
        }
            
        phrase << detail::get_markup(phrase_tags::break_mark).pre;
    }

    void error_message_action::operator()(parse_iterator first, parse_iterator last) const
    {
        file_position const pos = actions.current_file->position_of(first.base());

        std::string value(first, last);
        std::string formatted_message = message;
        boost::replace_all(formatted_message, "%s", value);
        boost::replace_all(formatted_message, "%c",
            boost::lexical_cast<std::string>(pos.column));

        detail::outerr(actions.current_file->path, pos.line)
            << detail::utf8(formatted_message) << std::endl;
        ++actions.error_count;
    }

    void error_action::operator()(parse_iterator first, parse_iterator /*last*/) const
    {
        file_position const pos = actions.current_file->position_of(first.base());

        detail::outerr(actions.current_file->path, pos.line)
            << "Syntax Error near column " << pos.column << ".\n";
        ++actions.error_count;
    }

    void block_action(quickbook::actions& actions, value block)
    {
        write_anchors(actions, actions.out);

        detail::markup markup = detail::get_markup(block.get_tag());

        value_consumer values = block;
        actions.out << markup.pre << values.consume().get_encoded() << markup.post;
        values.finish();
    }

    void block_empty_action(quickbook::actions& actions, value block)
    {
        write_anchors(actions, actions.out);

        detail::markup markup = detail::get_markup(block.get_tag());
        actions.out << markup.pre;
    }

    void phrase_action(quickbook::actions& actions, value phrase)
    {
        write_anchors(actions, actions.phrase);

        detail::markup markup = detail::get_markup(phrase.get_tag());

        value_consumer values = phrase;
        actions.phrase << markup.pre << values.consume().get_encoded() << markup.post;
        values.finish();
    }

    void role_action(quickbook::actions& actions, value role)
    {
        write_anchors(actions, actions.phrase);

        value_consumer values = role;
        actions.phrase
            << "<phrase role=\"";
        detail::print_string(values.consume().get_quickbook(), actions.phrase.get());
        actions.phrase
            << "\">"
            << values.consume().get_encoded()
            << "</phrase>";
        values.finish();
    }

    void footnote_action(quickbook::actions& actions, value phrase)
    {
        write_anchors(actions, actions.phrase);

        value_consumer values = phrase;
        actions.phrase
            << "<footnote id=\""
            << actions.ids.add_id("f", id_category::numbered)
            << "\"><para>"
            << values.consume().get_encoded()
            << "</para></footnote>";
        values.finish();
    }

    void raw_phrase_action(quickbook::actions& actions, value phrase)
    {
        write_anchors(actions, actions.phrase);

        detail::markup markup = detail::get_markup(phrase.get_tag());
        actions.phrase << markup.pre << phrase.get_quickbook() << markup.post;
    }

    void paragraph_action::operator()() const
    {
        std::string str;
        actions.phrase.swap(str);

        std::string::const_iterator
            pos = str.begin(),
            end = str.end();

        while(pos != end && cl::space_p.test(*pos)) ++pos;

        if(pos != end) {
            detail::markup markup = detail::get_markup(block_tags::paragraph);
            actions.out << markup.pre << str;
            write_anchors(actions, actions.out);
            actions.out << markup.post;
        }
    }

    void list_item_action::operator()() const
    {
        std::string str;
        actions.phrase.swap(str);
        actions.out << str;
        write_anchors(actions, actions.out);
    }

    void phrase_end_action::operator()() const
    {
        write_anchors(actions, actions.phrase);
    }
    
    namespace {
        void write_bridgehead(quickbook::actions& actions, int level,
            std::string const& str, std::string const& id, bool self_link)
        {
            if (self_link && !id.empty())
            {
                actions.out << "<bridgehead renderas=\"sect" << level << "\"";
                actions.out << " id=\"";
                actions.out << actions.ids.add_id("h", id_category::numbered);
                actions.out << "\">";
                actions.out << "<phrase id=\"" << id << "\"/>";
                actions.out << "<link linkend=\"" << id << "\">";
                actions.out << str;
                actions.out << "</link>";
                actions.out << "</bridgehead>";
            }
            else
            {
                actions.out << "<bridgehead renderas=\"sect" << level << "\"";
                if(!id.empty()) actions.out << " id=\"" << id << "\"";
                actions.out << ">";
                actions.out << str;
                actions.out << "</bridgehead>";
            }
        }
    }

    void header_action(quickbook::actions& actions, value heading_list)
    {
        value_consumer values = heading_list;

        bool generic = heading_list.get_tag() == block_tags::generic_heading;
        value element_id = values.optional_consume(general_tags::element_id);
        value content = values.consume();
        values.finish();

        int level;

        if (generic)
        {
            level = actions.ids.section_level() + 1;
                                            // We need to use a heading which is one greater
                                            // than the current.
            if (level > 6 )                 // The max is h6, clip it if it goes
                level =  6;                 // further than that
        }
        else
        {
            level = heading_list.get_tag() - block_tags::heading1 + 1;
        }

        write_anchors(actions, actions.out);

        if (!element_id.empty())
        {
            std::string anchor = actions.ids.add_id(
                element_id.get_quickbook(),
                id_category::explicit_id);

            write_bridgehead(actions, level,
                content.get_encoded(), anchor, self_linked_headers);
        }
        else if (!generic && actions.ids.compatibility_version() < 103) // version 1.2 and below
        {
            // This generates the old id style if both the interpreting
            // version and the generation version are less then 103u.

            std::string anchor = actions.ids.old_style_id(
                detail::make_identifier(
                    actions.ids.replace_placeholders_with_unresolved_ids(
                        content.get_encoded())),
                id_category::generated_heading);

            write_bridgehead(actions, level,
                content.get_encoded(), anchor, false);

        }
        else
        {
            std::string anchor = actions.ids.add_id(
                detail::make_identifier(
                    actions.ids.compatibility_version() >= 106 ?
                        content.get_quickbook() :
                        actions.ids.replace_placeholders_with_unresolved_ids(
                            content.get_encoded())
                ),
                id_category::generated_heading);

            write_bridgehead(actions, level,
                content.get_encoded(), anchor, self_linked_headers);
        }
    }

    void simple_phrase_action::operator()(char mark) const
    {
        write_anchors(actions, out);

        int tag =
            mark == '*' ? phrase_tags::bold :
            mark == '/' ? phrase_tags::italic :
            mark == '_' ? phrase_tags::underline :
            mark == '=' ? phrase_tags::teletype :
            0;
        
        assert(tag != 0);
        detail::markup markup = detail::get_markup(tag);

        value_consumer values = actions.values.release();
        value content = values.consume();
        values.finish();

        out << markup.pre;
        out << content.get_encoded();
        out << markup.post;
    }

    bool cond_phrase_push::start()
    {
        value_consumer values = actions.values.release();

        saved_conditional = actions.conditional;

        if (saved_conditional)
        {
            string_ref macro1 = values.consume().get_quickbook();
            std::string macro(macro1.begin(), macro1.end());

            actions.conditional = find(actions.macro, macro.c_str());

            if (!actions.conditional) {
                actions.phrase.push();
                actions.out.push();
                actions.anchors.swap(anchors);
            }
        }

        return true;
    }
    
    void cond_phrase_push::cleanup()
    {
        if (saved_conditional && !actions.conditional)
        {
            actions.phrase.pop();
            actions.out.pop();
            actions.anchors.swap(anchors);
        }

        actions.conditional = saved_conditional;
    }

    namespace {
        int indent_length(std::string const& indent)
        {
            int length = 0;
            for(std::string::const_iterator
                first = indent.begin(), end = indent.end(); first != end; ++first)
            {
                switch(*first) {
                    case ' ': ++length; break;
                    // hardcoded tab to 4 for now
                    case '\t': length = ((length + 4) / 4) * 4; break;
                    default: BOOST_ASSERT(false);
                }
            }
            
            return length;
        }
    }

    void actions::start_list(char mark)
    {
        write_anchors(*this, out);
        assert(mark == '*' || mark == '#');
        out << ((mark == '#') ? "<orderedlist>\n" : "<itemizedlist>\n");
    }

    void actions::end_list(char mark)
    {
        write_anchors(*this, out);
        assert(mark == '*' || mark == '#');
        out << ((mark == '#') ? "\n</orderedlist>" : "\n</itemizedlist>");
    }

    void actions::start_list_item()
    {
        out << "<listitem><simpara>";
        write_anchors(*this, out);
    }

    void actions::end_list_item()
    {
        write_anchors(*this, out);
        out << "</simpara></listitem>";
    }

    void explicit_list_action(quickbook::actions& actions, value list)
    {
        write_anchors(actions, actions.out);

        detail::markup markup = detail::get_markup(list.get_tag());

        actions.out << markup.pre;

        BOOST_FOREACH(value item, list)
        {
            actions.out << "<listitem>";
            actions.out << item.get_encoded();
            actions.out << "</listitem>";
        }

        actions.out << markup.post;
    }

    void anchor_action(quickbook::actions& actions, value anchor)
    {
        value_consumer values = anchor;
        value anchor_id = values.consume();
        // Note: anchor_id is never encoded as boostbook. If it
        // is encoded, it's just things like escapes.
        add_anchor(actions, anchor_id.is_encoded() ?
            anchor_id.get_encoded() : anchor_id.get_quickbook());
        values.finish();
    }

    void do_macro_action::operator()(std::string const& str) const
    {
        write_anchors(actions, phrase);

        if (str == quickbook_get_date)
        {
            char strdate[64];
            strftime(strdate, sizeof(strdate), "%Y-%b-%d", current_time);
            phrase << strdate;
        }
        else if (str == quickbook_get_time)
        {
            char strdate[64];
            strftime(strdate, sizeof(strdate), "%I:%M:%S %p", current_time);
            phrase << strdate;
        }
        else
        {
            phrase << str;
        }
    }

    void raw_char_action::operator()(char ch) const
    {
        out << ch;
    }

    void raw_char_action::operator()(parse_iterator first, parse_iterator last) const
    {
        while (first != last)
            out << *first++;
    }

    void source_mode_action(quickbook::actions& actions, value source_mode)
    {
        actions.source_mode = source_mode_tags::name(source_mode.get_tag());
    }

    void code_action::operator()(parse_iterator first, parse_iterator last) const
    {
        bool inline_code = type == inline_ ||
            (type == inline_block && qbk_version_n < 106u);
        bool block = type != inline_;

        if (inline_code) {
            write_anchors(actions, actions.phrase);
        }
        else {
            actions.paragraph();
            write_anchors(actions, actions.out);
        }

        std::string str;

        if (block) {
            // preprocess the code section to remove the initial indentation
            mapped_file_builder mapped;
            mapped.start(actions.current_file);
            mapped.unindent_and_add(first.base(), last.base());

            file_ptr f = mapped.release();

            if (f->source.empty())
                return; // Nothing left to do here. The program is empty.

            parse_iterator first_(f->source.begin());
            parse_iterator last_(f->source.end());

            file_ptr saved_file = f;
            boost::swap(actions.current_file, saved_file);

            // print the code with syntax coloring
            str = syntax_highlight(first_, last_, actions, actions.source_mode);

            boost::swap(actions.current_file, saved_file);
        }
        else {
            parse_iterator first_(first);
            str = syntax_highlight(first_, last, actions, actions.source_mode);
        }

        if (block) {
            collector& output = inline_code ? actions.phrase : actions.out;

            // We must not place a \n after the <programlisting> tag
            // otherwise PDF output starts code blocks with a blank line:
            //
            output << "<programlisting>";
            output << str;
            output << "</programlisting>\n";
        }
        else {
            actions.phrase << "<code>";
            actions.phrase << str;
            actions.phrase << "</code>";
        }
    }

    void plain_char_action::operator()(char ch) const
    {
        write_anchors(actions, phrase);

        detail::print_char(ch, phrase.get());
    }

    void plain_char_action::operator()(parse_iterator first, parse_iterator last) const
    {
        write_anchors(actions, phrase);

        while (first != last)
            detail::print_char(*first++, phrase.get());
    }

    void escape_unicode_action::operator()(parse_iterator first, parse_iterator last) const
    {
        write_anchors(actions, phrase);

        while(first != last && *first == '0') ++first;

        // Just ignore \u0000
        // Maybe I should issue a warning?
        if(first == last) return;
        
        std::string hex_digits(first, last);
        
        if(hex_digits.size() == 2 && *first > '0' && *first <= '7') {
            using namespace std;
            detail::print_char(strtol(hex_digits.c_str(), 0, 16), phrase.get());
        }
        else {
            phrase << "&#x" << hex_digits << ";";
        }
    }

    void write_plain_text(std::ostream& out, value const& v)
    {
        if (v.is_encoded())
        {
            detail::print_string(v.get_encoded(), out);
        }
        else {
            std::string value = v.get_quickbook();
            for(std::string::const_iterator
                first = value.begin(), last  = value.end();
                first != last; ++first)
            {
                if (*first == '\\' && ++first == last) break;
                detail::print_char(*first, out);
            }
        }
    }

    void image_action(quickbook::actions& actions, value image)
    {
        write_anchors(actions, actions.phrase);

        // Note: attributes are never encoded as boostbook, if they're
        // encoded, it's just things like escapes.
        typedef std::map<std::string, value> attribute_map;
        attribute_map attributes;

        value_consumer values = image;
        attributes["fileref"] = values.consume();

        BOOST_FOREACH(value pair_, values)
        {
            value_consumer pair = pair_;
            value name = pair.consume();
            value value = pair.consume();
            pair.finish();
            if(!attributes.insert(std::make_pair(name.get_quickbook(), value)).second)
            {
                detail::outwarn(name.get_file(), name.get_position())
                    << "Duplicate image attribute: "
                    << detail::utf8(name.get_quickbook())
                    << std::endl;
            }
        }
        
        values.finish();

        // Find the file basename and extension.
        //
        // Not using Boost.Filesystem because I want to stay in UTF-8.
        // Need to think about uri encoding.
        
        std::string fileref = attributes["fileref"].is_encoded() ?
            attributes["fileref"].get_encoded() :
            attributes["fileref"].get_quickbook();

        // Check for windows paths, then convert.
        // A bit crude, but there you go.

        if(fileref.find('\\') != std::string::npos)
        {
            (qbk_version_n >= 106u ?
                detail::outerr(attributes["fileref"].get_file(), attributes["fileref"].get_position()) :
                detail::outwarn(attributes["fileref"].get_file(), attributes["fileref"].get_position()))
                << "Image path isn't portable: '"
                << detail::utf8(fileref)
                << "'"
                << std::endl;
            if (qbk_version_n >= 106u) ++actions.error_count;
        }

        boost::replace(fileref, '\\', '/');

        // Find the file basename and extension.
        //
        // Not using Boost.Filesystem because I want to stay in UTF-8.
        // Need to think about uri encoding.

        std::string::size_type pos;
        std::string stem, extension;

        pos = fileref.rfind('/');
        stem = pos == std::string::npos ?
            fileref :
            fileref.substr(pos + 1);

        pos = stem.rfind('.');
        if (pos != std::string::npos)
        {
            extension = stem.substr(pos + 1);
            stem = stem.substr(0, pos);
        }

        // Extract the alt tag, to use as a text description.
        // Or if there isn't one, use the stem of the file name.

        attribute_map::iterator alt_pos = attributes.find("alt");
        quickbook::value alt_text =
            alt_pos != attributes.end() ? alt_pos->second :
            qbk_version_n < 106u ? encoded_value(stem) :
            quickbook::value();
        attributes.erase("alt");

        if(extension == "svg")
        {
           //
           // SVG's need special handling:
           //
           // 1) We must set the "format" attribute, otherwise
           //    HTML generation produces code that will not display
           //    the image at all.
           // 2) We need to set the "contentwidth" and "contentdepth"
           //    attributes, otherwise the image will be displayed inside
           //    a tiny box with scrollbars (Firefox), or else cropped to
           //    fit in a tiny box (IE7).
           //

           attributes.insert(attribute_map::value_type("format",
                encoded_value("SVG")));

           //
           // Image paths are relative to the html subdirectory:
           //
           fs::path img = detail::generic_to_path(fileref);
           if (!img.has_root_directory())
              img = quickbook::image_location / img;  // relative path

           //
           // Now load the SVG file:
           //
           std::string svg_text;
           fs::ifstream fs(img);
           char c;
           while(fs.get(c) && fs.good())
              svg_text.push_back(c);
           //
           // Extract the svg header from the file:
           //
           std::string::size_type a, b;
           a = svg_text.find("<svg");
           b = svg_text.find('>', a);
           svg_text = (a == std::string::npos) ? "" : svg_text.substr(a, b - a);
           //
           // Now locate the "width" and "height" attributes
           // and borrow their values:
           //
           a = svg_text.find("width");
           a = svg_text.find('=', a);
           a = svg_text.find('\"', a);
           b = svg_text.find('\"', a + 1);
           if(a != std::string::npos)
           {
              attributes.insert(std::make_pair(
                "contentwidth", encoded_value(std::string(
                    svg_text.begin() + a + 1, svg_text.begin() + b))
                ));
           }
           a = svg_text.find("height");
           a = svg_text.find('=', a);
           a = svg_text.find('\"', a);
           b = svg_text.find('\"', a + 1);
           if(a != std::string::npos)
           {
              attributes.insert(std::make_pair(
                "contentdepth", encoded_value(std::string(
                    svg_text.begin() + a + 1, svg_text.begin() + b))
                ));
           }
        }

        actions.phrase << "<inlinemediaobject>";

        actions.phrase << "<imageobject><imagedata";
        
        BOOST_FOREACH(attribute_map::value_type const& attr, attributes)
        {
            actions.phrase << " " << attr.first << "=\"";
            write_plain_text(actions.phrase.get(), attr.second);
            actions.phrase << "\"";
        }

        actions.phrase << "></imagedata></imageobject>";

        // Add a textobject containing the alt tag from earlier.
        // This will be used for the alt tag in html.
        if (alt_text.check()) {
            actions.phrase << "<textobject><phrase>";
            write_plain_text(actions.phrase.get(), alt_text);
            actions.phrase << "</phrase></textobject>";
        }

        actions.phrase << "</inlinemediaobject>";
    }

    void macro_definition_action(quickbook::actions& actions, quickbook::value macro_definition)
    {
        value_consumer values = macro_definition;
        std::string macro_id = values.consume().get_quickbook();
        value phrase_value = values.optional_consume();
        std::string phrase;
        if (phrase_value.check()) phrase = phrase_value.get_encoded();
        values.finish();

        std::string* existing_macro =
            boost::spirit::classic::find(actions.macro, macro_id.c_str());
        quickbook::ignore_variable(&existing_macro);

        if (existing_macro)
        {
            if (qbk_version_n < 106) return;

            // Do this if you're using spirit's TST.
            //
            // *existing_macro = phrase;
            // return;
        }

        actions.macro.add(
            macro_id.begin()
          , macro_id.end()
          , phrase);
    }

    void template_body_action(quickbook::actions& actions, quickbook::value template_definition)
    {
        value_consumer values = template_definition;
        std::string identifier = values.consume().get_quickbook();

        std::vector<std::string> template_values;
        BOOST_FOREACH(value const& p, values.consume()) {
            template_values.push_back(p.get_quickbook());
        }

        BOOST_ASSERT(values.check(template_tags::block) || values.check(template_tags::phrase));
        value body = values.consume();
        BOOST_ASSERT(!values.check());
    
        if (!actions.templates.add(
            template_symbol(
                identifier,
                template_values,
                body,
                &actions.templates.top_scope())))
        {
            detail::outwarn(body.get_file(), body.get_position())
                << "Template Redefinition: " << detail::utf8(identifier) << std::endl;
            ++actions.error_count;
        }
    }

    namespace
    {
        string_iterator find_first_seperator(string_iterator begin, string_iterator end)
        {
            if(qbk_version_n < 105) {
                for(;begin != end; ++begin)
                {
                    switch(*begin)
                    {
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                        return begin;
                    default:
                        break;
                    }
                }
            }
            else {
                unsigned int depth = 0;

                for(;begin != end; ++begin)
                {
                    switch(*begin)
                    {
                    case '[':
                        ++depth;
                        break;
                    case '\\':
                        if(++begin == end) return begin;
                        break;
                    case ']':
                        if (depth > 0) --depth;
                        break;
                    case ' ':
                    case '\t':
                    case '\n':
                    case '\r':
                        if (depth == 0) return begin;
                    default:
                        break;
                    }
                }
            }
            
            return begin;
        }
        
        std::pair<string_iterator, string_iterator> find_seperator(string_iterator begin, string_iterator end)
        {
            string_iterator first = begin = find_first_seperator(begin, end);

            for(;begin != end; ++begin)
            {
                switch(*begin)
                {
                case ' ':
                case '\t':
                case '\n':
                case '\r':
                    break;
                default:
                    return std::make_pair(first, begin);
                }
            }
            
            return std::make_pair(first, begin);
        }
    
        void break_arguments(
            std::vector<value>& args
          , std::vector<std::string> const& params
          , fs::path const& filename
        )
        {
            // Quickbook 1.4-: If there aren't enough parameters seperated by
            //                 '..' then seperate the last parameter using
            //                 whitespace.
            // Quickbook 1.5+: If '..' isn't used to seperate the parameters
            //                 then use whitespace to separate them
            //                 (2 = template name + argument).

            if (qbk_version_n < 105 || args.size() == 1)
            {
           
                while (args.size() < params.size())
                {
                    // Try to break the last argument at the first space found
                    // and push it into the back of args. Do this
                    // recursively until we have all the expected number of
                    // arguments, or if there are no more spaces left.

                    value last_arg = args.back();
                    string_iterator begin = last_arg.get_quickbook().begin();
                    string_iterator end = last_arg.get_quickbook().end();
                    
                    std::pair<string_iterator, string_iterator> pos =
                        find_seperator(begin, end);
                    if (pos.second == end) break;
                    value new_arg(
                        qbk_value(last_arg.get_file(),
                            pos.second, end, template_tags::phrase));

                    args.back() = qbk_value(last_arg.get_file(),
                        begin, pos.first, last_arg.get_tag());
                    args.push_back(new_arg);
                }
            }
        }

        std::pair<bool, std::vector<std::string>::const_iterator>
        get_arguments(
            std::vector<value> const& args
          , std::vector<std::string> const& params
          , template_scope const& scope
          , string_iterator first
          , quickbook::actions& actions
        )
        {
            std::vector<value>::const_iterator arg = args.begin();
            std::vector<std::string>::const_iterator tpl = params.begin();
            std::vector<std::string> empty_params;

            // Store each of the argument passed in as local templates:
            while (arg != args.end())
            {
                if (!actions.templates.add(
                        template_symbol(*tpl, empty_params, *arg, &scope)))
                {
                    detail::outerr(actions.current_file, first)
                        << "Duplicate Symbol Found" << std::endl;
                    ++actions.error_count;
                    return std::make_pair(false, tpl);
                }
                ++arg; ++tpl;
            }
            return std::make_pair(true, tpl);
        }
        
        bool parse_template(
            value const& content
          , quickbook::actions& actions
        )
        {
            file_ptr saved_current_file = actions.current_file;

            actions.current_file = content.get_file();
            string_ref source = content.get_quickbook();

            parse_iterator first(source.begin());
            parse_iterator last(source.end());

            bool r = cl::parse(first, last,
                    content.get_tag() == template_tags::block ?
                        actions.grammar().block :
                        actions.grammar().inline_phrase
                ).full;

            boost::swap(actions.current_file, saved_current_file);

            return r;
        }
    }

    void call_template(quickbook::actions& actions,
            template_symbol const* symbol,
            std::vector<value> const& args,
            string_iterator first)
    {
        // If this template contains already encoded text, then just
        // write it out, without going through any of the rigamarole.

        if (symbol->content.is_encoded())
        {
            if (symbol->content.get_tag() == template_tags::block)
            {
                actions.paragraph();
                actions.out << symbol->content.get_encoded();
            }
            else
            {
                actions.phrase << symbol->content.get_encoded();
            }

            return;
        }

        // The template arguments should have the scope that the template was
        // called from, not the template's own scope.
        //
        // Note that for quickbook 1.4- this value is just ignored when the
        // arguments are expanded.
        template_scope const& call_scope = actions.templates.top_scope();

        std::string block;
        std::string phrase;

        {
            template_state state(actions);
            actions.templates.start_template(symbol);

            qbk_version_n = symbol->content.get_file()->version();

            ++actions.template_depth;
            if (actions.template_depth > actions.max_template_depth)
            {
                detail::outerr(actions.current_file, first)
                    << "Infinite loop detected" << std::endl;
                ++actions.error_count;
                return;
            }

            // Store the current section level so that we can ensure that
            // [section] and [endsect] tags in the template are balanced.
            actions.min_section_level = actions.ids.section_level();

            ///////////////////////////////////
            // Prepare the arguments as local templates
            bool get_arg_result;
            std::vector<std::string>::const_iterator tpl;
            boost::tie(get_arg_result, tpl) =
                get_arguments(args, symbol->params, call_scope, first, actions);

            if (!get_arg_result)
            {
                return;
            }

            ///////////////////////////////////
            // parse the template body:

            if (!parse_template(symbol->content, actions))
            {
                detail::outerr(actions.current_file, first)
                    << "Expanding "
                    << (symbol->content.get_tag() == template_tags::block ? "block" : "phrase")
                    << " template: " << detail::utf8(symbol->identifier) << std::endl
                    << std::endl
                    << "------------------begin------------------" << std::endl
                    << detail::utf8(symbol->content.get_quickbook())
                    << "------------------end--------------------" << std::endl
                    << std::endl;
                ++actions.error_count;
                return;
            }

            if (actions.ids.section_level() != actions.min_section_level)
            {
                detail::outerr(actions.current_file, first)
                    << "Mismatched sections in template "
                    << detail::utf8(symbol->identifier)
                    << std::endl;
                ++actions.error_count;
                return;
            }

            actions.out.swap(block);
            actions.phrase.swap(phrase);
        }

        if(symbol->content.get_tag() == template_tags::block || !block.empty()) {
            actions.paragraph(); // For paragraphs before the template call.
            actions.out << block;
            actions.phrase << phrase;
            actions.paragraph();
        }
        else {
            actions.phrase << phrase;
        }
    }

    void call_code_snippet(quickbook::actions& actions,
            template_symbol const* symbol,
            string_iterator first)
    {
        value_consumer values = symbol->content;
        value content = values.consume(template_tags::block);
        value callouts = values.consume();
        values.finish();

        std::vector<std::string> callout_ids;
        std::vector<value> args;
        unsigned int size = symbol->params.size();
        std::string callout_base("c");

        for(unsigned int i = 0; i < size; ++i)
        {
            std::string callout_id1 = actions.ids.add_id(callout_base, id_category::numbered);
            std::string callout_id2 = actions.ids.add_id(callout_base, id_category::numbered);

            std::string code;
            code += "<co id=\"" + callout_id1 + "\" ";
            code += "linkends=\"" + callout_id2 + "\" />";

            args.push_back(encoded_value(code, template_tags::phrase));
            callout_ids.push_back(callout_id1);
            callout_ids.push_back(callout_id2);
        }

        // Create a fake symbol for call_template
        template_symbol t(
            symbol->identifier,
            symbol->params,
            content,
            symbol->lexical_parent);
        call_template(actions, &t, args, first);

        std::string block;

        if(!callouts.empty())
        {
            block += "<calloutlist>";
            int i = 0;
            BOOST_FOREACH(value c, callouts)
            {
                std::string callout_id1 = callout_ids[i++];
                std::string callout_id2 = callout_ids[i++];
    
                std::string callout_value;
                {
                    template_state state(actions);
                    ++actions.template_depth;

                    bool r = parse_template(c, actions);

                    if(!r)
                    {
                        detail::outerr(c.get_file(), c.get_position())
                            << "Expanding callout." << std::endl
                            << "------------------begin------------------" << std::endl
                            << detail::utf8(c.get_quickbook())
                            << std::endl
                            << "------------------end--------------------" << std::endl
                            ;
                        ++actions.error_count;
                        return;
                    }
    
                    actions.out.swap(callout_value);
                }
                
                block += "<callout arearefs=\"" + callout_id1 + "\" ";
                block += "id=\"" + callout_id2 + "\">";
                block += callout_value;
                block += "</callout>";
            }
            block += "</calloutlist>";
        }

        actions.out << block;
    }

    void do_template_action(quickbook::actions& actions, value template_list,
            string_iterator first)
    {
        // Get the arguments
        value_consumer values = template_list;

        bool template_escape = values.check(template_tags::escape);
        if(template_escape) values.consume();

        std::string identifier = values.consume(template_tags::identifier).get_quickbook();

        std::vector<value> args;

        BOOST_FOREACH(value arg, values)
        {
            args.push_back(arg);
        }
        
        values.finish();

        template_symbol const* symbol = actions.templates.find(identifier);
        BOOST_ASSERT(symbol);

        // Deal with escaped templates.

        if (template_escape)
        {
            if (!args.empty())
            {
                detail::outerr(actions.current_file, first)
                    << "Arguments for escaped template."
                    <<std::endl;
                ++actions.error_count;
            }

            if (symbol->content.is_encoded())
            {
                actions.phrase << symbol->content.get_encoded();
            }
            else
            {
                actions.phrase << symbol->content.get_quickbook();

                /*

                This would surround the escaped template in escape
                comments to indicate to the post-processor that it
                isn't quickbook generated markup. But I'm not sure if
                it would work.

                quickbook::detail::markup escape_markup
                    = detail::get_markup(phrase_tags::escape);

                actions.phrase
                    << escape_markup.pre
                    << symbol->content.get_quickbook()
                    << escape_markup.post
                    ;
                */
            }

            return;
        }

        ///////////////////////////////////
        // Initialise the arguments

        switch(symbol->content.get_tag())
        {
        case template_tags::block:
        case template_tags::phrase:
            // Break the arguments for a template

            break_arguments(args, symbol->params, actions.current_file->path);

            if (args.size() != symbol->params.size())
            {
                detail::outerr(actions.current_file, first)
                    << "Invalid number of arguments passed. Expecting: "
                    << symbol->params.size()
                    << " argument(s), got: "
                    << args.size()
                    << " argument(s) instead."
                    << std::endl;

                ++actions.error_count;
                return;
            }

            call_template(actions, symbol, args, first);
            break;

        case template_tags::snippet:

            if (!args.empty())
            {
                detail::outerr(actions.current_file, first)
                    << "Arguments for code snippet."
                    <<std::endl;
                ++actions.error_count;

                args.clear();
            }

            call_code_snippet(actions, symbol, first);
            break;

        default:
            assert(0);
        }
    }

    void link_action(quickbook::actions& actions, value link)
    {
        write_anchors(actions, actions.phrase);

        detail::markup markup = detail::get_markup(link.get_tag());

        value_consumer values = link;
        value dst_value = values.consume();
        value content = values.consume();
        values.finish();

        // Note: dst is never actually encoded as boostbook, which
        // is why the result is called with 'print_string' later.
        std::string dst = dst_value.is_encoded() ?
            dst_value.get_encoded() : dst_value.get_quickbook();
        
        actions.phrase << markup.pre;
        detail::print_string(dst, actions.phrase.get());
        actions.phrase << "\">";

        if (content.empty())
            detail::print_string(dst, actions.phrase.get());
        else
            actions.phrase << content.get_encoded();

        actions.phrase << markup.post;
    }

    void variable_list_action(quickbook::actions& actions, value variable_list)
    {
        write_anchors(actions, actions.out);

        value_consumer values = variable_list;
        std::string title = values.consume(table_tags::title).get_quickbook();

        actions.out << "<variablelist>\n";

        actions.out << "<title>";
        detail::print_string(title, actions.out.get());
        actions.out << "</title>\n";

        BOOST_FOREACH(value_consumer entry, values) {
            actions.out << "<varlistentry>";
            
            if(entry.check()) {
                actions.out << "<term>";
                actions.out << entry.consume().get_encoded();
                actions.out << "</term>";
            }
            
            if(entry.check()) {
                actions.out << "<listitem>";
                BOOST_FOREACH(value phrase, entry) actions.out << phrase.get_encoded();
                actions.out << "</listitem>";
            }

            actions.out << "</varlistentry>\n";
        }

        actions.out << "</variablelist>\n";
        
        values.finish();
    }

    void table_action(quickbook::actions& actions, value table)
    {
        write_anchors(actions, actions.out);

        value_consumer values = table;

        std::string element_id;
        if(values.check(general_tags::element_id))
            element_id = values.consume().get_quickbook();

        value title = values.consume(table_tags::title);
        bool has_title = !title.empty();
        
        std::string table_id;

        if (!element_id.empty()) {
            table_id = actions.ids.add_id(element_id, id_category::explicit_id);
        }
        else if (has_title) {
            if (actions.ids.compatibility_version() >= 105) {
                table_id = actions.ids.add_id(detail::make_identifier(title.get_quickbook()), id_category::generated);
            }
            else {
                table_id = actions.ids.add_id("t", id_category::numbered);
            }
        }

        // Emulating the old behaviour which used the width of the final
        // row for span_count.
        int row_count = 0;
        int span_count = 0;

        value_consumer lookahead = values;
        BOOST_FOREACH(value row, lookahead) {
            ++row_count;
            span_count = boost::distance(row);
        }
        lookahead.finish();

        if (has_title)
        {
            actions.out << "<table frame=\"all\"";
            if(!table_id.empty())
                actions.out << " id=\"" << table_id << "\"";
            actions.out << ">\n";
            actions.out << "<title>";
            if (qbk_version_n < 106u) {
                detail::print_string(title.get_quickbook(), actions.out.get());
            }
            else {
                actions.out << title.get_encoded();
            }
            actions.out << "</title>";
        }
        else
        {
            actions.out << "<informaltable frame=\"all\"";
            if(!table_id.empty())
                actions.out << " id=\"" << table_id << "\"";
            actions.out << ">\n";
        }

        actions.out << "<tgroup cols=\"" << span_count << "\">\n";

        if (row_count > 1)
        {
            actions.out << "<thead>" << "<row>";
            BOOST_FOREACH(value cell, values.consume()) {
                actions.out << "<entry>" << cell.get_encoded() << "</entry>";
            }
            actions.out << "</row>\n" << "</thead>\n";
        }

        actions.out << "<tbody>\n";

        BOOST_FOREACH(value row, values) {
            actions.out << "<row>";
            BOOST_FOREACH(value cell, row) {
                actions.out << "<entry>" << cell.get_encoded() << "</entry>";
            }
            actions.out << "</row>\n";
        }
        
        values.finish();

        actions.out << "</tbody>\n"
                     << "</tgroup>\n";

        if (has_title)
        {
            actions.out << "</table>\n";
        }
        else
        {
            actions.out << "</informaltable>\n";
        }
    }

    void begin_section_action(quickbook::actions& actions, value begin_section_list)
    {    
        value_consumer values = begin_section_list;

        value element_id = values.optional_consume(general_tags::element_id);
        value content = values.consume();
        values.finish();

        std::string full_id = actions.ids.begin_section(
            !element_id.empty() ?
                element_id.get_quickbook() :
                detail::make_identifier(content.get_quickbook()),
            !element_id.empty() ?
                id_category::explicit_section_id :
                id_category::generated_section);

        actions.out << "\n<section id=\"" << full_id << "\">\n";
        actions.out << "<title>";

        write_anchors(actions, actions.out);

        if (self_linked_headers && actions.ids.compatibility_version() >= 103)
        {
            actions.out << "<link linkend=\"" << full_id << "\">"
                << content.get_encoded()
                << "</link>"
                ;
        }
        else
        {
            actions.out << content.get_encoded();
        }
        
        actions.out << "</title>\n";
    }

    void end_section_action(quickbook::actions& actions, value end_section, string_iterator first)
    {
        write_anchors(actions, actions.out);

        if (actions.ids.section_level() <= actions.min_section_level)
        {
            file_position const pos = actions.current_file->position_of(first);

            detail::outerr(actions.current_file->path, pos.line)
                << "Mismatched [endsect] near column " << pos.column << ".\n";
            ++actions.error_count;
            
            return;
        }

        actions.out << "</section>";
        actions.ids.end_section();
    }
    
    void element_id_warning_action::operator()(parse_iterator first, parse_iterator) const
    {
        detail::outwarn(actions.current_file, first.base()) << "Empty id.\n";
    }

    // Not a general purpose normalization function, just
    // from paths from the root directory. It strips the excess
    // ".." parts from a path like: "x/../../y", leaving "y".
    std::vector<fs::path> normalize_path_from_root(fs::path const& path)
    {
        assert(!path.has_root_directory() && !path.has_root_name());
    
        std::vector<fs::path> parts;

        BOOST_FOREACH(fs::path const& part, path)
        {
            if (part.empty() || part == ".") {
            }
            else if (part == "..") {
                if (!parts.empty()) parts.pop_back();
            }
            else {
                parts.push_back(part);
            }
        }
        
        return parts;
    }

    // The relative path from base to path
    fs::path path_difference(fs::path const& base, fs::path const& path)
    {
        fs::path
            absolute_base = fs::absolute(base),
            absolute_path = fs::absolute(path);

        // Remove '.', '..' and empty parts from the remaining path
        std::vector<fs::path>
            base_parts = normalize_path_from_root(absolute_base.relative_path()),
            path_parts = normalize_path_from_root(absolute_path.relative_path());

        std::vector<fs::path>::iterator
            base_it = base_parts.begin(),
            base_end = base_parts.end(),
            path_it = path_parts.begin(),
            path_end = path_parts.end();

        // Build up the two paths in these variables, checking for the first
        // difference.
        fs::path
            base_tmp = absolute_base.root_path(),
            path_tmp = absolute_path.root_path();

        fs::path result;

        // If they have different roots then there's no relative path so
        // just build an absolute path.
        if (!fs::equivalent(base_tmp, path_tmp))
        {
            result = path_tmp;
        }
        else
        {
            // Find the point at which the paths differ    
            for(; base_it != base_end && path_it != path_end; ++base_it, ++path_it)
            {
                if(!fs::equivalent(base_tmp /= *base_it, path_tmp /= *path_it))
                    break;
            }
    
            // Build a relative path to that point
            for(; base_it != base_end; ++base_it) result /= "..";
        }

        // Build the rest of our path
        for(; path_it != path_end; ++path_it) result /= *path_it;

        return result;
    }

    fs::path check_path(value const& path, quickbook::actions& actions)
    {
        std::string path_text = path.is_encoded() ? path.get_encoded() :
            path.get_quickbook();

        if(path_text.find('\\') != std::string::npos)
        {
            (qbk_version_n >= 106u ?
                detail::outerr(path.get_file(), path.get_position()) :
                detail::outwarn(path.get_file(), path.get_position()))
                << "Path isn't portable: '"
                << detail::utf8(path_text)
                << "'"
                << std::endl;
            if (qbk_version_n >= 106u) ++actions.error_count;
        }
        
        boost::replace(path_text, '\\', '/');
        
        return detail::generic_to_path(path_text);
    }

    xinclude_path calculate_xinclude_path(value const& p, quickbook::actions& actions)
    {
        fs::path path = check_path(p, actions);
        fs::path full_path = path;

        // If the path is relative
        if (!path.has_root_directory())
        {
            // Resolve the path from the current file
            full_path = actions.current_file->path.parent_path() / path;

            // Then calculate relative to the current xinclude_base.
            path = path_difference(actions.xinclude_base, full_path);
        }

        return xinclude_path(full_path, detail::escape_uri(detail::path_to_generic(path)));
    }

    void xinclude_action(quickbook::actions& actions, value xinclude)
    {
        write_anchors(actions, actions.out);

        value_consumer values = xinclude;
        xinclude_path x = calculate_xinclude_path(values.consume(), actions);
        values.finish();

        actions.out << "\n<xi:include href=\"";
        detail::print_string(x.uri, actions.out.get());
        actions.out << "\" />\n";
    }

    namespace
    {
        struct include_search_return
        {
            include_search_return(fs::path const& x, fs::path const& y)
                : filename(x), filename_relative(y) {}

            fs::path filename;
            fs::path filename_relative;
        };

        include_search_return include_search(fs::path const& path,
                quickbook::actions const& actions)
        {
            fs::path current = actions.current_file->path.parent_path();

            // If the path is relative, try and resolve it.
            if (!path.has_root_directory() && !path.has_root_name())
            {
                // See if it can be found locally first.
                if (fs::exists(current / path))
                {
                    return include_search_return(
                        current / path,
                        actions.filename_relative.parent_path() / path);
                }

                // Search in each of the include path locations.
                BOOST_FOREACH(fs::path full, include_path)
                {
                    full /= path;
                    if (fs::exists(full))
                    {
                        return include_search_return(full, path);
                    }
                }
            }

            return include_search_return(path,
                actions.filename_relative.parent_path() / path);
        }
    }
    
    void load_quickbook(quickbook::actions& actions,
            include_search_return const& paths,
            value::tag_type load_type,
            value const& include_doc_id = value())
    {
        assert(load_type == block_tags::include ||
            load_type == block_tags::import);

        // Check this before qbk_version_n gets changed by the inner file.
        bool keep_inner_source_mode = (qbk_version_n < 106);
        
        {
            // When importing, state doesn't scope templates and macros so that
            // they're added to the existing scope. It might be better to add
            // them to a new scope then explicitly import them into the
            // existing scope.
            //
            // For old versions of quickbook, templates aren't scoped by the
            // file.
            file_state state(actions,
                load_type == block_tags::import ? file_state::scope_output :
                qbk_version_n >= 106u ? file_state::scope_callables :
                file_state::scope_macros);

            actions.current_file = load(paths.filename); // Throws load_error
            actions.filename_relative = paths.filename_relative;
            actions.imported = (load_type == block_tags::import);

            // update the __FILENAME__ macro
            *boost::spirit::classic::find(actions.macro, "__FILENAME__")
                = detail::path_to_generic(actions.filename_relative);
        
            // parse the file
            quickbook::parse_file(actions, include_doc_id, true);

            // Don't restore source_mode on older versions.
            if (keep_inner_source_mode) state.source_mode = actions.source_mode;
        }

        // restore the __FILENAME__ macro
        *boost::spirit::classic::find(actions.macro, "__FILENAME__")
            = detail::path_to_generic(actions.filename_relative);
    }

    void load_source_file(quickbook::actions& actions,
            include_search_return const& paths,
            value::tag_type load_type,
            string_iterator first,
            value const& include_doc_id = value())
    {
        assert(load_type == block_tags::include ||
            load_type == block_tags::import);

        std::string ext = paths.filename.extension().generic_string();
        std::vector<template_symbol> storage;
        // Throws load_error
        actions.error_count +=
            load_snippets(paths.filename, storage, ext, load_type);

        if (load_type == block_tags::include)
        {
            actions.templates.push();
        }

        BOOST_FOREACH(template_symbol& ts, storage)
        {
            std::string tname = ts.identifier;
            if (tname != "!")
            {
                ts.lexical_parent = &actions.templates.top_scope();
                if (!actions.templates.add(ts))
                {
                    detail::outerr(ts.content.get_file(), ts.content.get_position())
                        << "Template Redefinition: " << detail::utf8(tname) << std::endl;
                    ++actions.error_count;
                }
            }
        }

        if (load_type == block_tags::include)
        {
            BOOST_FOREACH(template_symbol& ts, storage)
            {
                std::string tname = ts.identifier;

                if (tname == "!")
                {
                    ts.lexical_parent = &actions.templates.top_scope();
                    call_code_snippet(actions, &ts, first);
                }
            }

            actions.templates.pop();
        }
    }

    void include_action(quickbook::actions& actions, value include, string_iterator first)
    {
        write_anchors(actions, actions.out);

        value_consumer values = include;
        value include_doc_id = values.optional_consume(general_tags::include_id);
        include_search_return paths = include_search(
            check_path(values.consume(), actions), actions);
        values.finish();

        try {
            if (qbk_version_n >= 106)
            {
                if (actions.imported && include.get_tag() == block_tags::include)
                    return;

                std::string ext = paths.filename.extension().generic_string();
                
                if (ext == ".qbk" || ext == ".quickbook")
                {
                    load_quickbook(actions, paths, include.get_tag(), include_doc_id);
                }
                else
                {
                    load_source_file(actions, paths, include.get_tag(), first, include_doc_id);
                }
            }
            else
            {
                if (include.get_tag() == block_tags::include)
                {
                    load_quickbook(actions, paths, include.get_tag(), include_doc_id);
                }
                else
                {
                    load_source_file(actions, paths, include.get_tag(), first, include_doc_id);
                }
            }
        }
        catch (load_error& e) {
            ++actions.error_count;

            detail::outerr(actions.current_file, first)
                << "Loading file "
                << paths.filename
                << ": "
                << detail::utf8(e.what())
                << std::endl;
        }
    }

    bool to_value_scoped_action::start(value::tag_type t)
    {
        actions.out.push();
        actions.phrase.push();
        actions.anchors.swap(saved_anchors);
        tag = t;

        return true;
    }

    void to_value_scoped_action::success(parse_iterator first, parse_iterator last)
    {
        std::string value;

        if (!actions.out.str().empty())
        {
            actions.paragraph();
            write_anchors(actions, actions.out);
            actions.out.swap(value);
        }
        else
        {
            write_anchors(actions, actions.phrase);
            actions.phrase.swap(value);
        }

        actions.values.builder.insert(encoded_qbk_value(
            actions.current_file, first.base(), last.base(), value, tag));
    }
    
    
    void to_value_scoped_action::cleanup()
    {
        actions.phrase.pop();
        actions.out.pop();
        actions.anchors.swap(saved_anchors);
    }
}
