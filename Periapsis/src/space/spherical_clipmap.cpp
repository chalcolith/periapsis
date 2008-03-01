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

#include "space/spherical_clipmap.hpp"

#include "math/quaternion.hpp"
#include "platform/vbuffer.hpp"
#include "platform/lowlevel.hpp"

#include <cmath>

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::platform;
using namespace gsgl::scenegraph;


namespace periapsis
{

    namespace space
    {

        class clipmap_ring
        {
        public:
            spherical_clipmap *parent;
            clipmap_ring *prev, *next;

            int level; // if 0, then start_phi and end_phi are set manually, if > 0 then automatically
            double start_phi; ///< The angle from the center of the hemisphere to the outer edge of the ring.
            double end_phi;   ///< The angle from the center of the hemisphere to the inner edge of the ring.

            data::array<gsgl::real_t> geographical_lon_and_lat; ///< Stores the geographical longitude and latitudes for vertices of the unrotated hemisphere facing north.
            data::array<gsgl::real_t> geographical_x_and_y;     ///< Stores the unrotated x & y coordinates for vertices of the unrotated hemisphere facing north.

            vertex_buffer vertices; ///< Stores the vertices of the rotated spheroid (not yet adjusted for height).
            vertex_buffer normals;  ///< Stores the normals of the rotated spheroid (not yet adjusted for terrain, but needed because the vertices are of the sphereoid, not a sphere).
            index_buffer indices;

            clipmap_ring(spherical_clipmap *parent, int level);
            clipmap_ring(spherical_clipmap *parent, double start_phi, double end_phi);
            ~clipmap_ring();

            void init();
            void draw();
        }; // class clipmap_ring


        clipmap_ring::clipmap_ring(spherical_clipmap *parent, int level)
            : parent(parent), prev(0), next(0), start_phi(0), end_phi(0), vertices(vbuffer::STATIC), indices(vbuffer::STATIC)
        {
            assert(level > 0);

            start_phi = ::pow(2.0, -level) * math::PI;
            end_phi = ::pow(2.0, -level-1) * math::PI;

            init();
        } // clipmap_ring::clipmap_ring()


        clipmap_ring::clipmap_ring(spherical_clipmap *parent, double start_phi, double end_phi)
            : parent(parent), prev(0), next(0), level(0), start_phi(start_phi), end_phi(end_phi), vertices(vbuffer::DYNAMIC), indices(vbuffer::DYNAMIC)
        {
            // we need to init each new frame...
        } // clipmap_ring::clipmap_ring()


        clipmap_ring::~clipmap_ring()
        {
        } // clipmap_ring::~clipmap_ring()


        static const int PHI_STEPS = 8;
        static const int THETA_STEPS = 32;

        void clipmap_ring::init()
        {
            // vertices of the unrotated hemisphere pointing north
            geographical_lon_and_lat.clear();

            for (int lat = 0; lat <= PHI_STEPS; ++lat)
            {
                double phi = 0;

                if (level)
                {
                    double exp = static_cast<double>(-lat) / static_cast<double>(PHI_STEPS);
                    phi = start_phi * ::pow(2.0, exp);
                }
                else
                {
                    double delta_phi = (start_phi - end_phi) / static_cast<double>(PHI_STEPS);
                    phi = start_phi - delta_phi * lat;
                }

                for (int lon = 0; lon <= THETA_STEPS; ++lon)
                {
                    double theta = (static_cast<double>(lon) / static_cast<double>(THETA_STEPS)) * math::PI_TIMES_2;

                    // geographical polar coordinates
                    geographical_lon_and_lat.append(static_cast<gsgl::real_t>(theta));
                    geographical_lon_and_lat.append(static_cast<gsgl::real_t>(phi));

                    // geographical cartesian coordinates (of a theoretical sphere)

                }
            }

            // only update indices the first time
            if (indices.size() == 0)
            {
                unsigned int stride = THETA_STEPS+1;
                for (unsigned int j = 0; j < PHI_STEPS; ++j)
                {
                    for (unsigned int i = 0; i <= THETA_STEPS; ++i)
                    {
                        indices.append((j+1)*stride + i); // down
                        indices.append(j*stride + i); // up
                    }
                }
            }
        } // clipmap_ring::init()


        void clipmap_ring::draw()
        {
            vertices.bind();
            glVertexPointer(2, GL_FLOAT, 0, 0);                                                                                                 CHECK_GL_ERRORS();

            indices.bind();

            int index_stride = (THETA_STEPS+1)*2;
            for (int i = 0; i < PHI_STEPS; ++i)
            {
                glDrawElements(GL_TRIANGLE_STRIP, index_stride, GL_UNSIGNED_INT, vbuffer::VBO_OFFSET(i*index_stride*sizeof(unsigned int)));     CHECK_GL_ERRORS();
            }
        } // clipmap_ring::draw()


        void clipmap_ring::update()
        {
        } // clipmap_ring::update()


        //////////////////////////////////////////////////////////////

        spherical_clipmap::spherical_clipmap(gsgl::real_t polar_radius, gsgl::real_t equatorial_radius)
            : polar_radius(polar_radius), equatorial_radius(equatorial_radius), first_ring(0), last_ring(0), clipmap_cap(0)
        {
            clipmap_cap = new clipmap_ring(this, 0, 0);
        } // spherical_clipmap::spherical_clipmap()


        spherical_clipmap::~spherical_clipmap()
        {
            for (int i = 0; i < clipmap_rings.size(); ++i)
            {
                delete clipmap_rings[i];
            }

            delete clipmap_cap;
        } // spherical_clipmap::~spherical_clipmap()


        void spherical_clipmap::init(context *)
        {
        } // spherical_clipmap::init()


        void spherical_clipmap::update(const transform & modelview, gsgl::real_t field_of_view, int screen_height)
        {
            // get maximum and minimum ring angles to draw
            double radius = gsgl::min_val(polar_radius, equatorial_radius);

            vector pt_in_eye_space = modelview * vector::ZERO;
            double altitude = pt_in_eye_space.mag() - radius;
            if (altitude < 1)
                altitude = 1;

            double max_phi = ::acos(radius / (radius + altitude));
            double min_phi = 100.0 * altitude * ::tan(field_of_view * math::DEG2RAD / static_cast<double>(screen_height)) / radius;

            // get first ring level to draw
            first_ring = 0;
            while (true)
            {
                while (first_ring >= clipmap_rings.size())
                    clipmap_rings.append(new clipmap_ring(this, clipmap_rings.size()+1));

                clipmap_ring *cur_ring = clipmap_rings[first_ring];
                if (cur_ring->start_phi > max_phi && cur_ring->end_phi < max_phi)
                    break;
                else
                    ++first_ring;
            }

            // get last ring to draw
            last_ring = 0;
            while (true)
            {
                while (last_ring >= clipmap_rings.size())
                    clipmap_rings.append(new clipmap_ring(this, clipmap_rings.size()+1));

                clipmap_ring *cur_ring = clipmap_rings[last_ring];
                if (cur_ring->end_phi < min_phi)
                    break;
                else
                    ++last_ring;
            }

            // adjust cap to match rings
            if (clipmap_cap->start_phi != clipmap_rings[last_ring]->end_phi)
            {
                clipmap_cap->start_phi = clipmap_rings[last_ring]->end_phi;
                clipmap_cap->init();
            }

            // update clipmap rings' pointers
            for (int i = 0; i < clipmap_rings.size(); ++i)
            {
                clipmap_rings[i]->prev = (i > 0) ? clipmap_rings[i-1] : 0;
                clipmap_rings[i]->next = (i < clipmap_rings.size()-1) ? clipmap_rings[i+1] : 0;
            }

            clipmap_cap->prev = clipmap_rings[last_ring];
            clipmap_cap->prev->next = clipmap_cap;
        } // spherical_clipmap::update()


        static const color GRAY(0.6f, 0.6f, 0.6f);


        void spherical_clipmap::draw(context *c)
        {
            // don't bother drawing on the first frame, as we don't have a modelview yet...
            if (clipmap_rings.size())
            {
                glPushAttrib(GL_ALL_ATTRIB_BITS);
                glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

                glEnableClientState(GL_VERTEX_ARRAY);
                glEnableClientState(GL_INDEX_ARRAY);

                glEnable(GL_VERTEX_PROGRAM_POINT_SIZE);

                color::WHITE.set();

                for (int i = 0; i <= last_ring; ++i)
                {
                    if (i >= first_ring)
                        clipmap_rings[i]->draw();

                    if (c->render_flags & context::RENDER_WIREFRAME)
                    {
                        if (i % 2)
                            color::WHITE.set();
                        else
                            GRAY.set();
                    }
                }

                clipmap_cap->draw();

                //
                glPopClientAttrib();
                glPopAttrib();
            }
        } // spherical_clipmap::draw()


    } // namespace space

} // namespace periapsis






#if 0

        static const int PHI_STEPS = 8;
        static const int THETA_STEPS = 128;


        class clipmap_ring
        {
        public:
            spherical_clipmap *parent;
            clipmap_ring *prev, *next;

            heightmap *hmap;
            double map_offset;
            
            int level; // if 0, then start_phi and end_phi are set manually, if > 0 then automatically
            double start_phi, end_phi;

            double old_view_phi, old_view_theta;

            quaternion roty, roty_conj;
            quaternion rotz, rotz_conj;

            int vbuf_id, ibuf_id;
            array<double> cbuf;       // phi/theta 
            array<float> vbuf;        // vertices (s, t, nx, ny, nz, x, y, z)
            array<unsigned int> ibuf; // indices

            clipmap_ring(spherical_clipmap *parent, int level, heightmap *hmap, double map_offset);
            clipmap_ring(spherical_clipmap *parent, double start_phi, double end_phi, heightmap *hmap, double map_offset);
            ~clipmap_ring();

            void update(const vector & eye_dir);
            void draw();

            void generate_vertices();
            void generate_indices();
            void repair_quad(unsigned int *indices);

            vector get_vertex(int i, int j);
            void generate_normals();
        }; // class clipmap_ring


        clipmap_ring::clipmap_ring(spherical_clipmap *parent, int level, heightmap *hmap, double map_offset)
            : parent(parent), prev(0), next(0), hmap(hmap), map_offset(map_offset), start_phi(0), end_phi(0), old_view_phi(12345), old_view_theta(12345), vbuf_id(0), ibuf_id(0)
        {
            assert(level > 0);

            start_phi = ::pow(2.0, -level) * math::PI;
            end_phi = ::pow(2.0, -level-1) * math::PI;
        } // clipmap_ring::clipmap_ring()

        clipmap_ring::clipmap_ring(spherical_clipmap *parent, double start_phi, double end_phi, heightmap *hmap, double map_offset)
            : parent(parent), prev(0), next(0), hmap(hmap), map_offset(map_offset), level(0), start_phi(start_phi), end_phi(end_phi), old_view_phi(12345), old_view_theta(12345), vbuf_id(0), ibuf_id(0)
        {
        } // clipmap_ring::clipmap_ring()


        clipmap_ring::~clipmap_ring()
        {
            if (vbuf_id)
                glDeleteBuffers(1, (GLuint *) &vbuf_id);
            if (ibuf_id)
                glDeleteBuffers(1, (GLuint *) &ibuf_id);
        } // clipmap_ring::clipmap_ring()


        void clipmap_ring::generate_vertices()
        {
            // generate vertices (PHI_STEPS rings of THETA_STEPS+1 vertices)
            cbuf.clear();
            vbuf.clear();

            double delta_theta = 2.0 * math::PI / static_cast<double>(THETA_STEPS);

            for (int j = 0; j <= PHI_STEPS; ++j)
            {
                double eye_phi = 0;

                if (level)
                {
                    double exp = static_cast<double>(-j) / static_cast<double>(PHI_STEPS);
                    eye_phi = start_phi * ::pow(2.0, exp);
                }
                else
                {
                    double delta_phi = (start_phi - end_phi) / static_cast<double>(PHI_STEPS);
                    eye_phi = start_phi - delta_phi * static_cast<double>(j);
                }


                for (int i = 0; i <= THETA_STEPS; ++i)
                {
                    double eye_theta = delta_theta * static_cast<double>(i);

                    double eye_x = ::cos(eye_theta) * ::sin(eye_phi);
                    double eye_y = ::sin(eye_theta) * ::sin(eye_phi);
                    double eye_z = ::cos(eye_phi);

                    quaternion eye_point(0, eye_x, eye_y, eye_z);
                    quaternion yrot_point = (roty * eye_point) * roty_conj;
                    quaternion obj_point = (rotz * yrot_point) * rotz_conj;

                    double obj_x = obj_point.x;
                    double obj_y = obj_point.y;
                    double obj_z = obj_point.z;

                    double obj_phi = ::acos(obj_z);
                    double obj_theta = ::atan2(obj_y, obj_x) + (map_offset * math::PI_TIMES_2);
                    if (obj_theta < 0.0)
                        obj_theta += math::PI_TIMES_2;

                    cbuf.append(obj_phi);
                    cbuf.append(obj_theta);

                    float s = static_cast<float>(obj_theta / math::PI_TIMES_2);
                    float t = 1.0f - static_cast<float>(obj_phi / math::PI);

                    float nx = static_cast<float>(obj_x);
                    float ny = static_cast<float>(obj_y);
                    float nz = static_cast<float>(obj_z);

                    float x = parent->get_equatorial_radius() * nx;
                    float y = parent->get_equatorial_radius() * ny;
                    float z = parent->get_polar_radius() * nz;

                    if (hmap)
                    {
                        double hval, alpha;
                        hmap->get_data(s, t, hval, alpha);

                        vector old_vertex(x, y, z);
                        vector normal(nx, ny, nz);

                        gsgl::real_t alt = static_cast<gsgl::real_t>(hval * hmap->get_altitude());

                        vector new_vertex = old_vertex + normal*alt;

                        x = new_vertex[0];
                        y = new_vertex[1];
                        z = new_vertex[2];
                    }

                    vbuf.append(s);
                    vbuf.append(t);
                    vbuf.append(nx);
                    vbuf.append(ny);
                    vbuf.append(nz);
                    vbuf.append(x);
                    vbuf.append(y);
                    vbuf.append(z);
                }
            }
        } // clipmap_ring::generate_vertices()


        vector clipmap_ring::get_vertex(int i, int j)
        {
            clipmap_ring *rp = this;

            if (j < 0)
            {
                if (prev)
                {
                    rp = prev;
                    j = PHI_STEPS + j;
                }
                else
                {
                    j = 0;
                }
            }
            else if (j > PHI_STEPS)
            {
                if (next)
                {
                    rp = next;
                    j = j - PHI_STEPS;
                }
                else if (rp->level == 0)
                {
                    j = PHI_STEPS-1;
                    i += THETA_STEPS;
                }
                else
                {
                    j = PHI_STEPS;
                }
            }

            if (i < 0)
                i = i + THETA_STEPS;
            else if (i > THETA_STEPS)
                i = i - THETA_STEPS;


            int index = i + j*(THETA_STEPS+1);

            float x = rp->vbuf[index*8 + 5];
            float y = rp->vbuf[index*8 + 6];
            float z = rp->vbuf[index*8 + 7];

            return vector(x, y, z);
        } // clipmap_ring::get_vertex()


        void clipmap_ring::generate_normals()
        {
            for (int j = 0; j <= PHI_STEPS; ++j)
            {
                for (int i = 0; i <= THETA_STEPS; ++i)
                {
                    int index = i + j*(THETA_STEPS+1);
                    
                    vector x0 = get_vertex(i-1, j);
                    vector x1 = get_vertex(i,   j);
                    vector x2 = get_vertex(i+1, j);
                    //vector xx = (x1 - x0) + (x2 - x1);
                    vector xx = x2 - x0;

                    vector y0 = get_vertex(i, j-1);
                    vector y1 = get_vertex(i, j);
                    vector y2 = get_vertex(i, j+1);
                    //vector yy = (y1 - y0) + (y2 - y1);
                    vector yy = y2 - y0;

                    vector nn = xx.cross(yy);
                    nn.normalize();

                    vbuf[index*8 + 2] = nn[0];
                    vbuf[index*8 + 3] = nn[1];
                    vbuf[index*8 + 4] = nn[2];
                }
            }
        } // clipmap_ring::generate_normals()


        void clipmap_ring::generate_indices()
        {
            unsigned int indices[4];

            ibuf.clear();

            for (int j = 0; j < PHI_STEPS; ++j)
            {
                for (int i = 0; i < THETA_STEPS; ++i)
                {
                    indices[0] = static_cast<unsigned int>( (j+0) * (THETA_STEPS+1) + (i+0) ); // lower left
                    indices[1] = static_cast<unsigned int>( (j+0) * (THETA_STEPS+1) + (i+1) ); // lower right
                    indices[2] = static_cast<unsigned int>( (j+1) * (THETA_STEPS+1) + (i+1) ); // upper right
                    indices[3] = static_cast<unsigned int>( (j+1) * (THETA_STEPS+1) + (i+0) ); // upper left
                    
                    repair_quad(indices);

                    ibuf.append(indices[0]);
                    ibuf.append(indices[1]);
                    ibuf.append(indices[3]);

                    ibuf.append(indices[2]);
                    ibuf.append(indices[3]);
                    ibuf.append(indices[1]);
                }
            }
        } // clipmap_ring::generate_indices()


        void clipmap_ring::repair_quad(unsigned int *indices)
        {
            int quadrant_one = 0;
            int quadrant_four = 0;

            for (int i = 0; i < 4; ++i)
            {
                double theta = cbuf[ indices[i]*2 + 1 ];
                if (theta < math::PI_TIMES_2 && theta > math::PI * 0.75)
                    quadrant_four++;
                else if (theta < math::PI_OVER_2)
                    quadrant_one++;
            }

            //
            if (quadrant_one && quadrant_four)
            {
                for (int i = 0; i < 4; ++i)
                {
                    double phi   = cbuf[ indices[i]*2 + 0 ];
                    double theta = cbuf[ indices[i]*2 + 1 ];
                    if (theta > math::PI * 0.75)
                        theta -= math::PI_TIMES_2;

                    unsigned int orig_index = indices[i];
                    unsigned int new_index = vbuf.size() / 8;
                    indices[i] = new_index;
                    
                    float s = static_cast<float>(theta / math::PI_TIMES_2);
                    float t = 1.0f - static_cast<float>(phi / math::PI);
                    
                    vbuf.append(s);
                    vbuf.append(t);
                    vbuf.append( vbuf[orig_index*8 + 2] );
                    vbuf.append( vbuf[orig_index*8 + 3] );
                    vbuf.append( vbuf[orig_index*8 + 4] );
                    vbuf.append( vbuf[orig_index*8 + 5] );
                    vbuf.append( vbuf[orig_index*8 + 6] );
                    vbuf.append( vbuf[orig_index*8 + 7] );
                }
            }
        } // clipmap_ring::repair_quad()


        static const double VIEW_EPSILON = 0.005;


        void clipmap_ring::update(const vector & eye_dir_in_object_space)
        {
            // get quaternion rotations for clipmap
            double view_phi = ::acos(eye_dir_in_object_space[2]);
            double view_theta = ::atan2(eye_dir_in_object_space[1], eye_dir_in_object_space[0]);

            if ( level && !( (::fabs(view_phi - old_view_phi) > VIEW_EPSILON) || (::fabs(view_theta - old_view_theta) > VIEW_EPSILON) ) )
            {
                return;
            }

            old_view_phi = view_phi;
            old_view_theta = view_theta;

            roty = quaternion(vector::Y_AXIS, view_phi);
            roty_conj = roty.conjugate();

            rotz = quaternion(vector::Z_AXIS, view_theta);
            rotz_conj = rotz.conjugate();
          
            // generate vertices & indices
            generate_vertices();
            generate_indices();

            // create buffers if necessary
            if (!vbuf_id)
            {
                glGenBuffers(1, (GLuint *) &vbuf_id);
                if (!vbuf_id)
                    throw opengl_exception(__FILE__, __LINE__, L"clipmap_ring unable to generate buffer id: ");
            }

            if (!ibuf_id)
            {
                glGenBuffers(1, (GLuint *) &ibuf_id);
                if (!ibuf_id)
                    throw opengl_exception(__FILE__, __LINE__, L"clipmap_ring unable to generate buffer id");
            }

            // update buffers
            glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
            glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vbuf.size(), vbuf.ptr(), GL_STREAM_DRAW);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf_id);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * ibuf.size(), ibuf.ptr(), GL_STREAM_DRAW);
        } // clipmap_ring::update()


        void clipmap_ring::draw()
        {
            glBindBuffer(GL_ARRAY_BUFFER, vbuf_id);
            glInterleavedArrays(GL_T2F_N3F_V3F, 0, 0);

            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibuf_id);
            glDrawElements(GL_TRIANGLES, ibuf.size(), GL_UNSIGNED_INT, 0);
        } // clipmap_ring::draw()
#endif

