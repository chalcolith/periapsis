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
#include "scenegraph/node.hpp"

#include "platform/color.hpp"
#include "platform/texture.hpp"
#include "platform/shader.hpp"
#include "platform/vbuffer.hpp"

namespace gsgl
{

    namespace scenegraph
    {

        class material_impl;


        /// A material is a named material.
        class SCENEGRAPH_API material
        {
            material_impl *impl;

        public:
            material(const gsgl::string & material_file, const gsgl::string & material_name);
            ~material();

            gsgl::string & get_name();
            platform::color & get_ambient();
            platform::color & get_diffuse();
            platform::color & get_specular();
            float & get_shininess();
            bool & get_flat();

            platform::texture *get_texture();
            platform::shader_base *get_shader();

            bool is_opaque();

            void bind(gsgl::flags_t render_flags = 0);

            static gsgl::data_object *create_global_material_cache();
        }; // class material


        //////////////////////////////////////////

        class submesh_node;
        class mesh;

        /// A model_part is a possibly animated part of a model.
        /// It contains child nodes for drawing the visual parts, and meshes for collision and inertia calculation.
        class model_part
            : public scenegraph::node
        {
            submesh_node *opaque, *translucent;
            mesh *inertial, *collision;

            data::list<platform::vertex_buffer *> inertial_triangles;
            data::list<platform::vertex_buffer *> collision_triangles;

        public:
            model_part(const gsgl::data::config_record & obj_config);
            virtual ~model_part();

            data::list<platform::vertex_buffer *> & get_inertial_triangles();
            data::list<platform::vertex_buffer *> & get_collision_triangles();
        }; // class model_part


        /// A model is a collection of model_parts.
        class SCENEGRAPH_API model
            : public scenegraph::node
        {
            data::simple_array<model_part *> model_parts;

            data::list<platform::vertex_buffer *> inertial_triangles;
            data::list<platform::vertex_buffer *> collision_triangles;

        public:
            model(const gsgl::data::config_record & obj_config);
            virtual ~model();

            data::list<platform::vertex_buffer *> & get_inertial_triangles();
            data::list<platform::vertex_buffer *> & get_collision_triangles();

            static gsgl::data_object *create_global_model_cache();
        }; // class model


    } // namespace scenegraph
    
} // namespace gsgl

#endif
