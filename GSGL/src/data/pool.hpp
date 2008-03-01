#ifndef GSGL_DATA_POOL_H
#define GSGL_DATA_POOL_H

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
#include "data/iterable.hpp"
#include "data/indexable.hpp"
#include "data/array.hpp"
#include "data/stack.hpp"

namespace gsgl
{

    namespace data
    {
    
#if 0
        /// Provides a memory pool of objects.
        /// \note Iterating over this collection will hit deallocated objects!
        template <typename T>
        class pool
            : public data_object, public iterable<T>, public indexable<T, gsgl::index_t>
        {
            array<T> data;
            stack<gsgl::index_t> free_indices;
        public:
            pool(const gsgl::index_t cap = 0);
            ~pool();
            
            const T *get_data() const
            {
                return data.ptr();
            }
            T *get_data()
            {
                return data.ptr();
            }
            
            /// Returns the index of the allocated item.
            gsgl::index_t allocate();
            void release(const gsgl::index_t);
            
            // iterable
            virtual iterator<T> iter() const;
            
            // indexable
            virtual const T & item(const gsgl::index_t &) const;
            virtual T & item(const gsgl::index_t &);
            
            // countable
            virtual gsgl::index_t size() const;
            virtual void clear();
            
            /// Reimplemented for performance -- does not check bounds.
            inline const T & operator[] (const gsgl::index_t index) const
            {
                return data.at(index);
            }
            
            inline T & operator[] (const gsgl::index_t index)
            {
                return data.at(index);
            }
        }; // class pool
        
        //
        
        template <typename T> 
        gsgl::index_t pool<T>::allocate()
        {
            gsgl::index_t index;
            
            if (free_indices.size())
            {
                index = free_indices.top();
                free_indices.pop();
            }
            else
            {
                index = data.size();
                data[index];
            }
            
            return index;
        } // pool<T>::allocate()
        
        template <typename T> 
        void pool<T>::release(const gsgl::index_t index)
        {
            free_indices.push(index);
        } // pool<T>::release()
        
        //
        
        template <typename T> 
        pool<T>::pool(const gsgl::index_t cap) 
            : data_object(), indexable<T, gsgl::index_t>(), data(cap), free_indices()
        {} // pool<T>::pool()
        
        template <typename T> 
        pool<T>::~pool()
        {} // pool<T>::~pool()
        
        template <typename T> 
        iterator<T> pool<T>::iter() const
        {
            return data.iter();
        } // pool<T>::iter()
        
        template <typename T> 
        gsgl::index_t pool<T>::size() const
        {
            return data.size() - free_indices.size();
        } // pool<T>::size()
        
        template <typename T> 
        void pool<T>::clear()
        {
            data.clear();
            free_indices.clear();
        } // pool<T>::clear()
        
        //
        
        template <typename T> 
        const T & pool<T>::item(const gsgl::index_t & index) const
        {
            return data.item(index);
        } // pool<T>::item()
        
        template <typename T> 
        T & pool<T>::item(const gsgl::index_t & index)
        {
            return data.item(index);
        } // pool<T>::item()
        
#endif

    } // namespace data
    
} // namespace gsgl

#endif
