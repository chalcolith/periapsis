#ifndef GSGL_MATH_UNITS_H
#define GSGL_MATH_UNITS_H

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

#include "math/math.hpp"

namespace gsgl
{

    class string;

    namespace math
    {

        namespace units
        {

            extern MATH_API const gsgl::real_t METERS_PER_MILLIMETER; // meters / millimeter
            extern MATH_API const gsgl::real_t METERS_PER_CENTIMETER; // meters / centimeter
            extern MATH_API const gsgl::real_t METERS_PER_METER;      // meters / meter
            extern MATH_API const gsgl::real_t METERS_PER_KILOMETER;  // meters / kilometer
            extern MATH_API const gsgl::real_t METERS_PER_AU;         // meters / au
            extern MATH_API const gsgl::real_t METERS_PER_PARSEC;     // meters / parsec

            extern MATH_API const gsgl::real_t KILOGRAMS_PER_MILLIGRAM;  // kilograms / milligram
            extern MATH_API const gsgl::real_t KILOGRAMS_PER_GRAM;       // kilograms / gram
            extern MATH_API const gsgl::real_t KILOGRAMS_PER_KILOGRAM;   // kilograms / kilogram

            extern MATH_API const gsgl::real_t SECONDS_PER_MILLISECOND;// seconds / millisecond
            extern MATH_API const gsgl::real_t SECONDS_PER_SECOND;     // seconds / second
            extern MATH_API const gsgl::real_t SECONDS_PER_MINUTE;     // seconds / minute
            extern MATH_API const gsgl::real_t SECONDS_PER_HOUR;       // seconds / hour
            extern MATH_API const gsgl::real_t SECONDS_PER_DAY;        // seconds / day
            extern MATH_API const gsgl::real_t SECONDS_PER_YEAR;       // seconds / year

            extern MATH_API gsgl::real_t parse(const gsgl::string &);

        } // namespace units

    } // namespace units

} // namespace gsgl

#endif
