#ifndef GSGL_DATA_QUEUE_H
#define GSGL_DATA_QUEUE_H

//
// $Id: queue.hpp 314 2008-03-01 16:33:47Z Gordon $
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

#include "data/data.h"
#include "data/list.h"

namespace gsgl
{

    namespace data
    {
    
        /// A queue is a FIFO structure.
        template <typename T>
        class queue
            : public list<T>
        {
        public:
            queue();
            queue(const queue & q);
            queue & operator= (const queue & q);
            virtual ~queue();
            
            /// Returns the item at the head of the queue.
            const T & front() const;
            
            /// Returns the item at the head of the queue.
            T & front();
            
            /// Adds an item on the back of the queue.
            void push(const T & a);
            
            /// Removes the front item from the queue.
            void pop();
        }; // class queue
        
        //
        
        template <typename T> queue<T>::queue() : list<T>()
        {} // queue<T>::queue()
        
        template <typename T> queue<T>::queue(const queue & q) : lisT<T>(q)
        {} // queue<T>::queue()
        
        template <typename T> queue<T> & queue<T>::operator= (const queue & q)
        {
            clear();
            for (list_node *l = q.head; l; l = l->next)
                add(l->item);
        } // queue<T>::operator= ()
        
        template <typename T> 
        queue<T>::~queue()
        {
        } // queue<T>::~queue()
    
        template <typename T> const T & queue<T>::front() const
        {
            return head->item;
        } // queue<T>::front()
        
        template <typename T> T & queue<T>::front()
        {
            return head->item;
        } // queue<T>::front()
        
        template <typename T> void queue<T>::push(const T & item)
        {
            add(item);
        } // queue<T>::push()
        
        template <typename T> void queue<T>::pop()
        {
            if (head)
            {
                list_node *temp = head;
                head = head->next;
                delete temp;
            }
        } // queue<T>::pop()
        
    } // namespace data
    
} // namespace data

#endif
