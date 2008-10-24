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

#include "scenegraph/utils.hpp"
#include "scenegraph/node.hpp"
#include "scenegraph/camera.hpp"
#include "scenegraph/freeview.hpp"

#include "platform/vbuffer.hpp"
#include "platform/lowlevel.hpp"

#include <cmath>


namespace gsgl
{

    using namespace data;
    using namespace math;
    using namespace platform;

    namespace scenegraph
    {

        namespace utils
        {


            //

            coord_system::coord_system(node *parent, const gsgl::real_t radius, const gsgl::real_t degree_step, const color & draw_color)
                : parent(parent), radius(radius), degree_step(degree_step), draw_color(draw_color), display_list_id(0)
            {
            } // coord_system::coord_system()


            coord_system::~coord_system()
            {
                if (display_list_id)
                {
                    glDeleteLists(display_list_id, 1);                                                              CHECK_GL_ERRORS();
                }
            } // coord_system::~coord_system()


            void coord_system::init(const simulation_context *c)
            {
            } // coord_system::init()


            void coord_system::draw(const simulation_context *sim_context, const drawing_context *draw_context)
            {
                glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

                //
                vector ep = pos_in_eye_space(parent);
                gsgl::real_t far_plane = (-ep.get_z() + radius) * 1.1f; // just to be safe

                gsgl::real_t near_plane = far_plane * static_cast<gsgl::real_t>(::cos(draw_context->cam->get_field_of_view() * math::DEG2RAD));
                if (near_plane < 0)
                    near_plane = 1;

                glMatrixMode(GL_PROJECTION);                                                                        CHECK_GL_ERRORS();
                glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
                gluPerspective(draw_context->cam->get_field_of_view(), draw_context->screen->get_aspect_ratio(), near_plane, far_plane);  CHECK_GL_ERRORS();

                //
                glDisable(GL_DEPTH_TEST);                                                                           CHECK_GL_ERRORS();

                glEnable(GL_BLEND);                                                                                 CHECK_GL_ERRORS();
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                                  CHECK_GL_ERRORS();

                glEnable(GL_LINE_SMOOTH);
                glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);

                draw_color.bind();

                if (display_list_id)
                {
                    glCallList(display_list_id);                                                                    CHECK_GL_ERRORS();
                }
                else
                {
                    if ((display_list_id = glGenLists(1)))
                    {
                        glNewList(display_list_id, GL_COMPILE_AND_EXECUTE);                                         CHECK_GL_ERRORS();

                        gsgl::real_t x, y, z;

                        // longitude lines
                        for (gsgl::real_t theta = 0.0f; theta < 360.0f; theta += degree_step)
                        {
                            if (theta == 0.0f)
                                glLineWidth(4.0f);
                            else
                                glLineWidth(2.0f);

                            glBegin(GL_LINE_STRIP);                                                                 

                            for (gsgl::real_t phi = 0.0f; phi <= 180.0f; phi += 1.0f)
                            {
                                x = static_cast<gsgl::real_t>(radius * ::cos(theta * math::DEG2RAD) * ::sin(phi * math::DEG2RAD));
                                y = static_cast<gsgl::real_t>(radius * ::sin(theta * math::DEG2RAD) * ::sin(phi * math::DEG2RAD));
                                z = static_cast<gsgl::real_t>(radius * ::cos(phi * math::DEG2RAD));

                                glVertex3f(x, y, z);
                            }

                            glEnd();                                                                                CHECK_GL_ERRORS();
                        }

                        // latitude lines
                        for (gsgl::real_t phi = degree_step; phi < 180.0f; phi += degree_step)
                        {
                            if (phi == 90.0f)
                                glLineWidth(4.0f);
                            else
                                glLineWidth(2.0f);

                            glBegin(GL_LINE_STRIP);                                                                 

                            for (gsgl::real_t theta = 0.0f; theta <= 360.0f; theta += 1.0f)
                            {
                                x = static_cast<gsgl::real_t>(radius * ::cos(theta * math::DEG2RAD) * ::sin(phi * math::DEG2RAD));
                                y = static_cast<gsgl::real_t>(radius * ::sin(theta * math::DEG2RAD) * ::sin(phi * math::DEG2RAD));
                                z = static_cast<gsgl::real_t>(radius * ::cos(phi * math::DEG2RAD));

                                glVertex3f(x, y, z);
                            }

                            glEnd();                                                                                CHECK_GL_ERRORS();
                        }

                        glEndList();                                                                                CHECK_GL_ERRORS();
                    }
                }

                glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
                glPopAttrib();                                                                                      CHECK_GL_ERRORS();
            } // coord_system::draw()


            //////////////////////////////////////////////////////////

            sphere::sphere(node *parent, const int num_steps, 
                                         const gsgl::real_t equatorial_radius, const gsgl::real_t polar_radius,
                                         const gsgl::real_t tex_offset_x, const gsgl::real_t tex_offset_y)
                : parent(parent), num_steps(num_steps),
                  equatorial_radius(equatorial_radius), polar_radius(polar_radius), 
                  tex_offset_x(tex_offset_x), tex_offset_y(tex_offset_y),
                  vertices(vbuffer::STATIC), indices(vbuffer::STATIC)
            {
                double b_over_a = polar_radius / equatorial_radius;
                double altitude = 0;

                // build vertices for triangle strips
                // there are num_steps+1 rows with num_steps*2+1 points each
                for (int lat = 0; lat <= num_steps; ++lat)
                {
                    // lat is geographical latitude, which we will need to convert to geocentric latitude
                    double pct_lat = static_cast<double>(lat) / static_cast<double>(num_steps);
                    double spherical_phi = pct_lat * math::PI;
                    double geographic_phi = math::PI_OVER_2 - spherical_phi;

                    double nz = ::cos(spherical_phi);
                    double nr = ::sin(spherical_phi);
                    double geocentric_phi = ::atan2(b_over_a * b_over_a * nz, nr);  // from Meeus
                    double geocentric_spherical_phi = math::PI_OVER_2 - geocentric_phi;

                    for (int lon = 0; lon <= num_steps*2; ++lon)
                    {
                        double pct_lon = static_cast<double>(lon) / static_cast<double>(num_steps*2);
                        double theta = pct_lon * math::PI_TIMES_2;

                        double s = pct_lon + tex_offset_x;
                        double t = (1.0 - pct_lat) + tex_offset_y;

                        double nx = ::cos(theta) * ::sin(spherical_phi);
                        double ny = ::sin(theta) * ::sin(spherical_phi);

                        double x = equatorial_radius * ::cos(theta) * ::sin(geocentric_spherical_phi);
                        double y = equatorial_radius * ::sin(theta) * ::sin(geocentric_spherical_phi);
                        double z = polar_radius * ::cos(geocentric_spherical_phi);

                        vertices.append(static_cast<float>(s));
                        vertices.append(static_cast<float>(t));
                        vertices.append(static_cast<float>(nx));
                        vertices.append(static_cast<float>(ny));
                        vertices.append(static_cast<float>(nz));
                        vertices.append(static_cast<float>(x));
                        vertices.append(static_cast<float>(y));
                        vertices.append(static_cast<float>(z));
                    }
                }

                // build rows of triangle strips
                int stride = num_steps*2+1;

                for (int j = 0; j < num_steps; ++j)
                {
                    for (int i = 0; i <= num_steps*2; ++i)
                    {
                        indices.append( j*stride + i ); // up
                        indices.append( (j+1)*stride + i ); // down
                    }
                }
            } // sphere::sphere()


            sphere::~sphere()
            {
            } // sphere::~sphere()

            
            void sphere::init(const simulation_context *c)
            {
                vertices.load();
                indices.load();
            } // sphere::init()


            void sphere::draw(const simulation_context *sim_context, const drawing_context *draw_context)
            {
#if 1
                display::scoped_buffer vb(*draw_context->screen, display::PRIMITIVE_TRIANGLE_STRIP, vertices, indices, true);

                int index_stride = (num_steps*2+1)*2;

                for (int i = 0; i < num_steps; ++i)
                {
                    vb.draw(index_stride, i*index_stride);
                }

#else
                glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

                glEnableClientState(GL_VERTEX_ARRAY);                                                               CHECK_GL_ERRORS();
                glEnableClientState(GL_NORMAL_ARRAY);
                glEnableClientState(GL_INDEX_ARRAY);                                                                CHECK_GL_ERRORS();

                vertices.bind();
                glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);                                                          CHECK_GL_ERRORS();

                indices.bind();

                int index_stride = (num_steps*2+1)*2;
                int vertex_stride = 8;

                for (int i = 0; i < num_steps; ++i)
                {
                    glDrawElements(GL_TRIANGLE_STRIP, index_stride, GL_UNSIGNED_INT, vbuffer::VBO_OFFSET<vbuffer::index_t>(i*index_stride));
                }

                //// for debugging
                //glColor4f(1, 1, 1, 1);
                //glDisable(GL_CULL_FACE);
                //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

                //int index_stride = (num_steps*2+1)*2;
                //int vertex_stride = 8;
                //for (int lat = 0; lat < num_steps; ++lat)
                //{
                //    glBegin(GL_TRIANGLE_STRIP);
                //    for (int i = 0; i < index_stride; ++i)
                //    {
                //        int index = indices[lat*index_stride + i];
                //        float *vptr = vertices.get_buffer().ptr() + index*vertex_stride;

                //        glTexCoord2f(vptr[0], vptr[1]);
                //        glNormal3f(vptr[2], vptr[3], vptr[4]);
                //        glVertex3f(vptr[5], vptr[6], vptr[7]);
                //    }
                //    glEnd();
                //}

                glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
                glPopAttrib();                                                                                      CHECK_GL_ERRORS();
#endif
            } // sphere::draw()


            void sphere::cleanup(const simulation_context *)
            {
                vertices.unload();
                indices.unload();
            } // sphere::cleanup()


            //////////////////////////////////////////////////////////

            vector pos_in_eye_space(node *frame, const vector & p)
            {
                return frame->get_modelview() * p;
            } // pos_in_eye_space()


            gsgl::real_t dot_in_eye_space(node *frame, const vector & pos)
            {
                vector eye_dir = frame->get_modelview() * pos;
                eye_dir.normalize();

                return eye_dir.dot(vector::NEG_Z_AXIS);
            } // dot_in_eye_space()


            bool is_on_screen(node *frame, 
                              const gsgl::real_t field_of_view, const gsgl::real_t aspect_ratio,
                              const vector & pos, const gsgl::real_t radius)
            {
                if (frame->get_draw_flags() & node::NODE_NO_FRUSTUM_CHECK)
                    return true;

                vector eye_dir = frame->get_modelview() * pos;
                double dist = eye_dir.mag();

                eye_dir.normalize();
                double cos_angle = vector::NEG_Z_AXIS.dot(eye_dir);

                double node_angle = ::acos(cos_angle);
                double radius_angle = ::atan(radius / dist);

                double obj_angle = node_angle - radius_angle;
                double view_angle = field_of_view * math::DEG2RAD * aspect_ratio;

                return obj_angle < view_angle;
            } // is_on_screen()


            gsgl::real_t pixel_size(const gsgl::real_t distance, const gsgl::real_t radius,
                                    const gsgl::real_t field_of_view, const int pixels_in_field_of_view)
            {
                double half_angle = ::atan(radius / distance);
                double pixel_size = (2.0 * half_angle / (field_of_view*math::DEG2RAD)) * static_cast<gsgl::real_t>(pixels_in_field_of_view);
                return static_cast<gsgl::real_t>(pixel_size);
            } // pixel_size()


            void save_screen_info(int viewport[4], transform & modelview_projection)
            {
                transform modelview, projection;

                glGetIntegerv(GL_VIEWPORT, viewport);                                                         CHECK_GL_ERRORS();
                glGetFloatv(GL_MODELVIEW_MATRIX, modelview.ptr());                                            CHECK_GL_ERRORS();
                glGetFloatv(GL_PROJECTION_MATRIX, projection.ptr());                                          CHECK_GL_ERRORS();
                modelview_projection = projection * modelview;
            } // save_screen_info()


            vector pos_in_screen_space(const vector & point, const int viewport[4], const transform & modelview_projection)
            {
                vector point_in_clip_space = modelview_projection * point;
                return vector(viewport[0] + viewport[2]*(point_in_clip_space.get_x() + 1)/2,
                              viewport[1] + viewport[3]*(point_in_clip_space.get_y() + 1)/2,
                              (point_in_clip_space.get_z() + 1)/2);
            } // pos_in_screen_space()


            gsgl::real_t greatest_extent(const node *cn)
            {
                node *n = const_cast<node *>(cn);

                gsgl::real_t max_val = n->view_radius();

                for (simple_array<node *>::iterator i = n->get_children().iter(); i.is_valid(); ++i)
                {
                    // special case: ignore viewpoint nodes!
                    if (dynamic_cast<freeview *>(*i))
                        continue;

                    gsgl::real_t dist = n->get_scale() * (*i)->get_translation().mag();
                    gsgl::real_t m = dist + greatest_extent(*i);

                    if (m > max_val)
                        max_val = m;
                }

                return max_val;
            } // greatest_extent()


            //

            void draw_billboard(node *frame, const vector & pos, const gsgl::real_t radius)
            {
                // get target position in eye space
                vector eye_pos = frame->get_modelview() * pos;
                
                // normalize to get the billboard front (in eye space)
                vector front = eye_pos; // vector::NEG_Z_AXIS;
                front.normalize();

                // get eye up (in eye space)
                const vector & eye_up = vector::Y_AXIS;

                // front x eye_up gives us billboard right (in eye space)
                vector right = front.cross(eye_up);

                // right x front gives us billboard up (in eye space)
                vector up = right.cross(front);

                // get the inverse modelview rotation and billboard vectors in world space
                transform mvi = frame->get_modelview().rotation_part().transpose();

                vector horiz = mvi * right;   horiz *= radius;
                vector vert = mvi * up;       vert *= radius;
                vector normal = mvi * -front;

                // draw the billboard in world space
                vector lower_left  = pos + -horiz + -vert;
                vector lower_right = pos + horiz + -vert;
                vector upper_right = pos + horiz + vert;
                vector upper_left  = pos + -horiz + vert;

                glBegin(GL_TRIANGLE_STRIP);
                {
                    // upper left
                    glTexCoord2f(0, 1);
                    glNormal3fv(normal.ptr());
                    glVertex3fv(upper_left.ptr());

                    // lower left
                    glTexCoord2f(0, 0);
                    glNormal3fv(normal.ptr());
                    glVertex3fv(lower_left.ptr());

                    // upper right
                    glTexCoord2f(1, 1);
                    glNormal3fv(normal.ptr());
                    glVertex3fv(upper_right.ptr());

                    // lower right
                    glTexCoord2f(0, 1);
                    glNormal3fv(normal.ptr());
                    glVertex3fv(lower_right.ptr());
                }
                glEnd();                                                                                            CHECK_GL_ERRORS();
            } // draw_billboard()


            
            //
            checkered_box::checkered_box(const string & name, node *parent, gsgl::real_t radius)
                : node(name, parent), radius(radius), normals(vbuffer::STATIC), vertices(vbuffer::STATIC)
            {
            } // checkered_box::checkered_box()


            checkered_box::~checkered_box()
            {
            } // checkered_box::~checkered_box()


            static void add_vertex(vertex_buffer & vb, gsgl::real_t x, gsgl::real_t y, gsgl::real_t z,
                                   vertex_buffer & nb, gsgl::real_t nx, gsgl::real_t ny, gsgl::real_t nz, 
                                   gsgl::real_t radius)
            {
                vb.append(x * radius);
                vb.append(y * radius);
                vb.append(z * radius);

                vector nn(nx, ny, nz);
                nn.normalize();

                nb.append(nn.get_x());
                nb.append(nn.get_y());
                nb.append(nn.get_z());
            } // add_vertex()


            void checkered_box::init(const simulation_context *)
            {
                // top face
                add_vertex(vertices, -1, -1,  1,  normals,  0,  0,  1,  radius);
                add_vertex(vertices, -1,  1,  1,  normals,  0,  0,  1,  radius);
                add_vertex(vertices,  1,  1,  1,  normals,  0,  0,  1,  radius);
                add_vertex(vertices,  1, -1,  1,  normals,  0,  0,  1,  radius);

                // left face
                add_vertex(vertices, -1, -1,  1,  normals, -1,  0,  0,  radius);
                add_vertex(vertices, -1, -1, -1,  normals, -1,  0,  0,  radius);
                add_vertex(vertices, -1,  1, -1,  normals, -1,  0,  0,  radius);
                add_vertex(vertices, -1,  1,  1,  normals, -1,  0,  0,  radius);

                // back face
                add_vertex(vertices, -1,  1,  1,  normals,  0,  1,  0,  radius);
                add_vertex(vertices, -1,  1, -1,  normals,  0,  1,  0,  radius);
                add_vertex(vertices,  1,  1, -1,  normals,  0,  1,  0,  radius);
                add_vertex(vertices,  1,  1,  1,  normals,  0,  1,  0,  radius);

                // right face
                add_vertex(vertices,  1,  1,  1,  normals,  1,  0,  0,  radius);
                add_vertex(vertices,  1,  1, -1,  normals,  1,  0,  0,  radius);
                add_vertex(vertices,  1, -1, -1,  normals,  1,  0,  0,  radius);
                add_vertex(vertices,  1, -1,  1,  normals,  1,  0,  0,  radius);

                // front face
                add_vertex(vertices, -1, -1,  1,  normals,  0, -1,  0,  radius);
                add_vertex(vertices,  1, -1,  1,  normals,  0, -1,  0,  radius);
                add_vertex(vertices,  1, -1, -1,  normals,  0, -1,  0,  radius);
                add_vertex(vertices, -1, -1, -1,  normals,  0, -1,  0,  radius);

                // bottom face
                add_vertex(vertices, -1, -1, -1,  normals,  0,  0, -1,  radius);
                add_vertex(vertices,  1, -1, -1,  normals,  0,  0, -1,  radius);
                add_vertex(vertices,  1,  1, -1,  normals,  0,  0, -1,  radius);
                add_vertex(vertices, -1,  1, -1,  normals,  0,  0, -1,  radius);
            } // checkered_box::init()


            void checkered_box::draw(const simulation_context *, const drawing_context *)
            {
                return;

#if 0
                glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

                glEnableClientState(GL_VERTEX_ARRAY);                                                               CHECK_GL_ERRORS();
                glEnableClientState(GL_NORMAL_ARRAY);                                                               CHECK_GL_ERRORS();

                platform::color cc(0.8f, 0.8f, 0.8f, 0.8f);
                glColor4fv(cc.ptr());

                glEnable(GL_LIGHTING);                                                                              CHECK_GL_ERRORS();

                glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);                                                CHECK_GL_ERRORS();
                glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);                                                    CHECK_GL_ERRORS();

                glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, cc.ptr());                                       CHECK_GL_ERRORS();
                glMaterialfv(GL_FRONT, GL_SPECULAR, color::BLACK.ptr());                                        CHECK_GL_ERRORS();
                glMaterialfv(GL_FRONT, GL_EMISSION, color::BLACK.ptr());                                        CHECK_GL_ERRORS();
                glMaterialf(GL_FRONT, GL_SHININESS, 8);                                                             CHECK_GL_ERRORS();

                normals.bind();
                glNormalPointer(GL_FLOAT, 0, 0);                                                                    CHECK_GL_ERRORS();

                vertices.bind();
                glVertexPointer(3, GL_FLOAT, 0, 0);                                                                 CHECK_GL_ERRORS();

                glDrawArrays(GL_QUADS, 0, vertices.size()/3);                                                       CHECK_GL_ERRORS();

                glPopClientAttrib();                                                                                CHECK_GL_ERRORS();
                glPopAttrib();                                                                                      CHECK_GL_ERRORS();
#endif
            } // checkered_box::draw()


            void checkered_box::update(const simulation_context *c)
            {
                node::update(c);
            } // checkered_box::update()


            void checkered_box::cleanup(const simulation_context *c)
            {
                node::cleanup(c);

                normals.unload();
                vertices.unload();
            } // checkered_bod::cleanup()


            gsgl::real_t checkered_box::draw_priority(const simulation_context *, const drawing_context *)
            {
                return node::NODE_DRAW_TRANSLUCENT;
            } // checkered_box::draw_priority()


            gsgl::real_t checkered_box::view_radius() const
            {
                return radius*2;
            } // checkered_box::view_radius()

            gsgl::real_t checkered_box::default_view_distance() const
            {
                return radius - 1.0f;
            }

            gsgl::real_t checkered_box::minimum_view_distance() const
            {
                return 1.0f;
            }

        } // namespace utils

    } // namespace scenegraph

} // namespace gsgl
