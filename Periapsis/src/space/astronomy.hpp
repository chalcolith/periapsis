#ifndef GSGL_PERIAPSIS_ASTRONOMY_H
#define GSGL_PERIAPSIS_ASTRONOMY_H

//
// $Id: astronomy.hpp 315 2008-03-01 16:33:59Z Gordon $
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
#include "math/math.hpp"
#include "math/transform.hpp"

namespace periapsis
{

    namespace space
    {

        extern SPACE_API double J2000; 


        /// \name Coordinate System Transforms
        /// Note that these are for the usual OpenGL setup where transforms are done by multiplying the matrix by the vector to transform.
        /// @{

        extern SPACE_API gsgl::math::transform EQUATORIAL_WRT_GALACTIC;
        extern SPACE_API gsgl::math::transform GALACTIC_WRT_EQUATORIAL;

        extern SPACE_API gsgl::math::transform ECLIPTIC_WRT_EQUATORIAL;
        extern SPACE_API gsgl::math::transform EQUATORIAL_WRT_ECLIPTIC;

        extern SPACE_API gsgl::math::transform ECLIPTIC_WRT_GALACTIC;
        extern SPACE_API gsgl::math::transform GALACTIC_WRT_ECLIPTIC;

        /// @}

        /// \name Geographic Coordinates
        /// @{

        /// Converts from geocentric cartesian coordinates to geographic polar coordinates.
        ///
        /// This function is much more expensive than its inverse, so use sparingly...
        ///
        /// \param polar_radius      The polar radius of the planet.
        /// \param equatorial_radius The equatorial radius of the planet.
        /// \param x                 The x coordinate.
        /// \param y                 The y coordinate.
        /// \param z                 The z coordinate.
        /// \param lat               The resulting geodedic latitude.
        /// \param lon               The resulting geodedic longitude.
        /// \param alt               The altitude above the surface of the earth.
        void SPACE_API geocentric_to_geographic(const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius,
                                                const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z,
                                                gsgl::real_t & lat, gsgl::real_t & lon, gsgl::real_t & alt);

        /// Converts from geographic polar coordinates to geocentric cartesian coordinates.
        ///
        /// \param polar_radius      The polar radius of the planet.
        /// \param equatorial_radius The equatorial radius of the planet.
        /// \param lat               The geodedic latitude.
        /// \param lon               The geodedic longitude.
        /// \param alt               The altitude above the planet.
        /// \param x                 The resulting x coordinate.
        /// \param y                 The resulting y coordinate.
        /// \param z                 The resulting z coordinate.
        void SPACE_API geographic_to_geocentric(const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius,
                                                const gsgl::real_t & lat, const gsgl::real_t & lon, const gsgl::real_t & alt,
                                                gsgl::real_t & x, gsgl::real_t & y, gsgl::real_t & z);

        /// @}

    } // namespace space

} // namespace periapsis

#endif
