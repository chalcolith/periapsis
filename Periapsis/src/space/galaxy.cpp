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

#include "space/galaxy.hpp"
#include "math/units.hpp"
#include "scenegraph/utils.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;

namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::galaxy);

        static config_variable<gsgl::platform::color> GALAXY_COORD_COLOR(L"space/galaxy/galaxy_coord_color", gsgl::platform::color(0.8f, 0.8f, 0.8f, 0.5f));


        galaxy::galaxy(const config_record & conf)
            : node(conf), cs(0)
        {
            set_flags(get_draw_flags(), node::NODE_NO_FRUSTUM_CHECK);
            cs = new utils::coord_system(this, 1 * units::METERS_PER_PARSEC, 15, GALAXY_COORD_COLOR);
        } // galaxy::galaxy()


        galaxy::~galaxy()
        {
            delete cs;
        } // galaxy::~galaxy()


        gsgl::real_t galaxy::draw_priority(const simulation_context *, const drawing_context *)
        {
            return NODE_DRAW_FIRST;
        } // galaxy::draw_priority()


        void galaxy::init(const simulation_context *c)
        {
            cs->init(c);
        } // galaxy::init()


        void galaxy::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            if (draw_context->render_flags & drawing_context::RENDER_COORD_SYSTEMS)
            {
                cs->draw(sim_context, draw_context);
            }
        } // galaxy::draw()

    } // namespace space

} // namespace periapsis
