#ifndef PERIAPSIS_SPACE_SATELLITE_ELEMENT_PROPAGATOR_H
#define PERIAPSIS_SPACE_SATELLITE_ELEMENT_PROPAGATOR_H

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

#include "space/space.hpp"
#include "space/propagator.hpp"

namespace periapsis
{

    namespace space
    {

        /// Propagates using satellite elements:
        /// 
        ///   - a     : semi-major axis (km)
        ///   - e     : eccentricity
        ///   - w     : argument of periapsis (degrees)
        ///   - M     : mean anomaly (degrees)
        ///   - I     : inclination (degrees)
        ///   - O     : longitude of the ascending node (degrees)
        ///   - n     : longitude rate (degrees/day)
        ///   - P     : sidereal period (days)
        ///   - Pw    : argument of periapsis precession period (years)
        ///   - Pnode : longitude of ascending node precession period (years)
        ///   - RA    : right ascension of the laplace plane pole
        ///   - DEC   : declination of the laplace plane pole
        ///   - tlit  : tilt of the laplace plane
        ///
        /// Note that this transforms from kilometers/days to meters/seconds.
        
        class SPACE_API satellite_element_propagator
            : public propagator
        {
            double data[13];
            bool has_data;

        public:
            satellite_element_propagator(const gsgl::data::config_record & obj_config);
            virtual ~satellite_element_propagator();

            virtual void update(const double jdn, gsgl::math::vector & position, gsgl::math::vector & velocity);

            BROKER_DECLARE_CREATOR(periapsis::space::satellite_element_propagator);

        private:
            void get_elements(const gsgl::string & str);
        }; // class satellite_element_propagator

    } // namespace space

} // namespace periapsis

#endif
