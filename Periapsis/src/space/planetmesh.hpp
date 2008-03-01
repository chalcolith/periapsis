#ifndef PERIAPSIS_PLANET_MESH_H
#define PERIAPSIS_PLANET_MESH_H

//
// $Id: planetmesh.hpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "scenegraph/node.hpp"
#include "data/list.hpp"
#include "data/string.hpp"
#include "data/array.hpp"
#include "data/stack.hpp"
#include "data/pool.hpp"
#include "data/pointer.hpp"
#include "math/vector.hpp"
#include "math/transform.hpp"

namespace periapsis
{

    struct heightfield;
    struct colortile;
    struct triangle;
    struct diamond;
    
    /// Planetary terrain mesh.
    class planetmesh 
        : public gsgl::scenegraph::node
    {
        gsgl::real_t eq_r;   ///< Equatorial radius.
        gsgl::real_t pol_r;  ///< Polar radius.
        gsgl::real_t vscale; ///< Vertical scale of terrain.
        
        triangle *root_nodes[12]; ///< The 12 root nodes of the mesh's initial cube.
        
        gsgl::data::pool<gsgl::index_t> vertex_pool; ///< Stores indices of vertices.
        gsgl::data::array<gsgl::real_t> vertices;    ///< The array of vertices used by the terrain mesh.
        gsgl::data::array<gsgl::real_t> normals;     ///< The array of normals of the terrain mesh.
        gsgl::data::array<gsgl::real_t> geo_coords;  ///< Latitude, longitude, altitude.
        gsgl::data::array<unsigned char>  colors;      ///< R, G, B.
        
        gsgl::data::pool<triangle *> leaf_pool; ///< Stores the leaf nodes of the tree.
        gsgl::data::array<gsgl::index_t> draw_indices; ///< Stores the indices of the vertices to draw.
        
        gsgl::data::pool<diamond *> diamond_pool;

        /// OpenGL vertex buffer indices.
        int vertex_buffer, normal_buffer, color_buffer, element_buffer;
        gsgl::math::transform modelview_rot;
        
        gsgl::data::list<heightfield *> height_fields; ///< Height fields used to construct the mesh.
        gsgl::data::list<colortile *>   color_tiles;   ///< Color tiles used to construct the mesh.
        
    public:
        planetmesh(const gsgl::data::config & conf, gsgl::real_t vscale = 1.0);
        ~planetmesh();
        
        /// \name Inherited from \ref gsgl::scenegraph::node.
        /// @{
        gsgl::real_t get_priority(const gsgl::scenegraph::context *);
        gsgl::real_t largest_extent() const { return eq_r; }
        
        void update(const gsgl::scenegraph::context *);
        void draw(const gsgl::scenegraph::context *);
        /// @}
        
        /// \name Utilities for calling code.
        /// @{
        gsgl::real_t get_height_field_alt(gsgl::real_t lat, gsgl::real_t lon);
        void get_color_tile_color(gsgl::real_t lat, gsgl::real_t lon, unsigned char & r, unsigned char & g, unsigned char & b);
        /// @}
        
    private:
        /// \name Mesh Operations
        /// @{
        gsgl::index_t allocate_vertex();
        void release_vertex(const gsgl::index_t vertex_index);
        
        void add_leaf(triangle *);
        void remove_leaf(triangle *);
        void add_diamond(unsigned long long, gsgl::index_t, gsgl::real_t, triangle *, triangle *);
        void add_diamond(unsigned long long, gsgl::index_t, gsgl::real_t, triangle *, triangle *, triangle *, triangle *);
        void remove_diamond(diamond *);
        
        bool split_node(triangle *, const gsgl::scenegraph::context *, bool force = false);
        void merge_diamond(diamond *, const gsgl::scenegraph::context *);
                        
        void planetmesh::init_node(triangle *node, triangle *parent,
                                   triangle *n0, triangle *n1, triangle *n2,
                                   const gsgl::index_t v0, const gsgl::index_t v1, const gsgl::index_t v2);
                                   
        gsgl::real_t pixel_error(gsgl::real_t r, gsgl::real_t d, const gsgl::scenegraph::context *);
        void make_surface_normal(triangle *);
        const gsgl::index_t make_middle_vertex(triangle *);
        /// @}
        
        /// \name Initialization
        /// @{
        void init_root_nodes();
        void init_root_node(triangle *node, int n0, int n1, int n2, 
                            gsgl::real_t, gsgl::real_t, gsgl::real_t, 
                            gsgl::real_t, gsgl::real_t, gsgl::real_t, 
                            gsgl::real_t, gsgl::real_t, gsgl::real_t);
        const gsgl::index_t make_root_vertex(gsgl::real_t, gsgl::real_t, gsgl::real_t);
        /// @}
        
        /// \name Data Loading
        /// @{
        void load_height_field(const gsgl::data::config & conf);
        void load_color_tile(const gsgl::data::config & conf);
        /// @}
    }; // class planetmesh
    
} // namespace periapsis

#endif
