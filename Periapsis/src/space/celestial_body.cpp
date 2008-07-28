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

#include "space/celestial_body.hpp"
#include "space/space_context.hpp"

#include "math/units.hpp"
#include "scenegraph/camera.hpp"
#include "platform/lowlevel.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;
using namespace gsgl::platform;

namespace periapsis
{

    namespace space
    {

        config_variable<gsgl::real_t> celestial_body::MIN_PIXEL_WIDTH(L"space/celestial_body/min_pixel_width", 1.7f);


        celestial_body::celestial_body(const config_record & obj_config)
            : orbital_frame(obj_config), 
              mass(1), polar_radius(1), equatorial_radius(1),
              rotating_frame(0), atmo_child(0), litho_child(0), 
              simple_sphere(0), simple_material(0), simple_height_max(0)
        {
            mass              = units::parse(obj_config[L"mass"]);              assert(mass > 0);
            polar_radius      = units::parse(obj_config[L"polar_radius"]);      assert(polar_radius > 0);
            equatorial_radius = units::parse(obj_config[L"equatorial_radius"]); assert(equatorial_radius > 0);

            for (list<config_record>::const_iterator child = obj_config.get_children().iter(); child.is_valid(); ++child)
            {
                if (child->get_name() == L"property")
                {
                    if ((*child)[L"name"] == L"simple_material")
                    {
                        simple_material = new material(L"space", *child);

                        if (!(*child)[L"color_offset"].is_empty())
                            simple_color_offset = vector::parse((*child)[L"color_offset"]);
                        if (!(*child)[L"height_offset"].is_empty())
                            simple_height_offset = vector::parse((*child)[L"height_offset"]);
                        if (!(*child)[L"height_max"].is_empty())
                            simple_height_max = units::parse((*child)[L"height_max"]);

                        simple_sphere = new utils::sphere(this, 32, equatorial_radius, polar_radius, simple_color_offset.get_x(), simple_color_offset.get_y());
                    }
                }
            }
        } // celestial_body::celestial_body()


        celestial_body::~celestial_body()
        {
            delete simple_material;
            delete simple_sphere;
        } // celestial_body::~celestial_body()


        gsgl::real_t celestial_body::view_radius() const
        {
            return gsgl::max_val(polar_radius, equatorial_radius);
        } // celestial_body::view_radius()


        gsgl::real_t celestial_body::default_view_distance() const
        {
            return view_radius() * 3;
        } // celestial_body::default_view_distance()


        gsgl::real_t celestial_body::minimum_view_distance() const
        {
            return view_radius();
        } // celestial_body::minimum_view_distance()


        gsgl::real_t celestial_body::draw_priority(const simulation_context *, const drawing_context *)
        {
            return utils::pos_in_eye_space(this).mag2();
        } // celestial_body::draw_priority()


        void celestial_body::init(const simulation_context *c)
        {
            if (simple_sphere)
                simple_sphere->init(c);
            if (simple_material)
                simple_material->load();

            // the various children will be initialized by the scene drawing function
        } // celestial_body::init()


        void celestial_body::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

            // set up projection
            vector ep = utils::pos_in_eye_space(this);

            gsgl::real_t radius = gsgl::max_val(get_polar_radius(), get_equatorial_radius());
            gsgl::real_t dist = ep.mag();
            gsgl::real_t zdist = -ep.get_z();
            gsgl::real_t far_plane = zdist + (radius * 1.1f);
            gsgl::real_t near_plane = zdist - (radius * 1.1f);
            if (near_plane <= 0)
                near_plane = 1;

            glMatrixMode(GL_PROJECTION);                                                                        CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
            gluPerspective(draw_context->cam->get_field_of_view(), draw_context->screen->get_aspect_ratio(), near_plane, far_plane);  CHECK_GL_ERRORS();

            // check to see if we're out of range
            gsgl::real_t screen_width = utils::pixel_size(dist, radius, draw_context->cam->get_field_of_view(), draw_context->screen->get_height());

            color::WHITE.bind();

            if (screen_width < MIN_PIXEL_WIDTH)
            {
                get_draw_results() |= node::NODE_DREW_POINT;
                draw_point(MIN_PIXEL_WIDTH);
            }
            else
            {
                utils::sphere *sph = get_simple_sphere();

                if (sph)
                {
                    // the simple sphere may be rotated...
                    rotating_body *rb = get_rotating_frame();

                    if (rb)
                    {
                        glMatrixMode(GL_MODELVIEW);
                        glPushMatrix();
                        glLoadMatrixf(rb->get_modelview().ptr());
                    }

                    glClearDepth(1);                                                                                CHECK_GL_ERRORS();
                    glClear(GL_DEPTH_BUFFER_BIT);                                                                   CHECK_GL_ERRORS();
                    glEnable(GL_DEPTH_TEST);                                                                        CHECK_GL_ERRORS();

                    glEnable(GL_CULL_FACE);                                                                         CHECK_GL_ERRORS();
                    glPolygonMode(GL_FRONT_AND_BACK, (draw_context->render_flags & drawing_context::RENDER_WIREFRAME) ? GL_LINE : GL_FILL);     CHECK_GL_ERRORS();

                    // set up lighting
                    if (!(draw_context->render_flags & drawing_context::RENDER_NO_LIGHTING) && !(get_draw_flags() & NODE_DRAW_UNLIT))
                    {
                        glEnable(GL_LIGHTING);                                                                          CHECK_GL_ERRORS();

                        glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);                                            CHECK_GL_ERRORS();
                        glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);                                               CHECK_GL_ERRORS();
                        //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);

                        glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, color::WHITE.get_val());                         CHECK_GL_ERRORS();
                        glMaterialfv(GL_FRONT, GL_SPECULAR, color::BLACK.get_val());                                    CHECK_GL_ERRORS();
                        glMaterialfv(GL_FRONT, GL_EMISSION, color::BLACK.get_val());                                    CHECK_GL_ERRORS();
                        glMaterialf(GL_FRONT, GL_SHININESS, 8);                                                         CHECK_GL_ERRORS();
                    }

                    if (simple_material)
                        simple_material->bind();

                    sph->draw(sim_context, draw_context);

                    if (rb)
                    {
                        glMatrixMode(GL_MODELVIEW);
                        glPopMatrix();
                    }
                }
            }

            // clean up
            glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
            glPopAttrib();                                                                                      CHECK_GL_ERRORS();

            // draw name
            draw_name(draw_context, 1, far_plane);
        } // celestial_body::draw()


        void celestial_body::cleanup(const simulation_context *c)
        {
            if (simple_sphere)
                simple_sphere->cleanup(c);
            if (simple_material)
                simple_material->unload();
        } // celestial_body::cleanup()


        void celestial_body::draw_point(float width)
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

            glEnable(GL_BLEND);                                                                                 CHECK_GL_ERRORS();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                                  CHECK_GL_ERRORS();

            glEnable(GL_POINT_SMOOTH);                                                                          CHECK_GL_ERRORS();
            glHint(GL_POINT_SMOOTH_HINT, GL_NICEST);                                                            CHECK_GL_ERRORS();
            glPointSize(width);                                                                                 CHECK_GL_ERRORS();

            glBegin(GL_POINTS);                                                                             
            glVertex3f(0, 0, 0);                                                                            
            glEnd();                                                                                            CHECK_GL_ERRORS();

            glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
            glPopAttrib();                                                                                      CHECK_GL_ERRORS();
        } // celestial_body::draw_point()


        void celestial_body::draw_name(const drawing_context *c, gsgl::real_t near_plane, gsgl::real_t far_plane)
        {
            if ((c->render_flags & drawing_context::RENDER_LABELS) && !get_name().is_empty())
            {
                glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

                const gsgl::platform::font *label_font = dynamic_cast<const space_drawing_context *>(c)->DEFAULT_LABEL_FONT.ptr();

                glMatrixMode(GL_PROJECTION);                                                                    CHECK_GL_ERRORS();
                glLoadIdentity();                                                                               CHECK_GL_ERRORS();
                gluPerspective(c->cam->get_field_of_view(), c->screen->get_aspect_ratio(), near_plane, far_plane);          CHECK_GL_ERRORS();           

                c->screen->record_3d_text_info();
                c->screen->draw_text_start();
                c->screen->draw_3d_text(vector::ZERO, label_font, get_name(), 4, -8);
                c->screen->draw_text_stop();

                glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
                glPopAttrib();                                                                                      CHECK_GL_ERRORS();
            }
        } // celestial_body::draw_name()


    } // namespace space

} // namespace periapsis
