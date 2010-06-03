#ifndef GSGL_TEST_DATA_FD_STREAM_H
#define GSGL_TEST_DATA_FD_STREAM_H

//
// $Id: test_fd_stream.hpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "base/fstream.hpp"
#include "unit_tester/unit_tester.hpp"

namespace test
{

    namespace data
    {

        class floats_constant
        {
            static const int NUM_FLOATS = 1000;
            float *buf;
        public:
            floats_constant()
                : buf(0)
            {
                buf = new float[NUM_FLOATS];
                for (int i = 0; i < NUM_FLOATS; ++i)
                    buf[i] = 1.0f;
            } // floats_constant()

            ~floats_constant()
            {
                delete [] buf;
            } // ~floats_constant()


            void test_rw()
            {
                {
                    gsgl::string fname(L"test_constant.fd");

                    gsgl::io::fd_stream f(fname, gsgl::io::FILE_OPEN_WRITE);
                    for (int i = 0; i < NUM_FLOATS; ++i)
                    {
                        f << buf[i];
                    }
                }

                {
                    gsgl::io::fd_stream f(L"test_constant.fd");
                    for (int i = 0; i < NUM_FLOATS; ++i)
                    {
                        float num;
                        f >> num;

                        TEST_ASSERT(num == buf[i]);
                    }
                }
            } // test_rw()

        }; // class floats_constant


        class floats_random
        {
            static const int NUM_FLOATS = 1000;
            
            float *buf;
        public:
            floats_random()
                : buf(0)
            {
                buf = new float[NUM_FLOATS];
                for (int i = 0; i < NUM_FLOATS; ++i)
                {
                    float num = ((float) rand()) / ((float) RAND_MAX);
                    buf[i] = num;
                }
            } // floats_random()

            ~floats_random()
            {
                delete [] buf;
            } // ~floats_random()


            void test_rw()
            {
                {
                    gsgl::io::fd_stream f(L"test_random.fd", gsgl::io::FILE_OPEN_WRITE);
                    for (int i = 0; i < NUM_FLOATS; ++i)
                    {
                        f << buf[i];
                    }
                }

                {
                    gsgl::io::fd_stream f(L"test_random.fd");
                    for (int i = 0; i < NUM_FLOATS; ++i)
                    {
                        float num;
                        f >> num;

                        TEST_ASSERT(num == buf[i]);
                    }
                }
            } // test_rw()

        }; // class floats_random

    } // namespace data

} // namespace test

#endif
