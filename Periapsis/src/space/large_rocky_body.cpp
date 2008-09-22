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

#include "space/large_rocky_body.hpp"
#include "space/large_lithosphere.hpp"
#include "space/rocky_body_atmosphere.hpp"
#include "scenegraph/camera.hpp"
#include "platform/color.hpp"


using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;
using namespace gsgl::platform;

namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::large_rocky_body);


        large_rocky_body::large_rocky_body(const config_record & obj_config)
            : celestial_body(obj_config)
        {
            // create rotating lithosphere (the rotating body will delete its rotator)
            body_rotator *rotator = !obj_config[L"rotator"].is_empty() ? rotator = dynamic_cast<body_rotator *>(broker::global_instance()->create_object(obj_config[L"rotator"], obj_config)) : 0;
            get_rotating_frame() = get_lithosphere() = new large_lithosphere(get_name() + L" lithosphere [rotating frame]", this, rotator);

            // create atmosphere if present
            if (!obj_config[L"atmosphere_depth"].is_empty())
            {
                get_atmosphere() = new rocky_body_atmosphere(get_name() + L" atmosphere [rotating]", get_lithosphere(), 0);
            }
        } // large_rocky_body()


        large_rocky_body::~large_rocky_body()
        {
        } // large_rocky_body::~large_rocky_body()


        gsgl::real_t large_rocky_body::draw_priority(const simulation_context *, const drawing_context *)
        {
            return utils::pos_in_eye_space(this).mag2();
        } // large_rocky_body::draw_priority()


        void large_rocky_body::init(const simulation_context *c)
        {
            celestial_body::init(c);
        } // large_rocky_body::init()


        void large_rocky_body::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            lithosphere *litho = get_lithosphere();

            if (litho)
            {
                display::scoped_state state(*draw_context->screen);

                vector pos_in_view = get_modelview() * vector::ZERO;

                gsgl::real_t radius = gsgl::max_val(get_polar_radius(), get_equatorial_radius());
                gsgl::real_t dist = pos_in_view.mag();
                gsgl::real_t zdist = -pos_in_view.get_z();
                gsgl::real_t far_plane = zdist + (radius * 1.1f);
                gsgl::real_t near_plane = zdist - (radius * 1.1f);
                if (near_plane <= 0)
                    near_plane = 1;

                display::scoped_perspective proj(*draw_context->screen, draw_context->cam->get_field_of_view(), draw_context->screen->get_aspect_ratio(), near_plane, far_plane);
                display::scoped_color cc(*draw_context->screen, color::WHITE);

                gsgl::real_t screen_width = utils::pixel_size(dist, radius, draw_context->cam->get_field_of_view(), draw_context->screen->get_height());

                if (screen_width < MIN_PIXEL_WIDTH)
                {
                    get_draw_results() |= node::NODE_DREW_POINT;
                    draw_context->screen->draw_point(vector::ZERO, MIN_PIXEL_WIDTH);
                }
                else
                {
                    draw_context->screen->clear(display::CLEAR_DEPTH);
                    display::scoped_lighting lighting(*draw_context->screen, !(draw_context->render_flags & drawing_context::RENDER_NO_LIGHTING) && !(get_draw_flags() & NODE_DRAW_UNLIT));
                    display::scoped_modelview mv(*draw_context->screen, &litho->get_modelview());

                    state.enable(display::ENABLE_DEPTH);
                    litho->draw(sim_context, draw_context);
                }
            }
            else
            {
                celestial_body::draw(sim_context, draw_context);
            }

#if 0
            lithosphere *litho = get_lithosphere();
            if (litho)
            {
                glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

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

                    // set up texturing
                    if (!(draw_context->render_flags & drawing_context::RENDER_NO_TEXTURES))
                    {
                        glEnable(GL_TEXTURE_2D);
                    }

                    // draw lithosphere
                    glMatrixMode(GL_MODELVIEW);
                    glPushMatrix();
                    glLoadMatrixf(litho->get_modelview().ptr());
                    
                    litho->draw(sim_context, draw_context);

                    glMatrixMode(GL_MODELVIEW);
                    glPopMatrix();
                }

                glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
                glPopAttrib();                                                                                      CHECK_GL_ERRORS();

                draw_name(draw_context, 1, far_plane);
            }
            else
            {
                celestial_body::draw(sim_context, draw_context);
            }
#endif
        } // large_rocky_body::draw()


        void large_rocky_body::update(const simulation_context *c)
        {
            celestial_body::update(c);
        } // large_rocky_body::update()


        void large_rocky_body::cleanup(const simulation_context *c)
        {
            celestial_body::cleanup(c);
        } // large_rocky_body::cleanup()


    } // namespace space

} // namespace periapsis
