#ifndef GSGL_PHYSICS_PHYSICS_FRAME_H
#define GSGL_PHYSICS_PHYSICS_FRAME_H

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

#include "physics/physics.hpp"

#include "math/vector.hpp"
#include "math/transform.hpp"
#include "math/quaternion.hpp"
#include "math/solver.hpp"
#include "scenegraph/node.hpp"

namespace gsgl
{

    namespace physics
    {

        /// Base class for moving and rotating frames that vehicles (or other moving objects) may transition between.
        class PHYSICS_API physics_frame            
            : public gsgl::scenegraph::node
        {            
            math::vector linear_velocity;
            math::vector angular_velocity;

        public:
            physics_frame(const gsgl::string & name, gsgl::scenegraph::node *parent);
            physics_frame(const gsgl::data::config_record & conf);
            virtual ~physics_frame();

            math::vector & get_linear_velocity() { return linear_velocity; }
            math::vector & get_angular_velocity() { return angular_velocity; }
        }; // class physics_frame

    } // namespace physics

} // namespace gsgl

#endif
