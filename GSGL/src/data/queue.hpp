#ifndef GSGL_DATA_QUEUE_H
#define GSGL_DATA_QUEUE_H

//
// $Id$
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
#include "data/list.hpp"
#include "data/array.hpp"

namespace gsgl
{

    namespace data
    {
    
        /// A queue is a FIFO structure.
        template <typename T>
        class queue
            : protected list<T>
        {
        public:
            queue();
            queue(const queue & q);
            queue & operator= (const queue & q);
            virtual ~queue();
            
            virtual gsgl::index_t size() const { return list<T>::size(); }

            /// Returns the item at the head of the queue.
            const T & front() const;
            
            /// Adds an item on the back of the queue.
            void push(const T & a);
            
            /// Removes the front item from the queue.
            void pop();
        }; // class queue
        
        //
        
        template <typename T> queue<T>::queue() : list<T>()
        {} // queue<T>::queue()
        
        template <typename T> queue<T>::queue(const queue & q) : list<T>(q)
        {} // queue<T>::queue()
        
        template <typename T> queue<T> & queue<T>::operator= (const queue & q)
        {
            clear();
            for (list_node *l = q.head; l; l = l->next)
                push(l->item);
            return *this;
        } // queue<T>::operator= ()
        
        template <typename T> 
        queue<T>::~queue()
        {
        } // queue<T>::~queue()
    
        template <typename T> const T & queue<T>::front() const
        {
            if (head)
                return head->item;
            else
                throw memory_exception(__FILE__, __LINE__, L"Queue underflow in front().");
        } // queue<T>::front()
                
        template <typename T> void queue<T>::push(const T & item)
        {
            append(item);
        } // queue<T>::push()
        
        template <typename T> void queue<T>::pop()
        {
            if (head)
            {
                list_node *cur = head;
                head = head->next;
                if (cur == tail)
                    tail = cur->prev;

                if (cur->prev)
                    cur->prev->next = cur->next;
                if (cur->next)
                    cur->next->prev = cur->prev;

                cur->prev = 0;
                cur->next = 0;

                delete cur;
                --count;
            }
            else
            {
                throw memory_exception(__FILE__, __LINE__, L"Queue underflow in pop().");
            }
        } // queue<T>::pop()
        

        /////////////////////////////////////////////////////////////////////

        /// A queue that stores simple data types (it may reallocate, copy and/or move items around in memory, so don't rely on addresses of items in the queue.
        /// It uses protected inheritance in order to force you to use the queue functions instead of array functions.
        template <typename T>
        class simple_queue 
            : protected data::simple_array<T>
        {
            gsgl::index_t front_pos;  ///< The position of the front element of the queue.
            gsgl::index_t insert_pos; ///< The position at which to insert items into the queue.
            gsgl::index_t queue_size; ///< The number of items actually in the queue.

        public:
            simple_queue(const gsgl::index_t & initial_capacity = 0);
            simple_queue(const simple_queue &);
            simple_queue & operator= (const simple_queue &);
            virtual ~simple_queue();

            /// Returns the item at the head of the queue.
            const T & front() const;

            /// Adds an item to the back of the queue.
            void push(const T & a);

            /// Removes the front item from the queue.
            void pop();

			/// \name Countable Implementation
			/// \{
			virtual gsgl::index_t size() const { return queue_size; }
			virtual void clear();
			/// \}
        }; // class simple_queue


        template <typename T>
        simple_queue<T>::simple_queue(const gsgl::index_t & initial_capacity)
            : simple_array(initial_capacity), front_pos(0), insert_pos(0), queue_size(0)
        {
        } // simple_queue<T>::simple_queue()


        template <typename T>
        simple_queue<T>::simple_queue(const simple_queue & sq)
            : simple_array(dynamic_cast<const simple_array<T> &>(sq)), 
              front_pos(sq.front_pos), insert_pos(sq.insert_pos), queue_size(sq.queue_size)
        {
        } // simple_queue<T>::simple_queue()


        template <typename T>
        simple_queue<T> & simple_queue<T>::operator= (const simple_queue & sq)
        {
            dynamic_cast<simple_array<T> &>(*this) = dynamic_cast<const simple_array<T> &>(sq);
            front_pos = sq.front_pos;
            insert_pos = sq.insert_pos;
            queue_size = sq.queue_size;
            return *this;
        } // simple_queue<T>::operator= ()


        template <typename T>
        simple_queue<T>::~simple_queue()
        {
        } // simple_queue<T>::~simple_queue()


        template <typename T>
        const T & simple_queue<T>::front() const
        {
            if (queue_size)
            {
                return simple_array<T>::item(front_pos);
            }
            else
            {
                throw gsgl::memory_exception(__FILE__, __LINE__, L"You cannot obtain the front element of an empty queue.");
            }
        } // simple_queue<T>::front()


        template <typename T>
        void simple_queue<T>::push(const T & a)
        {
            if (queue_size == simple_array<T>::size())
            {
                if (front_pos > insert_pos)
                    ++front_pos;

                simple_array<T>::insert(a, insert_pos);
                ++insert_pos;
            }
            else
            {
                simple_array<T>::item(insert_pos) = a;
                insert_pos = (insert_pos+1) % simple_array<T>::size();
            }

            ++queue_size;
        } // simple_queue<T>::push()


        template <typename T>
        void simple_queue<T>::pop()
        {
            if (queue_size)
            {
                front_pos = (front_pos+1) % simple_array<T>::size();
                --queue_size;
            }
            else
            {
                throw gsgl::memory_exception(__FILE__, __LINE__, L"You cannot pop the front element of an empty queue.");
            }
        } // simple_queue<T>::pop()


        template <typename T>
        void simple_queue<T>::clear()
        {
            front_pos  = 0;
            insert_pos = 0;
            queue_size = 0;
        } // simple_queue<T>::clear()


    } // namespace data
    
} // namespace gsgl

#endif
