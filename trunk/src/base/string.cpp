//
// $Id: string.cpp 15 2008-07-20 17:36:39Z kulibali $
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

#include "stdafx.h"
#include "string.hpp"
#include "pointer.hpp"
#include "array.hpp"
#include "list.hpp"
#include "stream.hpp"

namespace gsgl
{

    class string_impl
        : public data::shared_object
    {
        enum
        {
            STRING_IMPL_OLD_CDATA = 1 << 0,
            STRING_IMPL_OLD_PDATA = 1 << 1,
            STRING_IMPL_MODIFIED  = ~0
        };

        mutable gsgl::flags_t modified_flags;

        data::simple_array<wchar_t>               w_data; ///< Null-terminated wchar_t string.
        mutable data::simple_array<char>          c_data; ///< 8-bit null-terminated equivalent (in UTF-8).
        mutable data::simple_array<unsigned char> p_data; ///< 8-bit null-indexed (Pascal) equivalent (in UTF-8).

        friend class gsgl::string;

    public:
        string_impl();
        string_impl(const string_impl &);
        explicit string_impl(const wchar_t *);

        string_impl & operator= (const string_impl &);
        
        virtual ~string_impl();

        //

        bool is_modified() const { return modified_flags != 0; }
        void set_modified() { set_flags(modified_flags, STRING_IMPL_MODIFIED); }
        
        inline const wchar_t *get_w_string() const { return w_data.ptr(); }
        const char           *get_c_string() const;
        const unsigned char  *get_p_string() const;
    }; // class string_impl


    string_impl::string_impl()
        : shared_object(), modified_flags(STRING_IMPL_MODIFIED)
    {
        w_data.append(0);
    } // string_impl::string_impl()


    string_impl::string_impl(const string_impl & si)
        : shared_object(), modified_flags(STRING_IMPL_MODIFIED)
    {
        w_data = si.w_data;
    } // string_impl::string_impl()


    string_impl::string_impl(const wchar_t *str)
        : shared_object(), modified_flags(STRING_IMPL_MODIFIED)
    {
        if (str)
        {
            size_t len = ::wcslen(str);
            w_data[static_cast<gsgl::index_t>(len)] = 0;
            ::memcpy(w_data.ptr(), str, sizeof(wchar_t) * len);
        }
        else
        {
            w_data.append(0);
        }
    } // string_impl::string_impl()


    string_impl::~string_impl()
    {
    } // string_impl::string_impl()


    /// \todo Implement UFT-8 output.
    const char *string_impl::get_c_string() const
    {
        if (flag_is_set(modified_flags, STRING_IMPL_OLD_CDATA))
        {
            c_data.clear();
            for (const wchar_t *ch = w_data.ptr(); *ch; ++ch)
                c_data.append(static_cast<char>(*ch));
            c_data.append(0);

            unset_flags(modified_flags, STRING_IMPL_OLD_CDATA);
        }

        return c_data.ptr();
    } // string_impl::get_c_string()


    /// \todo Implement UTF-8 output.
    const unsigned char *string_impl::get_p_string() const
    {
        if (modified_flags & STRING_IMPL_OLD_PDATA)
        {
            p_data.clear();
            p_data.append(0);

            int count = 0;

            for (const wchar_t *ch = w_data.ptr(); *ch; ++ch)
            {
                if (count++ < 255)
                    p_data.append(static_cast<char>(*ch));
                else
                    break;
            }

            p_data[0] = static_cast<char>(count);
            modified_flags &= ~STRING_IMPL_OLD_PDATA;
        }

        return p_data.ptr();
    } // string_impl::get_p_string()


    //////////////////////////////////////////////////////////////////

    const string string::EMPTY_STRING = L"";

    string::string()
        : data_object(), 
          comparable(),
          iterable<wchar_t, string_iterator>(), 
          indexable<wchar_t, gsgl::index_t>(),
          io::printable(),
          io::serializable(),
          mode(STRING_NULL),
          ref(0)
    {
        on_update();
    } // string::string()


    string::string(const string & s)
        : data_object(), 
          comparable(),
          iterable<wchar_t, string_iterator>(), 
          indexable<wchar_t, gsgl::index_t>(),
          io::printable(),
          io::serializable(),
          mode(STRING_NULL),
          ref(0)
    {
        *this = s;
    } // string::string()


    string::string(wchar_t *str)
        : data_object(),
          comparable(),
          iterable<wchar_t, string_iterator>(),
          indexable<wchar_t, gsgl::index_t>(),
          io::printable(),
          io::serializable(),
          mode(str ? STRING_CONST_REF : STRING_NULL),
          ref(str)
    {
        unshare(); // copies the referenced string
        on_update();
    } // string::string()


    string::string(const wchar_t *str)
        : data_object(),
          comparable(),
          iterable<wchar_t, string_iterator>(),
          indexable<wchar_t, gsgl::index_t>(),
          io::printable(),
          io::serializable(),
          mode(str ? STRING_CONST_REF : STRING_NULL),
          ref(str)
    {
        on_update();
    } // string::string()


    /// \todo Implement UTF-8 import.
    string::string(const char *str)
        : data_object(),
          comparable(),
          iterable<wchar_t, string_iterator>(),
          indexable<wchar_t, gsgl::index_t>(),
          io::printable(),
          io::serializable(),
          mode(STRING_NULL),
          ref(0)
    {
        if (str)
        {
			data::simple_array<wchar_t> buf;
            for (const char *ch = str; *ch; ++ch)
                buf.append(static_cast<wchar_t>(*ch));
            buf.append(0);

            mode = STRING_SHARED_IMPL;
            impl = new string_impl(buf.ptr());
            impl->attach();
        }

        on_update();
    } // string::string()


    string & string::operator= (wchar_t *str)
    {
        make_null();

        mode = str ? STRING_CONST_REF : STRING_NULL;
        ref = str;

        unshare();
        on_update();

        return *this;
    }; // string::operator= ()


    string & string::operator= (const wchar_t *str)
    {
        make_null();

        mode = str ? STRING_CONST_REF : STRING_NULL;
        ref = str;

        on_update();

        return *this;
    } // string::operator= ()


    string & string::operator= (const string & s)
    {
        make_null();

        switch (s.mode)
        {
        case STRING_NULL:
            break;
        case STRING_CONST_REF:
            assert(s.ref);
            mode = STRING_CONST_REF;
            ref = s.ref;
            break;
        case STRING_SHARED_IMPL:
            assert(s.impl);
            mode = STRING_SHARED_IMPL;
            impl = s.impl;
            impl->attach();
            break;
        default:
            throw internal_exception(__FILE__, __LINE__, L"unknown error in string::operator= ()");
        }

        on_update();

        return *this;
    } // string::operator= ()


    string::~string()
    {
        make_null();
        mode = STRING_INVALID;
    } // string::~string()


    ////////////////////////////////////

    gsgl::index_t string::size() const
    {
        switch (mode)
        {
        case STRING_NULL:
            return 0;
        case STRING_CONST_REF:
            return static_cast<gsgl::index_t>(::wcslen(ref));
        case STRING_SHARED_IMPL:
            assert(impl);
            return impl->w_data.size() - 1;
        default:
            throw internal_exception(__FILE__, __LINE__, L"can't happen in string::size()");
        }
    } // string::size()


    void string::clear()
    {
        switch (mode)
        {
        case STRING_NULL:
        case STRING_CONST_REF:
            make_null();
            break;
        case STRING_SHARED_IMPL:
            unshare();
            impl->set_modified();
            impl->w_data.clear();
            impl->w_data.append(0);
            break;
        default:
            throw internal_exception(__FILE__, __LINE__, L"can't happen in string::clear()");
        }

        on_update();
    } // string::clear()


    ////////////////////////////////////

    void string::append(const wchar_t & ch)
    {
        unshare();
        impl->set_modified();
        impl->w_data[impl->w_data.size()-1] = ch;
        impl->w_data.append(0);
        on_update();
    } // string::append()


    void string::append(const wchar_t *str)
    {
        if (str && *str)
        {
            unshare();
            impl->set_modified();

            size_t la = size();
            size_t lb = ::wcslen(str);
            impl->w_data[static_cast<gsgl::index_t>(la + lb)] = 0;        // force resize
            ::memcpy(impl->w_data.ptr() + la, str, sizeof(wchar_t) * lb); // copy into available space
        }
        on_update();
    } // string::append()


    void string::append(const string & str)
    {
        append(str.w_string());
    } // string::append()


    void string::insert(const iterator & i, const wchar_t & ch)
    {
        assert(impl);

        unshare();
        impl->set_modified();
        impl->w_data.insert(ch, i.position);

        on_update();
    } // string::insert()


    void string::remove(const iterator & i)
    {
        assert(impl);

        unshare();
        impl->set_modified();
        impl->w_data.remove(i.position);

        on_update();
    } // string::remove()


    ////////////////////////////////////

    const wchar_t & string::item(const gsgl::index_t & index) const
    {
        switch (mode)
        {
        case STRING_NULL:
            throw memory_exception(__FILE__, __LINE__, L"bounds error in string.");
        case STRING_CONST_REF:
            if (index < static_cast<gsgl::index_t>(::wcslen(ref)))
                return ref[index];
            else
                throw memory_exception(__FILE__, __LINE__, L"bounds error in string");
        case STRING_SHARED_IMPL:
            try
            {
                return impl->w_data.item(index);
            }
            catch (memory_exception &)
            {
                throw memory_exception(__FILE__, __LINE__, L"bounds error in string");
            }
        default:
            throw internal_exception(__FILE__, __LINE__, L"can't happen in string::item()");
        }
    } // string::item()

    
    wchar_t & string::item(const gsgl::index_t & index)
    {
        unshare();
        impl->set_modified();

        if (index < size())
            return impl->w_data.item(index);
        else
            throw memory_exception(__FILE__, __LINE__, L"bounds error in string");
    } // string::item()


    bool string::contains_index(const gsgl::index_t & index) const
    {
        return index < size();
    } // string::contains_index()


    ////////////////////////////////////

    int string::compare(const comparable & c) const
    {
        const string *cp = dynamic_cast<const string *>(&c);
        if (cp)
        {
            return compare(cp->w_string());
        }
        else
        {
            throw internal_exception(__FILE__, __LINE__, L"attempted to compare string with non-string");
        }
    } // string::compare()


    ////////////////////////////////////

	void string::to_stream(io::text_stream & s) const
    {
        s << w_string();
    } // string::to_stream()


	void string::from_stream(io::text_stream & s)
    {
        clear();

        for (wchar_t ch = s.get(); ch != WEOF; ch = s.get())
        {
            if (ch == L'\r')
            {
                ch = L'\n';
                if (s.peek() == L'\n')
                    s.get();
            }

            if (ch == L'\n')
                break;
            else
                append(ch);
        }

        on_update();
    } // string::from_stream()


    ////////////////////////////////////

	void string::to_stream(io::data_stream & s) const
    {
        gsgl::index_t sz = size();
        s << sz;
        s.write(reinterpret_cast<const unsigned char *>(w_string()), sizeof(wchar_t) * sz);
    }; // string::to_stream()


	void string::from_stream(io::data_stream & s)
    {
        gsgl::index_t sz;
        s >> sz;
		data::smart_pointer<wchar_t, true> buf(new wchar_t[sz+1]);
        s.read(reinterpret_cast<unsigned char *>(buf.ptr()), sizeof(wchar_t) * sz);
        buf.ptr()[sz] = 0;

        *this = buf.ptr();

        on_update();
    }; // string::from_stream()


    ////////////////////////////////////

    static const wchar_t *NULL_STRING_LITERAL = L"";

    const wchar_t *string::w_string() const
    {
        switch (mode)
        {
        case STRING_NULL:
            return NULL_STRING_LITERAL;
        case STRING_CONST_REF:
            return ref;
        case STRING_SHARED_IMPL:
            assert(impl);
            return impl->get_w_string();
        default:
            throw internal_exception(__FILE__, __LINE__, L"can't happen in string::w_string()");
        }
    } // string::w_string()


    const char *string::c_string() const
    {
        unshare();
        return impl->get_c_string();
    } // string::c_string()


    const unsigned char *string::p_string() const
    {
        unshare();
        return impl->get_p_string();
    } // string::p_string()


    ////////////////////////////////////

    string & string::operator+= (const wchar_t & ch)
    {
        append(ch);
        return *this;
    } // string::operator+= ()


    string & string::operator+= (const wchar_t *str)
    {
        append(str);
        return *this;
    } // string::operator+= ()


    string & string::operator+= (const string & str)
    {
        append(str.w_string());
        return *this;
    } // string::operator+= ()


    string string::operator+ (const wchar_t & ch) const
    {
        string res(*this);
        res.append(ch);
        return res;
    } // string::operator+ ()


    string string::operator+ (const wchar_t *str) const
    {
        string res(*this);
        res.append(str);
        return res;
    } // string::operator+ ()


    string string::operator+ (const string & str) const
    {
        string res(*this);
        res.append(str.w_string());
        return res;
    } // string::operator+ ()


    ////////////////////////////////////

    string string::substring(const gsgl::index_t index, const gsgl::index_t len) const
    {
        const gsgl::index_t sz = size();
        gsgl::index_t length = len;
        string res;

        if (length == 0 || index >= sz)
            return res;
        if (length == -1 || index + length > sz)
            length = sz - index;

        res.unshare();
        res.impl->set_modified();
        res.impl->w_data[length] = 0;
        ::memcpy(res.impl->w_data.ptr(), w_string() + index, sizeof(wchar_t) * length);

        return res;
    } // string::substring()


    string string::left_substring(const gsgl::index_t length) const
    {
        return substring(0, length);
    } // string::left_substring()


    string string::right_substring(const gsgl::index_t length) const
    {
        if (length == 0)
            return string();

        const gsgl::index_t sz = size();

        if (length >= sz)
            return string(*this);

        return substring(sz - length);
    } // string::right_substring()


    gsgl::index_t string::find(const wchar_t *substr, const gsgl::index_t index) const
    {
        const wchar_t *ptr = w_string();
        const wchar_t *found_ptr = ::wcswcs(ptr + index, substr);
        return found_ptr ? static_cast<gsgl::index_t>(found_ptr - ptr) : -1;
    } // string::find()


    gsgl::index_t string::find(const string & substr, const gsgl::index_t index) const
    {
        return find(substr.w_string(), index);
    } // string::find()

    
    gsgl::index_t string::find_reverse(const wchar_t *substr, const gsgl::index_t index) const
    {
        const wchar_t *ptr = w_string();
        const int this_len = static_cast<const int>(::wcslen(ptr));
        const int str_len = static_cast<const int>(::wcslen(substr));

        for (int i = index != -1 ? index : this_len; i > 0; --i)
        {
            int j;
            for (j = 0; j < str_len; ++j)
            {
                int this_pos = i+j-1;

                if (this_pos > this_len || ptr[this_pos] != substr[j])
                    break;
            }

            if (j == str_len)
                return i;
        }

        return -1;
    } // string::find_reverse()


    gsgl::index_t string::find_reverse(const string & substr, const gsgl::index_t index) const
    {
        return find_reverse(substr.w_string(), index);
    } // string::find_reverse()


    ////////////////////////////////////

    string string::copy()
    {
        string result = *this;
        result.unshare();
        return result;
    } // string::copy()


    string & string::trim()
    {
        // we don't need to unshare, because we're assigning the new buffer below
        gsgl::index_t sz = size();

        if (sz)
        {
			data::smart_pointer<wchar_t, true> buf(new wchar_t[sz+1]);
            ::memcpy(buf.ptr(), w_string(), sizeof(wchar_t) * (sz+1));

            wchar_t *start = buf.ptr();
            wchar_t *end = buf.ptr() + (sz-1);

            while (*start && ::iswspace(*start))
                start++;

            while (end > start && ::iswspace(*end))
                *end-- = 0;

            *this = start;
        }

        on_update();
        return *this;
    } // string::trim()


    /// \todo Handle unicode case
    string & string::make_upper()
    {
        unshare();

        for (wchar_t *ch = impl->w_data.ptr(); ch && *ch; ++ch)
        {
            if (*ch >= L'a' && *ch <= L'z')
                *ch -= 32;
        }

        on_update();

        return *this;
    } // string::make_upper()


    /// \todo Handle unicode case.
    string & string::make_lower()
    {
        unshare();

        for (wchar_t *ch = impl->w_data.ptr(); ch && *ch; ++ch)
        {
            if (*ch >= L'A' && *ch <= L'Z')
                *ch += 32;
        }

        on_update();

        return *this;
    } // string::make_lower()


    bool string::to_bool() const
    {
        wchar_t *ch = impl->w_data.ptr();
        while (ch && *ch && ::iswspace(*ch))
            ++ch;

        return ch && (*ch == L't' || *ch == L'T');
    } // string::to_bool()


    int string::to_int() const
    {
        int res = 0;
        if (size())
            ::swscanf(w_string(), L"%d", &res);
        return res;
    } // string::to_int()


    double string::to_double() const
    {
        double res = 0.0f;
        if (size())
            ::swscanf(w_string(), L"%lf", &res);
        return res;
    } // string::to_double()


	data::list<string> string::split(const string & sep) const
    {
        return split(sep.w_string());
    } // string::split()

    
	data::list<string> string::split(const wchar_t *sep) const
    {
		data::list<string> tokens;
        const gsgl::index_t sz = size();
        
        if (sz)
        {
			data::smart_pointer<wchar_t, true> buf(new wchar_t[sz+1]);
            ::memcpy(buf.ptr(), w_string(), sizeof(wchar_t) * (sz+1));

            wchar_t *start, *cur;
            start = cur = buf.ptr();

            while (*cur)
            {
                bool in_sep = false;
                for (const wchar_t *ss = sep; *ss; ++ss)
                    if ((in_sep = (*cur == *ss)))
                        break;

                if (in_sep || sep[0] == 0)
                {
                    *cur = 0;
                    tokens.append(string(start));
                    start = cur+1;
                }
                cur++;
            }

            if (*start)
            {
                tokens.append(string(start));
            }
        }
                
        return tokens;
    } // string::split()
    
    
    string string::format(const string & format, ...)
    {
        const gsgl::index_t len = 1024;
        wchar_t buf[len];
        
        va_list ap;
        va_start(ap, format);
        ::vswprintf(buf, len, format.w_string(), ap);
        va_end(ap);
                
        return string(buf);
    } // string::format()
    

    string string::format(const wchar_t *format, ...)
    {
        const gsgl::index_t len = 1024;
        wchar_t buf[len];
        
        va_list ap;
        va_start(ap, format);
        ::vswprintf_s(buf, len, format, ap);
        va_end(ap);
                
        return string(buf);
    } // string::format()


    ////////////////////////////////////


#ifdef DEBUG

    static const wchar_t *STATIC_NULL_STRING = L"<null string>";
    static const wchar_t *STATIC_INVALID_STRING = L"<INVALID STRING>";

#define ASSIGN_DEBUG_PTR() \
    { \
        switch (mode) \
        { \
        case STRING_NULL: \
            wchar_ptr = STATIC_NULL_STRING; \
            break; \
        case STRING_CONST_REF: \
            wchar_ptr = ref; \
            break; \
        case STRING_SHARED_IMPL: \
            wchar_ptr = impl->get_w_string(); \
            break; \
        default: \
            wchar_ptr = STATIC_INVALID_STRING; \
            break; \
        } \
    }         

#else
#define ASSIGN_DEBUG_PTR() 
#endif


    void string::on_update() const
    {
        recalc_hash();
        ASSIGN_DEBUG_PTR();
    } // string::on_update()


    bool string::is_modified() const
    {
        switch (mode)
        {
        case STRING_NULL:
        case STRING_CONST_REF:
            return false;
        case STRING_SHARED_IMPL:
            return impl->is_modified();
        default:
            throw internal_exception(__FILE__, __LINE__, L"can't happen in string::is_modified()");
        }
    } // string::is_modified()


    void string::make_null()
    {
        if (mode == STRING_SHARED_IMPL)
        {
            assert(impl);
            impl->detach();
        }

        mode = STRING_NULL;
        ref = 0;

        on_update();
    } // string::make_null()


    void string::unshare() const
    {
        switch (mode)
        {
        case STRING_NULL:
        case STRING_CONST_REF:
            mode = STRING_SHARED_IMPL;
            impl = new string_impl(ref);
            impl->attach();
            break;
        case STRING_SHARED_IMPL:
            assert(impl);
            assert(impl->get_ref_count() > 0);

            if (impl->get_ref_count() > 1)
            {
                string_impl *copy = new string_impl(*impl);
                copy->attach();

                impl->detach();
                impl = copy;
            }
            break;
        default:
            throw internal_exception(__FILE__, __LINE__, L"can't happen in string::unshare()");
        }

        on_update();
    } // string::unshare()


} // namespace gsgl
