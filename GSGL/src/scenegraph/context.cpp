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

#include "scenegraph/context.hpp"
#include "scenegraph/node.hpp"

#include "platform/display.hpp"


namespace gsgl
{

    using namespace platform;

    namespace scenegraph
    {
    
        simulation_context::simulation_context() 
            : sim(0), 
              scenery(0),
              time_scale(1),
              frame(0),
              start_t_time(0),
              cur_t_time(0),
              start_tick(0),
              cur_tick(0),
              delta_tick(0),
              start_time(0),
              cur_time(0),
              delta_time(0),
              julian_start(0),
              julian_cur(0),
              julian_dt(0)
        {
        } // simulation_context::simulation_context()
        

        simulation_context::~simulation_context()
        {
        } // simulation_context::~simulation_context()


        drawing_context::drawing_context()
            : console(0), 
              screen(0), 
              view(0), 
              cam(0), 
              num_lights(0),
              render_flags(RENDER_NO_FLAGS)
        {
        } // drawing_context::drawing_context()


        drawing_context::drawing_context(const drawing_context & dc)
            : console(dc.console),
              screen(dc.screen),
              view(dc.view),
              cam(dc.cam),
              num_lights(dc.num_lights),
              render_flags(dc.render_flags)
        {
        } // drawing_context::drawing_context()


        drawing_context::~drawing_context()
        {
        } // drawing_context::~drawing_context()


        drawing_context *drawing_context::copy()
        {
            return new drawing_context(*this);
        } // drawing_context::copy()


        gsgl::flags_t drawing_context::display_flags(node *n, const flags_t & extra_flags) const
        {
            gsgl::flags_t flags = display::ENABLE_ALL;

            flags_t flags_to_test = render_flags | extra_flags;

            if (flag_is_set(flags_to_test, RENDER_WIREFRAME))
                flags &= ~display::ENABLE_FILLED_POLYS;

            if (flag_is_set(flags_to_test, RENDER_NO_TEXTURES))
                flags &= ~display::ENABLE_TEXTURES;

            if (flag_is_set(flags_to_test, RENDER_NO_LIGHTING) || (n && flag_is_set(n->get_draw_flags(), node::NODE_DRAW_UNLIT)))
                flags &= ~display::ENABLE_LIGHTING;

            if (flag_is_set(flags_to_test, RENDER_NO_DEPTH))
                flags &= ~display::ENABLE_DEPTH;

            return flags;
        } // drawing_context::display_flags()

    } // namespace scenegraph
    
} // namespace gsgl
