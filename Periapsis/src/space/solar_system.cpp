//
// $Id: solar_system.cpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "space/solar_system.hpp"
#include "space/astronomy.hpp"
#include "data/string.hpp"
#include "math/units.hpp"

#include "platform/color.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;

namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::solar_system);

        static config_variable<gsgl::platform::color> SS_COORD_COLOR(L"space/solar_system/solar_system_coord_color", gsgl::platform::color(0.8f, 0.8f, 0.0f, 0.5f));


        solar_system::solar_system(const config_record & conf)
            : node(conf), cs(0)
        {
            get_draw_flags() |= node::NODE_NO_FRUSTUM_CHECK;

            string oname = conf[L"transform"];
            if (!oname.is_empty())
            {
                if (oname == L"ecliptic_wrt_galactic")
                {
                    get_orientation() = ECLIPTIC_WRT_GALACTIC;
                }
                else
                {
                    throw runtime_exception(L"Unknown transform name %ls in %ls.", oname.w_string(), conf.get_file().get_full_path().w_string());
                }
            }

            cs = new utils::coord_system(this, 10000 * units::METERS_PER_AU, 15, SS_COORD_COLOR);
        } // solar_system::solar_system()


        solar_system::~solar_system()
        {
            delete cs;
        } // solar_system::~solar_system()


        gsgl::real_t solar_system::get_priority(context *)
        {
            return NODE_DRAW_FIRST / 100.0f;
        } // solar_system::get_priority()


        void solar_system::init(context *c)
        {
            cs->init(c);
        } // solar_system::init()


        void solar_system::draw(context *c)
        {
            if ((c->render_flags & context::RENDER_COORD_SYSTEMS) && get_name() == L"Sol System")
            {
                cs->draw(c);
            }
        } // solar_system::draw()

    } // namespace space

} // namespace periapsis
