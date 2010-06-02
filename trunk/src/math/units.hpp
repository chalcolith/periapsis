#ifndef GSGL_MATH_UNITS_H
#define GSGL_MATH_UNITS_H

//
// $Id: units.hpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "math.hpp"

namespace gsgl
{

    class string;

    namespace math
    {

        namespace units
        {

            extern MATH_API const gsgl::real_t METERS_PER_MILLIMETER; 
            extern MATH_API const gsgl::real_t METERS_PER_CENTIMETER; 
            extern MATH_API const gsgl::real_t METERS_PER_METER;      
            extern MATH_API const gsgl::real_t METERS_PER_KILOMETER;  
            extern MATH_API const gsgl::real_t METERS_PER_AU;         
            extern MATH_API const gsgl::real_t METERS_PER_PARSEC;     

            extern MATH_API const gsgl::real_t KILOGRAMS_PER_MILLIGRAM; 
            extern MATH_API const gsgl::real_t KILOGRAMS_PER_GRAM;      
            extern MATH_API const gsgl::real_t KILOGRAMS_PER_KILOGRAM;  

            extern MATH_API const gsgl::real_t SECONDS_PER_MILLISECOND;
            extern MATH_API const gsgl::real_t SECONDS_PER_SECOND;     
            extern MATH_API const gsgl::real_t SECONDS_PER_MINUTE;     
            extern MATH_API const gsgl::real_t SECONDS_PER_HOUR;       
            extern MATH_API const gsgl::real_t SECONDS_PER_DAY;        
            extern MATH_API const gsgl::real_t SECONDS_PER_YEAR;       

            extern MATH_API gsgl::real_t parse(const gsgl::string &);

        } // namespace units

    } // namespace units

} // namespace gsgl

#endif
