#ifndef GSGL_DATA_PQUEUE_H
#define GSGL_DATA_PQUEUE_H

//
// $Id: pqueue.hpp 15 2008-07-20 17:36:39Z kulibali $
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

#include "data.hpp"
#include "indexable.hpp"
#include "iterable.hpp"
#include "array.hpp"

namespace gsgl
{

    namespace data
    {

        template <typename T, typename I>
        class pqueue_iterator;

    
        /// A priority queue.  Should only be used with simple types, as it copies around chunks of memory.
        template <typename T, typename I>
        class pqueue
            : public data_object, public indexable<T, gsgl::index_t>, public iterable<T, pqueue_iterator<T,I> >
        {
            friend class pqueue_iterator<T,I>;

            data::simple_array<T> values;
            data::simple_array<I> priorities;

        public:
            pqueue();
            pqueue(const pqueue &);
            pqueue & operator= (const pqueue &);
            virtual ~pqueue();
            
            /// \name Countable Implementation
            /// \{
            virtual gsgl::index_t size() const;
            virtual void clear();
            /// \}

            /// \name Indexable Implementation
            /// \{
            virtual const T & item(const gsgl::index_t &) const;
            virtual T & item(const gsgl::index_t &);
            virtual bool contains_index(const gsgl::index_t &) const;
            /// \}

            /// \name Iterable Implementation
            /// \{
            virtual void append(const T &);
            virtual void insert(const iterator &, const T &);
            virtual void remove(const iterator &);
            /// \}

            /// \name Priority Queue Functionality
            /// \{
            const T & front() const;

            void push(const T &, const I &);
            void pop();
            /// \}
        }; // class pqueue
        

        //

        template <typename T, typename I>
        pqueue<T,I>::pqueue()
            : data_object(), iterable<T, pqueue_iterator<T,I> >()
        {
        } // pqueue<T,I>::pqueue()


        template <typename T, typename I>
        pqueue<T,I>::pqueue(const pqueue & pq)
            : data_object(), iterable<T, pqueue_iterator<T,I> >(), values(pq.values), priorities(pq.priorities)
        {
        } // pqueue<T,I>::pqueue()


        template <typename T, typename I>
        pqueue<T,I> & pqueue<T,I>::operator= (const pqueue & pq)
        {
            values = pq.values;
            priorities = pq.priorities;
            return *this;
        } // pqueue<T,I>::pqueue()


        template <typename T, typename I>
        pqueue<T,I>::~pqueue()
        {
        } // pqueue<T,I>::pqueue()


        //

        template <typename T, typename I>
        gsgl::index_t pqueue<T,I>::size() const
        {
            assert(values.size() == priorities.size());
            return values.size();
        } // pqueue<T,I>::size()


        template <typename T, typename I>
        void pqueue<T,I>::clear()
        {
            values.clear();
            priorities.clear();
        } // pqueue<T,I>::clear()


        //

        template <typename T, typename I>
        const T & pqueue<T,I>::item(const gsgl::index_t & index) const
        {
            return values.item(index);
        } // pqueue<T,I>::item()


        template <typename T, typename I>
        T & pqueue<T,I>::item(const gsgl::index_t & index)
        {
            return values.item(index);
        } // pqueue<T,I>::item()


        template <typename T, typename I>
        bool pqueue<T,I>::contains_index(const gsgl::index_t & index) const
        {
            return values.contains_index(index);
        } // pqueue<T,I>::contains_index()


        //

        template <typename T, typename I>
        void pqueue<T,I>::append(const T & item)
        {
            push(item, I());
        } // pqueue<T,I>::append()


        template <typename T, typename I>
        void pqueue<T,I>::insert(const iterator & i, const T & item)
        {
            push(item, priorities[i.position]);
        } // pqueue<T,I>::insert()


        template <typename T, typename I>
        void pqueue<T,I>::remove(const iterator & i)
        {
            values.remove(i.position);
            priorities.remove(i.position);
        } // pqueue<T,I>::remove()


        template <typename T, typename I>
        const T & pqueue<T,I>::front() const
        {
            try
            {
                return values[0];
            }
            catch (runtime_exception &)
            {
                throw memory_exception(__FILE__, __LINE__, L"Priority queue underflow.");
            }
        } // pqueue<T,I>::front()


        template <typename T, typename I>
        void pqueue<T,I>::push(const T & item, const I & priority)
        {
            gsgl::index_t i, sz = values.size();
            for (i = 0; i < sz; ++i)
            {
                if (priority >= priorities[i])
                    break;
            }
            values.insert(i, item);
            priorities.insert(i, priority);
        } // pqueue<T,I>::push()


        template <typename T, typename I>
        void pqueue<T,I>::pop()
        {
            if (values.size())
            {
                values.remove(0);
                priorities.remove(0);
            }
            else
            {
                throw memory_exception(__FILE__, __LINE__, L"Priority queue underflow in pop.");
            }
        } // pqueue<T,I>::pop()


        //////////////////////////////////////////

        template <typename T, typename I>
        class pqueue_iterator
        {
            friend class pqueue<T, I>;
            const pqueue<T, I> & parent;
            gsgl::index_t position;
        
        protected:
            pqueue_iterator(const iterable<T, pqueue_iterator<T,I> > & parent_iterable)
                : parent(dynamic_cast<const pqueue<T,I> &>(parent_iterable)), position(0) {}

            pqueue_iterator(const pqueue_iterator & i)
                : parent(i.parent), position(i.position) {}

            pqueue_iterator & operator= (const pqueue_iterator & i)
            {
                parent = i.parent;
                position = i.position;
                return *this;
            }

            inline bool is_valid() const { return position < parent.values.size(); }

            inline const T & operator* () const
            {
                if (position < parent.values.size())
                    return parent.values[position];
                else
                    throw memory_exception(__FILE__, __LINE__, L"Priority queue iterator overflow in iterator dereference.");
            }

            inline void operator++ ()
            {
                if (position < parent.values.size())
                    ++position;
                else
                    throw memory_exception(__FILE__, __LINE__, L"Priority queue iterator overflow in iterator preincrement.");
            }
        }; // class pqueue_iterator


    } // namespace data
    
} // namespace data

#endif
