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

#include "space/keplerian_element_propagator.hpp"
#include "data/list.hpp"
#include "math/units.hpp"

#include <cmath>

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;


namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::keplerian_element_propagator);


        keplerian_element_propagator::keplerian_element_propagator(const config_record & obj_config)
            : propagator(obj_config), has_data(false), has_aux(false)
        {
            // clear data
            for (int i = 0; i < 6; ++i)
            {
                elements[i] = 0;
                rates[i] = 0;
            }

            // get elements
            bool has_elements = get_array(obj_config[L"keplerian_elements"], elements, 6);
            bool has_rates = get_array(obj_config[L"keplerian_rates"], rates, 6);
            
            if ((has_data = has_elements && has_rates))
            {
                for (int i = 2; i < 6; ++i)
                {
                    elements[i] *= math::DEG2RAD;
                    rates[i] *= math::DEG2RAD;
                }
            }

            if ((has_aux = get_array(obj_config[L"keplerian_elements_aux"], aux, 4)))
            {
                for (int i = 0; i < 4; ++i)
                    aux[i] *= math::DEG2RAD;
            }
        } // keplerian_element_propagator::keplerian_element_propagator()


        keplerian_element_propagator::~keplerian_element_propagator()
        {
        } // keplerian_element_propagator::~keplerian_element_propagator()


        static double calc_E(const double M, const double e_star, const double e)
        {
            double E_zero = M + e_star * ::sin(M);

            double delta_M;
            double delta_E;
            double E_old;
            double E_new = E_zero;
            double tol = 1.0e-6;

            do
            {
                E_old = E_new;
                delta_M = M - (E_old - e_star * ::sin(E_old));
                delta_E = delta_M / (1.0 - e * ::cos(E_old));
                E_new = E_old + delta_E;
            }
            while (delta_E > tol);

            return E_new;
        } // calc_E()


        void keplerian_element_propagator::update(const double jdn, vector & position, vector & velocity)
        {
            if (!has_data)
                return;

            // calculate current elements
            double cur_elements[6];
            double T = (jdn - 2451545.0) / 36525.0;

            for (int i = 0; i < 6; ++i)
                cur_elements[i] = elements[i] + rates[i] * T;

            // calculate argument of perihelion and mean anomaly
            double omega = cur_elements[4] - cur_elements[5];
            double M = cur_elements[3] - cur_elements[4];

            if (has_aux)
                M += aux[0]*T*T + aux[1]*::cos(aux[3]*T) + aux[2]*::sin(aux[3]*T);

            double M_plus = M + math::PI;
            
            while (M_plus < 0.0)
                M_plus += math::PI * 2.0;
            while (M_plus > math::PI * 2.0)
                M_plus -= math::PI * 2.0;

            M = M_plus - math::PI;

            // calculate mean motion
            double n = (cur_elements[3] - elements[3]) / T;

            // calculate the eccentric anomaly
            double E = calc_E(M, cur_elements[1] * math::DEG2RAD, cur_elements[1]);

            // calculate the derivative of the eccentric anomaly
            double E_dot = n / (1.0f - cur_elements[1] * ::cos(E));

            // compute the planet's coordinates in its orbital plane
            double x_prime = cur_elements[0] * (::cos(E) - cur_elements[1]);
            double y_prime = cur_elements[0] * (::sqrt(1.0f - cur_elements[1]*cur_elements[1]) * ::sin(E));
            double z_prime = 0.0f;

            // compute the planet's velocity in its orbital plane
            double x_dot_prime = cur_elements[0] * ( -::sin(E) * E_dot );
            double y_dot_prime = cur_elements[0] * (::sqrt(1.0f - cur_elements[1]*cur_elements[1]) * ::cos(E) * E_dot);

            // compute the planet's coordinates in the ecliptic plane
            double cos_o = ::cos(omega);
            double cos_O = ::cos(cur_elements[5]);
            double cos_I = ::cos(cur_elements[2]);
            double sin_o = ::sin(omega);
            double sin_O = ::sin(cur_elements[5]);
            double sin_I = ::sin(cur_elements[2]);

            double x = (cos_o*cos_O - sin_o*sin_O*cos_I) * x_prime + (-sin_o*cos_O - cos_o*sin_O*cos_I) * y_prime;
            double y = (cos_o*sin_O + sin_o*cos_O*cos_I) * x_prime + (-sin_o*sin_O + cos_o*cos_O*cos_I) * y_prime;
            double z = (sin_o*sin_I)*x_prime + (cos_o*sin_I)*y_prime;

            // compute the planet's velocity in the ecliptic plane
            double x_dot = (cos_o*cos_O - sin_o*sin_O*cos_I) * x_dot_prime + (-sin_o*cos_O - cos_o*sin_O*cos_I) * y_dot_prime;
            double y_dot = (cos_o*sin_O + sin_o*cos_O*cos_I) * x_dot_prime + (-sin_o*sin_O + cos_o*cos_O*cos_I) * y_dot_prime;
            double z_dot = (sin_o*sin_I)*x_dot_prime + (cos_o*sin_I)*y_dot_prime;

            // convert velocity into units/second
            x_dot /= units::SECONDS_PER_YEAR * 100.0;
            y_dot /= units::SECONDS_PER_YEAR * 100.0;
            z_dot /= units::SECONDS_PER_YEAR * 100.0;

            // set position and velocity
            position.get_x() = static_cast<gsgl::real_t>(x);
            position.get_y() = static_cast<gsgl::real_t>(y);
            position.get_z() = static_cast<gsgl::real_t>(z);
            position.get_w() = 1;

            velocity.get_x() = static_cast<gsgl::real_t>(x_dot);
            velocity.get_y() = static_cast<gsgl::real_t>(y_dot);
            velocity.get_z() = static_cast<gsgl::real_t>(z_dot);
            velocity.get_w() = 1;
        } // keplerian_element_propagator::update()


        bool keplerian_element_propagator::get_array(const string & str, double *a, const int num)
        {
            // get values from string
            list<string> tokens = str.split(L" \t");

            int index = 0;
            for (list<string>::iterator i = tokens.iter(); i.is_valid(); ++i)
            {
                if (!i->is_empty())
                {
                    if (index < num)
                    {
                        a[index++] = i->to_double();
                    }
                    else
                        throw runtime_exception(L"Invalid keplerian elements %ls", str.w_string());
                }
            }

            return index == num;
        } // keplerian_element_propagator::get_array()

    } // namespace space

} // namespace periapsis
