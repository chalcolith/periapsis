#ifndef GSGL_TEST_DATA_DICTIONARY_H
#define GSGL_TEST_DATA_DICTIONARY_H

//
// $Id: test_dictionary.hpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "base/string.hpp"
#include "base/dictionary.hpp"
#include "unit_tester/unit_tester.hpp"

namespace test
{

    namespace data
    {

        //

        class dict_mixed
        {
            gsgl::data::dictionary<int, int> d;

        public:

            dict_mixed()
            {
                d[0] = 0;
                d[9] = 9;
                d[1] = 1;
                d[8] = 8;
                d[2] = 2;
                d[7] = 7;
                d[3] = 3;
                d[6] = 6;
                d[4] = 4;
                d[5] = 5;
            } // dict_mixed()


            void test_size()
            {
                TEST_ASSERT(d.size() == 10);
            } // test_size()


            void test_order()
            {
                for (gsgl::data::dictionary<int, int>::iterator i = d.iter(); i.is_valid(); ++i)
                {
                    TEST_ASSERT(d[i.get_index()] == *i);
                }

                for (int i = 0; i < 10; ++i)
                {
                    TEST_ASSERT(d[i] == i);
                }
            } // test_order()

        }; // class dict_mixed


        //


        class dict_mixed2
        {
            gsgl::data::dictionary<int, int> d;
        public:

            dict_mixed2()
            {
                d[5] = 5;
                d[3] = 3;
                d[4] = 4;
                d[1] = 1;
                d[2] = 2;
                d[0] = 0;
                d[8] = 8;
                d[6] = 6;
                d[7] = 7;
                d[9] = 9;
            }; // dict_mixed2()

            void test_size()
            {
                TEST_ASSERT(d.size() == 10);
            } // test_size()


            void test_order()
            {
                for (gsgl::data::dictionary<int, int>::iterator i = d.iter(); i.is_valid(); ++i)
                {
                    TEST_ASSERT(d[i.get_index()] == *i);
                }

                for (int i = 0; i < 10; ++i)
                {
                    TEST_ASSERT(d[i] == i);
                }
            } // test_order()

        }; // class dict_mixed2


        //

        
        class dict_balanced
        {
            gsgl::data::dictionary<int, int> d;

        public:

            dict_balanced()
            {
                d[5] = 5;
                d[4] = 4;
                d[6] = 6;
                d[3] = 3;
                d[7] = 7;
                d[2] = 2;
                d[8] = 8;
                d[1] = 1;
                d[9] = 9;
                d[0] = 0;
            } // dict_balanced()


            void test_size()
            {
                TEST_ASSERT(d.size() == 10);
            } // test_size()


            void test_order()
            {
                for (gsgl::data::dictionary<int, int>::iterator i = d.iter(); i.is_valid(); ++i)
                {
                    TEST_ASSERT(d[i.get_index()] == *i);
                }

                for (int i = 0; i < 10; ++i)
                {
                    TEST_ASSERT(d[i] == i);
                }
            } // test_order()

        }; // class dict_balanced
        
        //


        class dict_right
        {
            gsgl::data::dictionary<int, int> d;

        public:
            dict_right()
            {
                for (int i = 0; i < 10; ++i)
                    d[i] = i;
            } // dict_unbalanced()


            void test_size()
            {
                TEST_ASSERT(d.size() == 10);
            } // test_size()


            void test_order()
            {
                int n = 0;
                for (gsgl::data::dictionary<int, int>::iterator i = d.iter(); i.is_valid(); ++n, ++i)
                {
                    TEST_ASSERT(*i == n);
                }

                for (int i = 0; i < 10; ++i)
                {
                    TEST_ASSERT(d[i] == i);
                }
            } // test_order()

        }; // class dict_right


        //


        class dict_left
        {
            gsgl::data::dictionary<int, int> d;

        public:
            dict_left()
            {
                for (int i = 10; i > 0; --i)
                    d[i-1] = i-1;
            } // dict_left()


            void test_size()
            {
                TEST_ASSERT(d.size() == 10);
            } // test_size()


            void test_order()
            {
                int n = 0;
                for (gsgl::data::dictionary<int, int>::iterator i = d.iter(); i.is_valid(); ++n, ++i)
                {
                    TEST_ASSERT(*i == n);
                }

                for (int i = 0; i < 10; ++i)
                {
                    TEST_ASSERT(d[i] == i);
                }
            } // test_order()

        }; // class dict_left


        class dict_remove
        {
            gsgl::data::dictionary<int, gsgl::string> d;
        public:

            dict_remove()
            {
                d[L"one"]   = 1;
                d[L"two"]   = 2;
                d[L"three"] = 3;
                d[L"four"]  = 4;
                d[L"five"]  = 5;
                d[L"six"]   = 6;
                d[L"seven"] = 7;
                d[L"eight"] = 8;
                d[L"nine"]  = 9;
                d[L"ten"]   = 10;
            } // dict_remove()


            void check_missing(unsigned int missing)
            {
                unsigned int mask = 0;

                for (int i = 0; i < 10; ++i)
                {
                    mask |= (1 << (i+1));
                }

                for (gsgl::data::dictionary<int, gsgl::string>::iterator i = d.iter(); i.is_valid(); ++i)
                {
                    mask ^= (1 << *i);
                }

                TEST_ASSERT(mask == missing);
            }


            void test_remove()
            {
                unsigned int bits = 0;

                d.remove(L"three"); TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 3));

                d.remove(L"four");  TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 4));
                
                d.remove(L"seven"); TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 7));
                
                d.remove(L"eight"); TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 8));
                
                d.remove(L"five");  TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 5));
                
                d.remove(L"six");   TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 6));
                
                d.remove(L"nine");  TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 9));
                
                d.remove(L"ten");   TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 10));
                
                d.remove(L"one");   TEST_ASSERT(!d.is_empty());
                check_missing(bits |= (1 << 1));
                
                d.remove(L"two");   TEST_ASSERT(d.is_empty());
            } // test_remove()

        }; // class dict_remove

    } // namespace data

} // namespace test

#endif
