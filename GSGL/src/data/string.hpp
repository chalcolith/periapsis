#ifndef GSGL_DATA_STRING_H
#define GSGL_DATA_STRING_H

//
// $Id: string.hpp 314 2008-03-01 16:33:47Z Gordon $
//
// Copyright (c) 2008, The Periapsis Project. All rights reserved. 
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

#include "data/data.hpp"
#include "data/iterable.hpp"
#include "data/indexable.hpp"
#include "data/comparable.hpp"
#include "data/printable.hpp"
#include "data/serializable.hpp"
#include "data/exception.hpp"

namespace gsgl
{

    namespace data
    {
        template <typename T> 
		class list;
    }


	class string_iterator;
    class string_impl;


    /// A Unicode string class.
    class DATA_API string
        : public data_object, 
          public data::comparable,
          public data::iterable<wchar_t, string_iterator>, 
          public data::indexable<wchar_t, gsgl::index_t>, 
          public io::printable,
          public io::serializable
    {
		friend class string_iterator;

        enum string_mode
        {
            STRING_NULL        = 0,
            STRING_CONST_REF   = 1,
            STRING_SHARED_IMPL = 2,
            STRING_INVALID     = 3
        };

        mutable string_mode mode;

        //

        union
        {
            mutable string_impl *impl;
            const wchar_t *ref;
        };

#ifdef DEBUG
        mutable const wchar_t *wchar_ptr;
#endif

    public:
        string();
        string(const string &);
        string(const wchar_t *);       ///< Does NOT make a copy of the given wide-character string, only keeps a reference to it.
        string(wchar_t *);             ///< Makes a copy of the given wide-character string.
        explicit string(const char *); 

        string & operator= (const wchar_t *);
        string & operator= (wchar_t *);
        string & operator= (const string &);

        virtual ~string();


        /// \name Countable Implementation
        /// @{
        virtual gsgl::index_t size() const;
        virtual void clear();
        /// @}


        /// \name Iterable Implementation Etc.
        /// @{
        virtual void append(const wchar_t &);

        void append(const wchar_t *);
        void append(const string &);
        virtual void insert(const iterator &, const wchar_t &);
        virtual void remove(const iterator &);
        /// @}


        /// \name Indexable Implementation
        /// @{
        virtual const wchar_t & item(const gsgl::index_t & index) const;
        virtual wchar_t & item(const gsgl::index_t & index);

        virtual bool contains_index(const gsgl::index_t & index) const;
        /// @}


        /// \name Comparable Implementation Etc.
        /// @{
        virtual int compare(const data::comparable &) const;
        int compare(const wchar_t *) const;
        
        inline bool operator== (const wchar_t *s) const { return compare(s) == 0; }
        inline bool operator!= (const wchar_t *s) const { return compare(s) != 0; }
        inline bool operator<  (const wchar_t *s) const { return compare(s) <  0; }
        inline bool operator<= (const wchar_t *s) const { return compare(s) <= 0; }
        inline bool operator>  (const wchar_t *s) const { return compare(s) >  0; }
        inline bool operator>= (const wchar_t *s) const { return compare(s) >= 0; }        
        /// @}


        /// \name Printable Implementation
        /// @{
        virtual void to_stream(io::text_stream &) const;
        virtual void from_stream(io::text_stream &);
        /// @}


        /// \name Serializable Implementation
        /// @{
        virtual void to_stream(io::data_stream &) const;
        virtual void from_stream(io::data_stream &);
        /// @}


        /// \name Pointer Access
        /// @{
        const wchar_t       *w_string() const;
        const char          *c_string() const;
        const unsigned char *p_string() const;
        /// @}


        /// \name String Concatenation
        /// @{
        string & operator+= (const wchar_t &);
        string & operator+= (const wchar_t *);
        string & operator+= (const string &);

        string operator+ (const wchar_t &) const;
        string operator+ (const wchar_t *) const;
        string operator+ (const string &) const;
        /// @}


        /// \name Substrings
        /// @{
        string substring(const gsgl::index_t index, const gsgl::index_t length = -1) const;
        string left_substring(const gsgl::index_t length) const;
        string right_substring(const gsgl::index_t length) const;

        gsgl::index_t find(const wchar_t *substr, const gsgl::index_t index = 0) const;
        gsgl::index_t find(const string & substr, const gsgl::index_t index = 0) const;

        gsgl::index_t find_reverse(const wchar_t *substr, const gsgl::index_t index = -1) const;
        gsgl::index_t find_reverse(const string & substr, const gsgl::index_t index = -1) const;
        /// @}


        /// \name Various Useful Utilities
        /// @{
        string copy(); ///< Makes a deep copy of the string that is not shared.
        string & trim();

        string & make_upper();
        string & make_lower();

        bool to_bool() const;
        int to_int() const;
        double to_double() const;

        data::list<string> split(const wchar_t *separator) const;
        data::list<string> split(const string & separator) const;

        static string format(const wchar_t *format, ...);
        static string format(const string & format, ...);
        /// @}

        static const string EMPTY_STRING;

    private:
        void make_null();
        void unshare() const;
    }; // class string


	class DATA_API string_iterator
	{
        friend class gsgl::string;
		const gsgl::string & parent;
		gsgl::index_t position;

	protected:
		string_iterator(const data::iterable<wchar_t, string_iterator> & parent_iterable)
			: parent(dynamic_cast<const gsgl::string &>(parent_iterable)), position(0) {}

		string_iterator(const string_iterator & si)
			: parent(si.parent), position(si.position) {}

		inline string_iterator & operator= (const string_iterator & si)
		{
			const_cast<gsgl::string &>(parent) = si.parent;
			position = si.position;
			return *this;
		}

		inline bool is_valid() const { return position < parent.size(); }

		inline const wchar_t & operator* () const
		{
			if (position < parent.size())
				return const_cast<wchar_t &>(parent[position]);
			else
				throw memory_exception(__FILE__, __LINE__, L"Array index out of bounds in string iterator dereference.");
		}

		inline string_iterator operator++()
		{
			if (position < parent.size())
				++position;
			else
				throw memory_exception(__FILE__, __LINE__, L"Array index out of bounds in string iterator preincrement.");
            return *this;
		}
	}; // class string_iterator


} // namespace gsgl

#endif
