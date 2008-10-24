#ifndef GSGL_FRAMEWORK_SCREEN_H
#define GSGL_FRAMEWORK_SCREEN_H

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

#include "platform/platform.hpp"

#include "platform/color.hpp"
#include "platform/shader.hpp"
#include "platform/material.hpp"
#include "platform/font.hpp"
#include "platform/vbuffer.hpp"

#include "data/config.hpp"
#include "math/math.hpp"
#include "math/transform.hpp"

// forward declarations
struct SDL_Surface;

namespace gsgl
{

    namespace platform
    {

        //
        class texture;

    
        /// This class represents a screen on which to draw.
        class PLATFORM_API display
        {
            struct SDL_Surface *surface;
            bool is_console;
            
            int opengl_fbo_id;

            static int current_fbo_id; // avoid redundant bind calls

        public:
            display(const int & width, const int & height, bool is_console = false);
            virtual ~display();
            
            static data::config_variable<int> DISPLAY_WIDTH;
            static data::config_variable<int> DISPLAY_HEIGHT;

            int get_width() const;
            int get_height() const;
            gsgl::real_t get_aspect_ratio() const;

            /// Binds the OpenGL context associated with this display.
            void bind();

            /// Unbinds the OpenGL context associated with this display.
            void unbind();


            /// \name Lighting Functionality
            /// \{

            void define_ambient_light(const platform::color & ambient);

            int get_max_lights();

            /// Define a light using the given modelview and position.
            void define_light(const int & light_number, const math::transform & modelview, const math::vector & position,
                              const platform::color & ambient, const platform::color & diffuse, const platform::color & specular,
                              const gsgl::real_t & attenuation_constant, const gsgl::real_t & attenuation_linear, const gsgl::real_t & attenuation_quadratic);

            /// \}

            /// \name Miscellaneous Functionality

            enum clear_flags
            {
                CLEAR_NONE  = 0,
                CLEAR_COLOR = 1 << 0,
                CLEAR_DEPTH = 1 << 1,
                CLEAR_ALL   = ~0
            };

            /// Clears the given buffer (flags should come from clear_flags).
            void clear(const gsgl::flags_t flags, const platform::color & clear_color = color::BLACK);

            /// Draw a point.
            void draw_point(const math::vector & v = math::vector::ZERO, const gsgl::real_t & width = 1.0f);

            /// Draw a line in 2d.
            void draw_line_2d(float x1, float y1, float x2, float y2, float width = 1.0f);
            void draw_line_2d(int x1, int y1, int x2, int y2, int width = 1);

            /// Draws a possibly textured rectangle (a suitable projection and modelview must already be set up).
            /// x1 and y1 define the lower left corner, and x2 and y2 define the upper right corner.
            void draw_rect_2d(float x1, float y1, float x2, float y2, float s1 = 0, float t1 = 0, float s2 = 1, float t2 = 1);

            /// \}


            /// \name Basic State
            /// \{

            enum
            {
                ENABLE_NONE           = 0,
                ENABLE_DEPTH          = 1 << 0,
                ENABLE_BLEND          = 1 << 1,
                ENABLE_ANTIALIAS      = 1 << 2,
                ENABLE_SMOOTH_SHADING = 1 << 3,
                ENABLE_FILLED_POLYS   = 1 << 4,
                ENABLE_BF_CULL        = 1 << 5,
                ENABLE_TEXTURES       = 1 << 6,
                ENABLE_LIGHTING       = 1 << 7,
                
                ENABLE_ALL            = 0xffffffff,

                ENABLE_ORTHO_2D       = ENABLE_ALL ^ (ENABLE_DEPTH | ENABLE_LIGHTING)
            };

            /// An RAII class for setting a particular OpenGL attribute state.
            /// An object of this class should be the first one in a particular scope, as it unbinds its display when it goes out of scope.
            class PLATFORM_API scoped_state
            {
                display & parent;
            public:
                scoped_state(display & parent, const gsgl::flags_t & flags = ENABLE_ALL);
                ~scoped_state();

            private:
                void enable(const gsgl::flags_t & flags);
            }; // class scoped_state


            /// An RAII class for setting a particular viewport.
            class PLATFORM_API scoped_viewport
            {
                display & parent;
            public:
                scoped_viewport(display & parent);
                ~scoped_viewport();
            }; // class scoped_viewport


            /// An RAII class for setting a particular modelview matrix.
            class PLATFORM_API scoped_modelview
            {
                display & parent;
                const math::transform *mv;

            public:
                /// If you pass a null pointer for the matrix, the matrix stack will be pushed, but the modelview matrix will not be altered.
                scoped_modelview(display & parent, const math::transform *modelview = 0);
                ~scoped_modelview();

                void translate(const math::vector & v);
                void translate(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z = 0.0f);

                void scale(const math::vector & v);
                void scale(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z);

                void mult(const math::transform & t);
            }; // class scoped_modelview


            /// An RAII class for setting a perspective projection matrix.
            class PLATFORM_API scoped_perspective
            {
                display & parent;
            public:
                scoped_perspective(display & parent, const gsgl::real_t & field_of_view, const gsgl::real_t & aspect_ratio, const gsgl::real_t & near_distance, const gsgl::real_t & far_distance);
                ~scoped_perspective();
            }; // class scoped_perspective


            class PLATFORM_API scoped_ortho
            {
                display & parent;
            public:
                scoped_ortho(display & parent);
                ~scoped_ortho();
            }; // class scoped_ortho

            /// \}


            /// \name Colour and Material
            /// \{

            /// An RAII class for setting a colour.  Will NOT set the drawing context, so use only in conjunction with other utility classes that do.
            class PLATFORM_API scoped_color
            {
                display & parent;
                const color & c;
            public:
                scoped_color(display & parent, const color & c) : parent(parent), c(c) { parent.bind(); c.bind(); }
                ~scoped_color() { parent.bind(); c.unbind(); }
            }; // class scoped_color


            class PLATFORM_API scoped_texture
            {
                display & parent;
                const texture *t;
            public:
                scoped_texture(display & parent, const texture *t) : parent(parent), t(t) { parent.bind(); if (t) t->bind(); }
                ~scoped_texture() { parent.bind(); if (t) t->unbind(); }
            }; // class scoped_texture


            class PLATFORM_API scoped_shader
            {
                display & parent;
                const shader_program *s;
            public:
                scoped_shader(display & parent, const shader_program *s) : parent(parent), s(s) { parent.bind(); if (s) s->bind(); }
                ~scoped_shader() { parent.bind(); if (s) s->unbind(); }
            }; // class scoped_shader


            class PLATFORM_API scoped_material
            {
                display & parent;
                const material *m;
            public:
                scoped_material(display & parent, const material *m) : parent(parent), m(m) { parent.bind(); if (m) m->bind(); }
                ~scoped_material() { parent.bind(); if (m) m->unbind(); }
            }; // class scoped_material

            /// \}


            /// \name Drawing
            /// \{

            enum primitive_type
            {
                PRIMITIVE_POINTS,
                PRIMITIVE_LINES,
                PRIMITIVE_TRIANGLES,
                PRIMITIVE_TRIANGLE_STRIP,
                PRIMITIVE_TRIANGLE_FAN,
            };

            /// An RAII class for drawing arrays of primitives.
            class PLATFORM_API scoped_buffer
            {
                display & parent;

                vertex_buffer *vertices;
                vertex_buffer *normals;
                vertex_buffer *texcoords;
                index_buffer  *indices;

                int gl_type;
                bool interleaved;

            public:
                scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vertices);
                scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vertices, index_buffer & indices, bool interleaved = false);
                scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vertices, vertex_buffer & normals, vertex_buffer & texcoords);
                scoped_buffer(display & parent, const primitive_type & pt, vertex_buffer & vertices, vertex_buffer & normals, vertex_buffer & texcoords, index_buffer & indices);

                ~scoped_buffer();

                void draw(int count, int start = 0);

            private:
                void init();
                static int get_gl_type(const primitive_type & pt);
            }; // class scoped_buffer

            /// \}


            /// \name Text Drawing
            /// \{

            /// An RAII class for drawing 2d or 3d text.  Instantiating an object of this class will set up an orthographic projection for drawing text.
            /// Obviously, do not use display::projection objects in the same scope!
            class PLATFORM_API scoped_text
            {
                display & parent;

                int text_draw_viewport[4];
                math::transform text_draw_modelview;
                math::transform text_draw_projection;
                math::transform text_draw_pm;

            public:
                scoped_text(display & parent);
                ~scoped_text();

                /// Draws text at the given screen coordinates.
                void draw_2d(const gsgl::real_t & x, const gsgl::real_t & y, const font *f, const gsgl::string & str);

                /// Draws text at the given world coordinates.
                void draw_3d(const math::vector & p, const font *f, const gsgl::string & str, const gsgl::real_t & x_offset = 0, const gsgl::real_t & y_offset = 0);
            }; // class scoped_text

            /// \}

        private:
            void center_console_window(const int & width, const int & height);
        }; // class display
        
    } // namespace platform
    
} // namespace gsgl

#endif
