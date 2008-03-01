#ifndef GSGL_TEST_MATH_TRANSFORM_H
#define GSGL_TEST_MATH_TRANSFORM_H

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

#include "math/transform.hpp"
#include "math/quaternion.hpp"

#include "unit_tester.hpp"

namespace test
{

    namespace math
    {

        class points
        {
        public:

            void test_points()
            {
                using namespace gsgl::math;

                quaternion q(vector::Z_AXIS, static_cast<gsgl::real_t>(10.0 * DEG2RAD));
                transform t(q);

                vector a(1.0f, 0.0f, 0.0f);
                vector b = t * a;

                int i = 0;
                i = 1;
            } // test_points()


            void write_t(gsgl::string & str, gsgl::math::transform & t)
            {
                for (int row = 0; row < 4; ++row)
                {
                    for (int col = 0; col < 4; ++col)
                    {
                        str += gsgl::string::format(L"%f, ", t.item(row, col));
                    }

                    str += L"\n";
                }
            }


            void test_rot()
            {
                using namespace gsgl::math;

                gsgl::real_t angle = static_cast<gsgl::real_t>(90.0 * DEG2RAD);

                quaternion x_pos_q(vector::X_AXIS, angle);
                quaternion x_neg_q(vector::X_AXIS, -angle);

                quaternion y_pos_q(vector::Y_AXIS, angle);
                quaternion y_neg_q(vector::Y_AXIS, -angle);

                quaternion z_pos_q(vector::Z_AXIS, angle);
                quaternion z_neg_q(vector::Z_AXIS, -angle);

                transform x_pos_t(x_pos_q);
                transform x_neg_t(x_neg_q);

                transform y_pos_t(y_pos_q);
                transform y_neg_t(y_neg_q);

                transform z_pos_t(z_pos_q);
                transform z_neg_t(z_neg_q);

                gsgl::string str;

                write_t(str, x_pos_t);
                write_t(str, x_neg_t);
                write_t(str, y_pos_t);
                write_t(str, y_neg_t);
                write_t(str, z_pos_t);
                write_t(str, z_neg_t);



                int i = 0;
                i = 1;
            } // test_rot()


        }; // class points

    } // namespace math

} // namespace test

#endif
