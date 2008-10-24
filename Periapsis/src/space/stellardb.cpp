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

#include "space/stellardb.hpp"
#include "space/astronomy.hpp"
#include "space/space_context.hpp"

#include "data/fstream.hpp"
#include "platform/font.hpp"
#include "platform/shader.hpp"

#include "scenegraph/context.hpp"
#include "scenegraph/camera.hpp"
#include "scenegraph/utils.hpp"
#include "framework/application.hpp"

#include "platform/lowlevel.hpp"

#include <cmath>
#include <float.h>


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

        BROKER_DEFINE_CREATOR(periapsis::space::stellar_db);

        //

        stellar_db::stellar_db(const config_record & conf)
            : node(conf), nearest_distance(FLT_MAX), farthest_distance(0), 
              vertices(vbuffer::STATIC), uniform_farthest_distance(0)
        {
            get_draw_flags() |= node::NODE_NO_FRUSTUM_CHECK;

            // load stellar database
            string db_fname = conf[L"data"];
            if (!db_fname.is_empty())
            {
                load_db(conf.get_directory().get_full_path() + db_fname);
            }
            else
            {
                throw runtime_exception(L"No data specified for stellar database node.");
            }

            // load shader
            star_shader.add_vertex_shader(gsgl::framework::application::SYS_DATA_PATH + L"Shaders" + directory::SEPARATOR + L"star_vertex_shader.glsl");
        } // stellar_db::stellar_db()


        stellar_db::~stellar_db()
        {
            int i, len = star_names.size();
            for (i = 0; i < len; ++i)
                delete star_names[i];
        } // stellar_db::~stellar_db()


        gsgl::real_t stellar_db::draw_priority(const simulation_context *, const drawing_context *)
        {
            return NODE_DRAW_FIRST / 10.0f;
        } // stellar_db::draw_priority()


        //

        static const string STAR_DB_COOKIE = L"Periapsis Stellar Database 2.0";

        void stellar_db::load_db(const string & fname)
        {
            fd_stream f(fname);

            string cookie;
            f >> cookie;
            if (cookie != STAR_DB_COOKIE)
                throw io_exception(L"Invalid stellar database format in %ls.", fname.w_string());

            f >> num_stars;

            // read data
            smart_pointer<float, true> magnitudes(new float[num_stars]);

            float max_magnitude = 0.0f;
            float min_magnitude = 0.0f;

            for (int i = 0; i < num_stars; ++i)
            {
                int hyg_id;
                f >> hyg_id;

                // calculate position
                float right_ascension, declination, distance, abs_magnitude;

                f >> right_ascension;
                f >> declination;
                f >> distance;
                f >> abs_magnitude;

                float theta = static_cast<float>(right_ascension * (360.0f / 24.0f) * math::DEG2RAD);
                float phi = static_cast<float>(declination * math::DEG2RAD);

                float x_eq = distance * ::cos(theta) * ::cos(phi);
                float y_eq = distance * ::sin(theta) * ::cos(phi);
                float z_eq = distance * ::sin(phi);

                vector equatorial_pos(x_eq, y_eq, z_eq);
                vector galactic_pos = EQUATORIAL_WRT_GALACTIC * equatorial_pos;

                if (distance< nearest_distance)
                    nearest_distance = distance;
                if (distance > farthest_distance)
                    farthest_distance = distance;

                magnitudes[i] = abs_magnitude;
                if (abs_magnitude > max_magnitude)
                    max_magnitude = abs_magnitude;
                if (abs_magnitude < min_magnitude)
                    min_magnitude = abs_magnitude;

                // get color
                unsigned char r, g, b, a;

                f >> r;
                f >> g;
                f >> b;
                f >> a;

                // record coordinates and colors
                float temp_color;
                unsigned char *color_ptr = reinterpret_cast<unsigned char *>(&temp_color);
                color_ptr[0] = r;
                color_ptr[1] = g;
                color_ptr[2] = b;
                color_ptr[3] = 255;

                vertices[i*4+0] = temp_color;
                vertices[i*4+1] = galactic_pos.get_x();
                vertices[i*4+2] = galactic_pos.get_y();
                vertices[i*4+3] = galactic_pos.get_z();
            }

            // store absolute magnitude in alpha channel
            float magnitude_range = max_magnitude - min_magnitude;
            for (int i = 0; i < num_stars; ++i)
            {
                float mag_diff = math::clamp(magnitudes[i] - min_magnitude, 0.0f, 5.0f);
                float mag_pct = 1.0f - (mag_diff / 5.0f);

                unsigned char *color_ptr = reinterpret_cast<unsigned char *>(&vertices[i*4+0]);
                color_ptr[3] = static_cast<unsigned char>(mag_pct * 255.0f);

                //float r = color_scale * static_cast<float>(color_ptr[0]);
                //float g = color_scale * static_cast<float>(color_ptr[1]);
                //float b = color_scale * static_cast<float>(color_ptr[2]);

                //color_ptr[0] = static_cast<unsigned char>(r);
                //color_ptr[1] = static_cast<unsigned char>(g);
                //color_ptr[2] = static_cast<unsigned char>(b);
            }

            // get names
            int num_star_names;
            f >> num_star_names;

            for (int i = 0; i < num_star_names; ++i)
            {
                int star_id;
                f >> star_id;

                string star_name;
                f >> star_name;

                vector pos(vertices[(star_id-1)*4+1], vertices[(star_id-1)*4+2], vertices[(star_id-1)*4+3]);                
                add_star_name(pos, star_name);
            }

            // some signposts
            gsgl::real_t axis_dist = 100.0f;

            add_star_name(vector(axis_dist, 0, 0), L"Galactic Center (+X)");
            add_star_name(vector(-axis_dist, 0, 0), L"Galactic -X");

            add_star_name(vector(0, axis_dist, 0), L"Galactic +Y");
            add_star_name(vector(0, -axis_dist, 0), L"Galactic -Y");

            add_star_name(vector(0, 0, axis_dist), L"Galactic North (+Z)");
            add_star_name(vector(0, 0, -axis_dist), L"Galactic South (-Z)");
        } // stellar_db::load_db()


        void stellar_db::add_star_name(const vector & pos, const string & name)
        {
            star_name_vertices.append(pos.get_x());
            star_name_vertices.append(pos.get_y());
            star_name_vertices.append(pos.get_z());
            star_names.append(new string(name));
        } // stellar_db::add_star_name()


        //

        void stellar_db::init(const simulation_context *)
        {
            star_shader.load();
            uniform_farthest_distance = star_shader.get_uniform<float>(L"FarthestStarDistance");
        } // stellar_db::init()


        void stellar_db::cleanup(const simulation_context *)
        {
            star_shader.unload();
        } // stellar_db::clean()



        static float scale_factor = 1.0;


        void stellar_db::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            display::scoped_state state(*draw_context->screen, draw_context->display_flags(this) ^ (display::ENABLE_DEPTH));

            float star_scale = get_scale();
            float far_plane = farthest_distance * 1.1f * star_scale;
            float near_plane = nearest_distance * 0.9f * star_scale;
            if (near_plane <= 1)
                near_plane = 1;

            // draw stars
            {
                // projection
                display::scoped_perspective proj(*draw_context->screen, draw_context->cam->get_field_of_view(), draw_context->screen->get_aspect_ratio(), near_plane, far_plane);

                // set up shader
                display::scoped_shader shdr(*draw_context->screen, &star_shader);
                uniform_farthest_distance->set(farthest_distance * get_scale());

                // scale & draw
                display::scoped_modelview mv(*draw_context->screen);
                mv.scale(star_scale, star_scale, star_scale);

                glEnableClientState(GL_VERTEX_ARRAY);                                                                   CHECK_GL_ERRORS();
                glEnableClientState(GL_COLOR_ARRAY);                                                                    CHECK_GL_ERRORS();

                vertices.bind();
                glInterleavedArrays(GL_C4UB_V3F, 0, 0);                                                                 CHECK_GL_ERRORS();
                glDrawArrays(GL_POINTS, 0, num_stars);                                                                  CHECK_GL_ERRORS();
            }

            // draw names
            if ((draw_context->render_flags & drawing_context::RENDER_LABELS))
            {
                const font *label_font = dynamic_cast<const space_drawing_context *>(draw_context)->DEFAULT_LABEL_FONT.ptr();

                display::scoped_text labels(*draw_context->screen);

                int i, len = star_names.size();
                for (i = 0; i < len; ++i)
                {
                    vector pos(star_name_vertices[i*3+0]*star_scale, star_name_vertices[i*3+1]*star_scale, star_name_vertices[i*3+2]*star_scale);
                    labels.draw_3d(pos, label_font, *star_names[i], 4, -8);
                }
            }
        } // stellar_db::draw()

    } // namespace space

} // namespace periapsis
