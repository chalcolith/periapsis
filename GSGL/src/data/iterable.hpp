#ifndef GSGL_DATA_ITERABLE_H
#define GSGL_DATA_ITERABLE_H

//
// $Id: iterable.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "data/countable.hpp"

namespace gsgl
{

    namespace data
    {                
        
        /// A collection whose elements may be iterated over from beginning to end.
        /// Items in the collection must implement operator==().
		/// Iterators must implement the following functions: is_valid(), operator*() and operator++(), as well as the usual copy constructor and assignment operator.
        template <typename T, typename IBase>
        class iterable
            : public countable
        {
        public:
            iterable();
            virtual ~iterable();

            /// \name Countable Implementation
            /// \{

            /// A generic count function that iterates over the collection.
            /// \note Not very efficient; should be overridden if possible.
            virtual gsgl::index_t size() const;            
            /// \}


			/// \name Iterators
			/// \{
			class const_iterator
				: public IBase
			{
				friend class iterable;
				const_iterator(const iterable & parent) : IBase(parent) {}
			public:
				const_iterator(const const_iterator & ci) : IBase(ci) {}
				const_iterator & operator= (const const_iterator & ci) { return IBase::operator=(ci); }

				inline bool is_valid() const { return IBase::is_valid(); }
				inline const T & operator*() const { return IBase::operator*(); }
				inline const T *operator->() const { return &(IBase::operator*()); }
				inline const_iterator & operator++() { IBase::operator++(); return *this; }
			}; // class const_iterator


			class iterator
				: public IBase
			{
				friend class iterable;
				iterator(const iterable & parent) : IBase(parent) {}
			public:
				iterator(const iterator & i) : IBase(i) {}
				iterator & operator= (const iterator & i) { return IBase::operator=(i); }

				inline bool is_valid() const { return IBase::is_valid(); }
				inline T & operator*() { return const_cast<T &>(IBase::operator*()); }
				inline T *operator->() { return const_cast<T *>(&(IBase::operator*())); }
				inline iterator & operator++() { IBase::operator++(); return *this; }
			}; // class iterator


			const_iterator iter() const { return const_iterator(*this); } ///< \return A const iterator pointing to the first item in the collection.
			iterator iter() { return iterator(*this); }                   ///< \return An iterator pointing to the first item in the collection.
			/// \}


			/// \name Other Useful Functions
			/// \{
            virtual void append(const iterable<T, IBase> &);          ///< Append all members of a collection to this one.
            virtual void append(const T &) = 0;                       ///< Add an item to the end of the collection.
            virtual void insert(const iterator &, const T &) = 0;  ///< Insert an item before the one specified by the iterator.
            virtual void remove(const iterator &) = 0;             ///< Remove the item specified by the iterator.

            /// A generic find function that iterates over the collection to find a particular value.
            /// \note Not efficient; should be overridden if possible.
            virtual typename const_iterator find_value(const T &) const;
            virtual typename iterator find_value(const T &);
			/// \}
        }; // class iterable
        

        //////////////////////////////////////////

        template <typename T, typename IBase>
        iterable<T,IBase>::iterable()
            : countable()
        {
        } // iterable<T,IBase>::iterable()

        template <typename T, typename IBase>
        iterable<T,IBase>::~iterable()
        {
        } // iterable<T,IBase>::~iterable()

        
        template <typename T, typename IBase> 
        gsgl::index_t iterable<T,IBase>::size() const
        {
            gsgl::index_t count = 0;
			for (const_iterator i(*this); i.is_valid(); ++i)
                ++count;
            return count;
        } // iterable<T,IBase>::size()


        template <typename T, typename IBase>
        void iterable<T,IBase>::append(const iterable<T, IBase> & a)
        {
			for (const_iterator i(*this); i.is_valid(); ++i)
                append(*i);
        } // iterable<T,IBase>::append()


        template <typename T, typename IBase>
		typename iterable<T,IBase>::const_iterator iterable<T,IBase>::find_value(const T & item) const
        {
            const_iterator i(*this);

            for (; i.is_valid(); ++i)
            {
                if (*i == item)
                    break;
            }

            return i;
        } // iterable<T,IBase>::find_value()


        template <typename T, typename IBase>
		typename iterable<T,IBase>::iterator iterable<T,IBase>::find_value(const T & item)
        {
            iterator i(*this);

            for (; i.is_valid(); ++i)
            {
                if (*i == item)
                    break;
            }

            return i;
        } // iterable<T,IBase>::find_value()


    } // namespace data
    
} // namespace gsgl

#endif
