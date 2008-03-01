#ifndef GSGL_FRAMEWORK_SIMULATION_H
#define GSGL_FRAMEWORK_SIMULATION_H

//
// $Id: simulation.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "scenegraph/node.hpp"
#include "scenegraph/context.hpp"
#include "scenegraph/freeview.hpp"
#include "platform/display.hpp"
#include "platform/font.hpp"
#include "data/config.hpp"
#include "data/list.hpp"


namespace gsgl
{

    namespace scenegraph
    {
    
        /// Encapsulates a simulation.
        class SCENEGRAPH_API simulation
            : public scenegraph_object
        {
            bool running;

            platform::display *console;  ///< The game's single console.
            context *sim_context;

            node *scenery;  ///< The root node of the simulation scene graph.
            data::list<node *> non_scenery_nodes; ///< These get removed from the scene graph & deleted when the simulation ends.

            freeview *view; ///< The main view node.
            
            time_t start_time;
            gsgl::real_t time_scale;

            platform::font *info_font;
            data::simple_array<gsgl::real_t> frame_deltas;

            node::pre_draw_rec pre_rec;

        public:
            simulation(const data::config_record & sim_config, 
                       platform::display *console, 
                       context *sim_context,
                       scenegraph::node *scenery);
            ~simulation();

            bool is_running() const { return running; }
            context *get_context() { return sim_context; }
            
            void init();     ///< This is called at the beginning of a simulation.  It will initialize all the nodes in the scene graph.
            void pre_draw(); ///< This is called at the beginning of each frame.  It records drawing information for all the nodes in the scene graph.

            void draw();     ///< This is called to draw the frame.  It may be called concurrently with update(), which is updating the next frame while the current one is being drawn.
            void update();   ///< This is called to update the scene graph.  It may be called concurrently with draw(), which is drawing the previous frame.
            void cleanup();  ///< This is called at the end of the simulation.
            
            bool handle_event(sg_event & e); ///< This is called to pass events to the scene graph after they have been left unhandled by the UI.
            
        private:
            void init_context();
            void update_context();
            void cleanup_context();

            void init_node(scenegraph::node *n);
            void update_node(scenegraph::node *n);
            void cleanup_node(scenegraph::node *n);
            bool handle_event(sg_event & e, node *n);
        }; // class simulation


    } // namespace scenegraph
    
} // namespace gsgl

#endif
