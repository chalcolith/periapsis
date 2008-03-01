#ifndef GSGL_PLATFORM_HEIGHTMAP_H
#define GSGL_PLATFORM_HEIGHTMAP_H

//
// $Id: heightmap.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "data/array.hpp"
#include "math/math.hpp"

namespace gsgl
{

    namespace platform
    {

        class PLATFORM_API heightmap
        {
            const gsgl::index_t width;
            const gsgl::index_t height;
            gsgl::real_t *data;
            bool own_pointer;
            
        public:
            heightmap(const gsgl::index_t & width, const gsgl::index_t & height, gsgl::real_t *data = 0);
            virtual ~heightmap();

            const gsgl::index_t & get_width() const { return width; }
            const gsgl::index_t & get_height() const { return height; }

            /// Tells the interpolation function what method to use.
            enum interpolate_mode { INTERPOLATE_BILINEAR = 1, INTERPOLATE_BICUBIC = 2 };
            
            /// Flags for controlling wrapping.
            enum { 
                WRAP_S = 1 << 0,
                WRAP_T = 1 << 1
            };

            gsgl::real_t interpolate(const gsgl::real_t & s, const gsgl::real_t & t, const interpolate_mode mode = INTERPOLATE_BILINEAR, const gsgl::flags_t flags = WRAP_S | WRAP_T);
        }; // class heightmap


    } // namespace platform

} // namespace gsgl


#endif
