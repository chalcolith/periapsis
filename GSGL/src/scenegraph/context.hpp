#ifndef GSGL_SG_CONTEXT_H
#define GSGL_SG_CONTEXT_H

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

#include "scenegraph/scenegraph.hpp"
#include "platform/display.hpp"

#include <ctime>

namespace gsgl
{

    namespace scenegraph
    {
    
        class simulation;
        class node;
        class camera;
        
        /// The current game context.
        class SCENEGRAPH_API context
            : public scenegraph_object
        {
        public:
            simulation        *sim;     ///< The current simulation.
            platform::display *console; ///< The game console.            
            node              *scenery; ///< The root of the scene graph.

            platform::display *screen;  ///< The current screen (may be different from the console).
            node              *view;    ///< The current viewpoint node.
            camera            *cam;     ///< The current viewpoint's camera node.

            //
            int num_lights;             ///< The number of lights in the world.

            //
            gsgl::real_t time_scale;
            
            unsigned long frame;        ///< The current frame number.

            time_t start_t_time;        ///< The start time of the simulation.
            time_t cur_t_time;          ///< The current time.

            unsigned long start_tick;   ///< The starting tick value (in milliseconds).
            unsigned long cur_tick;     ///< The tick value at the start of the current frame.
            unsigned long delta_tick;   ///< The number of ticks since the last frame (milliseconds; NOT scaled).

            double start_time;          ///< In Unix time (seconds starting Jan 1, 1970).
            double cur_time;            ///< In Unix time (seconds starting Jan 1, 1970).
            double delta_time;          ///< Time since the last frame (in game-time seconds; i.e. may be scaled).

            double julian_start;        ///< The starting game time in Julian days.
            double julian_cur;          ///< The current game time in Julian days.
            double julian_dt;           ///< Time since the last frame (in game-time Julian days; i.e. may be scaled).

            //
            enum
            {
                RENDER_NO_FLAGS      = 0,
                RENDER_LABELS        = 1 << 0,
                RENDER_COORD_SYSTEMS = 1 << 1,
                RENDER_WIREFRAME     = 1 << 2,
                RENDER_NO_ANISO      = 1 << 3,
                RENDER_NO_LIGHTING   = 1 << 4,
                RENDER_NO_TEXTURES   = 1 << 5,
                RENDER_NO_NORMALMAP  = 1 << 6,
                RENDER_NO_HEIGHTMAP  = 1 << 7
            };

            gsgl::flags_t render_flags;

            //

            context();
            virtual ~context();
        }; // class context
        
    } // namespace scenegraph
    
} // namespace scenegraph

#endif
