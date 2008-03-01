#ifndef GSGL_DATA_LIST_H
#define GSGL_DATA_LIST_H

//
// $Id: list.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "data/exception.hpp"

namespace gsgl
{

    namespace data
    {

		template <typename T>
		class list_iterator;

    
        /// A simple linked list.
        /// This class is optimized for fast iteration, insertion and deletion.
        
        template <typename T>
        class list
            : public data_object, public iterable<T, list_iterator<T> >, public indexable<T, gsgl::index_t>
        {
			friend class list_iterator<T>;

        protected:
        
            struct list_node
            {
                T item;
                list_node *prev, *next;
                
                list_node(const T & item, list_node *prev = 0, list_node *next = 0) 
                    : item(item), prev(prev), next(next) {}
            }; // class list_node
            
            gsgl::index_t count;
            list_node *head, *tail;
            
        public:
            list();
            list(const list &);
            list & operator= (const list &);
            virtual ~list();
            
            const T & get_head() const;
            T & get_head();

            const T & get_tail() const;
            T & get_tail();

			bool operator== (const list &) const;

            /// \name Countable Implementation
            /// \{
            virtual gsgl::index_t size() const;
            virtual void clear();
            /// \}
            
            /// \name Indexable Implementation
            /// \{
            virtual bool contains_index(const gsgl::index_t &) const;
            virtual const T & item(const gsgl::index_t &) const;
            virtual T & item(const gsgl::index_t &);
            /// \}
            
            /// \name Iterable Implementation
            /// \{
            /// Adds a data item to the end of the list.
            virtual void append(const T &);
            using iterable<T, list_iterator<T> >::append;
            virtual void insert(const iterator &, const T &);
            virtual void remove(const iterator &);
            /// \}            
        }; // class list


		//////////////////////////////////////////

        template <typename T> 
        list<T>::list() 
            : data_object(), iterable<T, list_iterator<T> >(), indexable<T, gsgl::index_t>(), 
			  count(0), head(0), tail(0)
        {
        } // list<t>::list()

        
        template <typename T> 
        list<T>::list(const list & l) 
            : data_object(), iterable<T, list_iterator<T> >(), indexable<T, gsgl::index_t>(l), 
			  count(0), head(0), tail(0)
        {
            for (list_node *cur = l.head; cur; cur = cur->next)
                append(cur->item);
        } // list<T>::list()

        
        template <typename T> 
        list<T> & list<T>::operator= (const list & l)
        {
            clear();
            for (list_node *cur = l.head; cur; cur = cur->next)
                append(cur->item);
            return *this;
        } // list<T>::operator= ()

        
        template <typename T> 
        list<T>::~list()
        {
            clear();
        } // list<T>:~list()
        

        template <typename T>
        const T & list<T>::get_head() const
        {
            if (head)
                return head->item;
            else
                throw memory_exception(__FILE__, __LINE__, L"Cannot get the head of an empty list.");
        } // list<T>::get_head()


        template <typename T>
        T & list<T>::get_head()
        {
            if (head)
                return head->item;
            else
                throw memory_exception(__FILE__, __LINE__, L"Cannot get the head of an empty list.");
        } // list<T>::get_head()


        template <typename T>
        const T & list<T>::get_tail() const
        {
            if (tail)
                return tail->item;
            else
                throw memory_exception(__FILE__, __LINE__, L"Cannot get the tail of an empty list.");
        } // list<T>::get_tail()


        template <typename T>
        T & list<T>::get_tail()
        {
            if (tail)
                return tail->item;
            else
                throw memory_exception(__FILE__, __LINE__, L"Cannot get the tail of an empty list.");
        } // list<T>::get_tail()


		template <typename T>
		bool list<T>::operator== (const list<T> & l) const
		{
			list<T>::const_iterator a = this->iter();
			list<T>::const_iterator b = l.iter();

			for (; a.is_valid() && b.is_valid(); ++a, ++b)
			{
				if (!(*a == *b))
					return false;
			}

			if (a.is_valid() || b.is_valid())
				return false;

			return true;
		} // list<T>::size()


        template <typename T> 
        gsgl::index_t list<T>::size() const
        {
            return count;
        } // list<T>::size()
        

        template <typename T> 
        void list<T>::clear()
        {
            list_node *prev, *cur = head;

            while (cur)
            {
                prev = cur;
                cur = cur->next;
                delete prev;
            }

            head = 0;
            tail = 0;
            count = 0;
        } // list<T>::clear()
        

        template <typename T>
        bool list<T>::contains_index(const gsgl::index_t & index) const
        {
            return index < count;
        } // list<T>::contains_index()


        template <typename T> 
        const T & list<T>::item(const gsgl::index_t & index) const
        {
            list_node *cur = head;
            for (gsgl::index_t i = 0; cur && i < index; ++i)
				cur = cur->next;
            
            if (cur)
                return cur->item;
            else
                throw memory_exception(__FILE__, __LINE__, L"List index out of bounds in list item reference.");
        } // list<T>::item()

        
        template <typename T> 
        T & list<T>::item(const gsgl::index_t & index)
        {
            list_node *cur = head;
            for (gsgl::index_t i = 0; cur && i < index; ++i)
				cur = cur->next;
            
            if (cur)
                return cur->item;
            else
                throw memory_exception(__FILE__, __LINE__, L"List index out of bounds in list item reference.");
        } // list<T>::item()


        template <typename T> 
        void list<T>::append(const T & data)
        {
            if (tail)
            {
                tail->next = new list_node(data, tail, 0);
                tail = tail->next;
            }
            else
            {
                head = tail = new list_node(data);
            }
            
            ++count;
        } // list<T>::append()


        /// \todo Implement list::insert().
        template <typename T>
        void list<T>::insert(const iterator & i, const T & item)
        {
            if (i.cur)
            {
                list_node *new_node = new list_node(item, i.cur->prev, i.cur);
                if (i.cur == head)
                    head = new_node;
                if (i.cur->prev)
                    i.cur->prev->next = new_node;
                i.cur->prev = new_node;

                ++count;
            }
            else
            {
                append(item);
            }
        } // list<T>::insert()


        template <typename T>
        void list<T>::remove(const iterator & i)
        {
            if (i.cur)
            {
                if (i.cur == head)
                    head = i.cur->next;
                if (i.cur == tail)
                    tail = i.cur->prev;

                if (i.cur->prev)
                    i.cur->prev->next = i.cur->next;
                if (i.cur->next)
                    i.cur->next->prev = i.cur->prev;
                
                i.cur->prev = 0;
                i.cur->next = 0;

                delete i.cur;
                const_cast<list_node *>(i.cur) = 0;

                --count;
            }
        } // list<T>::remove()


		//////////////////////////////////////////

		template <typename T>
		class list_iterator
		{
            friend class list<T>;
			typename list<T>::list_node *cur;

		protected:
			list_iterator(const iterable<T, list_iterator<T> > & parent_iterable)
				: cur(const_cast<list<T>::list_node *>( dynamic_cast<const list<T> &>(parent_iterable).head )) {}
			
			list_iterator(const list_iterator<T> & li)
				: cur(li.cur) {}

			list_iterator & operator= (const list_iterator<T> & li)
			{
				cur = li.cur;
				return *this;
			}


			inline bool is_valid() const { return cur != 0; }

			inline const T & operator*() const
			{
				if (cur)
					return cur->item;
				else
					throw memory_exception(__FILE__, __LINE__, L"Iterator overflow in list iterator dereference.");
			}

			inline list_iterator & operator++()
			{
				if (cur)
					cur = cur->next;
				else
					throw memory_exception(__FILE__, __LINE__, L"Iterator overflow in list iterator preincrement.");
                return *this;
			}
		}; // class list_iterator

        
    } // namespace data
    
} // namespace gsgl

#endif
