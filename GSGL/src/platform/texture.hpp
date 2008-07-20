#ifndef GSGL_SG_TEXTURE_H
#define GSGL_SG_TEXTURE_H

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

#include "platform/platform.hpp"

#include "data/pointer.hpp"
#include "data/string.hpp"
#include "data/dictionary.hpp"

#include "platform/color.hpp"


namespace gsgl
{

    class string;

    namespace platform
    {

        class mapped_file;


        /// Encapsulates a 32-bit RGBA buffer (possibly memory-mapped).
        class PLATFORM_API rgba_buffer
        {
            int width, height;
            unsigned char *buffer;

            mapped_file *mf;

        public:
            rgba_buffer(const gsgl::string & fname);                            ///< Creates a memory-mapped buffer from an existing file.
            rgba_buffer(void *surface);                                  ///< Creates a buffer from an SDL surface.
            rgba_buffer(int width, int height, const color & c = color::WHITE); ///< Creates a blank buffer in memory.
            ~rgba_buffer();

            int get_width() const { return width; }
            int get_height() const { return height; }
            unsigned char *get_pointer() { return buffer; }

            void save(const gsgl::string & fname); ///< Saves to a file to be memory-mapped.

            void clear(const color & c);

        private:
            void flip_vertical();
        }; // class rgba_buffer


        //////////////////////////////////////////

        class PLATFORM_API texture_impl
            : public data::shared_object
        {
            string name;
            int format;
            bool compress;

            const unsigned int opengl_texture_unit;
            mutable unsigned int opengl_id;

            data::shared_pointer<rgba_buffer> buffer;

        protected:
            friend class texture;
            texture_impl(const gsgl::string & fname, const int & format, const unsigned int opengl_texture_unit, const bool compress);
            texture_impl(void *surface, const string & name, const int & format, const unsigned int opengl_texture_unit, const bool compress);

        public:
            ~texture_impl();

            const string & get_name() const { return name; }
            const int get_texture_unit() const { return static_cast<int>(opengl_texture_unit); }

            void load();
            void unload();

            void bind();
            void unbind();

            void update();
        }; // class texture_impl


        /// Texture class.  This class uses an internal cache, so you can freely create and delete multiple instances of textures from the same file.
        /// The reason that texture_impl exists is that you may want to use different parameters for drawing with the same buffer.
        class PLATFORM_API texture
        {
            data::shared_pointer<texture_impl> impl;

            gsgl::flags_t flags;
            int gl_env, gl_wrap, gl_filter;

            typedef data::dictionary<data::shared_pointer<texture_impl>, gsgl::string> texture_cache;
            static data::dictionary<texture_cache, gsgl::string> texture_impls; ///< Category -> cache

        public:
            enum texture_flags
            {
                TEXTURE_NO_FLAGS           = 0,
                TEXTURE_LOAD_UNCOMPRESSED  = 1 << 0, ///< Don't compress the texture.
                TEXTURE_LOAD_NO_PARAMS     = 1 << 1, ///< Don't set the OpenGL texture environment or params; calling code will handle it.
                TEXTURE_ENV_REPLACE        = 1 << 3,
                TEXTURE_ENV_MODULATE       = 1 << 4,
                TEXTURE_ENV_MAX            = 1 << 5,
                TEXTURE_WRAP_REPEAT        = 1 << 6,
                TEXTURE_WRAP_CLAMP         = 1 << 7,
                TEXTURE_WRAP_MAX           = 1 << 8,
                TEXTURE_FILTER_LINEAR      = 1 << 9,
                TEXTURE_FILTER_MAX         = 1 << 10
            };

            
            enum texture_render_flags
            {
                TEXTURE_NO_RENDER_FLAGS    = 0,
                TEXTURE_RENDER_ANISOTROPIC = 1 << 0
            };


            enum texture_format
            {
                TEXTURE_COLORMAP  = 1,
                TEXTURE_HEIGHTMAP = 2
            };


            explicit texture(const texture & tex);
            explicit texture(const gsgl::string & category,
                             const gsgl::string & fname, 
                             const gsgl::flags_t flags = TEXTURE_NO_FLAGS,
                             const texture_format & format = TEXTURE_COLORMAP,
                             const int & texture_unit = 0);
            explicit texture(const gsgl::string & category,
                             void *surface,
                             const gsgl::flags_t flags = TEXTURE_NO_FLAGS,
                             const texture_format & format = TEXTURE_COLORMAP,
                             const gsgl::string & identifier = gsgl::string::EMPTY_STRING,
                             const int & texture_unit = 0);

            virtual ~texture();

            const int get_texture_unit() const;

            void load(); ///< Load the texture into the video card.
            void unload(); ///< Unload the texture from the video card.
            void bind(gsgl::flags_t render_flags = TEXTURE_RENDER_ANISOTROPIC);   ///< Bind the texture for use in OpenGL.
            void unbind(); ///< Unbind the texture for use in OpenGL.

            void update(); ///< Update the texture on the card from the memory buffer.

            /// Clears the cache of texture for the given category.  If you specify "__ALL__" for the category, will clear all textures.
            static void clear_cache(const gsgl::string & category);

        private:
            void assign_gl_modes();
        }; // class texture
        
    } // namespace scenegraph
    
} // namespace gsgl

#endif
