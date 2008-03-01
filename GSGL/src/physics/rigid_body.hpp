#ifndef GSGL_PHYSICS_RIGID_BODY_H
#define GSGL_PHYSICS_RIGID_BODY_H

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
#include "physics/physics_frame.hpp"

#include "math/vector.hpp"
#include "math/transform.hpp"
#include "math/quaternion.hpp"
#include "math/solver.hpp"

namespace gsgl
{

    namespace physics
    {

        class rigid_body;


        struct PHYSICS_API rigid_body_state
        {
            rigid_body *parent_body;

            // state variables
            math::vector x;     // position of the center of mass
            math::quaternion q; // orientation
            math::vector p;     // linear momentum
            math::vector L;     // angular momentum

            rigid_body_state & operator= (const rigid_body_state & s);

            rigid_body_state operator+ (const rigid_body_state & s) const;
            rigid_body_state operator* (const double & n) const;
            rigid_body_state derivative(const double & t) const;
        }; // class rigid_body_state



        class PHYSICS_API rigid_body
            : public physics_frame
        {
        protected:
            // constants
            math::vector center_of_mass;

            gsgl::real_t mass, mass_inverse;
            math::transform jbody, jbody_inverse;

            // state variables
            rigid_body_state cur_state;
            friend struct rigid_body_state;

            // derived quantities
            math::transform R;  // orientation
            math::vector v;     // linear velocity
            math::vector w;     // angular velocity

            math::transform j_inverse; // inverse inertia tensor in world coordinates

            // computed quantities
            math::vector force, torque;

            // differential equation solver
            math::solver<rigid_body_state> *motion_solver;

        public:
            rigid_body(const data::config_record & obj_config);
            virtual ~rigid_body();

            gsgl::real_t & get_total_mass() { return mass; }

            virtual math::transform calculate_inertia_tensor(math::vector & center_of_mass) = 0;

            /// \note Forces & torques are in world coordinates, and the time is in UNIX time (seconds)!
            virtual void calculate_force_and_torque(const double & t, math::vector & force, math::vector & torque) = 0;

            // node implementation
            virtual void init(gsgl::scenegraph::context *c);
            virtual void update(gsgl::scenegraph::context *c);

        private:
            void compute_derived_quantities();
        }; // class rigid_body

    } // namespace physics

} // namespace gsgl

#endif
