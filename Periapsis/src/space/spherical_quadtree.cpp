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

#include "space/spherical_quadtree.hpp"
#include "math/vector.hpp"

#include "scenegraph/camera.hpp"
#include "scenegraph/utils.hpp"
#include "platform/texture.hpp"
#include "platform/heightmap.hpp"
#include "platform/lowlevel.hpp"

#include <cmath>


using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;
using namespace gsgl::platform;


namespace periapsis
{

    namespace space
    {

        static inline vector get_vector(const simple_array<gsgl::real_t> & a, const gsgl::index_t & index)
        {
            return vector(a[index*3+0], a[index*3+1], a[index*3+2]);
        }



        sph_qt_node::sph_qt_node(spherical_quadtree *parent_quadtree, sph_qt_node *parent_node)
            : parent_quadtree(parent_quadtree), 
              level(parent_node ? parent_node->level + 1 : 0), 
              dirty(true), delete_me(false), parent_node(parent_node),
              triangle_fan_indices(32),
              radius_in_world_space(0), radius_in_screen_space(0),
              last_radius_frame(static_cast<unsigned long>(-1)),
              last_merge_frame(static_cast<unsigned long>(-1)), 
              last_split_frame(static_cast<unsigned long>(-1))
        {
            for (int i = 0; i < 4; ++i)
                children[i] = 0;

            for (int i = 0; i < 4; ++i)
                adjacent_nodes[i] = 0;

            for (int i = 0; i < 25; ++i)
                vertex_indices[i] = 0;

            for (int i = 0; i < 4; ++i)
                num_indices_in_quadrants[i] = 0;

            // get space in a buffer pool
            assert(parent_quadtree->buffers);
            buffer_pool_rec = parent_quadtree->buffers->allocate_object();
        } // sph_qt_node()


        sph_qt_node::~sph_qt_node()
        {
            // free the space in the buffer pool
            parent_quadtree->buffers->free_object(buffer_pool_rec);

            // delete children
            for (int i = 0; i < 4; ++i)
            {
                if (children[i])
                {
                    delete children[i];
                    children[i] = 0;
                }
            }

            // free the vertices used (refcount will take care of shared indices
            for (int i = 0; i < 25; ++i)
                parent_quadtree->free_vertex_index(vertex_indices[i]);
        } // sph_qt_node::~sph_qt_node()


        //

        void sph_qt_node::draw(gsgl::scenegraph::context *c)
        {
            // bounds check
            if (true || utils::is_on_screen(parent_quadtree->parent_sg_node, c->cam->get_field_of_view(), c->screen->get_aspect_ratio(), get_vector(parent_quadtree->global_vertices, vertex_indices[12]), radius_in_world_space))
            {
                // are we a leaf?
                if (is_a_leaf())
                {
                    update_fan_indices();

                    buffer_pool_rec.parent->vertices.bind();
                    glInterleavedArrays(GL_T2F_N3F_V3F, 0, vbuffer::VBO_OFFSET<float>(buffer_pool_rec.pos_in_vertices));

                    buffer_pool_rec.parent->indices.bind();

                    int prev_elements_drawn = 0;
                    for (int i = 0; i < 4; ++i)
                    {
                        if (num_indices_in_quadrants[i])
                        {
                            //glDrawElements(GL_TRIANGLE_FAN, num_indices_in_quadrants[i], GL_UNSIGNED_INT, vbuffer::VBO_OFFSET<vbuffer::index_t>(prev_elements_drawn));
                            //glDrawElements(GL_TRIANGLE_FAN, num_indices_in_quadrants[i], GL_UNSIGNED_INT, triangle_fan_indices.ptr() + prev_elements_drawn);

                            glDrawElements(GL_TRIANGLE_FAN, num_indices_in_quadrants[i], GL_UNSIGNED_INT, vbuffer::VBO_OFFSET<unsigned int>( buffer_pool_rec.pos_in_indices + prev_elements_drawn ));

                            prev_elements_drawn += num_indices_in_quadrants[i];
                        }
                    }

                }
                else
                {
                    // draw child nodes
                    for (int i = 0; i < 4; ++i)
                    {
                        if (children[i])
                            children[i]->draw(c);
                    }
                }
            }
        } // sph_qt_node::draw()


        //

        bool sph_qt_node::is_a_leaf() const
        {
            return !children[0] && !children[1] && !children[2] && !children[3];
        } // sph_qt_node::is_a_leaf()


        bool sph_qt_node::is_a_quad() const
        {
            if (!is_a_leaf())
            {
                for (int i = 0; i < 4; ++i)
                    if (children[i] && !children[i]->is_a_leaf())
                        return false;
                return true;
            }

            return false;
        } // sph_qt_node::is_a_quad()


        void sph_qt_node::update_fan_indices()
        {
            if (dirty)
            {
                gsgl::index_t prev_total = 0;

                // subquadrant 0 (upper left)
                if (!children[0])
                {
                    int & num = num_indices_in_quadrants[0]; num = 0;

                    triangle_fan_indices[prev_total + num++] = vertex_indices[6];  // center
                    triangle_fan_indices[prev_total + num++] = vertex_indices[10]; // lower left
                    triangle_fan_indices[prev_total + num++] = vertex_indices[12]; // lower right
                    triangle_fan_indices[prev_total + num++] = vertex_indices[2];  // upper right
                    if (adjacent_nodes[0] && !adjacent_nodes[0]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[1]; // middle of upper face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[0];  // upper left
                    if (adjacent_nodes[3] && !adjacent_nodes[3]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[5]; // middle of left face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[10]; // lower left

                    prev_total += num_indices_in_quadrants[0];
                }
                else
                {
                    num_indices_in_quadrants[0] = 0;
                }

                // subquadrant 1 (upper right)
                if (!children[1])
                {
                    int & num = num_indices_in_quadrants[1]; num = 0;

                    triangle_fan_indices[prev_total + num++] = vertex_indices[8];  // center
                    triangle_fan_indices[prev_total + num++] = vertex_indices[2];  // upper left
                    triangle_fan_indices[prev_total + num++] = vertex_indices[12]; // lower left
                    triangle_fan_indices[prev_total + num++] = vertex_indices[14]; // lower right
                    if (adjacent_nodes[1] && !adjacent_nodes[1]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[9]; // middle of right face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[4];  // upper right
                    if (adjacent_nodes[0] && !adjacent_nodes[0]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[3]; // middle of upper face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[2];  // upper left

                    prev_total += num_indices_in_quadrants[1];
                }
                else
                {
                    num_indices_in_quadrants[1] = 0;
                }

                // subquadrant 2 (lower right)
                if (!children[2])
                {
                    int & num = num_indices_in_quadrants[2]; num = 0;

                    triangle_fan_indices[prev_total + num++] = vertex_indices[18]; // center
                    triangle_fan_indices[prev_total + num++] = vertex_indices[14]; // upper right
                    triangle_fan_indices[prev_total + num++] = vertex_indices[12]; // upper left
                    triangle_fan_indices[prev_total + num++] = vertex_indices[22]; // lower left
                    if (adjacent_nodes[2] && !adjacent_nodes[2]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[23]; // middle of lower face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[24]; // lower right
                    if (adjacent_nodes[1] && !adjacent_nodes[1]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[19]; // middle of right face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[14]; // upper right

                    prev_total += num_indices_in_quadrants[2];
                }
                else
                {
                    num_indices_in_quadrants[2] = 0;
                }

                // subquadrant 3 (lower left)
                if (!children[3])
                {
                    int & num = num_indices_in_quadrants[3]; num = 0;

                    triangle_fan_indices[prev_total + num++] = vertex_indices[16]; // center
                    triangle_fan_indices[prev_total + num++] = vertex_indices[22]; // lower right
                    triangle_fan_indices[prev_total + num++] = vertex_indices[12]; // upper right
                    triangle_fan_indices[prev_total + num++] = vertex_indices[10]; // upper left
                    if (adjacent_nodes[3] && !adjacent_nodes[3]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[15]; // middle of left face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[20]; // lower left
                    if (adjacent_nodes[2] && !adjacent_nodes[2]->is_a_leaf()) triangle_fan_indices[prev_total + num++] = vertex_indices[21]; // middle of bottom face
                    triangle_fan_indices[prev_total + num++] = vertex_indices[22]; // lower right

                    prev_total += num_indices_in_quadrants[3];
                }
                else
                {
                    num_indices_in_quadrants[3] = 0;
                }

                // copy vertex information from global memory to vbuffer
                vbuffer::index_t vertex_pos = buffer_pool_rec.pos_in_vertices;
                vbuffer::index_t index_pos = buffer_pool_rec.pos_in_indices;
                int triangle_index_pos = 0;
                for (int i = 0; i < 4; ++i)
                {
                    for (int j = 0; j < num_indices_in_quadrants[i]; ++j)
                    {
                        gsgl::index_t global_index = triangle_fan_indices[triangle_index_pos];

                        // index
                        buffer_pool_rec.parent->indices[index_pos++] = triangle_index_pos++;

                        // texture
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_polar_coords[ global_index*2 + 0 ];
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_polar_coords[ global_index*2 + 1 ];

                        // normal
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_normals[ global_index*3 + 0 ];
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_normals[ global_index*3 + 1 ];
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_normals[ global_index*3 + 2 ];

                        // vertex
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_vertices[ global_index*3 + 0 ];
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_vertices[ global_index*3 + 1 ];
                        buffer_pool_rec.parent->vertices[ vertex_pos++ ] = parent_quadtree->global_vertices[ global_index*3 + 2 ];
                    }
                }
            }

            dirty = false;
        } // sph_qt_node::update_fan_indices()


        //////////////////////////////////////////////////////////////

        spherical_quadtree::spherical_quadtree(gsgl::scenegraph::node *parent_sg_node, const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius)
            : parent_sg_node(parent_sg_node), polar_radius(polar_radius), equatorial_radius(equatorial_radius), 
              buffers(0), leaf_nodes(), merge_nodes(), delete_nodes()
        {
            for (int i = 0; i < 6; ++i)
                root_nodes[i] = 0;
        } // spherical_quadtree::spherical_quadtree()


        spherical_quadtree::~spherical_quadtree()
        {
            for (int i = 0; i < 6; ++i)
                delete root_nodes[i];

            // must be last!
            delete buffers;
        } // spherical_quadtree::~spherical_quadtree()

        
        //////////////////////////////////////////

        void spherical_quadtree::init(gsgl::scenegraph::context *c)
        {
            // initialize buffer pool
            if (!buffers)
            {
                // each quadrant can use up to 256 floats for its vertex data (32 vertices * 8 floats per vertex)
                // given a buffer size limit of 64K bytes, and a float takes 4 bytes,
                // this gives 64 quadrants per buffer

                buffers = new buffer_pool(vbuffer::DYNAMIC, 64, 32*8, 32);
            }

            // init root nodes
            init_root_nodes();
        } // spherical_quadtree::init()


        //////////////////////////////////////////

        void spherical_quadtree::draw(gsgl::scenegraph::context *c)
        {
            // make sure to split before drawing the first time
            // we can't do this during init because the positions aren't necessarily set
            if (c->frame == 0)
            {
                update(c, false);
            }

            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                   CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                      CHECK_GL_ERRORS();

            glEnableClientState(GL_VERTEX_ARRAY);                                                               CHECK_GL_ERRORS();
            glEnableClientState(GL_NORMAL_ARRAY);
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);
            glEnableClientState(GL_INDEX_ARRAY);                                                                CHECK_GL_ERRORS();
            
            for (int i = 0; i < 6; ++i)
                if (root_nodes[i])
                    root_nodes[i]->draw(c);

            utils::save_screen_info(last_frame_viewport, last_frame_modelview_projection);

            glPopClientAttrib();
            glPopAttrib();
        } // spherical_quadtree::draw()


        //////////////////////////////////////////

        static config_variable<gsgl::real_t> ANGLE_CUTOFF(L"space/spherical_quadtree/angle_cutoff", -0.5f); ///< Cosine cutoff of the quad's center normal.
        static config_variable<gsgl::real_t> PIXEL_CUTOFF(L"space/spherical_quadtree/pixel_cutoff", 128);    ///< The pixel radius cutoff of a quad.


        static string get_indent(int indent)
        {
            string res;
            for (int i = 0; i < indent; ++i)
                res += L"  ";
            return res;
        } // get_indent()


        void spherical_quadtree::add_leaf_node(sph_qt_node *qtn)
        {
            assert(qtn);
            qtn->dequeue_me = false;
            qtn->delete_me = false;
            leaf_nodes.push(qtn);
        } // spherical_quadtree::add_leaf_node()


        void spherical_quadtree::remove_leaf_node(sph_qt_node *qtn)
        {
            assert(qtn);
            qtn->dequeue_me = true;
        } // spherical_quadtree::remove_leaf_node()


        void spherical_quadtree::add_merge_node(sph_qt_node *qtn)
        {
            assert(qtn);
            qtn->dequeue_me = false;
            qtn->delete_me = false;
            merge_nodes.push(qtn);
        } // spherical_quadtree::add_merge_node()


        void spherical_quadtree::remove_merge_node(sph_qt_node *qtn)
        {
            assert(qtn);
            qtn->dequeue_me = true;
        } // spherical_quadtree::remove_merge_node()


        gsgl::real_t spherical_quadtree::node_cos_angle(sph_qt_node *qtn, const transform & modelview)
        {
            gsgl::real_t result;

            vector normal_pos_in_world_space = get_vector(global_vertices, qtn->vertex_indices[12]) + get_vector(global_normals, qtn->vertex_indices[12]) * 1000.0;
            vector normal_pos_in_eye_space   = modelview * normal_pos_in_world_space;
            
            vector center_pos_in_eye_space   = modelview * get_vector(global_vertices, qtn->vertex_indices[12]);            
            vector normal_in_eye_space       = normal_pos_in_eye_space - center_pos_in_eye_space;                 
            normal_in_eye_space.normalize();

            result = vector::Z_AXIS.dot(normal_in_eye_space);

            return result;
        } // spherical_quadtree::node_cos_angle()


        gsgl::real_t spherical_quadtree::node_radius(sph_qt_node *qtn, const gsgl::scenegraph::context *c)
        {
            if (qtn->last_radius_frame != c->frame)
            {
                gsgl::real_t radius   = (get_vector(global_vertices, qtn->vertex_indices[0]) - get_vector(global_vertices, qtn->vertex_indices[12])).mag();
                gsgl::real_t distance = (parent_sg_node->get_modelview() * get_vector(global_vertices, qtn->vertex_indices[12])).mag();  if (distance < 1.0f) distance = 1.0f;
                gsgl::real_t angle    = ::atan(radius / distance);

                gsgl::real_t pct_screen_angle = static_cast<gsgl::real_t>(angle / (c->cam->get_field_of_view() * math::DEG2RAD));
                gsgl::real_t pixel_radius = pct_screen_angle * c->screen->get_height();

                qtn->radius_in_world_space = radius;
                qtn->radius_in_screen_space = pixel_radius;
                qtn->last_radius_frame = c->frame;
            }

            return qtn->radius_in_screen_space;
        } // spherical_quadtree::node_radius()


#ifdef DEBUG_SPLITS_AND_MERGES
        static int get_ptr_num(sph_qt_node **adjacent_nodes, sph_qt_node *child)
        {
            assert(child);

            for (int i = 0; i < 4; ++i)
                if (adjacent_nodes[i] == child)
                    return i;
            return -1;
        }
#endif


        /// This function finds the appropriate adjacent node.  If the candiate node is not split, the function returns it, and zeroes the handle.
        /// If the candidate node is split, the function returns the correct child, and sets the handle to point to the child's adjacency pointer back to the new subnode.
        /// It also sets the side index pointers to the indices of the middle nodes on the side of the result (in the order that the input indices were specified).
        sph_qt_node *spherical_quadtree::get_adjacent(sph_qt_node *candidate, const vbuffer::index_t & index0, const vbuffer::index_t & index1, sph_qt_node ***peer_handle, vbuffer::index_t *side0, vbuffer::index_t *side1)
        {
            sph_qt_node *result = candidate;
            *peer_handle = 0;
            if (side0) *side0 = 0;
            if (side1) *side1 = 0;

            if (!candidate->is_a_leaf())
            {
                // look through the children of the candidate node
                for (int i = 0; i < 4; ++i)
                {
                    sph_qt_node *child = candidate->children[i];

                    // check all four sides of the child for matching indices
                    if (child)
                    {
                        // side 0
                        if (child->vertex_indices[0] == index0 && child->vertex_indices[4] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[0];
                            if (side0) *side0 = child->vertex_indices[1];
                            if (side1) *side1 = child->vertex_indices[3];
                        }
                        else if (child->vertex_indices[4] == index0 && child->vertex_indices[0] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[0];
                            if (side0) *side0 = child->vertex_indices[3];
                            if (side1) *side1 = child->vertex_indices[1];
                        }

                        // side 1
                        else if (child->vertex_indices[4] == index0 && child->vertex_indices[24] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[1];
                            if (side0) *side0 = child->vertex_indices[9];
                            if (side1) *side1 = child->vertex_indices[19];
                        }
                        else if (child->vertex_indices[24] == index0 && child->vertex_indices[4] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[1];
                            if (side0) *side0 = child->vertex_indices[19];
                            if (side1) *side1 = child->vertex_indices[9];
                        }

                        // side 2
                        else if (child->vertex_indices[24] == index0 && child->vertex_indices[20] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[2];
                            if (side0) *side0 = child->vertex_indices[23];
                            if (side1) *side1 = child->vertex_indices[21];
                        }
                        else if (child->vertex_indices[20] == index0 && child->vertex_indices[24] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[2];
                            if (side0) *side0 = child->vertex_indices[21];
                            if (side1) *side1 = child->vertex_indices[23];
                        }

                        // side 3
                        else if (child->vertex_indices[20] == index0 && child->vertex_indices[0] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[3];
                            if (side0) *side0 = child->vertex_indices[15];
                            if (side1) *side1 = child->vertex_indices[5];
                        }
                        else if (child->vertex_indices[0] == index0 && child->vertex_indices[20] == index1)
                        {
                            result = child;
                            *peer_handle = &child->adjacent_nodes[3];
                            if (side0) *side0 = child->vertex_indices[5];
                            if (side1) *side1 = child->vertex_indices[15];
                        }

                        if (result == child)
                            break;
                    } // if child
                } // for i
            } // if candidate has children

            return result;
        } // spherical_quadtree::get_adjacent()


        bool spherical_quadtree::neighbor_allows_merge(sph_qt_node *qtn, sph_qt_node *adj)
        {
            if (adj && adj->level >= qtn->level)
            {
                for (int i = 0; i < 4; ++i)
                {
                    if (adj->children[i] && !adj->children[i]->is_a_leaf())
                        return false;
                }
            }

            return true;
        } // spherical_quadtree::neighbor_allows_merge()


        void spherical_quadtree::merge_node_aux(sph_qt_node *qtn)
        {
            // for each child, if any of their neighbors (that are not also children of the parent), point to the child, point them to the parent
            for (int i = 0; i < 4; ++i)
            {
                sph_qt_node *child = qtn->children[i];

#ifdef DEBUG_SPLITS_AND_MERGES
                log(string::format(L"  child %d: {%ls}", i, child ? child->path.w_string() : L"<null>"));
#endif

                if (child)
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        if (child->adjacent_nodes[j] && child->adjacent_nodes[j]->parent_node != qtn)
                        {
                            sph_qt_node *adj = child->adjacent_nodes[j];

                            for (int k = 0; k < 4; ++k)
                            {
                                if (adj->adjacent_nodes[k] == child)
                                {
#ifdef DEBUG_SPLITS_AND_MERGES
                                    log(string::format(L"    switching {%ls}'s ptr %d to point to {%ls}", adj->path.w_string(), k, qtn->path.w_string()));
#endif

                                    adj->adjacent_nodes[k] = qtn;
                                    adj->dirty = true;
                                }
                            }
                        }
                    }
                }
            }

        } // spherical_quadtree::merge_node_aux()


        bool spherical_quadtree::merge_node(sph_qt_node *qtn, const transform & modelview, const gsgl::scenegraph::context *c)
        {
            // if we were already merged this frame, return
            if (qtn->last_merge_frame == c->frame)
                return true;

            // if we were split this frame, return
            if (qtn->last_split_frame == c->frame)
                return false;

            // sanity check: all our children must be leaves
            for (int i = 0; i < 4; ++i)
            {
                if (qtn->children[i] && !qtn->children[i]->is_a_leaf())
                    throw internal_exception(__FILE__, __LINE__, L"Trying to merge a node whose children are not leaves!");
            }

            // if the node is facing away from the eye, or is too small, merge it
            if (node_cos_angle(qtn, modelview) < ANGLE_CUTOFF || node_radius(qtn, c) < PIXEL_CUTOFF)
            {
                // check children's neighbors and see if they will allow us to merge...
                bool can_merge = true;
                for (int i = 0; can_merge && i < 4; ++i)
                {
                    if (qtn->children[i])
                    {
                        sph_qt_node *child = qtn->children[i];

                        for (int j = 0; can_merge && j < 4; ++j)
                        {
                            if (child->adjacent_nodes[j] && child->adjacent_nodes[j]->parent_node != qtn)
                            {
                                sph_qt_node *adj = child->adjacent_nodes[j];

                                if (adj->level > child->level)
                                    throw internal_exception(__FILE__, __LINE__, L"Trying to merge a node whose children are next to nodes of a greater level!");

                                if (adj->level == child->level && !adj->is_a_leaf())
                                    can_merge = false;
                            }
                        }
                    }
                }

                // merge me
                if (can_merge)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    gsgl::log(string(L"\nMERGE {") + qtn->path + L"}");
#endif

                    merge_node_aux(qtn);

                    // delete children & mark adjacent nodes dirty
                    for (int i = 0; i < 4; ++i)
                    {
                        if (qtn->children[i])
                            remove_leaf_node(qtn->children[i]);
                        //delete qtn->children[i];
                        qtn->children[i]->delete_me = true;
                        qtn->children[i] = 0;

                        if (qtn->adjacent_nodes[i])
                            qtn->adjacent_nodes[i]->dirty = true;
                    }

                    qtn->dirty = true;
                    qtn->last_merge_frame = c->frame;
                    remove_merge_node(qtn);
                    add_leaf_node(qtn);

                    // check parent to see if it is a new merge node
                    if (qtn->parent_node && qtn->parent_node->is_a_quad())
                    {
                        add_merge_node(qtn->parent_node);
                    }

                    return true;
                }
            }

            return false;
        } // spherical_quadtree::merge_node()


        static const bool NEW_NODE_VERTEX_FLAGS[] =
        {
            true,  false, true,  false, true,
            false, false, false, false, false,
            true,  false, true,  false, true,
            false, false, false, false, false,
            true,  false, true,  false, true
        };


        void spherical_quadtree::split_node_aux(sph_qt_node *qtn, int force_level)
        {
            // mark neighbors as dirty
            for (int i = 0; i < 4; ++i)
                if (qtn->adjacent_nodes[i])
                    qtn->adjacent_nodes[i]->dirty = true;

            // split the node
            vector geographic_normals[25];
            vbuffer::index_t indices[25];

            sph_qt_node *child0 = qtn->children[0] = create_node(qtn);
            sph_qt_node *child1 = qtn->children[1] = create_node(qtn);
            sph_qt_node *child2 = qtn->children[2] = create_node(qtn);
            sph_qt_node *child3 = qtn->children[3] = create_node(qtn);

            // child 0 (upper left)
            if (child0)
            {
#ifdef DEBUG_SPLITS_AND_MERGES
                log(string::format(L"%ls  creating child {%ls 0}", get_indent(force_level).w_string(), qtn->path.w_string()));
#endif

                add_leaf_node(child0);

                bool node_vertex_flags[25];
                for (int i = 0; i < 25; ++i)
                    node_vertex_flags[i] = NEW_NODE_VERTEX_FLAGS[i];

                // get normals
                geographic_normals[0]  = get_vector(global_normals, qtn->vertex_indices[0]);
                geographic_normals[2]  = get_vector(global_normals, qtn->vertex_indices[1]);
                geographic_normals[4]  = get_vector(global_normals, qtn->vertex_indices[2]);

                geographic_normals[10] = get_vector(global_normals, qtn->vertex_indices[5]);
                geographic_normals[12] = get_vector(global_normals, qtn->vertex_indices[6]);
                geographic_normals[14] = get_vector(global_normals, qtn->vertex_indices[7]);

                geographic_normals[24] = get_vector(global_normals, qtn->vertex_indices[12]);
                geographic_normals[22] = get_vector(global_normals, qtn->vertex_indices[11]);
                geographic_normals[20] = get_vector(global_normals, qtn->vertex_indices[10]);
                fill_in_normals(geographic_normals);

                // get adjacent node on the top (and info about its indices and pointers to us)
                vbuffer::index_t middle_top_left, middle_top_right; // indices for the middle vertices along the top row of the new node
                sph_qt_node **adj_top_handle_to_us = 0; // handle of the adjacent node's pointer to us
                sph_qt_node *adj_top = get_adjacent(qtn->adjacent_nodes[0], qtn->vertex_indices[0], qtn->vertex_indices[2], &adj_top_handle_to_us, &middle_top_left, &middle_top_right);

                // get adjacent node on the bottom
                vbuffer::index_t middle_left_top, middle_left_bottom;
                sph_qt_node **adj_left_handle_to_us = 0;
                sph_qt_node *adj_left = get_adjacent(qtn->adjacent_nodes[3], qtn->vertex_indices[0], qtn->vertex_indices[10], &adj_left_handle_to_us, &middle_left_top,  &middle_left_bottom);

                // get indices (some from parent, some from adjacent nodes, some new)
                indices[0]  = qtn->vertex_indices[0];
                indices[1]  = adj_top_handle_to_us ? middle_top_left : get_new_vertex_index();
                indices[2]  = qtn->vertex_indices[1];
                indices[3]  = adj_top_handle_to_us ? middle_top_right : get_new_vertex_index();
                indices[4]  = qtn->vertex_indices[2];

                indices[5]  = adj_left_handle_to_us ? middle_left_top : get_new_vertex_index();
                indices[6]  = get_new_vertex_index();
                indices[7]  = get_new_vertex_index();
                indices[8]  = get_new_vertex_index();
                indices[9]  = get_new_vertex_index();

                indices[10] = qtn->vertex_indices[5];
                indices[11] = get_new_vertex_index();
                indices[12] = qtn->vertex_indices[6];
                indices[13] = get_new_vertex_index();
                indices[14] = qtn->vertex_indices[7];

                indices[15] = adj_left_handle_to_us ? middle_left_bottom : get_new_vertex_index();
                indices[16] = get_new_vertex_index();
                indices[17] = get_new_vertex_index();
                indices[18] = get_new_vertex_index();
                indices[19] = get_new_vertex_index();

                indices[20] = qtn->vertex_indices[10];
                indices[21] = get_new_vertex_index();
                indices[22] = qtn->vertex_indices[11];
                indices[23] = get_new_vertex_index();
                indices[24] = qtn->vertex_indices[12];

                // if we got some indices from the adjacent nodes, don't re-calculate them
                if (adj_top_handle_to_us)
                    node_vertex_flags[1] = node_vertex_flags[3] = true;
                if (adj_left_handle_to_us)
                    node_vertex_flags[5] = node_vertex_flags[5] = true;

                // get vertices for the new indices
                generate_vertices(child0, geographic_normals, node_vertex_flags, indices);

                child0->adjacent_nodes[0] = adj_top;
                child0->adjacent_nodes[1] = child1;
                child0->adjacent_nodes[2] = child3;
                child0->adjacent_nodes[3] = adj_left;
                child0->dirty = true;

                if (adj_top_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_top->path.w_string(), get_ptr_num(adj_top->adjacent_nodes, *adj_top_handle_to_us), (*adj_top_handle_to_us)->path.w_string()));
#endif
                    *adj_top_handle_to_us = child0;
                    adj_top->dirty = true;
                }

                if (adj_left_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_left->path.w_string(), get_ptr_num(adj_left->adjacent_nodes, *adj_left_handle_to_us), (*adj_left_handle_to_us)->path.w_string()));
#endif
                    *adj_left_handle_to_us = child0;
                    adj_left->dirty = true;
                }
            }

            // child 1 (upper right)
            if (child1)
            {
#ifdef DEBUG_SPLITS_AND_MERGES
                log(string::format(L"%ls  creating child {%ls 1}", get_indent(force_level).w_string(), qtn->path.w_string()));
#endif

                add_leaf_node(child1);

                bool node_vertex_flags[25];
                for (int i = 0; i < 25; ++i)
                    node_vertex_flags[i] = NEW_NODE_VERTEX_FLAGS[i];

                // get normals from parent node
                geographic_normals[0]  = get_vector(global_normals, qtn->vertex_indices[2]);
                geographic_normals[2]  = get_vector(global_normals, qtn->vertex_indices[3]);
                geographic_normals[4]  = get_vector(global_normals, qtn->vertex_indices[4]);

                geographic_normals[10] = get_vector(global_normals, qtn->vertex_indices[7]);
                geographic_normals[12] = get_vector(global_normals, qtn->vertex_indices[8]);
                geographic_normals[14] = get_vector(global_normals, qtn->vertex_indices[9]);

                geographic_normals[24] = get_vector(global_normals, qtn->vertex_indices[14]);
                geographic_normals[22] = get_vector(global_normals, qtn->vertex_indices[13]);
                geographic_normals[20] = get_vector(global_normals, qtn->vertex_indices[12]);
                fill_in_normals(geographic_normals);

                // get adjacent node on the top
                vbuffer::index_t middle_top_left, middle_top_right;
                sph_qt_node **adj_top_handle_to_us = 0;
                sph_qt_node *adj_top = get_adjacent(qtn->adjacent_nodes[0], qtn->vertex_indices[2], qtn->vertex_indices[4], &adj_top_handle_to_us, &middle_top_left, &middle_top_right);

                // get adjacent node on the right
                vbuffer::index_t middle_right_top, middle_right_bottom;
                sph_qt_node **adj_right_handle_to_us = 0;
                sph_qt_node *adj_right = get_adjacent(qtn->adjacent_nodes[1], qtn->vertex_indices[4], qtn->vertex_indices[14], &adj_right_handle_to_us, &middle_right_top, &middle_right_bottom);

                // get indices
                indices[0]  = qtn->vertex_indices[2];
                indices[1]  = adj_top_handle_to_us ? middle_top_left : get_new_vertex_index();
                indices[2]  = qtn->vertex_indices[3];
                indices[3]  = adj_top_handle_to_us ? middle_top_right : get_new_vertex_index();
                indices[4]  = qtn->vertex_indices[4];

                indices[5]  = child0 ? child0->vertex_indices[9] : get_new_vertex_index();
                indices[6]  = get_new_vertex_index();
                indices[7]  = get_new_vertex_index();
                indices[8]  = get_new_vertex_index();
                indices[9]  = adj_right_handle_to_us ? middle_right_top : get_new_vertex_index();

                indices[10] = qtn->vertex_indices[7];
                indices[11] = get_new_vertex_index();
                indices[12] = qtn->vertex_indices[8];
                indices[13] = get_new_vertex_index();
                indices[14] = qtn->vertex_indices[9];

                indices[15] = child0 ? child0->vertex_indices[19] : get_new_vertex_index();
                indices[16] = get_new_vertex_index();
                indices[17] = get_new_vertex_index();
                indices[18] = get_new_vertex_index();
                indices[19] = adj_right_handle_to_us ? middle_right_bottom : get_new_vertex_index();

                indices[20] = qtn->vertex_indices[12];
                indices[21] = get_new_vertex_index();
                indices[22] = qtn->vertex_indices[13];
                indices[23] = get_new_vertex_index();
                indices[24] = qtn->vertex_indices[14];

                // if we got some indices from adjacent nodes, don't recalculate
                if (child0)
                    node_vertex_flags[5] = node_vertex_flags[15] = true;
                if (adj_top_handle_to_us)
                    node_vertex_flags[1] = node_vertex_flags[3] = true;
                if (adj_right_handle_to_us)
                    node_vertex_flags[9] = node_vertex_flags[19] = true;

                generate_vertices(child1, geographic_normals, node_vertex_flags, indices);

                // adjust our adjacency pointers
                child1->adjacent_nodes[0] = adj_top;
                child1->adjacent_nodes[1] = adj_right;
                child1->adjacent_nodes[2] = child2;
                child1->adjacent_nodes[3] = child0;
                child1->dirty = true;

                // adjust neighbors adjacency pointers if necessary
                if (adj_top_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_top->path.w_string(), get_ptr_num(adj_top->adjacent_nodes, *adj_top_handle_to_us), (*adj_top_handle_to_us)->path.w_string()));
#endif
                    *adj_top_handle_to_us = child1;
                    adj_top->dirty = true;
                }

                if (adj_right_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_right->path.w_string(), get_ptr_num(adj_right->adjacent_nodes, *adj_right_handle_to_us), (*adj_right_handle_to_us)->path.w_string()));
#endif
                    *adj_right_handle_to_us = child1;
                    adj_right->dirty = true;
                }
            }

            // child 2 (lower right)
            if (child2)
            {
#ifdef DEBUG_SPLITS_AND_MERGES
                log(string::format(L"%ls  creating child {%ls 2}", get_indent(force_level).w_string(), qtn->path.w_string()));
#endif

                add_leaf_node(child2);

                bool node_vertex_flags[25];
                for (int i = 0; i < 25; ++i)
                    node_vertex_flags[i] = NEW_NODE_VERTEX_FLAGS[i];

                // get normals
                geographic_normals[0]  = get_vector(global_normals, qtn->vertex_indices[12]);
                geographic_normals[2]  = get_vector(global_normals, qtn->vertex_indices[13]);
                geographic_normals[4]  = get_vector(global_normals, qtn->vertex_indices[14]);

                geographic_normals[10] = get_vector(global_normals, qtn->vertex_indices[17]);
                geographic_normals[12] = get_vector(global_normals, qtn->vertex_indices[18]);
                geographic_normals[14] = get_vector(global_normals, qtn->vertex_indices[19]);

                geographic_normals[24] = get_vector(global_normals, qtn->vertex_indices[24]);
                geographic_normals[22] = get_vector(global_normals, qtn->vertex_indices[23]);
                geographic_normals[20] = get_vector(global_normals, qtn->vertex_indices[22]);
                fill_in_normals(geographic_normals);

                // get adjacent node on the right
                vbuffer::index_t middle_right_top, middle_right_bottom;
                sph_qt_node **adj_right_handle_to_us = 0;
                sph_qt_node *adj_right = get_adjacent(qtn->adjacent_nodes[1], qtn->vertex_indices[14], qtn->vertex_indices[24], &adj_right_handle_to_us, &middle_right_top, &middle_right_bottom);

                // get adjacent node on the bottom
                vbuffer::index_t middle_bottom_left, middle_bottom_right;
                sph_qt_node **adj_bottom_handle_to_us = 0;
                sph_qt_node *adj_bottom = get_adjacent(qtn->adjacent_nodes[2], qtn->vertex_indices[22], qtn->vertex_indices[24], &adj_bottom_handle_to_us, &middle_bottom_left, &middle_bottom_right);

                // 
                indices[0]  = qtn->vertex_indices[12];
                indices[1]  = child1 ? child1->vertex_indices[21] : get_new_vertex_index();
                indices[2]  = qtn->vertex_indices[13];
                indices[3]  = child1 ? child1->vertex_indices[23] : get_new_vertex_index();
                indices[4]  = qtn->vertex_indices[14];

                indices[5]  = get_new_vertex_index();
                indices[6]  = get_new_vertex_index();
                indices[7]  = get_new_vertex_index();
                indices[8]  = get_new_vertex_index();
                indices[9]  = adj_right_handle_to_us ? middle_right_top : get_new_vertex_index();

                indices[10] = qtn->vertex_indices[17];
                indices[11] = get_new_vertex_index();
                indices[12] = qtn->vertex_indices[18];
                indices[13] = get_new_vertex_index();
                indices[14] = qtn->vertex_indices[19];

                indices[15] = get_new_vertex_index();
                indices[16] = get_new_vertex_index();
                indices[17] = get_new_vertex_index();
                indices[18] = get_new_vertex_index();
                indices[19] = adj_right_handle_to_us ? middle_right_bottom : get_new_vertex_index();

                indices[20] = qtn->vertex_indices[22];
                indices[21] = adj_bottom_handle_to_us ? middle_bottom_left : get_new_vertex_index();
                indices[22] = qtn->vertex_indices[23];
                indices[23] = adj_bottom_handle_to_us ? middle_bottom_right : get_new_vertex_index();
                indices[24] = qtn->vertex_indices[24];

                // if we got some indices from adjacent nodes, don't recalculate
                if (child1)
                    node_vertex_flags[1] = node_vertex_flags[3] = true;
                if (adj_right_handle_to_us)
                    node_vertex_flags[9] = node_vertex_flags[19] = true;
                if (adj_bottom_handle_to_us)
                    node_vertex_flags[21] = node_vertex_flags[23] = true;

                // get vertices for the new indices
                generate_vertices(child2, geographic_normals, node_vertex_flags, indices);

                // adjust adjacency pointers
                child2->adjacent_nodes[0] = child1;
                child2->adjacent_nodes[1] = adj_right;
                child2->adjacent_nodes[2] = adj_bottom;
                child2->adjacent_nodes[3] = child3;
                child2->dirty = true;

                if (adj_right_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_right->path.w_string(), get_ptr_num(adj_right->adjacent_nodes, *adj_right_handle_to_us), (*adj_right_handle_to_us)->path.w_string()));
#endif
                    *adj_right_handle_to_us = child2;
                    adj_right->dirty = true;
                }

                if (adj_bottom_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_bottom->path.w_string(), get_ptr_num(adj_bottom->adjacent_nodes, *adj_bottom_handle_to_us), (*adj_bottom_handle_to_us)->path.w_string()));
#endif
                    *adj_bottom_handle_to_us = child2;
                    adj_bottom->dirty = true;
                }
            }

            // child 3 (lower left)
            if (child3)
            {
#ifdef DEBUG_SPLITS_AND_MERGES
                log(string::format(L"%ls  creating child {%ls 3}", get_indent(force_level).w_string(), qtn->path.w_string()));
#endif

                add_leaf_node(child3);

                bool node_vertex_flags[25];
                for (int i = 0; i < 25; ++i)
                    node_vertex_flags[i] = NEW_NODE_VERTEX_FLAGS[i];

                // get normals
                geographic_normals[0]  = get_vector(global_normals, qtn->vertex_indices[10]);
                geographic_normals[2]  = get_vector(global_normals, qtn->vertex_indices[11]);
                geographic_normals[4]  = get_vector(global_normals, qtn->vertex_indices[12]);

                geographic_normals[10] = get_vector(global_normals, qtn->vertex_indices[15]);
                geographic_normals[12] = get_vector(global_normals, qtn->vertex_indices[16]);
                geographic_normals[14] = get_vector(global_normals, qtn->vertex_indices[17]);

                geographic_normals[24] = get_vector(global_normals, qtn->vertex_indices[22]);
                geographic_normals[22] = get_vector(global_normals, qtn->vertex_indices[21]);
                geographic_normals[20] = get_vector(global_normals, qtn->vertex_indices[20]);
                fill_in_normals(geographic_normals);

                // get adjacent node on the bottom
                vbuffer::index_t middle_bottom_left, middle_bottom_right;
                sph_qt_node **adj_bottom_handle_to_us = 0;
                sph_qt_node *adj_bottom = get_adjacent(qtn->adjacent_nodes[2], qtn->vertex_indices[20], qtn->vertex_indices[22], &adj_bottom_handle_to_us, &middle_bottom_left, &middle_bottom_right);

                // get adjacent node on the left
                vbuffer::index_t middle_left_top, middle_left_bottom;
                sph_qt_node **adj_left_handle_to_us = 0;
                sph_qt_node *adj_left = get_adjacent(qtn->adjacent_nodes[3], qtn->vertex_indices[10], qtn->vertex_indices[20], &adj_left_handle_to_us, &middle_left_top, &middle_left_bottom);

                // get indices
                indices[0]  = qtn->vertex_indices[10];
                indices[1]  = child0 ? child0->vertex_indices[21] : get_new_vertex_index();
                indices[2]  = qtn->vertex_indices[11];
                indices[3]  = child0 ? child0->vertex_indices[23] : get_new_vertex_index();
                indices[4]  = qtn->vertex_indices[12];

                indices[5]  = adj_left_handle_to_us ? middle_left_top : get_new_vertex_index();
                indices[6]  = get_new_vertex_index();
                indices[7]  = get_new_vertex_index();
                indices[8]  = get_new_vertex_index();
                indices[9]  = child2 ? child2->vertex_indices[5] : get_new_vertex_index();

                indices[10] = qtn->vertex_indices[15];
                indices[11] = get_new_vertex_index();
                indices[12] = qtn->vertex_indices[16];
                indices[13] = get_new_vertex_index();
                indices[14] = qtn->vertex_indices[17];

                indices[15] = adj_left_handle_to_us ? middle_left_bottom : get_new_vertex_index();
                indices[16] = get_new_vertex_index();
                indices[17] = get_new_vertex_index();
                indices[18] = get_new_vertex_index();
                indices[19] = child2 ? child2->vertex_indices[15] : get_new_vertex_index();

                indices[20] = qtn->vertex_indices[20];
                indices[21] = adj_bottom_handle_to_us ? middle_bottom_left : get_new_vertex_index();
                indices[22] = qtn->vertex_indices[21];
                indices[23] = adj_bottom_handle_to_us ? middle_bottom_right : get_new_vertex_index();
                indices[24] = qtn->vertex_indices[22];

                // if we got some indices from adjacent nodes, don't re-calculate them
                if (child0)
                    node_vertex_flags[1] = node_vertex_flags[3] = true;
                if (child2)
                    node_vertex_flags[9] = node_vertex_flags[19] = true;
                if (adj_bottom_handle_to_us)
                    node_vertex_flags[21] = node_vertex_flags[23] = true;
                if (adj_left_handle_to_us)
                    node_vertex_flags[5] = node_vertex_flags[15] = true;

                // get vertices
                generate_vertices(child3, geographic_normals, node_vertex_flags, indices);

                // adjust adjacency pointers
                child3->adjacent_nodes[0] = child0;
                child3->adjacent_nodes[1] = child2;
                child3->adjacent_nodes[2] = adj_bottom;
                child3->adjacent_nodes[3] = adj_left;
                child3->dirty = true;

                if (adj_bottom_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_bottom->path.w_string(), get_ptr_num(adj_bottom->adjacent_nodes, *adj_bottom_handle_to_us), (*adj_bottom_handle_to_us)->path.w_string()));
#endif
                    *adj_bottom_handle_to_us = child3;
                    adj_bottom->dirty = true;
                }

                if (adj_left_handle_to_us)
                {
#ifdef DEBUG_SPLITS_AND_MERGES
                    log(string::format(L"%ls    switching node {%s}'s pointer %d from {%ls} to us",
                                       get_indent(force_level).w_string(), adj_left->path.w_string(), get_ptr_num(adj_left->adjacent_nodes, *adj_left_handle_to_us), (*adj_left_handle_to_us)->path.w_string()));
#endif
                    *adj_left_handle_to_us = child3;
                    adj_left->dirty = true;
                }
            }

            // path
#ifdef DEBUG
            for (int i = 0; i < 4; ++i)
            {
                if (qtn->children[i])
                    qtn->children[i]->path = qtn->path + string::format(L" %d", i);
            }
#endif

#ifdef DEBUG_SPLITS_AND_MERGES
            // debugging log
            for (int i = 0; i < 4; ++i)
            {
                if (qtn->children[i])
                {
                    gsgl::log(string::format(L"%lschild %d {%ls}: neighbors are 0:{%ls}, 1:{%ls}, 2:{%ls}, 3:{%ls}",
                                       get_indent(force_level+1).w_string(), i, qtn->children[i]->path.w_string(),
                                       (qtn->children[i]->adjacent_nodes[0] ? qtn->children[i]->adjacent_nodes[0]->path.w_string() : L"<null>"),
                                       (qtn->children[i]->adjacent_nodes[1] ? qtn->children[i]->adjacent_nodes[1]->path.w_string() : L"<null>"),
                                       (qtn->children[i]->adjacent_nodes[2] ? qtn->children[i]->adjacent_nodes[2]->path.w_string() : L"<null>"),
                                       (qtn->children[i]->adjacent_nodes[3] ? qtn->children[i]->adjacent_nodes[3]->path.w_string() : L"<null>")));
                }
                else
                {
                    gsgl::log(get_indent(force_level+1) + string::format(L"child %d: <null>", i));
                }
            }
#endif
        } // spherical_quadtree::split_node_aux()


        bool spherical_quadtree::split_node(sph_qt_node *qtn, const transform & modelview, const gsgl::scenegraph::context *c, bool no_visual_check, int force_level)
        {
            assert(qtn);

            // have we already split this frame?  why would we?
            if (qtn->last_split_frame == c->frame)
                return true;

            // sanity check: are we a leaf?
            if (!qtn->is_a_leaf())
                throw internal_exception(__FILE__, __LINE__, L"Trying to split a non-leaf node!");

            // try to split
            if (no_visual_check || ((node_cos_angle(qtn, modelview) > ANGLE_CUTOFF) && (node_radius(qtn, c) > PIXEL_CUTOFF)))
            {
#ifdef DEBUG_SPLITS_AND_MERGES
                gsgl::log(string(L"\n") + get_indent(force_level) + L"SPLIT {" + qtn->path.w_string() + L"}");
#endif

                // split neighbors if necessary
                bool can_split = true;
                for (int i = 0; can_split && i < 4; ++i)
                {
                    if (qtn->adjacent_nodes[i] && qtn->adjacent_nodes[i]->level < qtn->level)
                    {
                        can_split = split_node(qtn->adjacent_nodes[i], modelview, c, true, force_level+1);
                    }
                }

                // make sure we weren't split as a part of all that
                if (qtn->last_split_frame == c->frame)
                    return true;

                // now split if possible
                if (can_split)
                {
                    remove_leaf_node(qtn);
                    split_node_aux(qtn, force_level);
                    qtn->last_split_frame = c->frame;

                    if (qtn->parent_node)
                        remove_merge_node(qtn->parent_node);
                    add_merge_node(qtn);
                    return true;
                }
            }

            return false;
        } // spherical_quadtree::split_node()


        static const bool allow_merge = true;
        static const bool allow_split = true;
        static const int NUM_TO_PROCESS = 128;

        void spherical_quadtree::update(gsgl::scenegraph::context *c, const bool not_visible)
        {
            // only update if the view has changed
            vector eye_pos = parent_sg_node->get_modelview().inverse() * vector::ZERO;

            // split nodes
            for (int i = 0; i < NUM_TO_PROCESS && leaf_nodes.size(); ++i) 
            {
                sph_qt_node *qtn = leaf_nodes.front();
                leaf_nodes.pop();

                if (qtn->delete_me)
                {
#ifdef DEBUG
                    if (delete_nodes.find_value(qtn).is_valid())
                        throw internal_exception(__FILE__, __LINE__, L"Duplicate deletion splitting nodes in spherical quadtree.");
#endif

                    delete_nodes.append(qtn);
                }
                else if (!qtn->dequeue_me && qtn->is_a_leaf())
                {
                    if (!split_node(qtn, parent_sg_node->get_modelview(), c, false, 0))
                        leaf_nodes.push(qtn);
                }
            }

            // merge nodes
            for (int i = 0; i < NUM_TO_PROCESS && merge_nodes.size(); ++i)
            {
                sph_qt_node *qtn = merge_nodes.front();
                merge_nodes.pop();

                if (qtn->delete_me)
                {
#ifdef DEBUG
                    if (delete_nodes.find_value(qtn).is_valid())
                        throw internal_exception(__FILE__, __LINE__, L"Duplicate deletion merging nodes in spherical quadtree.");
#endif

                    delete_nodes.append(qtn);
                }
                else if (!qtn->dequeue_me && qtn->is_a_quad())
                {
                    if (!merge_node(qtn, parent_sg_node->get_modelview(), c))
                        merge_nodes.push(qtn);
                }
            }

            // delete nodes
            for (int i = 0; i < delete_nodes.size(); ++i)
            {
                sph_qt_node *qtn = delete_nodes[i];
                delete qtn;
            }

            delete_nodes.clear();

            // save eye position
            eye_pos_in_object_space = eye_pos;
        } // spherical_quadtree::update()


        void spherical_quadtree::cleanup()
        {
            // clean up all child nodes; just keep root nodes
            for (int i = 0; i < 6; ++i)
            {
                if (root_nodes[i])
                {
                    for (int j = 0; j < 4; ++j)
                    {
                        if (root_nodes[i]->children[j])
                        {
                            delete root_nodes[i]->children[j];
                            root_nodes[i]->children[j] = 0;
                        }
                    }
                }
            }

            // unload the vertex buffers
            if (buffers)
                buffers->unload();
        } // spherical_quadtree::cleanup()


        //////////////////////////////////////////

        void spherical_quadtree::free_vertex_index(const vbuffer::index_t & index)
        {
            vbuffer::index_t & count = index_refcounts[index];

            if (count == 0)
                throw internal_exception(__FILE__, __LINE__, L"Something wrong with index refcounts.");

            if (--count == 0)
            {
                freed_vertex_indices.push(index);
            }
        } // spherical_quadtree::free_vertex_index()


        const vbuffer::index_t & spherical_quadtree::attach_vertex_index(const vbuffer::index_t & index)
        {
            index_refcounts[index]++;
            return index;
        } // spherical_quadtree::attach_vertex_index()


        vbuffer::index_t spherical_quadtree::get_new_vertex_index()
        {
            vbuffer::index_t result = 0;

            if (freed_vertex_indices.size())
            {
                result = freed_vertex_indices.top();
                freed_vertex_indices.pop();
            }
            else
            {
                result = global_vertices.size() / 3;
            }

            global_vertices[result*3+0] = 0;
            global_vertices[result*3+1] = 0;
            global_vertices[result*3+2] = 0;

            return result;
        } // spherical_quadtree::get_new_vertex_index()


        //

        static vector vavg(const vector & a, const vector & b)
        {
            return (a + b) * 0.5;
        } // vavg()


        /// This fills in the normals in the vector.  The nine corner normals should be initiated, that is, n[0, 2, 4, 10, 12, 14, 20, 22, 24].
        void spherical_quadtree::fill_in_normals(vector *n)
        {
            // normalize corners
            n[0].normalize();
            n[2].normalize();
            n[4].normalize();

            n[10].normalize();
            n[12].normalize();
            n[14].normalize();

            n[20].normalize();
            n[22].normalize();
            n[24].normalize();

            //// center
            //n[12] = vavg(n[0], n[24]);

            //// middles of sides
            //n[2] = vavg(n[0], n[4]);
            //n[14] = vavg(n[4], n[24]);
            //n[22] = vavg(n[24], n[20]);
            //n[10] = vavg(n[20], n[0]);

            // the rest
            n[1] = vavg(n[0], n[2]);
            n[3] = vavg(n[2], n[4]);
            
            n[5] = vavg(n[0], n[10]);
            n[6] = vavg(n[0], n[12]);
            n[7] = vavg(n[2], n[12]);
            n[8] = vavg(n[12], n[4]);
            n[9] = vavg(n[4], n[14]);

            n[11] = vavg(n[10], n[12]);
            n[13] = vavg(n[12], n[14]);

            n[15] = vavg(n[10], n[20]);
            n[16] = vavg(n[12], n[20]);
            n[17] = vavg(n[12], n[22]);
            n[18] = vavg(n[12], n[24]);
            n[19] = vavg(n[14], n[24]);

            n[21] = vavg(n[20], n[22]);
            n[23] = vavg(n[22], n[24]);

            // just to make sure
            for (int i = 0; i < 25; ++i)
                n[i].normalize();
        } // spherical_quadtree::fill_in_normals()


        void spherical_quadtree::generate_vertices(sph_qt_node *quad, vector *normals, const bool *vertex_flags, vbuffer::index_t *quad_indices)
        {
            gsgl::real_t b_over_a = polar_radius / equatorial_radius;

            for (int i = 0; i < 25; ++i)
            {
                gsgl::index_t index = quad_indices[i];
                attach_vertex_index(index);

                if (!vertex_flags || !vertex_flags[i])
                {
                    // normal 
                    global_normals[index*3+0] = normals[i].get_x();
                    global_normals[index*3+1] = normals[i].get_y();
                    global_normals[index*3+2] = normals[i].get_z();

                    // geographic coordinates
                    double normal_x = normals[i].get_x();
                    double normal_y = normals[i].get_y();
                    double normal_z = normals[i].get_z();

                    double geographic_longitude = ::atan2(normal_y, normal_x);
                    double geographic_latitude = ::asin(normal_z);

                    global_polar_coords[index*2+0] = static_cast<vbuffer::real_t>(geographic_longitude / math::PI_TIMES_2);
                    global_polar_coords[index*2+1] = static_cast<vbuffer::real_t>(0.5 + geographic_latitude / math::PI);

                    // actual cartesian coordinates
                    double normal_base = ::sqrt(normal_x*normal_x + normal_y*normal_y);
                    double geocentric_latitude = ::atan2(b_over_a*b_over_a*normal_z, normal_base); // from Meeus

                    double x = equatorial_radius * ::cos(geographic_longitude) * ::sin(math::PI_OVER_2 - geocentric_latitude);
                    double y = equatorial_radius * ::sin(geographic_longitude) * ::sin(math::PI_OVER_2 - geocentric_latitude);
                    double z = polar_radius * ::cos(math::PI_OVER_2 - geocentric_latitude);

                    global_vertices[index*3+0] = static_cast<vbuffer::real_t>(x);
                    global_vertices[index*3+1] = static_cast<vbuffer::real_t>(y);
                    global_vertices[index*3+2] = static_cast<vbuffer::real_t>(z);
                }

                quad->vertex_indices[i] = index;
            }
        } // spherical_quadtree::generate_vertices()


        void spherical_quadtree::init_root_nodes()
        {
            // geographic normals for the vertices
            vector geographic_normals[25];

            // quadrant 0 (top)
            sph_qt_node *top_quad = root_nodes[0] = create_node(0);

            geographic_normals[0]  = vector(-1, 1, 1);
            geographic_normals[2]  = vector( 0, 1, 1);
            geographic_normals[4]  = vector( 1, 1, 1);

            geographic_normals[10] = vector(-1, 0, 1);
            geographic_normals[12] = vector( 0, 0, 1);
            geographic_normals[14] = vector( 1, 0, 1);

            geographic_normals[24] = vector( 1,-1, 1);
            geographic_normals[22] = vector( 0,-1, 1);
            geographic_normals[20] = vector(-1,-1, 1);
            fill_in_normals(geographic_normals);

            static vbuffer::index_t top_indices[25] = {0, 1, 2, 3, 4,     5, 6, 7, 8, 9,     10, 11, 12, 13, 14,     15, 16, 17, 18, 19,     20, 21, 22, 23, 24 };
            generate_vertices(top_quad, geographic_normals, 0, top_indices);

            // quadrant 1 (front)
            sph_qt_node *front_quad = root_nodes[1] = create_node(0);

            geographic_normals[0]  = vector(-1,-1, 1);
            geographic_normals[2]  = vector( 0,-1, 1);
            geographic_normals[4]  = vector( 1,-1, 1);

            geographic_normals[10] = vector(-1,-1, 0);
            geographic_normals[12] = vector( 0,-1, 0);
            geographic_normals[14] = vector( 1,-1, 0);

            geographic_normals[24] = vector( 1,-1,-1);
            geographic_normals[22] = vector( 0,-1,-1);
            geographic_normals[20] = vector(-1,-1,-1);
            fill_in_normals(geographic_normals);

            static vbuffer::index_t front_indices[25] = { 20, 21, 22, 23, 24,    25, 26, 27, 28, 29,    30, 31, 32, 33, 34,    35, 36, 37, 38, 39,    40, 41, 42, 43, 44 };
            generate_vertices(front_quad, geographic_normals, 0, front_indices);

            // quadrant 2 (left)
            sph_qt_node *left_quad = root_nodes[2] = create_node(0);

            geographic_normals[0]  = vector(-1, 1, 1);
            geographic_normals[2]  = vector(-1, 0, 1);
            geographic_normals[4]  = vector(-1,-1, 1);

            geographic_normals[10] = vector(-1, 1, 0);
            geographic_normals[12] = vector(-1, 0, 0);
            geographic_normals[14] = vector(-1,-1, 0);

            geographic_normals[24] = vector(-1,-1,-1);
            geographic_normals[22] = vector(-1, 0,-1);
            geographic_normals[20] = vector(-1, 1,-1);
            fill_in_normals(geographic_normals);

            static vbuffer::index_t left_indices[25] = { 0, 5, 10, 15, 20,    45, 46, 47, 48, 25,    49, 50, 51, 52, 30,    53, 54, 55, 56, 35,    57, 58, 59, 60, 40 };
            generate_vertices(left_quad, geographic_normals, 0, left_indices);

            // quadrant 3 (right)
            sph_qt_node *right_quad = root_nodes[3] = create_node(0);

            geographic_normals[0]  = vector( 1,-1, 1);
            geographic_normals[2]  = vector( 1, 0, 1);
            geographic_normals[4]  = vector( 1, 1, 1);

            geographic_normals[10] = vector( 1,-1, 0);
            geographic_normals[12] = vector( 1, 0, 0);
            geographic_normals[14] = vector( 1, 1, 0);

            geographic_normals[24] = vector( 1, 1,-1);
            geographic_normals[22] = vector( 1, 0,-1);
            geographic_normals[20] = vector( 1,-1,-1);
            fill_in_normals(geographic_normals);

            static vbuffer::index_t right_indices[25] = { 24, 19, 14, 9, 4,    29, 61, 62, 63, 64,    34, 65, 66, 67, 68,    39, 69, 70, 71, 72,    44, 73, 74, 75, 76 };
            generate_vertices(right_quad, geographic_normals, 0, right_indices);

            // quadrant 4 (bottom)
            sph_qt_node *bottom_quad = root_nodes[4] = create_node(0);

            geographic_normals[0]  = vector(-1,-1,-1);
            geographic_normals[2]  = vector( 0,-1,-1);
            geographic_normals[4]  = vector( 1,-1,-1);

            geographic_normals[10] = vector(-1, 0, -1);
            geographic_normals[12] = vector( 0, 0, -1);
            geographic_normals[14] = vector( 1, 0, -1);

            geographic_normals[24] = vector( 1, 1,-1);
            geographic_normals[22] = vector( 0, 1,-1);
            geographic_normals[20] = vector(-1, 1,-1);
            fill_in_normals(geographic_normals);

            static vbuffer::index_t bottom_indices[25] = { 40, 41, 42, 43, 44,     60, 77, 78, 79, 73,    59, 80, 81, 82, 74,    58, 83, 84, 85, 75,    57, 86, 87, 88, 76 };
            generate_vertices(bottom_quad, geographic_normals, 0, bottom_indices);

            // quadrant 5 (back)
            sph_qt_node *back_quad = root_nodes[5] = create_node(0);

            geographic_normals[0]  = vector(-1, 1,-1);
            geographic_normals[2]  = vector( 0, 1,-1);
            geographic_normals[4]  = vector( 1, 1,-1);

            geographic_normals[10] = vector(-1, 1, 0);
            geographic_normals[12] = vector( 0, 1, 0);
            geographic_normals[14] = vector( 1, 1, 0);

            geographic_normals[24] = vector( 1, 1, 1);
            geographic_normals[22] = vector( 0, 1, 1);
            geographic_normals[20] = vector(-1, 1, 1);
            fill_in_normals(geographic_normals);

            static vbuffer::index_t back_indices[25] = { 57, 86, 87, 88, 76,    53, 89, 90, 91, 72,    49, 92, 93, 94, 68,    45, 95, 96, 97, 64,    0, 1, 2, 3, 4 };
            generate_vertices(back_quad, geographic_normals, 0, back_indices);

            // assign adjacency pointers
            if (root_nodes[0])
            {
                add_leaf_node(root_nodes[0]);

                root_nodes[0]->adjacent_nodes[0] = root_nodes[5];
                root_nodes[0]->adjacent_nodes[1] = root_nodes[3];
                root_nodes[0]->adjacent_nodes[2] = root_nodes[1];
                root_nodes[0]->adjacent_nodes[3] = root_nodes[2];
            }

            if (root_nodes[1])
            {
                add_leaf_node(root_nodes[1]);

                root_nodes[1]->adjacent_nodes[0] = root_nodes[0];
                root_nodes[1]->adjacent_nodes[1] = root_nodes[3];
                root_nodes[1]->adjacent_nodes[2] = root_nodes[4];
                root_nodes[1]->adjacent_nodes[3] = root_nodes[2];
            }

            if (root_nodes[2])
            {
                add_leaf_node(root_nodes[2]);

                root_nodes[2]->adjacent_nodes[0] = root_nodes[0];
                root_nodes[2]->adjacent_nodes[1] = root_nodes[1];
                root_nodes[2]->adjacent_nodes[2] = root_nodes[4];
                root_nodes[2]->adjacent_nodes[3] = root_nodes[5];
            }

            if (root_nodes[3])
            {
                add_leaf_node(root_nodes[3]);

                root_nodes[3]->adjacent_nodes[0] = root_nodes[0];
                root_nodes[3]->adjacent_nodes[1] = root_nodes[5];
                root_nodes[3]->adjacent_nodes[2] = root_nodes[4];
                root_nodes[3]->adjacent_nodes[3] = root_nodes[1];
            }

            if (root_nodes[4])
            {
                add_leaf_node(root_nodes[4]);

                root_nodes[4]->adjacent_nodes[0] = root_nodes[1];
                root_nodes[4]->adjacent_nodes[1] = root_nodes[3];
                root_nodes[4]->adjacent_nodes[2] = root_nodes[5];
                root_nodes[4]->adjacent_nodes[3] = root_nodes[2];
            }

            if (root_nodes[5])
            {
                add_leaf_node(root_nodes[5]);

                root_nodes[5]->adjacent_nodes[0] = root_nodes[4];
                root_nodes[5]->adjacent_nodes[1] = root_nodes[3];
                root_nodes[5]->adjacent_nodes[2] = root_nodes[0];
                root_nodes[5]->adjacent_nodes[3] = root_nodes[2];
            }

            // paths
#ifdef DEBUG
            for (int i = 0; i < 6; ++i)
            {
                if (root_nodes[i])
                    root_nodes[i]->path = string::format(L"%d", i);
            }
#endif
        } // spherical_quadtree::init_root_nodes()


        sph_qt_node *spherical_quadtree::create_node(sph_qt_node *parent)
        {
            return new sph_qt_node(this, parent);
        } // spherical_quadtree::create_node()


    } // namespace space


} // namespace periapsis
