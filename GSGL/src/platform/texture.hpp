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
#include "platform/color.hpp"
#include "data/string.hpp"

struct SDL_Surface;

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
            rgba_buffer(SDL_Surface *surface);                                  ///< Creates a buffer from an SDL surface.
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


        //

        enum texture_format
        {
            TEXTURE_COLORMAP  = 1,
            TEXTURE_HEIGHTMAP = 2
        };


        enum texture_flags
        {
            TEXTURE_NO_FLAGS           = 0,
            TEXTURE_LOAD_UNCOMPRESSED  = 1 << 0, ///< Don't compress the texture.
            TEXTURE_LOAD_NO_PARAMS     = 1 << 1, ///< Don't set the OpenGL texture environment or params; calling code will handle it.
            TEXTURE_RENDER_ANISOTROPIC = 1 << 2,
            TEXTURE_ENV_REPLACE        = 1 << 3,
            TEXTURE_ENV_MODULATE       = 1 << 4,
            TEXTURE_ENV_MAX            = 1 << 5,
            TEXTURE_WRAP_REPEAT        = 1 << 6,
            TEXTURE_WRAP_CLAMP         = 1 << 7,
            TEXTURE_WRAP_MAX           = 1 << 8,
            TEXTURE_FILTER_LINEAR      = 1 << 9,
            TEXTURE_FILTER_MAX         = 1 << 10
        };


        //

        class texture_impl;


        /// Texture class.  This class uses an internal cache, so you can freely create and delete multiple instances of textures from the same file.
        class PLATFORM_API texture
        {
            texture_impl *impl;    

            gsgl::flags_t flags;
            int gl_env, gl_wrap, gl_filter;

        public:
            explicit texture(const texture & tex);
            explicit texture(const gsgl::string & fname, 
                             const gsgl::flags_t flags = TEXTURE_NO_FLAGS,
                             const texture_format & format = TEXTURE_COLORMAP,
                             const int & texture_unit = 0);
            explicit texture(SDL_Surface *surface,
                             const gsgl::flags_t flags = TEXTURE_NO_FLAGS,
                             const texture_format & format = TEXTURE_COLORMAP,
                             const gsgl::string & identifier = gsgl::string::EMPTY_STRING,
                             const int & texture_unit = 0);

            ~texture();

            const int get_texture_unit() const;

            /// Bind the texture for use in OpenGL.
            void bind(gsgl::flags_t render_flags = TEXTURE_NO_FLAGS) const;   ///< Bind the texture for use in OpenGL, if necessary loading it first.
            void unbind() const; ///< Unbind the texture for use in OpenGL.
            void update() const; ///< Update the texture on the card from the memory buffer.
            void unload() const; ///< Unload the texture from the video card.

            static gsgl::data_object *create_global_texture_cache();

        private:
            void assign_gl_modes();
        }; // class texture
        
    } // namespace scenegraph
    
} // namespace gsgl

#endif
