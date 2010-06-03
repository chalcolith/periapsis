#ifndef GSGL_TEST_DATA_QUEUE_H
#define GSGL_TEST_DATA_QUEUE_H

//
// $Id: test_queue.hpp 12 2008-05-13 05:22:39Z kulibali $
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

#include "base/queue.hpp"
#include "unit_tester/unit_tester.hpp"

namespace test
{

    namespace data
    {

        class simple_queue_basic
        {
            gsgl::data::simple_queue<int> sq;

        public:
            simple_queue_basic() {}

            void test_001()
            {
                for (int i = 0; i < 10; ++i)
                    sq.push(i);
                TEST_ASSERT(sq.size() == 10);

                for (int i = 0; i < 5; ++i)
                {
                    int n = sq.front();
                    TEST_ASSERT(n == i);
                    sq.pop();
                }
                TEST_ASSERT(sq.size() == 5);

                for (int i = 0; i < 5; ++i)
                    sq.push(10+i);
                TEST_ASSERT(sq.size() == 10);

                for (int i = 5; i < 15; ++i)
                {
                    int n = sq.front();
                    TEST_ASSERT(n == i);
                    sq.pop();
                }
                TEST_ASSERT(sq.size() == 0);
            } // test_001()


            void test_002()
            {
                for (int i = 0; i < 50; ++i)
                    sq.push(i);

                for (int i = 0; i < 25; ++i)
                    sq.pop();

                for (int i = 0; i < 75; ++i)
                    sq.push(i);

                while (sq.size())
                {
                    int n = sq.front();
                    sq.pop();
                }
            } // test_001()

        }; // class simple_queue_basic

    } // namespace data

} // namespace test

#endif
