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

#include "platform/heightmap.hpp"

#include <cmath>

namespace gsgl
{

    using namespace data;

    namespace platform
    {

        heightmap::heightmap(const gsgl::index_t & width, const gsgl::index_t & height, gsgl::real_t *data)
            : width(width), height(height), data(data), own_pointer(!data)
        {
            if (!data)
                data = new gsgl::real_t[width*height];
        } // heightmap::heightmap()


        heightmap::~heightmap()
        {
            if (own_pointer)
                delete [] data;
        } // heightmap::~heightmap()


        gsgl::real_t heightmap::interpolate(const gsgl::real_t & s, const gsgl::real_t & t, const interpolate_mode mode, const gsgl::flags_t flags)
        {
            gsgl::real_t result = 0;

            if (mode == INTERPOLATE_BILINEAR)
            {
                // x values
                gsgl::real_t x_pixel = ::floor(s * width);
                gsgl::real_t x_diff = (s * width) - x_pixel;
                gsgl::real_t x = (x_diff >= 0.5f) ? x_diff - 0.5f : x_diff + 0.5f;
                
                int x_index_0 = (x_diff < 0.5f) ? static_cast<int>(x_pixel-1) : static_cast<int>(x_pixel);
                int x_index_1 = x_index_0 + 1;

                if (x_index_0 < 0)
                {
                    if (flags & WRAP_S)
                        x_index_0 = width + x_index_0;
                    else
                        x_index_0 = x_index_1;
                }

                if (x_index_1 >= width)
                {
                    if (flags & WRAP_S)
                        x_index_1 = width - x_index_1;
                    else
                        x_index_1 = x_index_0;
                }

                // y values
                gsgl::real_t y_pixel = ::floor(t * height);
                gsgl::real_t y_diff = (t * height) - y_pixel;
                gsgl::real_t y = (y_diff >= 0.5f) ? y_diff - 0.5f : y_diff + 0.5f;

                int y_index_0 = (y_diff < 0.5) ? static_cast<int>(y_pixel-1) : static_cast<int>(y_pixel);
                int y_index_1 = y_index_0 + 1;

                if (y_index_0 < 0)
                {
                    if (flags & WRAP_T)
                        y_index_0 = height + y_index_0;
                    else
                        y_index_0 = y_index_1;
                }

                if (y_index_1 >= height)
                {
                    if (flags & WRAP_T)
                        y_index_1 = height - y_index_1;
                    else
                        y_index_1 = y_index_0;
                }

                // get corner values
                gsgl::real_t f00 = data[ (y_index_0 * width) + x_index_0 ];
                gsgl::real_t f01 = data[ (y_index_1 * width) + x_index_0 ];
                gsgl::real_t f10 = data[ (y_index_0 * width) + x_index_1 ];
                gsgl::real_t f11 = data[ (y_index_1 * width) + x_index_1 ];

                // interpolate
                result = f00*(1-x)*(1-y) + f10*x*(1-y) + f01*(1-x)*y + f11*x*y;
            }
            else
            {
                throw runtime_exception(L"Interpolation other than bilinear is not supported at this time.");
            }

            return result;
        } // heightmap::interpolate_bilinear()


    } // namespace platform

} // namespace gsgl

