#ifndef PERIAPSIS_SPACE_SPHERICAL_QUADTREE_H
#define PERIAPSIS_SPACE_SPHERICAL_QUADTREE_H

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

#include "space/space.hpp"
#include "space/scenery_patch_set.hpp"

#include "data/stack.hpp"
#include "data/queue.hpp"
#include "platform/vbuffer.hpp"
#include "platform/buffer_pool.hpp"
#include "platform/shader.hpp"


namespace gsgl
{
    namespace scenegraph
    {
        class context;
        class node;
    }
}


namespace periapsis
{

    namespace space
    {

        /// Base class for quadtree nodes.
        class SPACE_API sph_qt_node
        {
        protected:
            friend class spherical_quadtree;
            spherical_quadtree *parent_quadtree;
            
            int level; ///< The level of detail of this node.
            bool dirty, dequeue_me, delete_me;

            sph_qt_node *parent_node;
            sph_qt_node *children[4];
            sph_qt_node *adjacent_nodes[4];                      ///< Pointers to the nodes at the quad's sides.

            gsgl::index_t vertex_indices[25]; ///< Stores the indices into the global buffers.

            gsgl::platform::buffer_pool::object_record buffer_pool_rec;
            gsgl::data::simple_array<gsgl::index_t> triangle_fan_indices; ///< Temporary for storing indices used by the quadrant.
            int num_indices_in_quadrants[4];                     ///< Counts how many indices are used by each subquadrant.

            gsgl::real_t radius_in_world_space;  ///< The "thickness" of the node in world space.
            gsgl::real_t radius_in_screen_space; ///< The "thickness" of the node in screen space.
            unsigned long last_radius_frame;     ///< The frame for which we last calculated the radius of the node.
            unsigned long last_merge_frame;      ///< The frame at which we last merged the node.
            unsigned long last_split_frame;      ///< The frame at which we last split the node.

#ifdef DEBUG
            gsgl::string path;
#endif

        public:
            sph_qt_node(spherical_quadtree *parent_quadtree, sph_qt_node *parent);
            virtual ~sph_qt_node();

            virtual void draw(gsgl::scenegraph::context *c);

            bool is_a_leaf() const; ///< Returns true if the node is a splittable leaf.
            bool is_a_quad() const; ///< Returns true if the node is a mergeable quad.

        private:
            void update_fan_indices();
        }; // class sph_qt_node


        /// A class that maintains and draws LOD spheroids.
        /// Splits are made depending on the bounding spheres of individual quadrants.
        class SPACE_API spherical_quadtree
        {
            friend class sph_qt_node;

        protected:
            gsgl::scenegraph::node *parent_sg_node;
            gsgl::real_t polar_radius, equatorial_radius;

            gsgl::data::simple_array<float> global_vertices; ///< Contains the cartesian vertices of the quadtree.
            gsgl::data::simple_array<float> global_normals;  ///< Contains the normals of each vertex in the quadtree.

            /// Contains the geographic (NOT geocentric) longitude and latitude of each vertex.
            /// Longitude is specified as the actual longitude / 2PI, and latitude as 0.5 + the actual latitude / PI.
            gsgl::data::simple_array<float> global_polar_coords;
            
            gsgl::data::simple_array<gsgl::platform::vbuffer::index_t> index_refcounts;
            gsgl::data::simple_stack<gsgl::platform::vbuffer::index_t> freed_vertex_indices; ///< A stack of indices to free spots in the vertex buffer.

            gsgl::platform::buffer_pool *buffers; // Pool of vertex buffers.

            sph_qt_node *root_nodes[6];

            // variables for keeping track of the screen so as to screen-space calculations
            int last_frame_viewport[4];
            gsgl::math::transform last_frame_modelview_projection;

            gsgl::math::vector eye_pos_in_object_space;

            gsgl::data::simple_queue<sph_qt_node *> leaf_nodes;
            gsgl::data::simple_queue<sph_qt_node *> merge_nodes;
            gsgl::data::simple_array<sph_qt_node *> delete_nodes;

        public:
            spherical_quadtree(gsgl::scenegraph::node *parent_sg_node, const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius);
            virtual ~spherical_quadtree();

            gsgl::scenegraph::node *get_parent_sg_node() { return parent_sg_node; }

            virtual void init(gsgl::scenegraph::context *c);
            virtual void draw(gsgl::scenegraph::context *c);
            virtual void update(gsgl::scenegraph::context *c, const bool not_visible);
            virtual void cleanup();

        protected:
            virtual sph_qt_node *create_node(sph_qt_node *parent);

        private:
            void add_leaf_node(sph_qt_node *qtn);
            void remove_leaf_node(sph_qt_node *qtn);

            void add_merge_node(sph_qt_node *qtn);
            void remove_merge_node(sph_qt_node *qtn);

            gsgl::real_t node_cos_angle(sph_qt_node *qtn, const gsgl::math::transform & modelview);
            gsgl::real_t node_radius(sph_qt_node *qtn, const gsgl::scenegraph::context *c);
            sph_qt_node *get_adjacent(sph_qt_node *candidate, const gsgl::platform::vbuffer::index_t & index0, const gsgl::platform::vbuffer::index_t & index1, 
                                      sph_qt_node ***peer_handle, gsgl::platform::vbuffer::index_t *side0 = 0, gsgl::platform::vbuffer::index_t *side1 = 0);

            //
            bool neighbor_allows_merge(sph_qt_node *qtn, sph_qt_node *adj);
            void merge_node_aux(sph_qt_node *qtn);
            bool merge_node(sph_qt_node *qtn, const gsgl::math::transform & modelview, const gsgl::scenegraph::context *c);

            void split_node_aux(sph_qt_node *qtn, int force_level);
            bool split_node(sph_qt_node *qtn, const gsgl::math::transform & modelview, const gsgl::scenegraph::context *c, bool no_visual_check, int force_level);

            //
            void init_root_nodes();
            void fill_in_normals(gsgl::math::vector *);
            void generate_vertices(sph_qt_node *quad, gsgl::math::vector *normals, const bool *vertex_flags, gsgl::platform::vbuffer::index_t *quad_indices);

            //
            const gsgl::platform::vbuffer::index_t & attach_vertex_index(const gsgl::platform::vbuffer::index_t & index);
            gsgl::platform::vbuffer::index_t get_new_vertex_index();
            void free_vertex_index(const gsgl::platform::vbuffer::index_t & index);
        }; // class spherical_quadtree


    } // namespace space

} // namespace periapsis

#endif
