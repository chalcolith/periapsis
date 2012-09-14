//
// $Id: units.cpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "stdafx.h"
#include "units.hpp"
#include "base/string.hpp"

namespace gsgl
{

    namespace math
    {

        namespace units
        {

            const gsgl::real_t METERS_PER_MILLIMETER  = (gsgl::real_t) 0.001;
            const gsgl::real_t METERS_PER_CENTIMETER  = (gsgl::real_t) 0.01;
            const gsgl::real_t METERS_PER_METER       = (gsgl::real_t) 1.0;
            const gsgl::real_t METERS_PER_KILOMETER   = (gsgl::real_t) 1000.0;
            const gsgl::real_t METERS_PER_AU          = (gsgl::real_t) 149597870691.0;
            const gsgl::real_t METERS_PER_PARSEC      = (gsgl::real_t) 3.08567758e16;

            const gsgl::real_t KILOGRAMS_PER_MILLIGRAM   = (gsgl::real_t) 0.000001;
            const gsgl::real_t KILOGRAMS_PER_GRAM        = (gsgl::real_t) 0.001;
            const gsgl::real_t KILOGRAMS_PER_KILOGRAM    = (gsgl::real_t) 1.0;

            const gsgl::real_t SECONDS_PER_MILLISECOND = (gsgl::real_t) 0.001;
            const gsgl::real_t SECONDS_PER_SECOND      = (gsgl::real_t) 1.0;
            const gsgl::real_t SECONDS_PER_MINUTE      = (gsgl::real_t) 60.0;
            const gsgl::real_t SECONDS_PER_HOUR        = (gsgl::real_t) 3600.0;
            const gsgl::real_t SECONDS_PER_DAY         = (gsgl::real_t) 86400.0;
            const gsgl::real_t SECONDS_PER_YEAR        = (gsgl::real_t) 31557600.0;



            static gsgl::real_t get_numeric_part(const string & str, int start, int & next)
            {
                int len = str.size();

                gsgl::real_t sign = 1;
                gsgl::real_t res = 0;

                int index = start;

                // strip whitespace
                while (index < len && ::iswspace(str[index]))
                    ++index;

                // get sign
                if (index < len && str[index] == L'-')
                {
                    sign = -1;
                    ++index;
                }
                else if (index < len && str[index] == L'+')
                {
                    sign = 1;
                    ++index;
                }

                // get whole part
                while (index < len && str[index] >= L'0' && str[index] <= L'9')
                {
                    res = (res * 10) + (str[index] - L'0');
                    ++index;
                }

                // get decimal part
                if (index < len && str[index] == L'.')
                {
                    ++index;
                    gsgl::real_t frac = 0;
                    gsgl::real_t factor = 0.1f;

                    while (index < len && str[index] >= L'0' && str[index] <= L'9')
                    {
                        frac = frac + (factor * (str[index] - L'0'));

                        factor /= 10;
                        ++index;
                    }

                    res += frac;
                }

                // get exponent
                if (index < len && str[index] == L'e' || str[index] == L'E')
                {
                    gsgl::real_t exp_sign = 1;
                    gsgl::real_t exp = 0;

                    if (len > index+2)
                    {
                        // sign of the exponent
                        if (str[index+1] == L'-')
                        {
                            exp_sign = -1;
                            ++index;
                        }
                        else if (str[index+1] == L'+')
                        {
                            exp_sign = 1;
                            ++index;
                        }

                        // number of the exponent
                        if (index < len && str[index+1] >= L'0' && str[index+1] <= L'9')
                        {
                            ++index;

                            while (index < len && str[index] >= L'0' && str[index] <= L'9')
                            {
                                exp = (exp * 10) + (str[index] - L'0');
                                ++index;
                            }
                        }
                    }

                    res = res * ::pow(10.0f, exp * exp_sign);
                }

                // we're done
                next = index;
                return res * sign;
            } // get_numeric_part()


            static string get_units_part(const string & str, int start)
            {
                string res = str.substring(start);
                return res.trim().make_lower();
            } // get_units_part()


            gsgl::real_t parse(const string & str)
            {
                gsgl::real_t res = 0;

                // get numeric part
                int index = 0;
                res = get_numeric_part(str, 0, index);

                // get units string
                string uname = get_units_part(str, index);

                if (uname == L"mm")
                {
                    res *= METERS_PER_MILLIMETER;
                }
                else if (uname == L"cm")
                {
                    res *= METERS_PER_CENTIMETER;
                }
                else if (uname == L"m")
                {
                    res *= METERS_PER_METER;
                }
                else if (uname == L"km")
                {
                    res *= METERS_PER_KILOMETER;
                }
                else if (uname == L"au")
                {
                    res *= METERS_PER_AU;
                }
                else if (uname == L"parsec")
                {
                    res *= METERS_PER_PARSEC;
                }
                else if (uname == L"mg")
                {
                    res *= KILOGRAMS_PER_MILLIGRAM;
                }
                else if (uname == L"g")
                {
                    res *= KILOGRAMS_PER_GRAM;
                }
                else if (uname == L"kg")
                {
                    res *= KILOGRAMS_PER_KILOGRAM;
                }
                else if (uname == L"ms")
                {
                    res *= SECONDS_PER_MILLISECOND;
                }
                else if (uname == L"s")
                {
                    res *= SECONDS_PER_SECOND;
                }
                else if (uname == L"min")
                {
                    res *= SECONDS_PER_MINUTE;
                }
                else if (uname == L"h")
                {
                    res *= SECONDS_PER_HOUR;
                }
                else if (uname == L"day")
                {
                    res *= SECONDS_PER_DAY;
                }
                else if (uname == L"year")
                {
                    res *= SECONDS_PER_YEAR;
                }       

                return res;
            } // parse()

        } // namespace units

    } // namespace math

} // namespace gsgl
