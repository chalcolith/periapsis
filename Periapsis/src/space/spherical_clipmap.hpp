#ifndef PERIAPSIS_SPACE_SPHERICAL_CLIPMAP_H
#define PERIAPSIS_SPACE_SPHERICAL_CLIPMAP_H

//
// $Id: spherical_clipmap.hpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "space/space.hpp"

#include "data/array.hpp"
#include "math/transform.hpp"
#include "scenegraph/context.hpp"

namespace periapsis
{

    namespace space
    {

        class clipmap_ring;


        class SPACE_API spherical_clipmap
        {
            gsgl::real_t polar_radius, equatorial_radius;

            gsgl::index_t first_ring, last_ring;
            gsgl::data::array<clipmap_ring *> clipmap_rings;
            clipmap_ring *clipmap_cap;

        public:
            spherical_clipmap(gsgl::real_t polar_radius, gsgl::real_t equatorial_radius);
            virtual ~spherical_clipmap();

            gsgl::real_t get_polar_radius() const { return polar_radius; }
            gsgl::real_t get_equatorial_radius() const { return equatorial_radius; }

            void init(gsgl::scenegraph::context *c);
            void update(const gsgl::math::transform & modelview, gsgl::real_t field_of_view, int screen_height);
            void draw(gsgl::scenegraph::context *c);
        }; // class spherical_clipmap


    } // namespace space

} // namespace periapsis

#endif
