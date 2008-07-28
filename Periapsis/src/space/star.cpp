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

#include "space/star.hpp"
#include "math/units.hpp"
#include "scenegraph/camera.hpp"
#include "platform/lowlevel.hpp"

#include <cmath>

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::io;
using namespace gsgl::math;
using namespace gsgl::platform;
using namespace gsgl::scenegraph;


namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::star);


        star::star(const config_record & obj_config)
            : gas_body(obj_config), corona_material(0), star_light(0)
        {
            get_draw_flags() |= NODE_DRAW_UNLIT;

            //// load textures
            //if (!obj_config[L"corona"].is_empty())
            //{
            //    corona = new texture(L"scene graph", obj_config.get_directory().get_full_path() + obj_config[L"corona"], texture::TEXTURE_ENV_REPLACE);
            //}

            // create light
            star_light = new light(this);
            star_light->get_ambient() = color::BLACK;
            star_light->get_diffuse() = color::WHITE;
            star_light->get_specular() = color::WHITE;
            star_light->get_attenuation_constant() = 1;
            star_light->get_attenuation_linear() = static_cast<gsgl::real_t>(1.0 / (10.0 * units::METERS_PER_AU)); // light halved at 10 AU, just for fun
            star_light->get_attenuation_quadratic() = 0;
        } // star::star()


        star::~star()
        {
            delete corona_material;
        } // star::~star()


        void star::init(const simulation_context *c)
        {
            gas_body::init(c);

            if (corona_material)
                corona_material->load();
        } // star::init()


        void star::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            gsgl::real_t corona_radius = get_equatorial_radius() * 16;

            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

            gas_body::draw(sim_context, draw_context);

            glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
            glPopAttrib();                                                                                      CHECK_GL_ERRORS();

            // draw corona
            if (corona_material)
            {
                glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

                vector ep = utils::pos_in_eye_space(this);
                gsgl::real_t dist = ep.mag();

                gsgl::real_t zdist = -ep.get_z();
                assert(zdist > 0);

                float far_plane = zdist + (corona_radius * 1.1f);
                float near_plane = zdist - (corona_radius * 1.1f);
                if (near_plane < 1.0f)
                    near_plane = 1.0f;

                glMatrixMode(GL_PROJECTION);                                                                CHECK_GL_ERRORS();
                glLoadIdentity();                                                                           CHECK_GL_ERRORS();
                gluPerspective(draw_context->cam->get_field_of_view(), draw_context->screen->get_aspect_ratio(), near_plane, far_plane);

                glDisable(GL_DEPTH_TEST);                                                                   CHECK_GL_ERRORS();

                glEnable(GL_BLEND);                                                                         CHECK_GL_ERRORS();
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                          CHECK_GL_ERRORS();

                color::WHITE.bind();

                glEnable(GL_CULL_FACE);                                                                         CHECK_GL_ERRORS();

                if (draw_context->render_flags & (drawing_context::RENDER_WIREFRAME | drawing_context::RENDER_NO_TEXTURES))
                {
                    glPolygonMode(GL_FRONT, GL_LINE);
                }
                else
                {
                    glPolygonMode(GL_FRONT, GL_FILL);
                    glEnable(GL_TEXTURE_2D);                                                                    CHECK_GL_ERRORS();
                    corona_material->bind();
                }

                utils::draw_billboard(this, vector::ZERO, corona_radius);

                glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
                glPopAttrib();                                                                                      CHECK_GL_ERRORS();

                // draw twice, but there won't be many stars
                draw_name(draw_context, 1, far_plane);
            }

        } // star::draw()


        void star::cleanup(const simulation_context *c)
        {
            gas_body::cleanup(c);

            if (corona_material)
                corona_material->unload();
        } // star::cleanup()


    } // namespace space

} // namespace periapsis
