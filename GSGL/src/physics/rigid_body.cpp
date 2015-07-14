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

#include "physics/rigid_body.hpp"

namespace gsgl
{

    using namespace data;
    using namespace math;


    namespace physics
    {

        rigid_body_state & rigid_body_state::operator= (const rigid_body_state & s)
        {
            parent_body = s.parent_body;
            x = s.x;
            q = s.q;
            p = s.p;
            L = s.L;
            return *this;
        } // rigid_body_state::operator= ()


        rigid_body_state rigid_body_state::operator+ (const rigid_body_state & s) const
        {
            rigid_body_state result;

            result.parent_body = parent_body;
            result.x = x + s.x;
            result.q = q + s.q;
            result.p = p + s.p;
            result.L = L + s.L;

            return result;
        } // rigid_body_state::rigid_body_state()


        rigid_body_state rigid_body_state::operator* (const double & n) const
        {
            rigid_body_state result;

            result.parent_body = parent_body;
            result.x = x * static_cast<gsgl::real_t>(n);
            result.q = q * static_cast<gsgl::real_t>(n);
            result.p = p * static_cast<gsgl::real_t>(n);
            result.L = L * static_cast<gsgl::real_t>(n);

            return result;
        } // rigid_body_state::rigid_body_state()


        rigid_body_state rigid_body_state::derivative(const double & t) const
        {
            rigid_body_state result;
            result.parent_body = parent_body;
            
            parent_body->calculate_force_and_torque(t, parent_body->force, parent_body->torque);
            result.x = parent_body->mass_inverse * p;

            vector w = parent_body->j_inverse * L;
            result.q = (quaternion(w.get_w(), w.get_x(), w.get_y(), w.get_z()) * q) * 0.5f;

            result.p = parent_body->force;
            result.L = parent_body->torque;

            return result;
        } // rigid_body_state::derivative()



        //

        rigid_body::rigid_body(const config_record & obj_config)
            : physics_frame(obj_config), 
              center_of_mass(vector::ZERO), mass(1), mass_inverse(1),
              jbody(transform::IDENTITY), jbody_inverse(transform::IDENTITY)
        {
            motion_solver = new runge_kutta_solver<rigid_body_state>();
            //motion_solver = new euler_solver<rigid_body_state>();

            cur_state.parent_body = this;
        } // rigid_body::rigid_body()


        rigid_body::~rigid_body()
        {
        } // rigid_body::~rigid_body()


        /// \todo When saving, make sure to account for the center of mass...
        void rigid_body::init(const gsgl::scenegraph::simulation_context *c)
        {
            // calculate inertia tensor, mass and center of mass
            jbody = calculate_inertia_tensor(center_of_mass);
            jbody_inverse = jbody.inverse();
            mass_inverse = 1.0f / mass;

            // move the center of our space to the world-space position of our center of mass
            get_translation() = get_translation() + center_of_mass;

            // move sub-nodes to rotate about the center of mass
            for (simple_array<node *>::iterator i = get_children().iter(); i.is_valid(); ++i)
            {
                (*i)->get_translation() = (*i)->get_translation() - center_of_mass;
            }

            // initialize state
            cur_state.x = get_translation();
            cur_state.p = get_linear_velocity() * mass;
            cur_state.q = get_orientation();
            
            R = get_orientation();
            cur_state.L = (R * jbody_inverse * R.transpose()).inverse() * get_angular_velocity();

            compute_derived_quantities();
        } // rigid_body::init()


        void rigid_body::update(const gsgl::scenegraph::simulation_context *c)
        {
            // calculate next state
            cur_state = motion_solver->next(cur_state, c->cur_time, c->delta_time);
            compute_derived_quantities();

            // set scenegraph variables
            get_translation() = cur_state.x;
            get_orientation() = R;
            get_linear_velocity() = v;
            get_angular_velocity() = w;
        } // rigid_body::update()


        void rigid_body::compute_derived_quantities()
        {
            cur_state.q.normalize();
            R = cur_state.q;
            j_inverse = R * jbody_inverse * R.transpose();
            v = mass_inverse * cur_state.p;
            w = j_inverse * cur_state.L;
        } // rigid_body::compute_derived_quantities()


    } // namespace physics

} // namespace gsgl
