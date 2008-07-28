#ifndef PERIAPSIS_SPACE_CARTO_BODY_H
#define PERIAPSIS_SPACE_CARTO_BODY_H

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

#include "space/space.hpp"
#include "data/broker.hpp"
#include "scenegraph/node.hpp"
#include "physics/physics_frame.hpp"

namespace periapsis
{

    namespace space
    {

        /// Base class for celestial body rotators.
        class SPACE_API body_rotator
            : public gsgl::data::brokered_object
        {
        public:        
            virtual void calc_orientation(double jdn, gsgl::math::transform & orientation, gsgl::math::vector & angular_velocity) = 0;

        protected:
            void calc_orientation_aux(double alpha, double delta, double W, gsgl::math::transform & orientation);
            void calc_angular_velocity_aux(double ang_diff, double d, const gsgl::math::transform & orientation, gsgl::math::vector & angular_velocity);
        }; // class body_rotator


        /// Implements bodies that are rotated using a body_rotator.
        class SPACE_API rotating_body
            : public gsgl::physics::physics_frame
        {
            body_rotator *rotator;

        public:
            rotating_body(const gsgl::string & name, gsgl::scenegraph::node *parent, body_rotator *rotator);
            virtual ~rotating_body();

            virtual void init(const gsgl::scenegraph::simulation_context *);
            virtual void update(const gsgl::scenegraph::simulation_context *);
        }; // class rotating_body

    } // namespace space

} // namespace periapsis

#endif
