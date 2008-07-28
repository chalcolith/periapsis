#ifndef PERIAPSIS_SPACE_CELESTIAL_BODY_H
#define PERIAPSIS_SPACE_CELESTIAL_BODY_H

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
#include "space/orbital_frame.hpp"
#include "space/rotating_body.hpp"
#include "space/atmosphere.hpp"
#include "space/lithosphere.hpp"

#include "platform/material.hpp"
#include "scenegraph/utils.hpp"

namespace periapsis
{

    namespace space
    {


        /// Base class for celestial bodies: stars, planets, asteroids, etc.
        /// This node represents the inertial (nonrotating) frame centered at the center of the body.
        /// The body's lithosphere (in the case of a rocky body) or atmosphere (in the case of a gas body) represents the inclined and rotating frame.
        class SPACE_API celestial_body
            : public orbital_frame
        {
            gsgl::real_t mass;
            gsgl::real_t polar_radius;
            gsgl::real_t equatorial_radius;

            rotating_body *rotating_frame;

            atmosphere  *atmo_child;
            lithosphere *litho_child;

            gsgl::scenegraph::utils::sphere *simple_sphere;   ///< Used to draw simple spheroids if there's no litho or atomosphere info.
            gsgl::platform::material        *simple_material; ///< Material to draw simple sphere.

            gsgl::math::vector simple_color_offset;  ///< Offset (only x and y are used) to draw the color map of the simple sphere.
            gsgl::math::vector simple_height_offset; ///< Offset (only x and y are used) for the height map of the simple sphere.
            gsgl::real_t       simple_height_max;    ///< Maximum height of the simple sphere heightmap.

        public:
            celestial_body(const gsgl::data::config_record & obj_config);
            virtual ~celestial_body();

            gsgl::real_t get_mass() const { return mass; }
            gsgl::real_t get_polar_radius() const { return polar_radius; }
            gsgl::real_t get_equatorial_radius() const { return equatorial_radius; }

            const rotating_body *get_rotating_frame() const { return rotating_frame; }
            const atmosphere    *get_atmosphere() const { return atmo_child; }
            const lithosphere   *get_lithosphere() const { return litho_child; }

            const gsgl::scenegraph::utils::sphere *get_simple_sphere() const { return simple_sphere; }
            const gsgl::platform::material        *get_simple_material() const { return simple_material; }
            const gsgl::math::vector             & get_simple_color_offset() const { return simple_color_offset; }
            const gsgl::math::vector             & get_simple_height_offset() const { return simple_height_offset; }
            const gsgl::real_t                   & get_simple_height_max() const { return simple_height_max; }

            //
            virtual gsgl::real_t view_radius() const;
            virtual gsgl::real_t default_view_distance() const;
            virtual gsgl::real_t minimum_view_distance() const;

            virtual gsgl::real_t draw_priority(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);
            virtual void init(const gsgl::scenegraph::simulation_context *c);
            virtual void draw(const gsgl::scenegraph::simulation_context *sim_context, const gsgl::scenegraph::drawing_context *draw_context); ///< This will draw the simple sphere, so override if you want to do something else...
            virtual void cleanup(const gsgl::scenegraph::simulation_context *sim_context);

            //
            void draw_point(float width);
            void draw_name(const gsgl::scenegraph::drawing_context *c, gsgl::real_t near_plane, gsgl::real_t far_plane);

            static gsgl::data::config_variable<gsgl::real_t> MIN_PIXEL_WIDTH;

        protected:
            rotating_body * & get_rotating_frame() { return rotating_frame; }
            atmosphere * & get_atmosphere() { return atmo_child; }
            lithosphere * & get_lithosphere() { return litho_child; }
            gsgl::scenegraph::utils::sphere * & get_simple_sphere() { return simple_sphere; }
        }; // class celestial_body


    } // namespace space

} // namespace periapsis

#endif
