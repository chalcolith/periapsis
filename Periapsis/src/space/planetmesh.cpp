//
// $Id: planetmesh.cpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "planetmesh.hpp"
#include "astronomy.hpp"

#include "data/array.hpp"
#include "data/stack.hpp"
#include "data/fstream.hpp"

#include "scenegraph/camera.hpp"

#include "graphics/platform.hpp"
#include "graphics/extensions.hpp"

#include <math.h>
#include <string.h>
#include <stdio.h>

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::io;
using namespace gsgl::math;
using namespace gsgl::scenegraph;

namespace periapsis
{

    struct heightfield
    {
        // in radians
        int width, height;
        gsgl::real_t left, right, bottom, top;
        gsgl::real_t min, max;
        array<gsgl::real_t> data;
        
        heightfield()
        {
            left = (gsgl::real_t) 0.0 * RADIANS;
            right = (gsgl::real_t) 360.0 * RADIANS;
            bottom = (gsgl::real_t) -90.0 * RADIANS;
            top = (gsgl::real_t) 90.0 * RADIANS;
            min = (gsgl::real_t) 0.0;
            max = (gsgl::real_t) 1.0;
        }
    }; // struct heightfield
    
    struct colortile
    {
        int width, height;
        gsgl::real_t left, right, bottom, top;
        array<unsigned char> data;
        
        colortile()
        {
            left = (gsgl::real_t) 0.0 * RADIANS;
            right = (gsgl::real_t) 360.0 * RADIANS;
            bottom = (gsgl::real_t) -90.0 * RADIANS;
            top = (gsgl::real_t) 90.0 * RADIANS;
        }
    }; // struct colortile
    
    
    /// A node in the planetary terrain mesh.
    struct triangle
    {
        int level;                  ///< How deep the triangle is in the tree.
        gsgl::index_t leaf_index; ///< If this triangle is a leaf, where its vertex indices are stored in the element array.
        
        /// Relationships to other triangles.
        triangle *parent, *left, *right, *neighbors[3];
        
        diamond *di;

        /// Indices of the triangles vertices, and its (correct) midpoint.
        gsgl::index_t vertices[4];
        
        gsgl::math::vector surface_normal;
        gsgl::math::vector midpoint; ///< The midpoint of side 0.

        gsgl::real_t midpoint_error;
        
        triangle()
            : parent(0), left(0), right(0), di(0)
        {
        }
        
        ~triangle()
        {
            delete left;
            delete right;
        }
    }; // struct triangle

    //////////////////////////////////////////////

    struct diamond
    {
        unsigned long long frame;
        gsgl::index_t pool_index;
        triangle *leaves[2];
        triangle *nodes[4];
        gsgl::index_t midpoint_index;
        gsgl::real_t midpoint_error;
    }; // struct diamond

    //////////////////////////////////////////////
    
    planetmesh::planetmesh(const config & conf, gsgl::real_t vscale) 
        : node(conf), vscale(vscale)
    {
        // get parameters
        eq_r = (gsgl::real_t) conf[L"equatorial_radius"].to_real();
        pol_r = (gsgl::real_t) conf[L"polar_radius"].to_real();

        // load data
        iterator<config *> child;
        for (child = conf.get_children().iter(); child.has_next(); ++child)
        {
            const string & child_name = (*child)->get_name();

            if (child_name == L"heightfield")
            {
                load_height_field(**child);
            }
            else if (child_name == L"colortile")
            {
                load_color_tile(**child);
            }
            else
            {
                throw EXP(runtime_exception, L"unknown property %ls in planetmesh file %ls", child_name.w_string(), conf.get_file().get_name().w_string());
            }
        }
        
        // allocate dummy first indices
        gsgl::index_t li = leaf_pool.allocate();
        leaf_pool[li] = 0;

        gsgl::index_t vi = vertex_pool.allocate();
        vertex_pool[vi] = 0;
        
        for (int i = 0; i < 3; ++i)
        {
            vertices[i]     = 0;
            normals[i]      = 0;
            geo_coords[i]   = 0;
            colors[i]       = 0;
            draw_indices[i] = 0;
        }
        
        // initialize buffer ids
        vertex_buffer  = 0;
        normal_buffer  = 0;
        color_buffer   = 0;
        element_buffer = 0;
        
        // initialize root nodes
        init_root_nodes();
    } // planetmesh::planetmesh()
    
    planetmesh::~planetmesh()
    {
        for (int i = 0; i < 12; ++i)
            delete root_nodes[i];

        iterator<diamond *> di;
        for (di = diamond_pool.iter(); di.has_next(); ++di)
            delete *di;

        iterator<heightfield *> hf;
        for (hf = height_fields.iter(); hf.has_next(); ++hf)
            delete *hf;

        iterator<colortile *> ct;
        for (ct = color_tiles.iter(); ct.has_next(); ++ct)
            delete *ct;
    } // planetmesh::~planetmesh()
    
    //
    
    gsgl::real_t planetmesh::get_priority(const context *)
    {
        return this->view_distance();
    } // planetmesh::get_priority()
    
    void planetmesh::update(const context *c)
    {
        // skip the first frame so the modelview and projection matrices get updated...
        if (c->frame == 0)
            return;
            
        // split triangles
        int i, len = leaf_pool.size();
        pointer<triangle *> leaves_to_process = new triangle *[len];
        ::memcpy(leaves_to_process, leaf_pool.get_data(), sizeof(triangle *) * len);

        for (i = 0; i < len; ++i)
        {
            triangle *tri = leaves_to_process[i];

            if (tri && tri->leaf_index)
            {
                split_node(tri, c);
            }
        }

        // merge diamonds
        len = diamond_pool.size();
        pointer<diamond *> diamonds_to_process = new diamond *[len];
        ::memcpy(diamonds_to_process, diamond_pool.get_data(), sizeof(diamond *) * len);

        for (i = 0; i < len; ++i)
        {
            diamond *di = diamonds_to_process[i];

            if (di)
                merge_diamond(di, c);
        }
    } // planetmesh::update()
    
    void planetmesh::draw(const context *c)
    {
        if (!c->cam)
            return;
        
        modelview_rot = this->get_modelview().rotation_transform();
        
        // initialize buffer objects if necessary
        if (vertex_buffer == 0)
            glGenBuffers(1, (GLuint *) &vertex_buffer);
        if (normal_buffer == 0)
            glGenBuffers(1, (GLuint *) &normal_buffer);
        if (color_buffer == 0)
            glGenBuffers(1, (GLuint *) &color_buffer);
        if (element_buffer == 0)
            glGenBuffers(1, (GLuint *) &element_buffer);
            
        // save state
        glPushAttrib(GL_ENABLE_BIT);
        
        glEnable(GL_DEPTH_TEST);        
        //glEnable(GL_CULL_FACE);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(3.0f);

        // set up projection
        /// \todo Move this to the parent planet node...
        glMatrixMode(GL_PROJECTION);
        glPushMatrix();
        glLoadIdentity();
        
        gsgl::real_t view_dist = this->view_distance();
        gsgl::real_t far_plane = view_dist + eq_r + 10000.0f;
        gsgl::real_t near_plane = (view_dist - eq_r) > 0 ? (view_dist - eq_r) * 2.0f / 3.0f : 1.0f;
        
        gluPerspective(c->cam->get_field_of_view(), c->cam->get_aspect_ratio(), near_plane, far_plane);
        
        // draw triangles...
        glEnableClientState(GL_VERTEX_ARRAY);
        glEnableClientState(GL_NORMAL_ARRAY);
        glEnableClientState(GL_COLOR_ARRAY);
        glEnableClientState(GL_INDEX_ARRAY);

        // set up buffers
        glBindBuffer(GL_ARRAY_BUFFER_ARB, vertex_buffer);
        glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(gsgl::real_t) * vertices.size(), vertices.ptr(), GL_STREAM_DRAW_ARB);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER_ARB, normal_buffer);
        glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(gsgl::real_t) * normals.size(), normals.ptr(), GL_STREAM_DRAW_ARB);
        glNormalPointer(GL_FLOAT, 0, 0);
                
        glBindBuffer(GL_ARRAY_BUFFER_ARB, color_buffer);
        glBufferData(GL_ARRAY_BUFFER_ARB, sizeof(unsigned char) * colors.size(), colors.ptr(), GL_STREAM_DRAW_ARB);
        glColorPointer(3, GL_UNSIGNED_BYTE, 0, 0);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, element_buffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gsgl::index_t) * draw_indices.size(), draw_indices.ptr(), GL_STREAM_DRAW_ARB);
        glDrawElements(GL_TRIANGLES, draw_indices.size(), GL_UNSIGNED_INT, 0);        
        
        // clean up
        glMatrixMode(GL_PROJECTION);
        glPopMatrix();
        
        glPopAttrib();
    } // planetmesh::draw()
    
    //
    
    template <typename F, typename D> 
    D get_field_val(list<F *> & fields, gsgl::real_t lat, gsgl::real_t lon, gsgl::index_t stride, gsgl::index_t offset)
    {
        iterator<F *> i;
        for (i = fields.iter(); i.has_next(); ++i)
        {
            F *hf = *i;
            gsgl::real_t dpp_x = (hf->right - hf->left) / hf->width;
            gsgl::real_t dpp_y = (hf->top - hf->bottom) / hf->height;
            
            if ((lat <= hf->top && lat >= hf->bottom) && (lon >= hf->left && lon <= hf->right))
            {
                gsgl::real_t u = (lon - hf->left) / dpp_x;
                gsgl::real_t v = (lat - hf->bottom) / dpp_y;
                gsgl::real_t px, py, dx, dy;
                dx = modf(u, &px);
                dy = modf(v, &py);
                
                gsgl::real_t lon0 = dx < 0.5 ? lon - dpp_x : lon;
                gsgl::real_t lon1 = dx < 0.5 ? lon : lon + dpp_x;
                gsgl::real_t lat0 = dy < 0.5 ? lat - dpp_y : lat;
                gsgl::real_t lat1 = dy < 0.5 ? lat : lat + dpp_y;
                
                // this is cheating a bit
                if (lat0 < (gsgl::real_t) -90.0 * RADIANS)
                    lat0 = (gsgl::real_t) -90.0 * RADIANS;
                if (lat0 > (gsgl::real_t) 90.0 * RADIANS)
                    lat0 = (gsgl::real_t) 90.0 * RADIANS;
                    
                if (lat1 < (gsgl::real_t) -90.0 * RADIANS)
                    lat1 = (gsgl::real_t) -90.0 * RADIANS;
                if (lat1 > (gsgl::real_t) 90.0 * RADIANS)
                    lat1 = (gsgl::real_t) 90.0 * RADIANS;
                    
                if (lon0 < (gsgl::real_t) -180.0 * RADIANS)
                    lon0 += (gsgl::real_t) 360.0 * RADIANS;
                if (lon0 > (gsgl::real_t) 180.0 * RADIANS)
                    lon0 -= (gsgl::real_t) 360.0 * RADIANS;
                    
                if (lon1 < (gsgl::real_t) -180.0*RADIANS)
                    lon1 += (gsgl::real_t) 360.0*RADIANS;
                if (lon1 > (gsgl::real_t) 180.0*RADIANS)
                    lon1 -= (gsgl::real_t) 360.0*RADIANS;
                    
                gsgl::real_t h00, h01, h11, h10;
                bool b00 = false;
                bool b01 = false;
                bool b10 = false;
                bool b11 = false;
                
                if (lon0 < hf->left)
                {
                    h00 = get_field_val<F, D>(fields, lat0, lon0, stride, offset);
                    b00 = true;
                    h01 = get_field_val<F, D>(fields, lat1, lon0, stride, offset);
                    b01 = true;
                }
                
                if (lon1 > hf->right)
                {
                    h10 = get_field_val<F, D>(fields, lat0, lon1, stride, offset);
                    b10 = true;
                    h11 = get_field_val<F, D>(fields, lat1, lon1, stride, offset);
                    b11 = true;
                }
                
                if (lat0 < hf->bottom && (b00 || b10))
                {
                    h00 = get_field_val<F, D>(fields, lat0, lon0, stride, offset);
                    b00 = true;
                    h10 = get_field_val<F, D>(fields, lat0, lon1, stride, offset);
                    b10 = true;
                }
                
                if (lat1 > hf->top && (b01 || b11))
                {
                    h01 = get_field_val<F, D>(fields, lat1, lon0, stride, offset);
                    b01 = true;
                    h11 = get_field_val<F, D>(fields, lat1, lon1, stride, offset);
                    b11 = true;
                }
                
                gsgl::index_t u0 = ((gsgl::index_t) ((lon0 - hf->left) / dpp_x)) % hf->width;
                gsgl::index_t u1 = ((gsgl::index_t) ((lon1 - hf->left) / dpp_x)) % hf->width;
                gsgl::index_t v0 = ((gsgl::index_t) ((lat0 - hf->bottom) / dpp_y)) % hf->height;
                gsgl::index_t v1 = ((gsgl::index_t) ((lat1 - hf->bottom) / dpp_y)) % hf->height;
                
                if (!b00)
                    h00 = hf->data.at((u0 + v0*hf->width) * stride + offset);
                if (!b01)
                    h01 = hf->data.at((u0 + v1*hf->width) * stride + offset);
                if (!b10)
                    h10 = hf->data.at((u1 + v0*hf->width) * stride + offset);
                if (!b11)
                    h11 = hf->data.at((u1 + v1*hf->width) * stride + offset);
                    
                dx = dx < (gsgl::real_t) 0.5 ? dx + (gsgl::real_t) 0.5 : dx - (gsgl::real_t) 0.5;
                dy = dy < (gsgl::real_t) 0.5 ? dy + (gsgl::real_t) 0.5 : dy - (gsgl::real_t) 0.5;
                
                gsgl::real_t hx0 = h00 + (h10 - h00) * dx;
                gsgl::real_t hx1 = h01 + (h11 - h01) * dx;
                gsgl::real_t hy0 = h00 + (h01 - h00) * dy;
                gsgl::real_t hy1 = h10 + (h11 - h10) * dy;
                
                return (D) ((hx0 + hx1 + hy0 + hy1) / 4.0);
            }
        }
        
        return 0;
    } // get_field_val()
    
    gsgl::real_t planetmesh::get_height_field_alt(gsgl::real_t lat, gsgl::real_t lon)
    {
        return vscale * get_field_val<heightfield, gsgl::real_t>(height_fields, lat, lon, 1, 0);
    } // planetmesh::get_height_field_alt()
    
    void planetmesh::get_color_tile_color(gsgl::real_t lat, gsgl::real_t lon, unsigned char & r, unsigned char & g, unsigned char & b)
    {
        r = get_field_val<colortile, unsigned char>(color_tiles, lat, lon, 3, 0);
        g = get_field_val<colortile, unsigned char>(color_tiles, lat, lon, 3, 1);
        b = get_field_val<colortile, unsigned char>(color_tiles, lat, lon, 3, 2);
    } // planetmesh::get_color_tile_color()
    
    //
    
    gsgl::index_t planetmesh::allocate_vertex()
    {
        return vertex_pool.allocate();
    } // planetmesh::allocate_vertex()
    
    void planetmesh::release_vertex(const gsgl::index_t index)
    {
        for (int i = 0; i < 3; ++i)
        {
            vertices[index*3+i]   = 0;
            normals[index*3+i]    = 0;
            geo_coords[index*3+i] = 0;
            colors[index*3+i]     = 0;
        }
        vertex_pool.release(index);
    } // planetmesh::release_vertex()
    
    void planetmesh::add_leaf(triangle *tri)
    {
        tri->leaf_index = leaf_pool.allocate();
        leaf_pool[tri->leaf_index] = tri;
        
        for (int i = 0; i < 3; ++i)
        {
            draw_indices[tri->leaf_index*3+i] = tri->vertices[i];
        }
    } // planetmesh::add_leaf()
    
    void planetmesh::remove_leaf(triangle *tri)
    {
        if (tri->leaf_index)
        {
            for (int i = 0; i < 3; ++i)
            {
                draw_indices[tri->leaf_index*3+i] = 0;
            }
            leaf_pool[tri->leaf_index] = 0;
            leaf_pool.release(tri->leaf_index);
            tri->leaf_index = 0;
        }
    } // planetmesh::remove_leaf()

    void planetmesh::add_diamond(unsigned long long frame, index_t vertex_index, real_t midpoint_error, triangle *t0, triangle *t1, triangle *t2, triangle *t3)
    {
        if (t0->parent == t1->parent)
        {
            if (t3->parent == t2->parent)
                add_diamond(frame, vertex_index, midpoint_error, t0->parent, t3->parent);
            else
                throw INTERNAL_EXP(internal_exception, L"invalid diamond");
        }
        else if (t0->parent == t3->parent)
        {
            if (t1->parent == t2->parent)
                add_diamond(frame, vertex_index, midpoint_error, t1->parent, t0->parent);
            else
                throw INTERNAL_EXP(internal_exception, L"invalid diamond");
        }
        else
        {
            throw INTERNAL_EXP(internal_exception, L"invalid diamond");
        }
    } // planetmesh::add_diamond()


    void planetmesh::add_diamond(unsigned long long frame, index_t vertex_index, real_t midpoint_error, triangle *tri0, triangle *tri1)
    {
        index_t di_index = diamond_pool.allocate();
        diamond *di = new diamond();
        di->pool_index = di_index;
        diamond_pool[di_index] = di;
        di->frame = frame;

        di->midpoint_index = vertex_index;
        di->midpoint_error = midpoint_error;

        di->leaves[0] = tri0;
        di->leaves[1] = tri1;

        di->nodes[0] = tri0->right; tri0->right->di = di;
        di->nodes[1] = tri0->left;  tri0->left->di  = di;
        di->nodes[2] = tri1->right; tri1->right->di = di;
        di->nodes[3] = tri1->left;  tri1->left->di  = di;
    } // planetmesh::add_diamond()

    void planetmesh::remove_diamond(diamond *di)
    {
        diamond_pool[di->pool_index] = 0;
        diamond_pool.release(di->pool_index);

        for (int i = 0; i < 4; ++i)
            di->nodes[i]->di = 0;

        delete di;
    } // planetmesh::remove_diamond()

    //

    static void hook_up(triangle *tri, triangle *orig, triangle *sp)
    {
        for (int i = 0; i < 3; ++i)
        {
            if (tri->neighbors[i] == orig)
            {
                tri->neighbors[i] = sp;
                break;
            }
        }
    } // hook_up()
    
    gsgl::real_t planetmesh::pixel_error(gsgl::real_t d, gsgl::real_t r, const context *c)
    {
        gsgl::real_t theta = asin(d / r);
        gsgl::real_t pix_per_rad = c->cam->get_width() / (c->cam->get_field_of_view() * RADIANS);
        gsgl::real_t pe = theta * pix_per_rad;
        return pe;
    } // planetmesh::pixel_error()
    
    void planetmesh::init_node(triangle *tri, triangle *parent,
                               triangle *n0, triangle *n1, triangle *n2,
                               const gsgl::index_t v0, const gsgl::index_t v1, const gsgl::index_t v2)
    {
        tri->parent = parent;
        tri->neighbors[0] = n0;
        tri->neighbors[1] = n1;
        tri->neighbors[2] = n2;
        tri->vertices[0] = v0;
        tri->vertices[1] = v1;
        tri->vertices[2] = v2;
        make_surface_normal(tri);
        make_middle_vertex(tri);
    } // planetmesh::init_node()
    
    
    static const real_t max_pixel_error = 1.0f;


    bool planetmesh::split_node(triangle *tri, const context *c, bool force)
    {   
        if (tri->level > 8)
            return false;

        // visibility and error checks
        if (!force)
        {
            // back faces...
            vector tri_norm = modelview_rot * tri->surface_normal;
            tri_norm.normalize();

            vector tri_pos = this->get_modelview() * tri->midpoint;
            real_t dist = tri_pos.mag();
            tri_pos.normalize();

            real_t dot_prod = tri_norm.dot(tri_pos);

            if (dot_prod < 0.0f)
                return false;

            // ignore triangles with small error
            if (tri->level > 4)
            {
                gsgl::real_t pe = pixel_error(tri->midpoint_error, dist, c);
                
                if (pe < max_pixel_error)
                    return false;
            }
        }
        
        // split neighbor if necessary
        while (tri->neighbors[0]->level < tri->level)
        {
            if (!split_node(tri->neighbors[0], c, true))
                return false;
        }

        if (tri->neighbors[0]->level > tri->level || tri->neighbors[0]->leaf_index == 0)
        {
            throw INTERNAL_EXP(internal_exception, L"invalid leaf node neighbor");
        }
        
        // get new vertex
        gsgl::index_t vertex_index = tri->vertices[3];
        
        // make new nodes
        triangle *tri0 = tri;
        triangle *tri1 = tri->neighbors[0];

        tri0->left = new triangle();  tri0->left->level = tri->level + 1;
        tri0->right = new triangle(); tri0->right->level = tri->level + 1;
        tri1->left = new triangle();  tri1->left->level = tri->level + 1;
        tri1->right = new triangle(); tri1->right->level = tri->level + 1;

        init_node(tri0->left,  tri0, tri0->neighbors[2], tri1->right, tri0->right, tri0->vertices[2], tri0->vertices[0], vertex_index);
        init_node(tri0->right, tri0, tri0->neighbors[1], tri0->left,  tri1->left,  tri0->vertices[1], tri0->vertices[2], vertex_index);

        init_node(tri1->left,  tri1, tri1->neighbors[2], tri0->right, tri1->right, tri1->vertices[2], tri1->vertices[0], vertex_index);
        init_node(tri1->right, tri1, tri1->neighbors[1], tri1->left,  tri0->left,  tri1->vertices[1], tri1->vertices[2], vertex_index);
        
        add_leaf(tri0->left);
        add_leaf(tri0->right);
        add_leaf(tri1->left);
        add_leaf(tri1->right);

        hook_up(tri0->neighbors[2], tri0, tri0->left);
        hook_up(tri0->neighbors[1], tri0, tri0->right);

        hook_up(tri1->neighbors[2], tri1, tri1->left);
        hook_up(tri1->neighbors[1], tri1, tri1->right);

        // handle diamonds
        if (tri0->di)
        {
            remove_diamond(tri0->di);
        }
        if (tri1->di)
        {
            remove_diamond(tri1->di);
        }

        add_diamond(c->frame, vertex_index, tri->midpoint_error, tri0, tri1);

        // clean up
        remove_leaf(tri0);
        remove_leaf(tri1);

        return true;
    } // planetmesh::split_node()


    void planetmesh::merge_diamond(diamond *di, const context *c)
    {
        // don't bother checking diamonds just created
        if (c->frame - di->frame < 10)
            return;

        // back faces
        vector midpoint;
        midpoint[0] = vertices[di->midpoint_index*3+0];
        midpoint[1] = vertices[di->midpoint_index*3+1];
        midpoint[2] = vertices[di->midpoint_index*3+2];

        vector di_norm = modelview_rot * midpoint;
        di_norm.normalize();

        vector di_pos = this->get_modelview() * midpoint;
        real_t dist = di_pos.mag();
        di_pos.normalize();

        real_t dot_prod = di_norm.dot(di_pos);

        if (dot_prod < 0.0f)
        {
            // errors
            real_t pe = pixel_error(di->midpoint_error, dist, c);

            if (pe > max_pixel_error)
                return;
        }

        // sanity check
        if ( di->nodes[0]->leaf_index == 0 || di->nodes[1]->leaf_index == 0 || di->nodes[2]->leaf_index == 0 || di->nodes[3]->leaf_index == 0 )
            throw INTERNAL_EXP(internal_exception, L"non-leaf diamond");

        for (int i = 0; i < 4; ++i)
        {
            if (di->nodes[i]->left || di->nodes[i]->right)
                throw INTERNAL_EXP(internal_exception, L"non-leaf diamond");
        }

        // 
        hook_up(di->nodes[0]->neighbors[0], di->nodes[0], di->nodes[0]->parent);
        hook_up(di->nodes[1]->neighbors[0], di->nodes[1], di->nodes[1]->parent);
        hook_up(di->nodes[2]->neighbors[0], di->nodes[2], di->nodes[2]->parent);
        hook_up(di->nodes[3]->neighbors[0], di->nodes[3], di->nodes[3]->parent);

        di->leaves[0]->neighbors[0] = di->leaves[1];
        di->leaves[0]->neighbors[1] = di->nodes[0]->neighbors[0];
        di->leaves[0]->neighbors[2] = di->nodes[1]->neighbors[0];
        
        di->leaves[1]->neighbors[0] = di->leaves[0];
        di->leaves[1]->neighbors[1] = di->nodes[2]->neighbors[0];
        di->leaves[1]->neighbors[2] = di->nodes[3]->neighbors[0];

        add_leaf(di->leaves[0]);
        add_leaf(di->leaves[1]);

        // check for creation of new diamonds
        if (di->leaves[0]->level > 1 && di->leaves[0]->neighbors[1]->neighbors[1]->neighbors[1]->neighbors[1] == di->leaves[0])
            add_diamond(c->frame, di->leaves[0]->vertices[2], di->leaves[0]->midpoint_error, 
                        di->leaves[0]->neighbors[1]->neighbors[1], 
                        di->leaves[0]->neighbors[1]->neighbors[1]->neighbors[1],
                        di->leaves[0],
                        di->leaves[0]->neighbors[1]);

        if (di->leaves[1]->level > 1 && di->leaves[1]->neighbors[2]->neighbors[2]->neighbors[2]->neighbors[2] == di->leaves[1])
            add_diamond(c->frame, di->leaves[1]->vertices[2], di->leaves[1]->midpoint_error, 
                        di->leaves[1],
                        di->leaves[1]->neighbors[2]->neighbors[2]->neighbors[2],
                        di->leaves[1]->neighbors[2]->neighbors[2],
                        di->leaves[1]->neighbors[2]);
        // clean up
        di->leaves[0]->left  = 0;
        di->leaves[0]->right = 0;
        di->leaves[1]->left  = 0;
        di->leaves[1]->right = 0;

        triangle *nodes_to_delete[4];
        ::memcpy(nodes_to_delete, di->nodes, sizeof(triangle *) * 4);

        remove_diamond(di); // di gets deleted here

        for (int i = 0; i < 4; ++i)
        {
            remove_leaf(nodes_to_delete[i]);
            delete nodes_to_delete[i];
        }
    } // planetmesh::merge_diamond()

    
#define VECTOR(name, index) vector name(vertices.at((index)*3+0), vertices.at((index)*3+1), vertices.at((index)*3+2))
    
    void planetmesh::make_surface_normal(triangle *tri)
    {
        VECTOR(v0, tri->vertices[0]);
        VECTOR(v1, tri->vertices[1]);
        VECTOR(v2, tri->vertices[2]);
        
        tri->surface_normal = (v2 - v0).cross(v1 - v0);
        tri->surface_normal.normalize();
    } // planetmesh::make_surface_normal()
    
    const gsgl::index_t planetmesh::make_middle_vertex(triangle *tri)
    {
        // calculate current midpoint
        VECTOR(a, tri->vertices[0]);
        VECTOR(b, tri->vertices[1]);
        tri->midpoint = a + 0.5 * (b - a);
        
        // get geodedic coordinates
        gsgl::real_t lat, lon, alt;
        geocentric_to_geographic(pol_r, eq_r, tri->midpoint[0], tri->midpoint[1], tri->midpoint[2], lat, lon, alt);
        
        // get mesh altitude
        gsgl::real_t mesh_alt = get_height_field_alt(lat, lon);
        
        // get real coordinates...
        gsgl::index_t index = vertex_pool.allocate();
        tri->vertices[3] = index;
        
        gsgl::real_t xx, yy, zz;
        geographic_to_geocentric(pol_r, eq_r, lat, lon, mesh_alt, xx, yy, zz);
        vertices[index*3+0] = xx;
        vertices[index*3+1] = yy;
        vertices[index*3+2] = zz;
        
        /// \todo Calculate the correct normal.
        vector vn(xx, yy, zz);
        vn.normalize();
        normals[index*3+0] = vn[0];
        normals[index*3+1] = vn[1];
        normals[index*3+2] = vn[2];
        
        geo_coords[index*3+0] = lat;
        geo_coords[index*3+1] = lon;
        geo_coords[index*3+2] = mesh_alt;
        
        unsigned char rr,gg,bb;
        get_color_tile_color(lat, lon, rr, gg, bb);
        colors[index*3+0] = rr;
        colors[index*3+1] = gg;
        colors[index*3+2] = bb;
        
        VECTOR(d, index);
        tri->midpoint_error = (d - tri->midpoint).mag();
        
        return index;
    } // planetmesh::make_middle_vertex()
    
    
    void planetmesh::init_root_nodes()
    {
        // allocate nodes
        for (int i = 0; i < 12; ++i)
        {
            root_nodes[i] = new triangle();
            root_nodes[i]->level = 1;
        }
        
        // init nodes with normals
        init_root_node(root_nodes[0],   1,  6,  5,   -1, -1, -1,     1, -1,  1,   -1, -1,  1);
        init_root_node(root_nodes[1],   0, 11,  2,    1, -1,  1,    -1, -1, -1,    1, -1, -1);
        init_root_node(root_nodes[2],   3,  1, 11,    1,  1, -1,     1, -1,  1,    1, -1, -1);
        init_root_node(root_nodes[3],   2,  8,  7,    1, -1,  1,     1,  1, -1,    1,  1,  1);

        init_root_node(root_nodes[4],   5,  9, 10,   -1, -1, -1,    -1,  1,  1,   -1,  1, -1);
        init_root_node(root_nodes[5],   4,  0,  6,   -1,  1,  1,    -1, -1, -1,   -1, -1,  1);
        init_root_node(root_nodes[6],   7,  5,  0,    1, -1,  1,    -1,  1,  1,   -1, -1,  1);
        init_root_node(root_nodes[7],   6,  3,  8,   -1,  1,  1,     1, -1,  1,    1,  1,  1);

        init_root_node(root_nodes[8],   9,  7,  3,    1,  1, -1,    -1,  1,  1,    1,  1,  1);
        init_root_node(root_nodes[9],   8, 10,  4,   -1,  1,  1,     1,  1, -1,   -1,  1, -1);
        init_root_node(root_nodes[10], 11,  4,  9,    1,  1, -1,    -1, -1, -1,   -1,  1, -1);
        init_root_node(root_nodes[11], 10,  2,  1,   -1, -1, -1,     1,  1, -1,    1, -1, -1);
    } // planetmesh::init_root_nodes()
    
    void planetmesh::init_root_node(triangle *tri, int n0, int n1, int n2,
                                    gsgl::real_t v0_x, gsgl::real_t v0_y, gsgl::real_t v0_z,
                                    gsgl::real_t v1_x, gsgl::real_t v1_y, gsgl::real_t v1_z,
                                    gsgl::real_t v2_x, gsgl::real_t v2_y, gsgl::real_t v2_z)
    {
        tri->parent = tri->left = tri->right = 0;
        
        tri->neighbors[0] = root_nodes[n0];
        tri->neighbors[1] = root_nodes[n1];
        tri->neighbors[2] = root_nodes[n2];
        
        tri->vertices[0] = make_root_vertex(v0_x, v0_y, v0_z);
        tri->vertices[1] = make_root_vertex(v1_x, v1_y, v1_z);
        tri->vertices[2] = make_root_vertex(v2_x, v2_y, v2_z);
        make_surface_normal(tri);
        make_middle_vertex(tri);

        add_leaf(tri);
    } // planetmesh::init_root_node()
    
    const gsgl::index_t planetmesh::make_root_vertex(gsgl::real_t x, gsgl::real_t y, gsgl::real_t z)
    {
        // normalize vertex and get geo coordinates
        vector v1n(x, y, z);
        v1n.normalize();
        vector v2 = v1n * eq_r;
        
        // get geodedic coordinates
        gsgl::real_t lat, lon, alt;
        geocentric_to_geographic(pol_r, eq_r, v2[0], v2[1], v2[2], lat, lon, alt);
        
        // get mesh altitude from geodedic coordinates
        gsgl::real_t mesh_alt = get_height_field_alt(lat, lon);
        
        // convert back to cartesian, using mesh altitude
        vector v;
        geographic_to_geocentric(pol_r, eq_r, lat, lon, mesh_alt, v[0], v[1], v[2]);
        
        // find mesh in vertex list...
        gsgl::index_t i = vertex_pool.allocate();
        
        // record vertex
        vertices[i*3+0] = v[0];
        vertices[i*3+1] = v[1];
        vertices[i*3+2] = v[2];
        
        // record normal
        normals[i*3+0] = v1n[0];
        normals[i*3+1] = v1n[1];
        normals[i*3+2] = v1n[2];
        
        // record geo_coords
        geo_coords[i*3+0] = lat;
        geo_coords[i*3+1] = lon;
        geo_coords[i*3+2] = mesh_alt;
        
        // record colors
        unsigned char rr,gg,bb;
        get_color_tile_color(lat, lon, rr, gg, bb);

        colors[i*3+0] = rr;
        colors[i*3+1] = gg;
        colors[i*3+2] = bb;
        
        // return
        return i;
    } // planetmesh::make_root_vertex()
    
    
    /// Flips the texture in an SDL surface vertically to be used by OpenGL.
    static void flip_texture(SDL_Surface *s)
    {
        SDL_LockSurface(s);
        
        int a = 0, b = s->h - 1;
        array<unsigned char> temp(s->pitch);
        unsigned char *ptr_a, *ptr_b, *ptr_t = (unsigned char *) temp.ptr();
        
        while (b > a)
        {
            ptr_a = (unsigned char *)s->pixels + a++*s->pitch;
            ptr_b = (unsigned char *)s->pixels + b--*s->pitch;
            
            ::memcpy(ptr_t, ptr_a, s->pitch);
            ::memcpy(ptr_a, ptr_b, s->pitch);
            ::memcpy(ptr_b, ptr_t, s->pitch);
        }
        
        SDL_UnlockSurface(s);
    } // flip_texture()
    
    static gsgl::real_t normalize_lon(gsgl::real_t lon)
    {
        return (gsgl::real_t) ((lon > PI) ? (lon - 2.0 * PI) : lon);
    } // normalize_lon()
    
    void planetmesh::load_height_field(const config & conf)
    {
        // initialize height field structure
        heightfield *hf = new heightfield();

        // get parameters
        hf->left   = (gsgl::real_t) normalize_lon((gsgl::real_t) conf[L"left"].to_real() * RADIANS);
        hf->right  = (gsgl::real_t) normalize_lon((gsgl::real_t) conf[L"right"].to_real() * RADIANS);
        hf->top    = (gsgl::real_t) conf[L"top"].to_real() * RADIANS;
        hf->bottom = (gsgl::real_t) conf[L"bottom"].to_real() * RADIANS;
        hf->min    = (gsgl::real_t) conf[L"min"].to_real();
        hf->max    = (gsgl::real_t) conf[L"max"].to_real();
        
        // load texture and copy
        string fname = conf.get_directory().get_full_path() + directory::SEPARATOR + conf[L"file"];

        if (!file::exists(fname))
            throw EXP(runtime_exception, L"height field file %ls does not exist", fname.w_string());
        
        SDL_Surface *surface = IMG_Load(fname.c_string());
        
        if (!surface)
            throw EXP(io_exception, L"unable to load height field from %ls", fname.w_string());
            
        flip_texture(surface);
        
        SDL_LockSurface(surface);
        
        hf->height = surface->h;
        hf->width = surface->w;
        hf->data.resize(surface->w * surface->h);
        
        unsigned char *ptr = (unsigned char *) surface->pixels;
        
        unsigned int pixel, temp;
        unsigned char r, g, b, y;
        
        for (int v = 0; v < hf->height; ++v)
        {
            for (int u = 0; u < hf->width; ++u)
            {
                pixel = *(unsigned int *) ptr;
                pixel >>= 8 * (4 - surface->format->BytesPerPixel);
                
                temp = pixel & surface->format->Rmask;
                temp = temp >> surface->format->Rshift;
                temp = temp << surface->format->Rloss;
                r = (unsigned char) temp;
                
                temp = pixel & surface->format->Gmask;
                temp = temp >> surface->format->Gshift;
                temp = temp << surface->format->Gloss;
                g = (unsigned char) temp;
                
                temp = pixel & surface->format->Bmask;
                temp = temp >> surface->format->Bshift;
                temp = temp << surface->format->Bloss;
                b = (unsigned char) temp;
                
                y = (unsigned char) (0.3*(gsgl::real_t)r + 0.59*(gsgl::real_t)g + 0.11*(gsgl::real_t)b);
                hf->data[u + v*hf->width] = hf->min + ((gsgl::real_t) y / 255.0f) * (hf->max - hf->min);
                
                ptr += surface->format->BytesPerPixel;
            }
        }
        
        SDL_UnlockSurface(surface);
        SDL_FreeSurface(surface);

        // add to collection
        height_fields.add(hf);
    } // planetmesh::load_height_field()
    
    void planetmesh::load_color_tile(const config & conf)
    {
        // initialize color tile structure
        colortile *ct = new colortile();

        // get parameters
        ct->left   = (gsgl::real_t) normalize_lon((gsgl::real_t) conf[L"left"].to_real() * RADIANS);
        ct->right  = (gsgl::real_t) normalize_lon((gsgl::real_t) conf[L"right"].to_real() * RADIANS);
        ct->top    = (gsgl::real_t) conf[L"top"].to_real() * RADIANS;
        ct->bottom = (gsgl::real_t) conf[L"bottom"].to_real() * RADIANS;

        // load texture and copy
        string fname = conf.get_directory().get_full_path() + directory::SEPARATOR + conf[L"file"];

        if (!file::exists(fname))
            throw EXP(runtime_exception, L"color tile file %ls does not exist", fname.w_string());
        
        SDL_Surface *surface = IMG_Load(fname.c_string());
        
        if (!surface)
            throw EXP(runtime_exception, L"unable to open %ls", fname.w_string());
            
        flip_texture(surface);
        
        SDL_LockSurface(surface);
        
        unsigned char *ptr = (unsigned char *) surface->pixels;
        
        ct->height = surface->h;
        ct->width = surface->w;
        ct->data.resize(ct->height*ct->width*3);
        unsigned int temp, pixel;
        
        for (int v = 0; v < ct->height; ++v)
        {
            for (int u = 0; u < ct->width; ++u)
            {
                int index = u + v*ct->width;
                
                pixel = *(unsigned int *) ptr;
                // only if big-endian!
                //pixel >>= 8 * (4 - surface->format->BytesPerPixel);
                
                temp = pixel & surface->format->Rmask;
                temp = temp >> surface->format->Rshift;
                temp = temp << surface->format->Rloss;
                ct->data[index*3+0] = (unsigned char) temp;
                
                temp = pixel & surface->format->Gmask;
                temp = temp >> surface->format->Gshift;
                temp = temp << surface->format->Gloss;
                ct->data[index*3+1] = (unsigned char) temp;
                
                temp = pixel & surface->format->Bmask;
                temp = temp >> surface->format->Bshift;
                temp = temp << surface->format->Bloss;
                ct->data[index*3+2] = (unsigned char) temp;
                
                ptr += surface->format->BytesPerPixel;
            }
        }
        
        SDL_UnlockSurface(surface);
        SDL_FreeSurface(surface);

        // add to collection
        color_tiles.add(ct);
    } // planetmesh::load_color_tile()
    
} // namespace periapsis
