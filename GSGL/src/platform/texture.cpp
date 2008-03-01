//
// $Id: texture.cpp 319 2008-03-01 20:40:39Z Gordon $
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

#include "platform/texture.hpp"

#include "data/shared.hpp"
#include "data/array.hpp"
#include "data/dictionary.hpp"
#include "data/file.hpp"
#include "data/fstream.hpp"
#include "data/cache.hpp"

#include "platform/mapped_file.hpp"
#include "platform/lowlevel.hpp"

#include <cstdlib>
#include <cstring>


namespace gsgl
{

    using namespace data;
    using namespace io;


    namespace platform
    {

        rgba_buffer::rgba_buffer(const string & fname)
            : width(0), height(0), buffer(0), mf(0)
        {
            string full_path = io::file::get_full_path(fname);

            if (!file::exists(full_path))
                throw runtime_exception(L"Unable to map %ls: file does not exist!", full_path.w_string());

            mf = new mapped_file(fname, io::FILE_OPEN_READ | io::FILE_OPEN_WRITE);
            int *iptr = static_cast<int *>( mf->get_pointer() );
            
            width = *iptr++;
            height = *iptr++;

            buffer = reinterpret_cast<unsigned char *>( iptr );
        } // rgba_buffer::rgba_buffer()


        rgba_buffer::rgba_buffer(SDL_Surface *surface)
            : width(surface->w), height(surface->h), buffer(0), mf(0)
        {
            buffer = static_cast<unsigned char *>( ::malloc(surface->w * surface->h * 4) );
            
            if (!buffer)
                throw runtime_exception(L"Out of memory for texture.");

            SDL_LockSurface(surface);

            unsigned char *ptr = static_cast<unsigned char *>(surface->pixels);
            unsigned int temp, pixel;

            for (int v = 0; v < surface->h; ++v)
            {
                for (int u = 0; u < surface->w; ++u)
                {
                    int index = u + v*surface->w;

                    pixel = * reinterpret_cast<unsigned int *>(ptr);

                    // only if big-endian!
                    //pixel >>= 8 * (4 - surface->format->BytesPerPixel);

                    temp = pixel & surface->format->Rmask;
                    temp = temp >> surface->format->Rshift;
                    temp = temp << surface->format->Rloss;
                    buffer[index*4+0] = static_cast<unsigned char>(temp);

                    temp = pixel & surface->format->Gmask;
                    temp = temp >> surface->format->Gshift;
                    temp = temp << surface->format->Gloss;
                    buffer[index*4+1] = static_cast<unsigned char>(temp);

                    temp = pixel & surface->format->Bmask;
                    temp = temp >> surface->format->Bshift;
                    temp = temp << surface->format->Bloss;
                    buffer[index*4+2] = static_cast<unsigned char>(temp);

                    temp = pixel & surface->format->Amask;
                    temp = temp >> surface->format->Ashift;
                    temp = temp << surface->format->Aloss;
                    buffer[index*4+3] = static_cast<unsigned char>(temp);

                    ptr += surface->format->BytesPerPixel;
                }
            }

            SDL_UnlockSurface(surface);

            flip_vertical();
        } // rgba_buffer::rgba_buffer()


        rgba_buffer::rgba_buffer(int width, int height, const color & c)
            : width(width), height(height), buffer(0), mf(0)
        {
            buffer = static_cast<unsigned char *>( ::malloc(width * height * 4) );

            clear(c);
        } // rgba_buffer::rgba_buffer()


        rgba_buffer::~rgba_buffer()
        {
            if (mf)
            {
                delete mf;
            }
            else
            {
                ::free(buffer);
            }
        } // rgba_buffer::~rgba_buffer()


        void rgba_buffer::save(const string & fname)
        {
            fd_stream s(fname, io::FILE_OPEN_WRITE | io::FILE_OPEN_BINARY);

            s.write(reinterpret_cast<unsigned char *>(&width), sizeof(int));
            s.write(reinterpret_cast<unsigned char *>(&height), sizeof(int));

            unsigned char *p = buffer;
            for (int i = 0; i < height; ++i, p += width*4)
            {
                s.write(p, width*4);
            }
        } // rgba_buffer::save()


        void rgba_buffer::clear(const color & c)
        {
            unsigned char r = static_cast<unsigned char>( c[color::COMPONENT_RED] * 255.0f );
            unsigned char g = static_cast<unsigned char>( c[color::COMPONENT_GREEN] * 255.0f );
            unsigned char b = static_cast<unsigned char>( c[color::COMPONENT_BLUE] * 255.0f );
            unsigned char a = static_cast<unsigned char>( c[color::COMPONENT_ALPHA] * 255.0f );

            unsigned char *p = buffer;
            for (int i = 0; i < width*height; ++i)
            {
                *p++ = r;
                *p++ = g;
                *p++ = b;
                *p++ = a;
            }
        } // rgba_buffer::clear()


        void rgba_buffer::flip_vertical()
        {
            int a = 0, b = height - 1;
            unsigned char *temp = static_cast<unsigned char *>( ::malloc(width*4) );
            unsigned char *ptr_a, *ptr_b, *ptr_t = temp;

            while (b > a)
            {
                ptr_a = buffer + a++*width*4;
                ptr_b = buffer + b--*width*4;

                ::memcpy(ptr_t, ptr_a, width*4);
                ::memcpy(ptr_a, ptr_b, width*4);
                ::memcpy(ptr_b, ptr_t, width*4);
            }

            ::free(temp);
        } // rgba_buffer::flip_vertical()


        //////////////////////////////////////////////////////////////


        class texture_impl
            : public shared_object
        {
            string name;
            texture_format format;
            gsgl::flags_t flags;

            const GLenum opengl_texture_unit;
            GLuint opengl_id;

            rgba_buffer *buffer;

        public:
            texture_impl(const string & fname, const texture_format & format, const GLenum opengl_texture_unit, const gsgl::flags_t flags);
            texture_impl(SDL_Surface *surface, const string & name, const texture_format & format, const GLenum opengl_texture_unit, const gsgl::flags_t flags);
            virtual ~texture_impl();

            const string & get_name() const { return name; }
            const int get_texture_unit() { return static_cast<int>(opengl_texture_unit); }

            void bind();
            void unbind();
            void update();
            void unload();
        }; // class texture_impl


        texture_impl::texture_impl(const string & fname, const texture_format & format, const GLenum opengl_texture_unit, const gsgl::flags_t flags)
            : shared_object(), name(fname), format(format), opengl_texture_unit(opengl_texture_unit), flags(flags), opengl_id(0), buffer(0)
        {
            if (!file::exists(fname))
                throw io_exception(L"Texture file %ls not found.", fname.w_string());

            SDL_Surface *surface = IMG_Load(fname.c_string());
            if (!surface)
                throw runtime_exception(L"Unable to load texture file %ls: %hs", fname.w_string(), IMG_GetError());

            buffer = new rgba_buffer(surface);

            SDL_FreeSurface(surface);
        } // texture_impl::texture_impl()


        texture_impl::texture_impl(SDL_Surface *surface, const string & name, const texture_format & format, const GLenum opengl_texture_unit, const gsgl::flags_t flags)
            : shared_object(), name(name), format(format), opengl_texture_unit(opengl_texture_unit), flags(flags), opengl_id(0), buffer(0)
        {
            buffer = new rgba_buffer(surface);
        } // texture_impl::texture_impl()

        
        texture_impl::~texture_impl()
        {
            unload();
            delete buffer;
        } // texture_impl::~texture_impl()


        void texture_impl::bind()
        {
            GLint max_texture_units;
            glGetIntegerv(GL_MAX_TEXTURE_UNITS, &max_texture_units);

            if (opengl_texture_unit >= static_cast<GLenum>(max_texture_units))
                throw runtime_exception(L"Invalid texture unit %d; there are only %d texture units available.", opengl_texture_unit, max_texture_units);

            glActiveTexture(GL_TEXTURE0 + opengl_texture_unit);                                                     CHECK_GL_ERRORS();

            if (opengl_id)
            {
                glBindTexture(GL_TEXTURE_2D, opengl_id);                                                            CHECK_GL_ERRORS();
            }
            else
            {
                glGenTextures(1, &opengl_id);                                                                       CHECK_GL_ERRORS();
                if (opengl_id == 0)
                    throw runtime_exception(L"Unable to generate an OpenGL texture ID for %ls", name.w_string());

                glBindTexture(GL_TEXTURE_2D, opengl_id);                                                            CHECK_GL_ERRORS();

                GLint ff = 0;
                switch (format)
                {
                case TEXTURE_HEIGHTMAP:
                    ff = (flags & TEXTURE_LOAD_UNCOMPRESSED) ? GL_LUMINANCE : GL_COMPRESSED_LUMINANCE;
                    break;
                case TEXTURE_COLORMAP:
                default:
                    ff = (flags & TEXTURE_LOAD_UNCOMPRESSED) ? GL_RGBA : GL_COMPRESSED_RGBA;
                    break;
                }

                glTexImage2D(GL_TEXTURE_2D, 0, ff, buffer->get_width(), buffer->get_height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, buffer->get_pointer()); CHECK_GL_ERRORS();
            }
        } // texture_impl::bind()


        void texture_impl::unbind()
        {
            glBindTexture(GL_TEXTURE_2D, 0);                                                                        CHECK_GL_ERRORS();
        } // texture_impl::unbind()


        void texture_impl::update()
        {
            bind();
            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, buffer->get_width(), buffer->get_height(), GL_RGBA, GL_UNSIGNED_BYTE, buffer->get_pointer());
            unbind();
        } // texture_impl::update()


        void texture_impl::unload()
        {
            if (opengl_id)
            {
                glDeleteTextures(1, &opengl_id);                                                                    CHECK_GL_ERRORS();
                opengl_id = 0;
            }
        } // texture_impl::unload()


        //

        typedef data::cache<texture_impl> texture_cache;


        texture::texture(const texture & tex)
            : impl(tex.impl), flags(tex.flags),
              gl_env(tex.gl_env), gl_wrap(tex.gl_wrap), gl_filter(tex.gl_filter)
        {
            impl->attach();
        } // texture::texture()

        
        texture::texture(const string & fname, const gsgl::flags_t flags, const texture_format & format, const int & texture_unit)
            : impl(0), flags(flags),
              gl_env(GL_REPLACE), gl_wrap(GL_REPEAT), gl_filter(GL_LINEAR)
        {
            texture_cache & global_textures = *texture_cache::global_instance();

            assign_gl_modes();

            string full_path = io::file::get_full_path(fname);
            string full_name = string::format(L"%ls: %08d; %d", full_path.w_string(), flags, texture_unit);

            if (global_textures.contains_index(full_name))
            {
                impl = global_textures[full_name];
            }
            else
            {
                impl = new texture_impl(full_path, format, static_cast<GLenum>(texture_unit), flags);
                global_textures[full_name] = impl; // assign this after in case the creator throws
                impl->attach(); // keep in the cache
            }

            impl->attach();
        } // texture::texture()


        static int TEX_COUNT = 0;


        texture::texture(SDL_Surface *surface,
                         const gsgl::flags_t flags,
                         const texture_format & format, 
                         const string & identifier,
                         const int & texture_unit)
            : impl(0), flags(flags),
              gl_env(GL_REPLACE), gl_wrap(GL_REPEAT), gl_filter(GL_LINEAR)
        {
            texture_cache & global_textures = *texture_cache::global_instance();

            assign_gl_modes();

            // make sure they're different every time (the surface may have changed)
            string ptr_name = string::format(L"SDL surface (%ls): %08p %08d %08d; %d", identifier.w_string(), surface, ++TEX_COUNT, flags, texture_unit);
            
            if (global_textures.contains_index(ptr_name))
            {
                impl = global_textures[ptr_name];
            }
            else
            {
                impl = new texture_impl(surface, ptr_name, format, static_cast<GLenum>(texture_unit), flags);
                global_textures[ptr_name] = impl;
                impl->attach(); // keep it in the cache
            }

            impl->attach();
        } // texture::texture()
        

        texture::~texture()
        {
            impl->detach();
        } // texture::~texture()


        const int texture::get_texture_unit() const
        {
            return impl->get_texture_unit();
        } // texture::get_texture_unit()
        

        void texture::bind(gsgl::flags_t render_flags)
        {
            assert(impl);
            impl->bind();

            if (!(flags & TEXTURE_LOAD_NO_PARAMS))
            {
                glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, gl_env);                                             CHECK_GL_ERRORS();

                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, gl_wrap);                                         CHECK_GL_ERRORS();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, gl_wrap);                                         CHECK_GL_ERRORS();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, gl_filter);                                   CHECK_GL_ERRORS();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, gl_filter);                                   CHECK_GL_ERRORS();

                if (GL_EXT_texture_filter_anisotropic && (render_flags & TEXTURE_RENDER_ANISOTROPIC))
                {
                    GLfloat max_aniso;
                    glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_aniso);                                     CHECK_GL_ERRORS();
                    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, max_aniso);                       CHECK_GL_ERRORS();
                }
            }
        } // texture::bind()


        void texture::unbind()
        {
            assert(impl);
            impl->unbind();
        } // texture::unbind()


        void texture::update()
        {
            assert(impl);
            impl->update();
        } // texture::update()


        void texture::unload()
        {
            assert(impl);
            impl->unload();
        } // texture::unload()


        void texture::assign_gl_modes()
        {
            // env
            if (flags & TEXTURE_ENV_REPLACE)
                gl_env = GL_REPLACE;
            else if (flags & TEXTURE_ENV_MODULATE)
                gl_env = GL_MODULATE;
            else 
                gl_env = GL_REPLACE;

            // wrap
            if (flags & TEXTURE_WRAP_REPEAT)
                gl_wrap = GL_REPEAT;
            else if (flags & TEXTURE_WRAP_CLAMP)
                gl_wrap = GL_CLAMP;
            else
                gl_wrap = GL_REPEAT;

            // filter
            if (flags & TEXTURE_FILTER_LINEAR)
                gl_filter = GL_LINEAR;
            else
                gl_filter = GL_LINEAR;
        } // texture::assign_gl_modes()


        gsgl::data_object *texture::create_global_texture_cache()
        {
            return new texture_cache(L"texture cache");
        } // texture::create_global_texture_cache()
    

    } // namespace platform
    

    // global texture cache
    platform::texture_cache *platform::texture_cache::instance = 0;

} // namespace gsgl
