#ifndef GSGL_TEST_DATA_ARRAY_HPP
#define GSGL_TEST_DATA_ARRAY_HPP

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


#include "data/array.hpp"
#include "unit_tester.hpp"

namespace test
{

	namespace data
	{

		class array_simple
		{
		public:
			array_simple() {}

			void test_insert()
			{
				gsgl::data::simple_array<int> aa;

				for (int i = 0; i < 5; ++i)
					aa[i] = i*2;

				for (int i = 0; i < 5; ++i)
					aa.insert(i*2+1, i*2+1);

				int aaa[10];
				for (int i = 0; i < 10; ++i)
					aaa[i] = aa[i];

				for (int i = 0; i < 10; ++i)
					TEST_ASSERT(aa[i] == i);
			}


			void test_remove()
			{
				gsgl::data::simple_array<int> aa;

				for (int i = 0; i < 10; ++i)
					aa[i] = i;

				for (int i = 0; i < 10; ++i)
					TEST_ASSERT(aa[i] == i);

				int pos = 1;
				for (int i = 0; i < 10; ++i)
				{
					if (i % 2 == 1)
						aa.remove(pos++);
				}

				for (int i = 0; i < 5; ++i)
					TEST_ASSERT(aa[i] == i*2);
			}


			void test_iter_traverse()
			{
				gsgl::data::simple_array<int> aa;

				for (int i = 0; i < 10; ++i)
					aa[i] = i;

				int num = 0;
				for (gsgl::data::simple_array<int>::iterator i = aa.iter(); i.is_valid(); ++i, ++num)
					TEST_ASSERT(*i == num);

				gsgl::data::simple_array<int>::iterator iter = aa.iter();
				for (int i = 0; i < 5; ++i)
					++iter;

				for (int i = 5; i < 10; ++i, ++iter) 
					TEST_ASSERT(*iter == i);
			}


			void test_iter_insert()
			{
				gsgl::data::simple_array<int> aa;

			}


		}; // class array_simple



		class array_object
		{
		public:

			template <typename T>
			class temp_data
			{
			public:
				T val;

				temp_data() : val(T()) {}
				temp_data(const T & val) : val(val) {}
				temp_data(const temp_data & td) : val(td.val) {}
				temp_data & operator= (const temp_data & td) { val = td.val; return *this; }
				virtual ~temp_data() {}

				bool operator== (const temp_data & td) const { return val == td.val; }
				bool operator!= (const temp_data & td) const { return val != td.val; }
				bool operator<  (const temp_data & td) const { return val < td.val; }
				bool operator>  (const temp_data & td) const { return val > td.val; }
			}; // class temp_data


			array_object() {}


			void test_insert()
			{
				const int NUM = 500;

				gsgl::data::object_array< temp_data<int> > aa;

				for (int i = 0; i < NUM; ++i)
					aa[i].val = i*2;

				for (int i = 0; i < NUM; ++i)
					aa.insert(i*2+1, i*2+1);

				int aaa[NUM*2];
				for (int i = 0; i < NUM*2; ++i)
					aaa[i] = aa[i].val;

				for (int i = 0; i < NUM*2; ++i)
					TEST_ASSERT(aa[i].val == i && aaa[i] == i);
			}


			void test_remove()
			{
				const int NUM = 500;

				gsgl::data::object_array< temp_data<int> > aa;

				for (int i = 0; i < NUM*2; ++i)
					aa[i].val = i;

				for (int i = 0; i < NUM*2; ++i)
					TEST_ASSERT(aa[i].val == i);

				int pos = 1;
				for (int i = 0; i < NUM*2; ++i)
				{
					if (i % 2 == 1)
						aa.remove(pos++);
				}

				for (int i = 0; i < NUM; ++i)
					TEST_ASSERT(aa[i].val == i*2);
			}


			void test_iter_traverse()
			{
				const int NUM = 500;

				gsgl::data::object_array< temp_data<int> > aa;

				for (int i = 0; i < NUM*2; ++i)
					aa[i].val = i;

				int num = 0;
				for (gsgl::data::object_array< temp_data<int> >::iterator i = aa.iter(); i.is_valid(); ++i, ++num)
					TEST_ASSERT(i->val == num);

				gsgl::data::object_array< temp_data<int> >::iterator iter = aa.iter();
				for (int i = 0; i < 5; ++i)
					++iter;

				for (int i = 5; i < NUM*2; ++i, ++iter)
					TEST_ASSERT(iter->val == i);
			}


		}; // class array_object


	} // namespace data

} // namespace test

#endif
