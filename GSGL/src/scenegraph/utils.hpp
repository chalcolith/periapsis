#ifndef GSGL_SCENEGRAPH_UTILS_H
#define GSGL_SCENEGRAPH_UTILS_H

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

#include "data/broker.hpp"
#include "math/vector.hpp"
#include "scenegraph/node.hpp"
#include "platform/color.hpp"
#include "platform/vbuffer.hpp"
#include "platform/texture.hpp"

namespace gsgl
{

    namespace scenegraph
    {

        class context;
        class node;

        namespace utils
        {

            /// A utility class for drawing coordinate systems of nodes.
            class SCENEGRAPH_API coord_system
            {
                node *parent;
                const gsgl::real_t radius, degree_step;
                const platform::color draw_color;
                int display_list_id;

            public:
                coord_system(node *parent, const gsgl::real_t radius, const gsgl::real_t degree_step, const platform::color & draw_color);
                ~coord_system();

                void init(context *);
                void draw(context *);
            }; // class coord_system


            //
            class SCENEGRAPH_API simple_sphere
            {
                node *parent;
                const int num_steps;
                const gsgl::real_t equatorial_radius, polar_radius;
                
                platform::vertex_buffer vertices;
                platform::index_buffer indices;

                platform::texture *tex;
                const gsgl::real_t tex_offset_x, tex_offset_y;

            public:
                simple_sphere(node *parent, const int num_latitude_steps, 
                              const gsgl::real_t equatorial_radius, const gsgl::real_t polar_radius,
                              platform::texture *tex, const gsgl::real_t tex_offset_x = 0, const gsgl::real_t tex_offset_y = 0);
                ~simple_sphere();

                const gsgl::real_t get_tex_offset_x() const { return tex_offset_x; }
                const gsgl::real_t get_tex_offset_y() const { return tex_offset_y; }

                void init(context *);
                void draw(context *);
            }; // class simple_sphere


            //

            /// \return A point in the node's frame transformed to eye coordinates.
            SCENEGRAPH_API math::vector pos_in_eye_space(node *frame, const math::vector & pos = math::vector::ZERO);

            /// \return The cosine of the angle between the given point and the eye direction.
            SCENEGRAPH_API gsgl::real_t dot_in_eye_space(node *frame, const math::vector & pos = math::vector::ZERO);

            /// \return True if a sphere around the given frame would be on screen.  This is not a true frustum test, as it doesn't check distance.
            SCENEGRAPH_API bool is_on_screen(node *frame, const gsgl::real_t field_of_view, const gsgl::real_t aspect_ratio, 
                                             const math::vector & pos = math::vector::ZERO, const gsgl::real_t radius = 0);

            /// \return The size of the given object in screen pixels.
            SCENEGRAPH_API gsgl::real_t pixel_size(const gsgl::real_t distance, const gsgl::real_t radius,
                                                   const gsgl::real_t field_of_view, const int pixels_in_field_of_view);

            /// Save projection information for later doing calculations in screen space.
            SCENEGRAPH_API void save_screen_info(int viewport[4], gsgl::math::transform & modelview_projection);

            /// \return The position of the given point in screen space.
            SCENEGRAPH_API gsgl::math::vector pos_in_screen_space(const gsgl::math::vector & point, const int viewport[4], const gsgl::math::transform & modelview_projection);

            /// \return The greatest extent of the node and all the child nodes.
            SCENEGRAPH_API gsgl::real_t greatest_extent(const node *n);

            //

            /// Draws a billboard at the specified point in the specified frame.
            /// Make sure to bind the appropriate texture before calling.
            SCENEGRAPH_API void draw_billboard(node *frame, const math::vector & pos, const gsgl::real_t radius);


            //

            class SCENEGRAPH_API checkered_box
                : public scenegraph::node
            {
                gsgl::real_t radius;
                platform::vertex_buffer normals, vertices;

            public:
                checkered_box(const gsgl::string & name, scenegraph::node *parent, gsgl::real_t radius);
                virtual ~checkered_box();

                gsgl::real_t & get_radius() { return radius; }

                virtual void init(gsgl::scenegraph::context *c);
                virtual void draw(gsgl::scenegraph::context *c);
                virtual void update(gsgl::scenegraph::context *c);
                virtual void cleanup(gsgl::scenegraph::context *c);

                virtual gsgl::real_t get_priority(gsgl::scenegraph::context *);
                virtual gsgl::real_t max_extent() const;

                virtual gsgl::real_t default_view_distance() const;
                virtual gsgl::real_t minimum_view_distance() const;
            }; // class checkered_box

        } // namespace utils

    } // namespace scenegraph

} // namespace gsgl

#endif
