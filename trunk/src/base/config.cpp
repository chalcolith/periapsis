//
// $Id: config.cpp 28 2008-11-12 01:41:40Z kulibali $
//
// Copyright (c) 2008-2010, The Periapsis Project. All rights reserved. 
// 
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions are 
// met: 
// 
// * Redistributions of source code must retain the above copyright notice, 
//   this list of conditions and the following disclaimer. 
// 
// * Redistributions in binary form must reproduce the above copyright 
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the distribution. 
// 
// * Neither the name of the The Periapsis Project nor the names of its 
//   contributors may be used to endorse or promote products derived from 
//   this software without specific prior written permission. 
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS 
// IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED 
// TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
// PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER 
// OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, 
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR 
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
// LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
// NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS 
// SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//

#include "data/config.hpp"
#include "data/directory.hpp"
#include "data/fstream.hpp"

#include <cwctype>

namespace gsgl
{

    namespace data
    {
    
        config_record::config_record()
            : data_object(), parent(0), associated_var(0), f(0), line_number(0)
        {
        } // config_record::config_record()


        config_record::config_record(const config_record & conf)
            : data_object(), parent(conf.parent), associated_var(conf.associated_var), f(conf.f), line_number(conf.line_number)
        {
            children = conf.children;
            name = conf.name;
            text = conf.text;
            attributes = conf.attributes;
        } // config_record::config_record()

        
        config_record::config_record(const string & fname)
            : data_object(), parent(0), associated_var(0), f(new io::file(fname))
        {
            if (io::file::exists(f->get_full_path()))
            {
                smart_pointer<io::ft_stream> s(f->open_text());
                from_stream(*s);
            }
            else
            {
                throw runtime_exception(L"Trying to open non-existent configuration file: %ls", fname.w_string());
            }
        }  // config_record::config_record()


        config_record & config_record::operator= (const config_record & conf)
        {
            parent = conf.parent;
            children = conf.children;
            name = conf.name;
            text = conf.text;
            attributes = conf.attributes;
            associated_var = conf.associated_var;
            f = conf.f;
            line_number = conf.line_number;

            return *this;
        } // config_record::operator= ()

                
        config_record::~config_record()
        {
        } // config_record::~config_record()
        

        bool config_record::operator== (const config_record & conf) const
        {
            return parent == conf.parent
                && children == conf.children
                && name == conf.name
                && text == conf.text
                && attributes == conf.attributes
                && f == conf.f
                && line_number == conf.line_number;
        } // config_record::operator== ()


        //
        
        const io::file & config_record::get_file() const
        {
            return *f;
        } // config_record::get_file()


        const int config_record::get_line_number() const
        {
            return line_number;
        } // config_record::get_line_number()
        

        const io::directory & config_record::get_directory() const
        {
            return f->get_directory();
        } // io::directory()
        

        const string & config_record::get_name() const
        {
            return name;
        } // config_record::get_name()
        

        string & config_record::get_name()
        {
            return name;
        } // config_record::get_name()
        

        const string & config_record::get_text() const
        {
            return text;
        } // config_record::get_text()
        

        string & config_record::get_text()
        {
            return text;
        } // config_record::get_text()
        

        //
        
        const string & config_record::get_attribute(const string & a) const
        {
            return attributes[a];
        } // config_record::get_attribute()
        

        string & config_record::get_attribute(const string & a)
        {
            return attributes[a];
        } // config_record::get_attribute()


        const dictionary<gsgl::string, gsgl::string> & config_record::get_attributes() const
        {
            return attributes;
        } // config_record::get_attributes()


        dictionary<gsgl::string, gsgl::string> & config_record::get_attributes()
        {
            return attributes;
        } // config_record::get_attributes()
        

        //

        config_record *config_record::find_child(gsgl::data::list<gsgl::string>::iterator & pos_in_path, bool create)
        {
            assert(pos_in_path.is_valid());

            config_record *child = 0;
            string child_name = *pos_in_path;
            child_name.trim();

            // find child at pos in path
            for (list<config_record>::iterator i = children.iter(); i.is_valid(); ++i)
            {
                if (i->name == child_name)
                {
                    child = &*i;
                    break;
                }
            }

            // if not found, create if asked
            if (!child)
            {
                if (create)
                {
                    children.append(config_record());
                    child = &children.get_tail();
                    child->parent = this;
                    child->name = *pos_in_path;
                    child->name.trim();
                }
                else
                {
                    return 0;
                }
            }

            // recurse
            if ((++pos_in_path).is_valid())
                return child->find_child(pos_in_path, create);
            else
                return child;
        } // config_record::find_child()


        bool config_record::contains_child(const string & path) const
        {
            list<string> path_list = path.split(L"/");
            list<string>::iterator pos = path_list.iter();

            return pos.is_valid() && const_cast<config_record *>(this)->find_child(pos, false);
        } // config_record::contains_child()


        const config_record & config_record::get_child(const string & path) const
        {
            list<string> path_list = path.split(L"/");
            list<string>::iterator pos = path_list.iter();

            if (pos.is_valid())
            {
                return *const_cast<config_record *>(this)->find_child(pos, false);
            }
            else
            {
                throw runtime_exception(L"Invalid config path '%ls'", path.w_string());
            }
        } // config_record::get_child()


        config_record & config_record::get_child(const string & path)
        {
            list<string> path_list = path.split(L"/");
            list<string>::iterator pos = path_list.iter();

            if (pos.is_valid())
            {
                return *find_child(pos, true);
            }
            else
            {
                throw runtime_exception(L"Invalid config path '%ls'", path.w_string());
            }
        } // config_record::get_child()

        
        const list<config_record> & config_record::get_children() const
        {
            return children;
        } // config_record::get_children()
        

        list<config_record> & config_record::get_children()
        {
            return children;
        } // config_record::get_children()


        //

        void config_record::override_with(const config_record & cr)
        {
            // the name must be the same; overwrite the text
            if (name != cr.name)
                throw runtime_exception(L"You cannot override a record named '%ls' with one named '%ls'!", name.w_string(), cr.name.w_string());

            text = cr.text;

            // override attributes
            for (dictionary<string, string>::const_iterator cr_att = cr.attributes.iter(); cr_att.is_valid(); ++cr_att)
            {
                attributes[cr_att.get_index()] = *cr_att;
            }

            // override children
            dictionary<int, string> num_children_assigned;

            for (list<config_record>::const_iterator cr_child = cr.children.iter(); cr_child.is_valid(); ++cr_child)
            {
                // find the next child with this name
                int num_to_skip = num_children_assigned[cr_child->name];
                int num_seen = 0;
                list<config_record>::iterator my_child = children.iter();
                for (; my_child.is_valid(); ++my_child)
                {
                    if (my_child->name == cr_child->name)
                    {
                        if (num_seen == num_to_skip)
                        {
                            my_child->override_with(*cr_child);
                            ++num_children_assigned[cr_child->name];
                            break;
                        }
                        
                        ++num_seen;
                    }
                }

                // add if not found
                if (!my_child.is_valid())
                {
                    children.append(config_record());
                    children.get_tail().parent = this;
                    children.get_tail().name = cr_child->name;
                    children.get_tail().override_with(*cr_child);
                }
            }

            // override config variable, if present
            if (associated_var)
            {
                associated_var->assign_from_string(text);
            }
        } // config_record::override_with()


        void config_record::save()
        {
            if (parent)
                parent->save();

            if (f.ptr())
            {
                smart_pointer<io::ft_stream> s(f->open_text(io::FILE_OPEN_WRITE));
                to_stream(*s);
            }
            else
            {
                throw runtime_exception(L"You cannot save a config record that has no associated file.");
            }
        } // config_record::save()


        //

        static string quoted(const string & str)
        {
            string res;
            for (int i = 0; i < str.size(); ++i)
            {
                res.append(str[i]);
                if (str[i] == L'\\')
                    res.append(L'\\');
            }
            return res;
        } // quoted()
        

        void config_record::to_stream(io::text_stream & s) const
        {
            to_stream(s, 0);
        } // config_record::to_stream()


        void config_record::to_stream(io::text_stream & s, int indent) const
        {
            for (int i = 0; i < indent; ++i)
                s << L"  ";
                
            if (children.size() || text.size())
            {
                s << L"<" << name;

				for (dictionary<string,string>::const_iterator i = attributes.iter(); i.is_valid(); ++i)
                    s << L" " << i.get_index() << L"=\"" << quoted(*i) << L"\"";
                
                s << L">";
                
                if (text.size() > 32)
                {
                    s << L"\n";
                    for (int i = 0; i < indent+1; ++i)
                        s << L"  ";
                }
                
                s << text;
                
                if (children.size() || text.size() > 32)
                    s << L"\n";
                    
				for (list<config_record>::const_iterator child = children.iter(); child.is_valid(); ++child)
                {
                    child->to_stream(s, indent+1);
                }
                
                if (children.size() || text.size() > 32)
                    for (int i = 0; i < indent; ++i)
                        s << L"  ";
                s << L"</" << name << L">\n";
            }
            else
            {
                s << L"<" << name;
                
				for (dictionary<string, string>::const_iterator i = attributes.iter(); i.is_valid(); ++i)
                    s << L" " << i.get_index() << L"=\"" << quoted(*i) << L"\"";

                s << L"/>\n";
            }
        } // config_record::to_stream()
        
        //
        
        static wchar_t get_char(io::text_stream & s, int & line)
        {
            wchar_t ch = s.get();

            if (ch == L'\n')
            {
                ++line;                
            }
            else if (ch == L'\r')
            {
                ++line;
                if (s.peek() == L'\n')
                    ch = s.get();
            }

            return ch;
        } // get_char()

        void config_record::from_stream(io::text_stream & s)
        {
            int line = 1;
            from_stream(s, line);
        } // config_record::from_stream()


#define SYNTAX_ERROR(msg) throw runtime_exception(L"%ls (%d): syntax error: %ls", f->get_full_path().w_string(), line, msg)
#define CHECK_EOF if (ch == WEOF) throw runtime_exception(L"%ls (%d): unexpected end of file", f->get_full_path().w_string(), line)

        void config_record::from_stream(io::text_stream & s, int & line)
        {
            simple_array<wchar_t> buf(32);
            wchar_t ch;

            // read header
            ch = get_char(s, line);

            while (::iswspace(ch))
                ch = get_char(s, line);

            if (ch == WEOF)
                return;

            if (ch != L'<')
                SYNTAX_ERROR(L"missing '<'");

            line_number = line;

            // read name
            buf.clear();

            ch = get_char(s, line);

            while (::iswalnum(ch) || ch == L'_' || ch == L':')
            {
                buf.append(ch);
                ch = get_char(s, line);
            }

            buf.append(0);
            name = buf.ptr();
            name.trim();

            CHECK_EOF;
            if (name.is_empty())
                SYNTAX_ERROR(L"empty element name");

            // read attributes, if any
            while (::iswspace(ch))
            {
                string att_name, att_val;

                // name
                buf.clear();

                ch = get_char(s, line);
                while (::iswspace(ch))
                {
                    ch = get_char(s, line);
                    CHECK_EOF;
                }

                if (ch == L'/' || ch == L'>')
                    break;

                while (::iswalnum(ch) || ch == L'_')
                {
                    buf.append(ch);
                    ch = get_char(s, line);
                    CHECK_EOF;
                }

                buf.append(0);
                att_name = buf.ptr();
                att_name.trim();

                if (att_name.size() == 0)
                    SYNTAX_ERROR(L"missing attribute name");
                if (ch != L'=')
                    SYNTAX_ERROR(L"missing '='");

                ch = get_char(s, line);
                if (ch != L'"')
                    SYNTAX_ERROR(L"missing '\"'");

                // value
                buf.clear();

                ch = get_char(s, line);
                while (ch != L'"')
                {
                    if (ch == L'\\')
                    {
                        ch = get_char(s, line);
                        if (ch == L'\n')
                            ch = L' ';
                    }

                    buf.append(ch);
                    ch = get_char(s, line);
                    CHECK_EOF;
                }

                buf.append(0);
                att_val = buf.ptr();
                att_val.trim();

                attributes[att_name] = att_val;

                // next
                ch = get_char(s, line);
                CHECK_EOF;
            }

            bool no_body = false;
            if ((no_body = ch == L'/'))
                ch = get_char(s, line);

            if (ch != L'>')
                SYNTAX_ERROR(L"missing '>'");

            if (no_body)
                return;

            // read body
            bool in_space = false;
            while (true)
            {
                ch = s.peek();
                CHECK_EOF;

                if (ch == L'<')
                {
                    wchar_t brak = get_char(s, line);

                    ch = s.peek();
                    CHECK_EOF;

                    if (ch == L'/') // read end tag
                    {
                        ch = get_char(s, line);
                        CHECK_EOF;
                    
                        buf.clear();

                        ch = get_char(s, line);
                        CHECK_EOF;

                        while (ch != L'>')
                        {
                            buf.append(ch);

                            ch = get_char(s, line);
                            CHECK_EOF;
                        }

                        buf.append(0);
                        string end_name = buf.ptr();
                        end_name.trim();

                        if (this->name != end_name)
                            SYNTAX_ERROR(L"mismatched end tag");
                        break;
                    }
                    else if (ch == L'!')
                    {
                        get_char(s, line);

                        // handle comment
                        ch = get_char(s, line);
                        CHECK_EOF;
                        if (ch != L'-')
                            SYNTAX_ERROR(L"missing '-' in comment");

                        ch = get_char(s, line);
                        CHECK_EOF;
                        if (ch != L'-')
                            SYNTAX_ERROR(L"missing '-' in comment");

                        int dash_count = 0;

                        do
                        {
                            ch = get_char(s, line);
                            CHECK_EOF;

                            if (ch == L'-')
                                ++dash_count;
                            else if (ch != L'>')
                                dash_count = 0;
                        }
                        while (!(ch == L'>' && dash_count >= 2));
                    }
                    else
                    {
                        s.unget(brak);

                        children.append(config_record());                        
                        config_record & child = children.get_tail();
                        child.parent = this;
                        child.f = f;
                        child.from_stream(s, line);

                        continue;
                    }
                }
                else if (::iswspace(ch))
                {
                    in_space = true;
                    get_char(s, line);
                    CHECK_EOF;
                }
                else
                {
                    CHECK_EOF;

                    if (in_space)
                    {
                        this->text.append(L' ');
                        in_space = false;
                    }

                    this->text.append(get_char(s, line));
                }
            }

            this->text.trim();
        } // config_record::from_stream()
        

        //

        config_record global_config::global_config_vars;


        global_config::global_config(const gsgl::string & path)
            : associated_record(global_config_vars.get_child(path))
        {
            assert(!associated_record.associated_var);
            associated_record.associated_var = this;
        } // global_config::global_config()


        global_config::~global_config()
        {
            // what if the global record is freed first?
            // associated_record.associated_var = 0;
        } // global_config::~global_config()


        void global_config::override_with(const config_record & cr)
        {
            global_config_vars.override_with(cr);
        } // global_config::override_with()

        
        void global_config::save(const string & fname)
        {
            config_record temp(fname);
            temp.override_with(global_config_vars);
            temp.save();
        } // global_config::save()

    } // namespace data
    
} // namespace gsgl
