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

#include "scenegraph/test_frame.hpp"
#include "math/quaternion.hpp"

#include "platform/hardware.hpp"

#include <math.h>

namespace gsgl
{

    using namespace data;
    using namespace math;

    namespace scenegraph
    {

        BROKER_DEFINE_CREATOR(gsgl::scenegraph::test_frame);


        test_frame::test_frame(const config_record & obj_conf, const config_record & sys_conf)
            : node(obj_conf, sys_conf), rotate(true), draw_me(true), angle(0), radius(10.0f)
        {
            //if (obj_conf[L"rotate"].to_lower() == L"false")
            //    rotate = false;
            //if (obj_conf[L"draw"].to_lower() == L"false")
            //    draw_me = false;
            //if (!obj_conf[L"radius"].is_empty())
            //    radius = static_cast<gsgl::real_t>(obj_conf[L"radius"].to_double());
        } // test_frame::test_frame()
        

        test_frame::~test_frame()
        {
        } // test_frame::~test_frame()


        void test_frame::init(context *)
        {
            if (rotate)
            {
                angle = 0;
                recalc();
            }
        } // test_frame::init()


        void test_frame::draw(context *c)
        {
            if (draw_me)
            {
                glColor4f(0.0f, 1.0f, 0.0f, 1.0f);
                draw_coordinate_system(c, radius);
            }
        } // test_frame::draw()


        void test_frame::update(context *c)
        {
            if (draw_me)
            {
                angle = static_cast<gsgl::real_t>((::fmod((c->cur_time - c->start_time) * c->time_scale, 60.0) / 60.0f) * math::PI * 2.0);
                recalc();
            }
        } // test_frame::update()


        gsgl::real_t test_frame::get_priority(context *)
        {
            return pos_in_eye_space().mag2();
        } // test_frame::get_priority()


        void test_frame::recalc()
        {
            quaternion rq(vector::Z_AXIS, angle);
            quaternion qq(vector::X_AXIS, static_cast<gsgl::real_t>(-23.0 * math::DEG2RAD));

            get_orientation() = qq * rq;
        } // test_frame::recalc()

    } // namespace scenegraph

} // namespace gsgl
