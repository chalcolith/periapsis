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

#include "scenegraph/heightmap.hpp"

#include "data/string.hpp"
#include "data/pointer.hpp"
#include "data/file.hpp"
#include "math/math.hpp"
#include "platform/hardware.hpp"

#include <cstdlib>

namespace gsgl
{

    using namespace data;
    using namespace platform;


    namespace scenegraph
    {

        heightmap::heightmap(const string & fname, double altitude)
            : scenegraph_object(), width(0), height(0), altitude(altitude), surface(0)
        {
            init(fname);
        } // heightmap::heightmap()


        heightmap::~heightmap()
        {
            if (surface)
                SDL_FreeSurface(surface);
        } // heightmap::~heightmap()


        static void flip_texture(SDL_Surface *s)
        {
            SDL_LockSurface(s);

            int a = 0, b = s->h - 1;
            smart_pointer<unsigned char, true> temp(new unsigned char[s->pitch]);
            unsigned char *ptr_a, *ptr_b, *ptr_t = temp.ptr();

            while (b > a)
            {
                ptr_a = static_cast<unsigned char *>(s->pixels) + a++*s->pitch;
                ptr_b = static_cast<unsigned char *>(s->pixels) + b--*s->pitch;

                ::memcpy(ptr_t, ptr_a, s->pitch);
                ::memcpy(ptr_a, ptr_b, s->pitch);
                ::memcpy(ptr_b, ptr_t, s->pitch);
            }

            SDL_UnlockSurface(s);
        } // flip_texture()


        void heightmap::init(const string & fname)
        {
            if (!io::file::exists(fname))
                throw io_exception(L"%ls does not exist!", fname.w_string());

            surface = IMG_Load(fname.c_string());

            if (!surface)
                throw io_exception(L"Unable to load %ls!", fname.w_string());

            flip_texture(surface);

            width = surface->w;
            height = surface->h;
        } // heightmap::init()


        void heightmap::get_data(const double s, const double t, double & hval, double & alpha)
        {
            unsigned char rgba[4];

            SDL_LockSurface(surface);
            unsigned char *buf = static_cast<unsigned char *>(surface->pixels);
            math::interpolate_bilinear<unsigned char>(buf, width, height, surface->format->BytesPerPixel, s, t, rgba, math::INTERPOLATE_NO_FLAGS);
            SDL_UnlockSurface(surface);           

            double r = static_cast<double>(rgba[0]) / 255.0;
            double g = static_cast<double>(rgba[1]) / 255.0;
            double b = static_cast<double>(rgba[2]) / 255.0;
            double a = static_cast<double>(rgba[3]) / 255.0;

            hval = (r + g + b) / 3.0;
            alpha = a;
        } // height_map::get_data()


    } // namespace scenegraph

} // namespace gsgl
