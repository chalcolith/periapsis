//
// $Id: astronomy.cpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "space/astronomy.hpp"

#include <cmath>

using namespace gsgl::math;

namespace periapsis
{

    namespace space
    {

        double J2000 = 2451545.0;


        /// This is from the Hipparcos data paper; it is the pre-multiplicative orientation of the equatorial frame wrt. the galactic.

        static gsgl::real_t eq_wrt_gal_pre_mult[] = 
        { 
            -0.0548755604f,
            -0.8734370902f,
            -0.4838350155f,
            0.0f,

            0.4941094279f,
            -0.4448296300f,
            0.7469822445f,
            0.0f,

            -0.8676661490f,
            -0.1980763734f,
            0.4559837762f,
            0.0f,

            0.0f, 
            0.0f, 
            0.0f, 
            1.0f
        };


        transform GALACTIC_WRT_EQUATORIAL(eq_wrt_gal_pre_mult);
        transform EQUATORIAL_WRT_GALACTIC = GALACTIC_WRT_EQUATORIAL.transpose();


        /// Also from the Hipparcos data paper; the pre-multiplicative orientation of the orientation of the equatorial frame wrt. the ecliptic.

        gsgl::real_t eq_wrt_ec_pre_mult[] = 
        {
            1.0f,
            0.0f,
            0.0f,
            0.0f,

            0.0f,
            0.9174820621f,
            0.3977771559f,
            0.0f,

            0.0f,
            -0.3977771559f,
            0.9174820621f,
            0.0f,

            0.0f,
            0.0f,
            0.0f,
            1.0f
        };

        
        transform ECLIPTIC_WRT_EQUATORIAL(eq_wrt_ec_pre_mult);
        transform EQUATORIAL_WRT_ECLIPTIC = ECLIPTIC_WRT_EQUATORIAL.transpose();


        // 

        transform ECLIPTIC_WRT_GALACTIC = EQUATORIAL_WRT_GALACTIC * ECLIPTIC_WRT_EQUATORIAL;
        transform GALACTIC_WRT_ECLIPTIC = ECLIPTIC_WRT_GALACTIC.transpose();


        //

        void geocentric_to_geographic(const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius,
                                      const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z,
                                      gsgl::real_t & lat, gsgl::real_t & lon, gsgl::real_t & alt)
        {
            gsgl::real_t a = equatorial_radius;
            gsgl::real_t b = polar_radius;

            gsgl::real_t e = ::sqrt(a*a - b*b) / a;
            gsgl::real_t phi = 0.0;
            gsgl::real_t N;
            gsgl::real_t h;
            gsgl::real_t sinphi;

            for (int i = 0; i < 10; ++i)
            {
                sinphi = ::sin(phi);
                N = a / ::sqrt((gsgl::real_t) 1.0 - e*e*sinphi*sinphi);
                h = ::sqrt(x*x + y*y) / cos(phi) - N;
                phi = ::atan( (z + e*e*N*sinphi) / ::sqrt(x*x + y*y) );
            }

            lat = phi;
            lon = ::atan2(y, x);
            alt = x / (::cos(lon) * ::cos(lat)) - N;
        } // geocentric_to_geographic()


        void geographic_to_geocentric(const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius,
                                      const gsgl::real_t & lat, const gsgl::real_t & lon, const gsgl::real_t & alt,
                                      gsgl::real_t & x, gsgl::real_t & y, gsgl::real_t & z)
        {
            gsgl::real_t a = equatorial_radius;
            gsgl::real_t b = polar_radius;

            gsgl::real_t phi = lat;
            gsgl::real_t lambda = lon;
            gsgl::real_t h = alt;

            gsgl::real_t e = ::sqrt(a*a - b*b) / a;
            gsgl::real_t sinphi = ::sin(phi);
            gsgl::real_t N = a / ::sqrt((gsgl::real_t) 1.0 - e*e*sinphi*sinphi);

            x = (N + h) * ::cos(phi) * ::cos(lambda);
            y = (N + h) * ::cos(phi) * ::sin(lambda);
            z = (N * ((gsgl::real_t) 1.0 - e*e) + h) * ::sin(phi);
        } // geographic_to_geocentric()

    } // namespace space

} // namespace periapsis
