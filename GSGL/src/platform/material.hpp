#ifndef GSGL_SG_MATERIAL_H
#define GSGL_SG_MATERIAL_H

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
#include "data/dictionary.hpp"
#include "data/string.hpp"

#include "platform/color.hpp"
#include "platform/texture.hpp"
#include "platform/shader.hpp"

namespace gsgl
{

    namespace data
    {
        class config_record;
    }


    namespace platform
    {

        class PLATFORM_API material_impl
        {
        protected:
            friend class material;
            friend class material_file;

            material_impl();
            material_impl(const data::config_record & conf);

        public:
            virtual ~material_impl();

            platform::color ambient;
            platform::color diffuse;
            platform::color specular;
            platform::color emissive;
            
            data::shared_pointer<platform::texture> color_map;
            data::shared_pointer<platform::texture> normal_map;
            data::shared_pointer<platform::texture> height_map;
            data::shared_pointer<platform::shader_program> shader;

            float shininess;
            bool render_flat;
        }; // class material_impl


        /// Material.
        /// By convention, the color map will be in texture unit 0, the normal map in unit 1, the heightmap in unit 2.
        class PLATFORM_API material
            : public platform_object
        {
        public:
            enum
            {
                DRAW_NO_FLAGS   = 0,
                DRAW_AMBIENT    = 1 << 0,
                DRAW_DIFFUSE    = 1 << 1,
                DRAW_SPECULAR   = 1 << 2,
                DRAW_EMISSIVE   = 1 << 3,
                DRAW_COLOR_MAP  = 1 << 4,
                DRAW_NORMAL_MAP = 1 << 5,
                DRAW_HEIGHT_MAP = 1 << 6,
                DRAW_USE_SHADER = 1 << 7
            };

        private:
            data::shared_pointer<material_impl> impl;
            gsgl::flags_t draw_flags;

        public:
            material(const gsgl::string & category, const data::config_record & conf);
            material(const gsgl::string & category, const gsgl::string & fname, const gsgl::string & material_name);
            virtual ~material();

            gsgl::flags_t & get_draw_flags() { return draw_flags; }

            const platform::color & get_ambient() const;
            const platform::color & get_diffuse() const;
            const platform::color & get_specular() const;
            const platform::color & get_emissive() const;

            const platform::texture *get_color_map() const;
            const platform::texture *get_normal_map() const;
            const platform::texture *get_height_map() const;
            const platform::shader_program *get_shader() const;

            bool is_opaque() const;

            void load();
            void unload();

            void bind(gsgl::flags_t render_flags = 0) const;
            void unbind() const;

            /// Clears the cache for a particular category.  Throws an exception if any of the materials still have references to them.
            /// \note Pass the string "__ALL__" to clear all the caches at once.
            static void clear_cache(const gsgl::string & category);

        private:
            void set_material_flags();
        }; // class material


    } // namespace platform

} // namespace gsgl

#endif
