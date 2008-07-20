#ifndef GSGL_SG_MODEL_H
#define GSGL_SG_MODEL_H

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

#include "scenegraph/scenegraph.hpp"

#include "data/pointer.hpp"

#include "scenegraph/node.hpp"

#include "platform/material.hpp"
#include "platform/vbuffer.hpp"

namespace gsgl
{

    namespace scenegraph
    {

        /// A submesh is part of a mesh that is drawn with a particular material.  It is only for internal use.
        /// A submesh_node will collect one or more submeshes to use in a model part.
        class SCENEGRAPH_API submesh
        {
            submesh();
            friend class mesh;
            friend class mesh_file;

        public:
            ~submesh();

            data::shared_pointer<platform::material> mat;

            // we are not bothering to share vertices
            platform::vertex_buffer point_vertices;
            platform::vertex_buffer line_vertices;
            platform::vertex_buffer triangle_vertices;
            platform::vertex_buffer triangle_texcoords;
            platform::vertex_buffer triangle_normals;

            void load();
            void unload();
            void draw(gsgl::flags_t render_flags = 0);
        }; // class submesh


        /// A mesh is a named part of a mesh file.  Only for internal use.
        class SCENEGRAPH_API mesh
        {
            mesh(const string & name);
            friend class mesh_file;

        public:
            ~mesh();

            string name;
            data::object_array<data::shared_pointer<submesh> > submeshes;
        }; // class mesh


        /// A submesh_node holds number of submeshes, to be drawn either in the opaque or translucent group.
        /// Only for internal use.
        class SCENEGRAPH_API submesh_node
            : public scenegraph::node
        {
            data::object_array<data::shared_pointer<submesh> > submeshes;
            bool opaque;

            mutable gsgl::real_t cached_max_extent;

        public:
            submesh_node(const string & name, node *parent, bool opaque);
            virtual ~submesh_node();

            bool get_opaque() { return opaque; }
            const data::object_array<data::shared_pointer<submesh> > & get_submeshes() const { return submeshes; }

            virtual gsgl::real_t get_priority(gsgl::scenegraph::context *); 
            virtual gsgl::real_t max_extent() const;

            virtual void init(gsgl::scenegraph::context *c);
            virtual void draw(gsgl::scenegraph::context *c);
            virtual void update(gsgl::scenegraph::context *c);
            virtual void cleanup(gsgl::scenegraph::context *c);
        }; // class submesh_node


        /// A model_part is a possibly animated part of a model.
        /// It contains child nodes for drawing the visual parts, and meshes for collision and inertia calculation.
        class SCENEGRAPH_API model_part
            : public scenegraph::node
        {
            submesh_node *opaque;
            submesh_node *translucent;

            data::shared_pointer<mesh> inertial, collision;

            data::list<platform::vertex_buffer *> inertial_triangles;
            data::list<platform::vertex_buffer *> collision_triangles;

            model_part(const gsgl::string & category, const gsgl::data::config_record & obj_config);
            friend class model;

        public:
            virtual ~model_part();

            data::list<platform::vertex_buffer *> & get_inertial_triangles();
            data::list<platform::vertex_buffer *> & get_collision_triangles();
        }; // class model_part


        /// A model is a collection of model_part objects.  Each model_part object contains a list of opaque and translucent submesh_nodes.
        /// Each submesh_node contains a list of submesh objects, each to be drawn with a different material.
        /// There is no draw() method, because its model_part objects are its children.
        class SCENEGRAPH_API model
            : public scenegraph::node
        {
            data::simple_array<model_part *> model_parts;

            data::list<platform::vertex_buffer *> inertial_triangles;
            data::list<platform::vertex_buffer *> collision_triangles;

        public:
            model(const gsgl::string & category, const gsgl::data::config_record & obj_config);
            virtual ~model();

            data::list<platform::vertex_buffer *> & get_inertial_triangles();
            data::list<platform::vertex_buffer *> & get_collision_triangles();

            //
            static void clear_cache(const gsgl::string & category);
        }; // class model


    } // namespace scenegraph
    
} // namespace gsgl

#endif
