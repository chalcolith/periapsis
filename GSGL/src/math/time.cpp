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

#include "math/time.hpp"
#include "data/string.hpp"
#include "data/exception.hpp"

#include <cmath>
#include <cwchar>

namespace gsgl
{

    namespace math
    {
        const double julian_day::J1970 = 2440587.5; ///< Julian day number at UNIX epoch, 1970-01-01
        const double julian_day::JDAY  = 86400.0;   ///< Number of seconds in a Julian day.
        

        julian_day::julian_day()
            : math_object(), jdn(0)
        {
        } // julian_day::julian_day()


        julian_day::julian_day(const double & jdn)
            : math_object(), jdn(jdn)
        {
        } // julian_day::julian_day()


        julian_day::julian_day(const time_t & t)
            : math_object(), jdn( J1970 + static_cast<double>(t) / JDAY )
        {
        } // julian_day::julian_day()


        time_t julian_day::to_time_t() const
        {
            return static_cast<time_t>( (jdn - J1970) * JDAY );
        } // julian_day::to_time_t()


        //

        struct info
        {
            int year, month, day;
            int hour, minute, second;
        }; // struct info

        static void jdn2info(const double jdn, info & i)
        {
            double jdd = jdn + 0.5;
            double Z;
            double F = ::modf(jdd, &Z);

            double A;

            if (Z < 2299161.0)
            {
                A = Z;
            }
            else
            {
                double a = ::floor( (Z - 1867216.25) / 36524.25 );
                A = Z + 1.0 + a - ::floor(a/4.0);
            }

            double B = A + 1524.0;
            double C = ::floor( (B - 122.1) / 365.25 );
            double D = ::floor( 365.25 * C );
            double E = ::floor( (B - D) / 30.6001 );

            double day = B - D - ::floor(30.6001 * E) + F;
            double month = (E < 14.0) ? E - 1.0 : E - 13.0;
            double year = (month > 2.0) ? C - 4716.0 : C - 4715;

            double day_i;
            double day_f = ::modf(day, &day_i);
            
            i.year = static_cast<int>(year);
            i.month = static_cast<int>(month);
            i.day = static_cast<int>(day_i);

            double hours = day_f * 24.0;
            double hours_i;
            double hours_f = ::modf(hours, &hours_i);

            double minutes = hours_f * 60.0;
            double minutes_i;
            double minutes_f = ::modf(minutes, &minutes_i);

            double seconds = minutes_f * 60.0;

            i.hour = static_cast<int>(hours_i);
            i.minute = static_cast<int>(minutes_i);
            i.second = static_cast<int>(::floor(seconds));
        } // jdn2info()

        static double info2jdn(const info & i)
        {
            double t = static_cast<double>(i.hour) * 3600.0 
                + static_cast<double>(i.minute) * 60.0 
                + static_cast<double>(i.second);
            double Y = static_cast<double>(i.year);
            double M = static_cast<double>(i.month);
            double D = static_cast<double>(i.day) + t/julian_day::JDAY;

            if (M <= 2.0)
            {
                Y -= 1.0;
                M += 12.0;
            }

            double A = ::floor(Y/100.0);
            double B = 2.0 - A + ::floor(A/4.0);

            double jdn = ::floor(365.25 * (Y + 4716.0)) + ::floor(30.6001 * (M + 1.0)) + D + B - 1524.5;

            return jdn;
        } // info2jdn()


        string julian_day::to_gregorian_string() const
        {
            info i;
            jdn2info(jdn, i);

            return string::format(L"%04d-%02d-%02d %02d:%02d:%02d", i.year, i.month, i.day, i.hour, i.minute, i.second);
        } // julian_day::to_string()


        void julian_day::from_gregorian_string(const string & s)
        {
            info i;

            int num = ::swscanf(s.w_string(), L"%d-%d-%d %d:%d:%d", &i.year, &i.month, &i.day, &i.hour, &i.minute, &i.second);

            if (num != 6)
                throw runtime_exception(L"Invalid datetime string.");

            if (i.month < 1 || i.month > 12)
                throw runtime_exception(L"Invalid month value %d in datetime string.", i.month);

            if (i.day < 1 || i.day > 31)
                throw runtime_exception(L"Invalid day value %d in datetime string.", i.day);

            if (i.hour < 0 || i.hour > 23)
                throw runtime_exception(L"Invalid hour value %d in datetime string.", i.hour);

            if (i.minute < 0 || i.minute > 59)
                throw runtime_exception(L"Invalid minute value %d in datetime string.", i.minute);

            if (i.second < 0 || i.second > 59)
                throw runtime_exception(L"Invalid second value %d in datetime string.", i.second);

            jdn = info2jdn(i);
        } // julian_day::from_string()


        julian_day julian_day::now()
        {
            return julian_day(::time(0));
        } // julian_day::now()

    } // namespace math

} // namespace gsgl
