#ifndef PERIAPSIS_SPACE_KEPLERIAN_ELEMENT_PROPAGATOR_H
#define PERIAPSIS_SPACE_KEPLERIAN_ELEMENT_PROPAGATOR_H

//
// $Id: keplerian_element_propagator.hpp 315 2008-03-01 16:33:59Z Gordon $
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

        /// Propagates using 6 Keplerian elements and rates:
        ///
        ///   - a : semi-major axis (au, au/century)
        ///   - e : eccentricity (radians, radians/century)
        ///   - I : inclination (degrees, degrees/century)
        ///   - L : mean longitude (degrees, degrees/century)
        ///   - W : longitude of perihelion (degrees, degrees/century)
        ///   - O : longitude of the ascending node (degrees, degrees/century)
        ///
        /// In order to work correctly, the parent node's scale must be set to AU.

        class SPACE_API keplerian_element_propagator
            : public propagator
        {
            double elements[6];
            double rates[6];
            double aux[4];
            bool has_data, has_aux;

        public:
            keplerian_element_propagator(const gsgl::data::config_record & obj_config);
            virtual ~keplerian_element_propagator();

            virtual void update(const double jdn, gsgl::math::vector & position, gsgl::math::vector & velocity);

            BROKER_DECLARE_CREATOR(periapsis::space::keplerian_element_propagator);

        private:
            bool get_array(const gsgl::string & str, double *a, const int num);
        }; // class keplerian_element_propagator

    } // namespace space

} // namespace periapsis

#endif
