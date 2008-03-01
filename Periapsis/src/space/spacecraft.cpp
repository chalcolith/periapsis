//
// $Id: spacecraft.cpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "space/spacecraft.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;
using namespace gsgl::physics;

namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::spacecraft);


        spacecraft_module::spacecraft_module(const config_record & obj_config)
            : vehicle_module(obj_config)
        {
        } // spacecraft_module::spacecraft_module()


        spacecraft_module::~spacecraft_module()
        {
        } // spacecraft_module::~spacecraft_module()


        //

        spacecraft::spacecraft(const config_record & obj_config)
            : vehicle(obj_config)
        {
        } // spacecraft::spacecraft()


        spacecraft::~spacecraft()
        {
        } // spacecraft::~spacecraft()


        void spacecraft::init(gsgl::scenegraph::context *c)
        {
            vehicle::init(c);
        } // spacecraft::init()


        void spacecraft::draw(gsgl::scenegraph::context *c)
        {
            vehicle::draw(c);
        } // spacecraft::draw()


        static int accel = 0;


        void spacecraft::update(gsgl::scenegraph::context *c)
        {
            vehicle::update(c);

            double delta = c->cur_time - c->start_time;

            if (delta > 5 && delta < 10)
                accel = 1;
            else if (delta > 15 && delta < 20)
                accel = 2;
            else if (delta > 25 && delta < 30)
                accel = 3;
            else
                accel = 0;
        } // spacecraft::update()


        void spacecraft::cleanup(gsgl::scenegraph::context *c)
        {
            vehicle::cleanup(c);
        } // spacecraft::cleanup()


        void spacecraft::calculate_force_and_torque(const double & t, math::vector & force, math::vector & torque)
        {
            switch (accel)
            {
            case 1:
                force = vector(0, 0, 0);
                torque = vector(1000, 0, 0);
                break;
            case 2:
                force = vector(0, 0, 0);
                torque = vector(0, 1000, 0);
                break;
            case 3:
                force = vector(0, 0, 0);
                torque = vector(0, 0, 1000);
                break;
            case 0:
            default:
                force = vector::ZERO;
                torque = vector::ZERO;
                break;
            }
        } // spacecraft::calculate_force_and_torque()


    } // namespace space

} // namespace periapsis
