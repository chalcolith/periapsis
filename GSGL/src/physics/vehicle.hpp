#ifndef GSGL_PHYSICS_VEHICLE_HPP
#define GSGL_PHYSICS_VEHICLE_HPP

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
#include "physics/rigid_body.hpp"

#include "data/array.hpp"
#include "data/broker.hpp"

namespace gsgl
{

    namespace physics
    {

        class vehicle_module;


        class PHYSICS_API vehicle
            : public rigid_body
        {
            data::simple_array<vehicle_module *> modules;

        public:
            vehicle(const gsgl::data::config_record & obj_config);
            virtual ~vehicle();

            const data::simple_array<vehicle_module *> & get_modules() const { return modules; }

            // node implementation
            virtual gsgl::real_t draw_priority(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);
            
            virtual gsgl::real_t default_view_distance() const;
            virtual gsgl::real_t minimum_view_distance() const;

            virtual void init(const gsgl::scenegraph::simulation_context *);
            virtual void draw(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);
            virtual void update(const gsgl::scenegraph::simulation_context *);
            virtual void cleanup(const gsgl::scenegraph::simulation_context *);

            // rigid_body implementation
            virtual gsgl::math::transform calculate_inertia_tensor(gsgl::math::vector & center_of_mass);
        }; // class vehicle

    } // namespace physics

} // namespace gsgl

#endif
