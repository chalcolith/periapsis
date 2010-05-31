#ifndef GSGL_DATA_ARRAY_H
#define GSGL_DATA_ARRAY_H

//
// $Id: array.hpp 19 2008-07-28 16:13:28Z kulibali $
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

#include "base.hpp"
#include "data.hpp"
#include "indexable.hpp"
#include "exception.hpp"

namespace gsgl
{

    namespace data
    {

		/// Mixin class for memory allocation to keep stdlib functions out of the headers.
		class BASE_API array_base
			: public data_object
		{
		protected:
			array_base() : data_object() {}
			virtual ~array_base() {}

			void *allocate(const gsgl::index_t & num_bytes);
			void *reallocate(void *, const gsgl::index_t & num_bytes);
			void  deallocate(void *);

			void move(void *dest, void *src, const gsgl::index_t & num_bytes);
			void copy(void *dest, void *src, const gsgl::index_t & num_bytes);
			void set(void *dest, const unsigned char & val, const gsgl::index_t & num_bytes);
		}; // array_base


		template <typename T>
		class simple_array_iterator;


		/// A simple dynamic array.
		///
		/// This kind of array should only be used to store simple types (integers & pointers), as it copies its memory around and doesn't call copy constructors, etc.
		/// Pointers to items in the array will probably swiftly become invalid!
		template <typename T>
		class simple_array
			: public array_base, 
			  public iterable<T, simple_array_iterator<T> >, 
			  public indexable<T, gsgl::index_t>
		{
			friend class simple_array_iterator<T>;

			gsgl::index_t capacity;     ///< The capacity of the array.
			gsgl::index_t num_elements; ///< The highest index referenced so far plus one.

			T * data;

		public:
			simple_array(const gsgl::index_t & initial_capacity = 0);
			simple_array(const simple_array & a);
			simple_array & operator= (const simple_array & a);
			virtual ~simple_array();


			/// \name Countable Implementation
			/// \{
			virtual gsgl::index_t size() const { return num_elements; }
			virtual void clear() { resize(num_elements = 0); }
			/// \}


			/// \name Iterable Implementation
			/// \{
			using iterable::append;
			virtual void append(const T &);
            virtual void insert(const iterator & i, const T & item);
            virtual void remove(const iterator & i);
			/// \}


			/// \name Indexable Implementation
			/// \{
			virtual const T & item(const gsgl::index_t & index) const;
			virtual T & item(const gsgl::index_t & index);
			virtual bool contains_index(const gsgl::index_t & index) const { return index < num_elements; }
			/// \}


			/// \name Array Functionality
			/// \{
			inline const T *ptr() const { return data; }
			inline T * ptr() { return data; }

			void insert(const gsgl::index_t & index, const T & item);
			void remove(const gsgl::index_t & index);
			/// \}

		protected:
			void resize(const gsgl::index_t & new_num_elements);
		}; // class simple_array


		//////////////////////////////////////////

		template <typename T>
		simple_array<T>::simple_array(const gsgl::index_t & initial_capacity)
			: array_base(), 
			  iterable<T, simple_array_iterator<T> >(), 
			  indexable<T, gsgl::index_t>(),
			  capacity(initial_capacity), 
			  num_elements(0), 
			  data(0)
		{
			if (initial_capacity)
				data = reinterpret_cast<T *>(allocate(initial_capacity * sizeof(T)));
		} // simple_array<T>::simple_array()


		template <typename T>
		simple_array<T>::simple_array(const simple_array<T> & a)
			: array_base(), 
			  iterable<T, simple_array_iterator<T> >(), 
			  indexable<T, gsgl::index_t>(),
			  capacity(a.capacity), 
			  num_elements(a.num_elements), 
			  data(0)
		{
			if (num_elements)
			{
				data = reinterpret_cast<T *>(allocate(num_elements * sizeof(T)));
				copy(data, a.data, num_elements * sizeof(T));
			}
		} // simple_array<T>::simple_array()


		template <typename T>
		simple_array<T> & simple_array<T>::operator= (const simple_array<T> & a)
		{
			if (a.num_elements)
			{
				resize(a.num_elements);
				copy(data, a.data, num_elements * sizeof(T));
			}
			else
			{
                capacity = 0;
                num_elements = 0;
				deallocate(data);
				data = 0;
			}

			return *this;
		} // simple_array<T>::operator= ()


		template <typename T>
		simple_array<T>::~simple_array()
		{
			deallocate(data);
		} // simple_array<T>::~simple_array()


		template <typename T>
		void simple_array<T>::append(const T & item)
		{
			resize(num_elements+1); // this changes the value of num_elements
			data[num_elements-1] = item;
		} // simple_array<T>::append()


        template <typename T>
        void simple_array<T>::insert(const iterator & i, const T & item)
        {
            insert(i.position, item);
        } // simple_array<T>::insert()


        template <typename T>
        void simple_array<T>::remove(const iterator & i)
        {
            remove(i.position);
        } // simple_array<T>::remove()


		template <typename T>
		const T & simple_array<T>::item(const gsgl::index_t & index) const
		{
			if (index < num_elements)
				return data[index];
			else
				throw memory_exception(__FILE__, __LINE__, L"Array index overflow in const item access.");
		} // simple_array<T>::item()


		template <typename T>
		T & simple_array<T>::item(const gsgl::index_t & index)
		{
            if (index >= num_elements)
                resize(index+1);
            return data[index];
		} // simple_array<T>::item()


		template <typename T>
		void simple_array<T>::insert(const gsgl::index_t & index, const T & item)
		{
			const index_t old_num_elements = num_elements;

			resize(gsgl::max_val(old_num_elements+1, index+1));
			if (index < old_num_elements)
				move(data + (index+1), data + index, (old_num_elements - index) * sizeof(T));
			data[index] = item;
		} // simple_array<T>::insert()


		template <typename T>
		void simple_array<T>::remove(const gsgl::index_t & index)
		{
			if (index < num_elements)
				move(data + index, data + (index+1), (num_elements - (index+1)) * sizeof(T));
			else
				throw memory_exception(__FILE__, __LINE__, L"Array index out of bounds in remove.");

            --num_elements;
		} // simple_array<T>::remove()


		template <typename T>
        void simple_array<T>::resize(const gsgl::index_t & new_num_elements)
		{
			if (new_num_elements > capacity)
			{
				index_t new_capacity = new_num_elements * 3 / 2;
				new_capacity = new_capacity + ((new_capacity + 256) % 256);
				data = reinterpret_cast<T *>(reallocate(data, new_capacity * sizeof(T)));
				set(data + capacity, 0, (new_capacity - capacity) * sizeof(T));
				capacity = new_capacity;
			}

			num_elements = new_num_elements;
		} // simple_array<T>::resize()



		//////////////////////////////////////////

		template <typename T>
		class simple_array_iterator
		{
            friend class simple_array<T>;
			const simple_array<T> & parent;
			gsgl::index_t position;
		protected:
			simple_array_iterator(const iterable<T, simple_array_iterator<T> > & parent_iterable)
				: parent(dynamic_cast<const simple_array<T> &>(parent_iterable)), position(0) {}
			
			simple_array_iterator(const simple_array_iterator & i)
				: parent(i.parent), position(i.position) {}

			simple_array_iterator & operator=(const simple_array_iterator & i)
			{
				parent = i.parent;
				position = i.position;
				return *this;
			}

			inline bool is_valid() const { return position < parent.num_elements; }
			
			inline const T & operator*() const
			{
				if (position < parent.num_elements)
					return const_cast<T &>(parent.data[position]);
				else
					throw memory_exception(__FILE__, __LINE__, L"Array iterator overflow in dereference operator.");
			}

			inline simple_array_iterator & operator++() 
			{
				if (position < parent.num_elements)
					++position;
				else
					throw memory_exception(__FILE__, __LINE__, L"Array iterator overflow in preincrement operator.");

                return *this;
			}
		}; // class simple_array_iterator



		/////////////////////////////////////////////////////////////////////

		template <typename T>
		class object_array_iterator;


		/// An array of objects.  Pointers to these objects will remain valid as long as the objects remain in the array.
		/// You may also use placement new to allocate directly in the array, avoiding a copy.
		/// Note that insert and remove operations will be expensive, as they will copy large numbers of objects around.
		///
		/// Arrays created with the no_init flag, however, will not do any construction or destruction.  This could be very dangerous.
		
		template <typename T>
		class object_array
			: public array_base,
			  public iterable<T, object_array_iterator<T> >,
			  public indexable<T, gsgl::index_t>
		{
			friend class object_array_iterator<T>;

			gsgl::index_t num_elements;
			simple_array<T *> buckets;

			const gsgl::index_t bucket_size;
			const bool no_init;

		public:
			object_array(const gsgl::index_t & initial_capacity = 0, const gsgl::index_t & bucket_size = 32, const bool & no_init = false);
			object_array(const object_array & oa);
			object_array & operator= (const object_array & oa);
			virtual ~object_array();

			/// \name Countable Implementation
			/// \{
			virtual gsgl::index_t size() const { return num_elements; }
			virtual void clear();
			/// \}


			/// \name Iterable Implementation
			/// \{
			using iterable::append;
			virtual void append(const T &);
			virtual void insert(const iterator & i, const T & item);
			virtual void remove(const iterator & i);
			/// \}


			/// \name Indexable Implementation
			/// \{
			virtual const T & item(const gsgl::index_t & index) const;
			virtual T & item(const gsgl::index_t & index);
			virtual bool contains_index(const gsgl::index_t & index) const { return index < num_elements; }
			/// \}

			/// \name Array Functionality
			/// \{
			void insert(const gsgl::index_t & index, const T & item);
			void remove(const gsgl::index_t & index);
			/// \}
		}; // class object_array


		template <typename T>
		object_array<T>::object_array(const gsgl::index_t & initial_capacity, const gsgl::index_t & bucket_size, const bool & no_init = false)
			: array_base(),
			  iterable<T, object_array_iterator<T> >(),
			  indexable<T, gsgl::index_t>(),
			  num_elements(0), buckets(initial_capacity/bucket_size + 1),
			  bucket_size(bucket_size), no_init(no_init)
		{
			for (int i = 0; i < (initial_capacity/bucket_size + 1); ++i)
			{
				buckets[i] = static_cast<T *>(allocate(sizeof(T) * bucket_size));
			}
		} // object_array<T>::object_array()


		template <typename T>
		object_array<T>::object_array(const object_array<T> & oa)
			: array_base(),
			  iterable<T, object_array_iterator<T> >(),
			  indexable<T, gsgl::index_t>(),
			  num_elements(oa.num_elements), buckets(oa.buckets.size()),
			  bucket_size(oa.bucket_size), no_init(oa.no_init)
		{
			for (int i = 0; i < oa.num_elements; ++i)
			{
				item(i) = oa.item(i);
			}
		} // object_array<T>::object_array()


		template <typename T>
		object_array<T> & object_array<T>::operator= (const object_array<T> & oa)
		{
			if (no_init != oa.no_init)
				throw internal_exception(__FILE__, __LINE__, L"You cannot assign object arrays whose initialization flags differ.");

			clear();

			for (int i = 0; i < oa.num_elements; ++i)
			{
				item(i) = oa.item(i);
			}

			// we keep our bucket size the same as before, so as not to reallocate everything
			num_elements = oa.num_elements;

            return *this;
		} // object_array<T>::operator= ()


		template <typename T>
		object_array<T>::~object_array()
		{
			clear();

			for (int i = 0; i < buckets.size(); ++i)
			{
				deallocate(buckets[i]);
			}
		} // object_array<T>::~object_array()


		/////////////////////////////

		template <typename T>
		void object_array<T>::clear()
		{
			// clean up all objects
			if (!no_init)
			{
				for (int i = 0; i < size(); ++i)
				{
					T *obj = &item(i);
					obj->~T();
				}
			}

			num_elements = 0;
		} // object_array<T>::clear()


		template <typename T>
		void object_array<T>::append(const T & item)
		{
			insert(size(), item);
		} // object_array<T>::append()


		template <typename T>
		void object_array<T>::insert(const iterator & i, const T & item_to_insert)
		{
			const object_array_iterator<T> & oi = dynamic_cast<const object_array_iterator<T> &>(i);
			insert(oi.position, item_to_insert);
		} // object_array<T>::insert()


		template <typename T>
		void object_array<T>::remove(const iterator & i)
		{
			const object_array_iterator<T> & oi = dynamic_cast<const object_array_iterator<T> &>(i);
			remove(oi.position);
		} // object_array<T>::remove()


		template <typename T>
		const T & object_array<T>::item(const gsgl::index_t & index) const
		{
			if (index < num_elements)
			{
				assert(buckets[index / bucket_size]);
				return buckets[index / bucket_size][index % bucket_size];
			}
			else
			{
				throw memory_exception(__FILE__, __LINE__, L"Index out of range in object array access.");
			}
		} // object_array<T>::item()


		template <typename T>
		T & object_array<T>::item(const gsgl::index_t & index)
		{
			// add new elements if necessary
			if (index >= num_elements)
			{
				// create new bucket(s)
				int target_bucket = index / bucket_size;
				for (int i = buckets.size(); i <= target_bucket; ++i)
				{
					buckets[i] = reinterpret_cast<T *>(allocate(sizeof(T) * bucket_size));
				}

				// initialize intermediate elements
				if (!no_init)
				{
					for (int i = num_elements; i < index+1; ++i)
					{
						new (&buckets[i / bucket_size][i % bucket_size]) T();
					}
				}

				num_elements = index+1;
			}

			assert(buckets[index/bucket_size]);
			return buckets[index/bucket_size][index % bucket_size];
		} // object_array<T>::item()


		template <typename T>
		void object_array<T>::insert(const gsgl::index_t & index, const T & item_to_insert)
		{
			for (int i = num_elements; i > index; --i)
			{
				item(i) = item(i-1); // this will adjust num_elements
			}

			item(index) = item_to_insert;
		} // object_array<T>::insert()


		template <typename T>
		void object_array<T>::remove(const gsgl::index_t & index)
		{
			if (!no_init)
			{
				(&item(index))->~T();
				new (&item(index)) T();
			}

			for (int i = index; i < num_elements-1; ++i)
			{
				item(i) = item(i+1);
			}

			if (!no_init)
			{
				(&item(num_elements-1))->~T();
			}

			--num_elements;
		} // object_array<T>::remove()



		/////////////////////////////

		template <typename T>
		class object_array_iterator
		{
			friend class object_array<T>;
			const object_array<T> & parent;
			gsgl::index_t position;
		protected:
			object_array_iterator(const iterable<T, object_array_iterator<T> > & parent_iterable)
				: parent(dynamic_cast<const object_array<T> &>(parent_iterable)), position(0) {}

			object_array_iterator(const object_array_iterator & i)
				: parent(i.parent), position(i.position) {}

			object_array_iterator & operator= (const object_array_iterator & i)
			{
				parent = i.parent;
				position = i.position;
				return *this;
			}

			inline bool is_valid() const { return position < parent.num_elements; }

			inline const T & operator*() const
			{
				if (position < parent.num_elements)
					return const_cast<T &>(parent.item(position));
				else
					throw memory_exception(__FILE__, __LINE__, L"Array iterator overflow in dereference operator.");
			}

			inline object_array_iterator & operator++ ()
			{
				if (position < parent.num_elements)
					++position;
				else
					throw memory_exception(__FILE__, __LINE__, L"Array iterator overflow in preincrement operator.");

				return *this;
			}
		}; // class object_array_iterator


    } // namespace data
    
} // namespace gsgl

#endif
