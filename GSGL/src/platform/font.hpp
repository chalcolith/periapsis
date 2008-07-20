#ifndef GSGL_SG_FONT_H
#define GSGL_SG_FONT_H

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
#include "data/dictionary.hpp"

#include "platform/color.hpp"
#include "platform/texture.hpp"


namespace gsgl
{

    class string;

    namespace platform
    {

        class PLATFORM_API font_impl
        {
            const string face;
            const int size;

            int font_height;
            int font_ascent;
            int texture_height; // this will be the nearest power of 2 greater than max_height

            mutable data::dictionary<float, wchar_t> glyph_pct_x, glyph_pct_y;
            mutable data::dictionary<float, wchar_t> glyph_widths;

            void *ttf_font_ptr;
            color fg;

            mutable data::dictionary<data::shared_pointer<texture>, wchar_t> glyph_textures;

            static const gsgl::string FONT_TEXTURE_CATEGORY;
            static const gsgl::string FONT_DIR;

        protected:
            font_impl(const string &, int size, const color & fg);
            friend class font;

        public:
            ~font_impl();

            const string & get_face() const { return face; }
            const int get_size() const { return size; }
            const color & get_fg() const { return fg; }

            float calc_height(const string &) const;
            float calc_width(const string &) const;

            void draw(const string &) const;

        private:
            texture *get_glyph(const wchar_t) const;

            void get_font_dir();
        }; // class font_impl
    
        
        /// Encapsulates a font for use in OpenGL.
        class PLATFORM_API font
            : public platform_object
        {
            data::shared_pointer<font_impl> impl;
            
        public:
            font(const gsgl::string & face, int size, const color & fg);
            virtual ~font();

            //
            const gsgl::string & get_face() const;
            const int get_size() const;

            /// 
            float calc_height(const gsgl::string &) const;
            float calc_width(const gsgl::string &) const;

            /// Draws a string.
            void draw(const gsgl::string &) const;

            /// Clears the cache of all fonts.
            static void clear_cache();
        }; // class font

        
    } // namespace platform
    
} // namespace gsgl

#endif
