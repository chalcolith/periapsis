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

#include "space/planet_system.hpp"
#include "space/astronomy.hpp"

#include "math/math.hpp"
#include "math/units.hpp"
#include "platform/color.hpp"
#include "platform/display.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;
using namespace gsgl::physics;
using namespace gsgl::platform;


namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::planet_system);

        static config_variable<gsgl::platform::color> PLANET_COORD_COLOR(L"space/planet_system/planet_coord_color", gsgl::platform::color(0.0f, 0.0f, 1.0f, 0.5f));


        planet_system::planet_system(const config_record & conf)
            : orbital_frame(conf), cs(0)
        {
            get_draw_flags() |= node::NODE_NO_FRUSTUM_CHECK;
            cs = new utils::coord_system(this, 10000*units::METERS_PER_AU, 15, PLANET_COORD_COLOR);
        } // planet_system::planet_system()


        planet_system::~planet_system()
        {
            delete cs;
        } // planet_system::~planet_system()


        gsgl::real_t planet_system::draw_priority(const simulation_context *, const drawing_context *)
        {
            if (get_name() == L"Earth Barysystem")
            {
                return utils::pos_in_eye_space(this).mag2() * 10;
            }
            else
            {
                return NODE_DRAW_IGNORE;
            }
        } // planet_system::draw_priority()

        


        void planet_system::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            if ((draw_context->render_flags & drawing_context::RENDER_COORD_SYSTEMS) && get_name() == L"Earth Barysystem")
            {
                // the earth barysystem is still oriented to the ecliptic; we want to display the equatorial, but not move...
                display::scoped_modelview mv(*draw_context->screen, 0);
                mv.mult(EQUATORIAL_WRT_ECLIPTIC);

                cs->draw(sim_context, draw_context);
            }
        } // planet_system::draw()


    } // namespace space

} // namespace periapsis
