#ifndef GSGL_DATA_POOL_H
#define GSGL_DATA_POOL_H

//
// $Id: pool.hpp 14 2008-05-23 23:46:03Z kulibali $
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

#include "data/data.hpp"
#include "data/array.hpp"
#include "data/stack.hpp"

namespace gsgl
{

    namespace data
    {
    
		/// A pool of objects.
		template <typename T>
		class pool
			: public data_object
		{
			object_array<T> data;
			simple_stack<T *> deleted;
			gsgl::index_t count;

		public:
			pool(const gsgl::index_t & initial_capacity = 32, const gsgl::index_t & bucket_size = 32);
			pool(const pool &);
			pool & operator= (const pool &);
			virtual ~pool();

			T *allocate();
			void *allocate_inplace();
			void deallocate(T *);

			gsgl::index_t size() const { return count; }
		}; // class pool


		template <typename T>
		pool<T>::pool(const gsgl::index_t & initial_capacity, const gsgl::index_t & bucket_size)
			: data_object(), data(initial_capacity, bucket_size, true), count(0)
		{
		} // pool<T>::pool()


		template <typename T>
		pool<T>::pool(const pool &)
			: data_object(), data(0, 32), count(0)
		{
			throw internal_exception(__FILE__, __LINE__, L"Copying a pool object is illegal.");
		} // pool<T>::pool()


		template <typename T>
		pool<T> & pool<T>::operator= (const pool &)
		{
			throw internal_exception(__FILE__, __LINE__, L"Copying a pool object is illegal.");
		} // pool<T>::operator= ()


		template <typename T>
		pool<T>::~pool()
		{
			// since the array's no_init flag is set, we have to delete the objects ourselves
			for (int i = 0; i < data.size(); ++i)
			{
				T *ptr = &data[i];

				if (!deleted.find_value(ptr).is_valid())
					ptr->~T();
			}
		} // pool<T>::~pool()


		/// Returns a pointer to a fully-initialized object of type T.
		template <typename T>
		T *pool<T>::allocate()
		{
			T *result = allocate_inplace();
			new (result) T();
			return result;
		} // pool<T>::allocate()


		template <typename T>
		void *pool<T>::allocate_inplace()
		{
			T *result = 0;

			if (deleted.size())
			{
				result = deleted.top();
				deleted.pop();
			}
			else
			{
				result = &data[data.size()];
			}

			++count;
			return result;
		} // pool<T>::allocate_inplace()


		template <typename T>
		void pool<T>::deallocate(T *ptr)
		{
			deleted.push(ptr);
			ptr->~T();
			--count;
		} // pool<T>::deallocate()


    } // namespace data
    
} // namespace gsgl

#endif
