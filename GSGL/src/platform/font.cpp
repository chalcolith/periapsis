//
// $Id: font.cpp 319 2008-03-01 20:40:39Z Gordon $
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

#include "platform/font.hpp"
#include "platform/texture.hpp"

#include "data/shared.hpp"
#include "data/pointer.hpp"
#include "data/dictionary.hpp"
#include "data/cache.hpp"
#include "data/directory.hpp"
#include "data/file.hpp"

#include "platform/display.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace data;

    namespace platform
    {

        static void clear_pixel_alpha(SDL_Surface *surface)
        {
            SDL_LockSurface(surface);

            unsigned char *pixels = static_cast<unsigned char *>(surface->pixels);

            for (int v = 0; v < surface->h; ++v)
            {
                for (int u = 0; u < surface->w; ++u)
                {
                    int index = u * surface->format->BytesPerPixel + v * surface->pitch;
                    unsigned int *pixel = reinterpret_cast<unsigned int *>(&pixels[index]);

                    *pixel &= ~surface->format->Amask;
                }
            }

            SDL_UnlockSurface(surface);
        } // clear_pixel_alpha()


        static void src_alpha_blit(SDL_Surface *src, SDL_Surface *dest, int dest_x, int dest_y)
        {
            SDL_LockSurface(src);
            SDL_LockSurface(dest);

            unsigned char *src_pixels = static_cast<unsigned char *>(src->pixels);
            unsigned char *dest_pixels = static_cast<unsigned char *>(dest->pixels);

            for (int v = 0; v < src->h; ++v)
            {
                for (int u = 0; u < src->w; ++u)
                {
                    int src_index = u * src->format->BytesPerPixel + v * src->pitch;
                    int dest_index = (u + dest_x) * src->format->BytesPerPixel + (v + dest_y) * dest->pitch;

                    unsigned int *src_pixel = reinterpret_cast<unsigned int *>(&src_pixels[src_index]);
                    unsigned int *dest_pixel = reinterpret_cast<unsigned int *>(&dest_pixels[dest_index]);

                    unsigned int sp = *src_pixel & (src->format->Rmask | src->format->Gmask | src->format->Bmask | src->format->Amask);
                    *dest_pixel |= sp;
                }
            }
            
            SDL_UnlockSurface(dest);
            SDL_UnlockSurface(src);
        } // src_alpha_blit()


        //

        static int nearest_power_2(int n)
        {
            int res = 1;
            while (res < n)
                res <<= 1;
            return res;
        } // nearest_power_2()
            
        //

        class font_impl
            : public shared_object
        {
            const string face;
            const int size;

            int font_height;
            int font_ascent;
            int texture_height; // this will be the nearest power of 2 greater than max_height

            dictionary<float, wchar_t> glyph_pct_x, glyph_pct_y;
            dictionary<float, wchar_t> glyph_widths;

            TTF_Font *font;
            color fg;
            dictionary<texture *, wchar_t> glyph_textures;
        public:
            font_impl(const string &, int size, const color & fg);
            virtual ~font_impl();

            const string & get_face() const { return face; }
            const int get_size() const { return size; }
            const color & get_fg() const { return fg; }

            float calc_height(const string &);
            float calc_width(const string &);

            void draw(const string &);

        private:
            texture *get_glyph(const wchar_t);

            void get_font_dir();
        }; // class font_impl

        //

        static const string FONT_DIR;

        //

        font_impl::font_impl(const string & face, int size, const color & fg)
            : shared_object(), face(face), size(size), font(0), fg(fg)
        {
            //LOG_BASIC(L"font_impl: creating %ls %d (%f, %f, %f)", face.w_string(), size, fg[color::COMPONENT_RED], fg[color::COMPONENT_GREEN], fg[color::COMPONENT_BLUE]);

            if (FONT_DIR.is_empty())
                get_font_dir();

            // common font names
            string fname = FONT_DIR;

#ifdef WIN32
            if (face == L"Mono")
            {
                fname += L"cour.ttf";
            }
            else if (face == L"Sans")
            {
                fname += L"arial.ttf";
            }
            else
            {
                throw runtime_exception(L"Unable to determine font file name for '%ls'", face.w_string());
            }
#else
#error Font name lookup unimplemented!
#endif

            //
            font = TTF_OpenFont(fname.c_string(), size);

            if (!font)
                throw runtime_exception(L"Unable to load font '%ls': %hs", face.w_string(), TTF_GetError());

            //
            font_height = TTF_FontHeight(font);
            font_ascent = TTF_FontAscent(font);
            texture_height = nearest_power_2(font_height);
        } // font_impl::font_impl()


        font_impl::~font_impl()
        {
            //LOG_BASIC(L"font_impl: deleting %ls %d (%f, %f, %f)", face.w_string(), size, fg[color::COMPONENT_RED], fg[color::COMPONENT_GREEN], fg[color::COMPONENT_BLUE]);

            for (dictionary<texture *, wchar_t>::iterator i = glyph_textures.iter(); i.is_valid(); ++i)
                delete *i;

            TTF_CloseFont(font);
        } // font_impl::~font_impl()


        float font_impl::calc_height(const string & str)
        {
            int w, h;

            if (TTF_SizeUTF8(font, str.c_string(), &w, &h) != -1)
                return static_cast<float>(h);
            else
                throw runtime_exception(L"%hs", TTF_GetError());
        } // font_impl::calc_height()


        float font_impl::calc_width(const string & str)
        {
            int w, h;

            if (TTF_SizeUTF8(font, str.c_string(), &w, &h) != -1)
                return static_cast<float>(w);
            else
                throw runtime_exception(L"%hs", TTF_GetError());
        } // font_impl::calc_width()


        void font_impl::draw(const string & str)
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                       CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                          CHECK_GL_ERRORS();

            glPolygonMode(GL_FRONT, GL_FILL);                                                                       CHECK_GL_ERRORS();

            glEnable(GL_BLEND);                                                                                     CHECK_GL_ERRORS();
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                                      CHECK_GL_ERRORS();

            glEnable(GL_TEXTURE_2D);                                                                                CHECK_GL_ERRORS();

            const gsgl::index_t len = str.size();
            for (gsgl::index_t i = 0; i < len; ++i)
            {
                const wchar_t ch = str[i];

                texture *t = get_glyph(ch);
                if (t)
                {
                    t->bind();
                    
                    float pct_y = glyph_pct_y[ch];
                    float pct_x = glyph_pct_x[ch];

                    glBegin(GL_TRIANGLE_STRIP);

                    // upper left
                    glTexCoord2f(0.0f, pct_y);
                    glVertex2f(0.0f, static_cast<GLfloat>(font_height));

                    // lower left
                    glTexCoord2f(0.0f, 0.0f);
                    glVertex2f(0.0f, 0.0f);

                    // upper right
                    glTexCoord2f(pct_x, pct_y);
                    glVertex2f(glyph_widths[ch], static_cast<GLfloat>(font_height));

                    // lower right
                    glTexCoord2f(pct_x, 0.0f);
                    glVertex2f(glyph_widths[ch], 0.0f);

                    glEnd();                                                                                            CHECK_GL_ERRORS();

                    //display::draw_rectangle(0, 0, glyph_widths[ch], static_cast<GLfloat>(font_height), 0, 0, pct_x, pct_y);
                }

                glMatrixMode(GL_MODELVIEW);                                                                         CHECK_GL_ERRORS();
                glTranslatef(glyph_widths[ch], 0.0f, 0.0f);                                                         CHECK_GL_ERRORS();
            }

            glPopClientAttrib();                                                                                    CHECK_GL_ERRORS();
            glPopAttrib();                                                                                          CHECK_GL_ERRORS();
        } // font_impl::draw()


        texture *font_impl::get_glyph(const wchar_t ch)
        {
            texture *tex = glyph_textures[ch];

            if (!tex)
            {
                int minx, maxx, miny, maxy, advance;
                if (TTF_GlyphMetrics(font, ch, &minx, &maxx, &miny, &maxy, &advance) == -1)
                    throw runtime_exception(L"error getting glyph metrics: %hs", TTF_GetError());

                SDL_Color c;
                c.r = static_cast<Uint8>(fg[color::COMPONENT_RED] * 255.0f);
                c.g = static_cast<Uint8>(fg[color::COMPONENT_GREEN] * 255.0f);
                c.b = static_cast<Uint8>(fg[color::COMPONENT_BLUE] * 255.0f);

                SDL_Surface *surf1 = TTF_RenderGlyph_Blended(font, ch, c);

                SDL_Surface *surf2 = SDL_CreateRGBSurface(SDL_SWSURFACE, nearest_power_2(minx + surf1->w), texture_height, 32, surf1->format->Rmask, surf1->format->Gmask, surf1->format->Bmask, surf1->format->Amask);

                if (!surf2)
                    throw runtime_exception(L"unable to create SDL surface: %hs", SDL_GetError());

                SDL_Rect dest;

                dest.x = minx > 0 ? minx : 0;
                dest.y = (surf2->h - font_height) + (font_ascent - maxy);
                dest.w = surf1->w;
                dest.h = surf1->h;

                clear_pixel_alpha(surf2);
                src_alpha_blit(surf1, surf2, dest.x, dest.y);

                string glyph_id = string::format(L"%ls %d: %lc", face.w_string(), size, ch);
                glyph_textures[ch] = tex = new texture(surf2, TEXTURE_ENV_REPLACE | TEXTURE_WRAP_CLAMP | TEXTURE_FILTER_LINEAR, TEXTURE_COLORMAP, glyph_id.w_string());

                glyph_pct_x[ch] = static_cast<float>(advance) / static_cast<float>(surf2->w);
                glyph_pct_y[ch] = static_cast<float>(font_height) / static_cast<float>(surf2->h);
                glyph_widths[ch] = static_cast<float>(advance);

                SDL_FreeSurface(surf2);
                SDL_FreeSurface(surf1);
            }

            return tex;
        } // font_impl::get_glyph()


        void font_impl::get_font_dir()
        {
#ifdef WIN32
            smart_pointer<wchar_t, true> buf(new wchar_t[MAX_PATH]);
            
            if (SUCCEEDED(SHGetFolderPath(0, CSIDL_WINDOWS, 0, 0, buf)))
            {
                string path(buf);
                io::directory font_dir(path);
                const_cast<string &>(FONT_DIR) = font_dir.get_full_path() + L"Fonts" + io::directory::SEPARATOR;
            }
            else
            {
                throw io_exception(L"unable to find system font directory");
            }
#endif
        } // font_impl::get_font_dir()


        //

        typedef data::cache<font_impl> font_cache;

        //
        
        font::font(const string & face, int size, const color & fg)
        {
            //LOG_BASIC(L"font: creating font %ls %d (%f, %f, %f)", face.w_string(), size, fg[color::COMPONENT_RED], fg[color::COMPONENT_GREEN], fg[color::COMPONENT_BLUE]);
            
            font_cache & global_fonts = *font_cache::global_instance();

            string cache_name = string::format(L"%s %d %f %f %f %f", face.w_string(), size, fg[color::COMPONENT_RED], fg[color::COMPONENT_GREEN], fg[color::COMPONENT_BLUE], fg[color::COMPONENT_ALPHA]);
            
            if (global_fonts.contains_index(cache_name))
            {
                impl = global_fonts[cache_name];
            }
            else
            {
                impl = new font_impl(face, size, fg);
                global_fonts[cache_name] = impl;
                impl->attach();
            }
            impl->attach();
        } // font::font()
        

        font::~font()
        {
            assert(impl);

            //LOG_BASIC(L"font: deleting font %ls %d (%f, %f, %f)", impl->get_face().w_string(), impl->get_size(), impl->get_fg()[color::COMPONENT_RED], impl->get_fg()[color::COMPONENT_GREEN], impl->get_fg()[color::COMPONENT_BLUE]);
            impl->detach();
        } // font::~font()
        

        const string & font::get_face() const
        {
            assert(impl);
            return impl->get_face();
        } // font::get_face()


        const int font::get_size() const
        {
            assert(impl);
            return impl->get_size();
        } // font::get_size()


        float font::calc_height(const string & str) const
        {
            assert(impl);
            return impl->calc_height(str);
        } // font::calc_height()


        float font::calc_width(const string & str) const
        {
            assert(impl);
            return impl->calc_width(str);
        } // font::calc_width()


        void font::draw(const string & str) const
        {
            assert(impl);
            impl->draw(str);
        } // font::draw()


        gsgl::data_object *font::create_global_font_cache()
        {
            return new font_cache(L"font cache");
        } // font::create_global_font_cache()


    } // namespace platform


    platform::font_cache *platform::font_cache::instance = 0; // global font cache

} // namespace gsgl
