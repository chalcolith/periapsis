#ifndef GSGL_SG_FREEVIEW_H
#define GSGL_SG_FREEVIEW_H

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
#include "scenegraph/node.hpp"
#include "scenegraph/event.hpp"
#include "scenegraph/camera.hpp"

#include "math/vector.hpp"
#include "math/quaternion.hpp"
#include "data/string.hpp"
#include "data/broker.hpp"

namespace gsgl
{

    namespace scenegraph
    {
    
        /// A freely rotatable viewpoint.
        class SCENEGRAPH_API freeview 
            : public node
        {
            camera *cam;
            sg_event::event_code last_code;
            bool relative;

            unsigned long transition_end_tick;
            gsgl::real_t transition_angular_velocity;
            math::vector transition_axis;

            gsgl::real_t transition_linear_velocity;
            math::vector transition_path;

        public:
            freeview(const gsgl::string & name, node *parent);
            freeview(const data::config_record & conf);
            virtual ~freeview();
            
            camera *get_camera() { return cam; }
            bool is_relative() const { return relative; }

            void reset();
            
            virtual void update(const gsgl::scenegraph::simulation_context *c);
            virtual bool handle_event(const gsgl::scenegraph::simulation_context *c, sg_event & e);

            BROKER_DECLARE_CREATOR(gsgl::scenegraph::freeview);

        private:
            bool rot_absolute(sg_event::event_code code, const gsgl::scenegraph::simulation_context *c);
            bool rot_relative(sg_event::event_code code, const gsgl::scenegraph::simulation_context *c);
        }; // class freeview
        
    } // namespace scenegraph
    
} // namespace gsgl

#endif
