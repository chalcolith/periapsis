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

#include "space/rotating_body.hpp"
#include "space/astronomy.hpp"
#include "data/broker.hpp"
#include "math/quaternion.hpp"
#include "math/units.hpp"

#include <cmath>

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;
using namespace gsgl::physics;

namespace periapsis
{

    namespace space
    {

        void body_rotator::calc_orientation_aux(double alpha, double delta, double W, transform & orientation)
        {
            // convert angles to radians
            alpha *= math::DEG2RAD;
            delta *= math::DEG2RAD;
            W *= math::DEG2RAD;

            // calculate orientation
            gsgl::real_t rot_angle = static_cast<gsgl::real_t>(PI_OVER_2 + alpha + W);
            quaternion rotation(vector::Z_AXIS, rot_angle);

            gsgl::real_t inc_angle = static_cast<gsgl::real_t>( PI_OVER_2 - delta );

            double axis_angle = PI_OVER_2 + alpha;
            gsgl::real_t axis_x = static_cast<gsgl::real_t>( ::cos(axis_angle) );
            gsgl::real_t axis_y = static_cast<gsgl::real_t>( ::sin(axis_angle) );

            quaternion inclination(vector(axis_x, axis_y, 0), inc_angle);
            quaternion oq = inclination * rotation;

            orientation = EQUATORIAL_WRT_ECLIPTIC * transform(oq);
        } // body_rotator::calc_orientation_aux()


        void body_rotator::calc_angular_velocity_aux(double ang_diff, double d, const transform & orientation, vector & angular_velocity)
        {
            // ang_diff is degrees since J2000
            // d is days since J2000
            // so ang_rate = ang_diff / d is degrees/day
            double mag = (ang_diff / d) * (math::DEG2RAD / math::units::SECONDS_PER_DAY); // radians / second
            vector axis = orientation * vector::Z_AXIS;

            angular_velocity = axis * static_cast<gsgl::real_t>(mag);
        } // body_rotator::calc_angular_velocity_aux()


        //////////////////////////////////////////////////////////////

        struct major_planet_rec
        {
            wchar_t *name;
            double alpha_zero, alpha_rate;
            double delta_zero, delta_rate;
            double w_zero, w_rate, w_sin_term;
            double n_zero, n_rate;
        }; // struct major_planet_rec

        static major_planet_rec major_planet_data[] = 
        {
            { L"Sol",           286.13,       0.0,       63.87,       0.0,       84.10,      14.1844000,      0.0,     0.0,   0.0    },
            { L"Mercury",       281.01,      -0.033,     61.45,      -0.005,    329.548,      6.1385025,      0.0,     0.0,   0.0    },
            { L"Venus",         272.76,       0.0,       67.16,       0.0,      160.20,      -1.4813688,      0.0,     0.0,   0.0    },
            { L"Earth",           0.00,      -0.641,     90.00,      -0.557,    190.147,    360.9856235,      0.0,     0.0,   0.0    },
            { L"Mars",          317.68143,   -0.1061,    52.88650,   -0.0609,   176.630,    350.89198226,     0.0,     0.0,   0.0    },
            { L"Jupiter",       268.05,      -0.009,     64.49,       0.003,    284.95,     870.5366420,      0.0,     0.0,   0.0    },
            { L"Saturn",        40.589,      -0.036,     83.537,     -0.004,     38.90,     810.7939024,      0.0,     0.0,   0.0    },
            { L"Uranus",       257.311,       0.0,      -15.175,      0.0,      203.81,    -501.1600928,      0.0,     0.0,   0.0    },
            { L"Neptune",      299.36,        0.70,      43.46,      -0.51,     253.18,     536.3128492,     -0.48,  357.85, 52.316  },
            { L"Pluto",        313.02,        0.0,        9.09,       0.0,      236.77,     -56.3623195,      0.0,     0.0,   0.0    },

            { 0,                 0.0,         0.0,        0.0,        0.0,        0.0,        0.0,            0.0,     0.0,   0.0    }

        };


        class major_planet_rotator 
            : public body_rotator
        {
            const major_planet_rec *data;
        public:
            major_planet_rotator(const major_planet_rec *data) : body_rotator(), data(data) {}

            virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity);
        }; // class major_planet_rotator


        void major_planet_rotator::calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
        {
            double d = jdn - J2000;
            double T = (d / 36525.0);

            double alpha, delta, W, ang_diff;

            if (data->n_zero != 0.0 || data->n_rate != 0.0)
            {
                double N = data->n_zero + data->n_rate * T;
                N *= math::DEG2RAD;

                alpha = data->alpha_zero + data->alpha_rate * ::sin(N);
                delta = data->delta_zero + data->delta_rate * ::cos(N);
                W = data->w_zero + (ang_diff = data->w_rate * d + data->w_sin_term * ::sin(N));
            }
            else
            {
                alpha = data->alpha_zero + data->alpha_rate * T;
                delta = data->delta_zero + data->delta_rate * T;
                W = data->w_zero + (ang_diff = data->w_rate * d);
            }

            calc_orientation_aux(alpha, delta, W, orientation);
            calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
        } // major_planet_rotator::calc_orientation()


        //////////////////////////////////////////////////////////////


        namespace rotator
        {

            namespace sol
            {

                class sol : public major_planet_rotator
                {
                public:
                    sol(const config_record &) : major_planet_rotator(&major_planet_data[0]) {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::sol::sol);
                }; // class sol

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::sol::sol);

            } // namespace sol


            namespace mercury
            {

                class mercury : public major_planet_rotator
                {
                public:
                    mercury(const config_record &) : major_planet_rotator(&major_planet_data[1]) {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::mercury::mercury);
                }; // class mercury

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::mercury::mercury);

            } // namespace mercury


            namespace venus
            {

                class venus : public major_planet_rotator
                {
                public:
                    venus(const config_record &) : major_planet_rotator(&major_planet_data[2]) {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::venus::venus);
                }; // class venus

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::venus::venus);

            } // namespace venus


            namespace earth
            {

                class earth : public major_planet_rotator
                {
                public:
                    earth(const config_record &) : major_planet_rotator(&major_planet_data[3]) {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::earth::earth);
                }; // class earth

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::earth::earth);


                class moon : public body_rotator
                {
                public:
                    moon(const config_record &) : body_rotator() {}

                    virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
                    {
                        double d = jdn - J2000;
                        double T = (d / 36525.0);

                        double alpha, delta, W, ang_diff;

                        double E1  = 125.045 -  0.0529921*d;  E1 *= math::DEG2RAD;
                        double E2  = 250.089 -  0.1059842*d;  E2 *= math::DEG2RAD;
                        double E3  = 260.008 + 13.0120009*d;  E3 *= math::DEG2RAD;
                        double E4  = 176.625 + 13.3407154*d;  E4 *= math::DEG2RAD;
                        double E5  = 357.529 +  0.9856003*d;  E5 *= math::DEG2RAD;
                        double E6  = 311.589 + 26.4057084*d;  E6 *= math::DEG2RAD;
                        double E7  = 134.963 + 13.0649930*d;  E7 *= math::DEG2RAD;
                        double E8  = 276.617 +  0.3287146*d;  E8 *= math::DEG2RAD;
                        double E9  =  34.226 +  1.7484877*d;  E9 *= math::DEG2RAD;
                        double E10 =  15.134 -  0.1589763*d; E10 *= math::DEG2RAD;
                        double E11 = 119.743 +  0.0036096*d; E11 *= math::DEG2RAD;
                        double E12 = 239.961 +  0.1643573*d; E12 *= math::DEG2RAD;
                        double E13 =  25.053 + 12.9590088*d; E13 *= math::DEG2RAD;

                        alpha = 269.9949 + 0.0031*T          - 3.8787*::sin(E1)  - 0.1204*::sin(E2)
                                                             + 0.0700*::sin(E3)  - 0.0172*::sin(E4)  + 0.0072*::sin(E6)
                                                             - 0.0052*::sin(E10) + 0.0043*::sin(E13);

                        delta =  66.5392 + 0.0130*T          + 1.5419*::cos(E1)  + 0.0239*::cos(E2)
                                                             - 0.0278*::cos(E3)  + 0.0068*::cos(E4)  - 0.0029*::cos(E6)
                                                             + 0.0009*::cos(E7)  + 0.0008*::cos(E10) - 0.0009*::cos(E13);

                        W =      38.3213 + (ang_diff = 13.17635815*d     -  1.4e-12*d*d       + 3.5610*::sin(E1)
                                                             +  0.1208*::sin(E2)  - 0.0642*::sin(E3)  + 0.0158*::sin(E4)
                                                             +  0.0252*::sin(E5)  - 0.0066*::sin(E6)  - 0.0047*::sin(E7)
                                                             -  0.0046*::sin(E8)  + 0.0028*::sin(E9)  + 0.0052*::sin(E10)
                                                             +  0.0040*::sin(E11) + 0.0019*::sin(E12) - 0.0044*::sin(E13));

                        calc_orientation_aux(alpha, delta, W, orientation);
                        calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
                    } // calc_orientation()

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::earth::moon);
                }; // class moon

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::earth::moon);

            } // namespace earth


            namespace mars
            {

                class mars : public major_planet_rotator
                {
                public:
                    mars(const config_record &) : major_planet_rotator(&major_planet_data[4]) {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::mars::mars);
                }; // class mars

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::mars::mars);

                
                //

                class mars_rotator : public body_rotator
                {
                public:
                    mars_rotator() : body_rotator() {}

                protected:
                    double d, T;
                    double M1, M2, M3;
                    double alpha, delta, W;

                    void calc_orientation_pre(double jdn)
                    {
                        d = jdn - J2000;
                        T = d / 36525.0;

                        M1 = 169.51 -    0.4357640*d;             M1 *= math::DEG2RAD;
                        M2 = 192.93 + 1128.4096700*d + 8.864*T*T; M2 *= math::DEG2RAD;
                        M3 =  53.47 -    0.0181510*d;             M3 *= math::DEG2RAD;
                    } // calc_orientation_pre()
                }; // class mars_rotator


                class phobos : public mars_rotator
                {
                public:
                    phobos(const config_record &) : mars_rotator() {}

                    virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
                    {
                        double ang_diff;
                        calc_orientation_pre(jdn);

                        alpha = 317.68 -    0.108*T        + 1.79*::sin(M1);
                        delta = 52.90  -    0.061*T        - 1.08*::cos(M1);
                        W     = 35.06  + (ang_diff = 1128.8445850*d    + 8.864*T*T       - 1.42*::sin(M1) - 0.78*::sin(M2));

                        calc_orientation_aux(alpha, delta, W, orientation);
                        calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
                    } // calc_orientation()

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::mars::phobos);
                }; // class phobos

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::mars::phobos);


                class deimos : public mars_rotator
                {
                public:
                    deimos(const config_record &) : mars_rotator() {}

                    virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
                    {
                        double ang_diff;
                        calc_orientation_pre(jdn);

                        alpha = 316.65 -   0.108*T        + 2.98*::sin(M3);
                        delta =  53.52 -   0.061*T        - 1.78*::cos(M3);
                        W     =  79.41 + (ang_diff = 285.1618970*d    - 0.520*T*T       - 2.58*::sin(M3) + 0.19*::cos(M3));

                        calc_orientation_aux(alpha, delta, W, orientation);
                        calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
                    } // calc_orientation()

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::mars::deimos);
                }; // class deimos_rotator

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::mars::deimos);

            } // namespace mars


            namespace jupiter
            {

                class jupiter : public major_planet_rotator
                {
                public:
                    jupiter(const config_record &) : major_planet_rotator(&major_planet_data[5]) {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::jupiter::jupiter);
                }; // class jupiter

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::jupiter::jupiter);

                //

                class jupiter_rotator : public body_rotator
                {
                public:
                    jupiter_rotator() : body_rotator() {}

                protected:
                    double d, T, alpha, delta, W;
                    double J1, J2, J3, J4, J5, J6, J7, J8;

                    void calc_orientation_pre(double jdn)
                    {
                        d = jdn - J2000;
                        T = d / 36525.0;

                        J1 =  73.32 + 91472.9 * T; J1 *= math::DEG2RAD;
                        J2 =  24.62 + 45137.2 * T; J2 *= math::DEG2RAD;
                        J3 = 283.90 +  4850.7 * T; J3 *= math::DEG2RAD;
                        J4 = 355.80 +  1191.3 * T; J4 *= math::DEG2RAD;
                        J5 = 119.90 +   262.1 * T; J5 *= math::DEG2RAD;
                        J6 = 229.80 +    64.3 * T; J6 *= math::DEG2RAD;
                        J7 = 352.35 +  2382.6 * T; J7 *= math::DEG2RAD;
                        J8 = 113.35 +  6070.0 * T; J8 *= math::DEG2RAD;
                    } // calc_orientation_pre()
                }; // class jupiter_rotator


                class io : public jupiter_rotator
                {
                public:
                    io(const config_record &) : jupiter_rotator() {}

                    virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
                    {
                        double ang_diff;
                        calc_orientation_pre(jdn);

                        alpha = 268.05 -   0.009*T     + 0.094*::sin(J3) + 0.024*::sin(J4);
                        delta =  64.50 +   0.003*T     + 0.040*::cos(J3) + 0.011*::cos(J4);
                        W     = 200.39 + (ang_diff = 203.4889538*d - 0.085*::sin(J3) - 0.022*::sin(J4));

                        calc_orientation_aux(alpha, delta, W, orientation);
                        calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
                    } // calc_orientation()

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::jupiter::io);
                }; // class io

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::jupiter::io);


                class europa : public jupiter_rotator
                {
                public:
                    europa(const config_record &) : jupiter_rotator() {}

                    virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
                    {
                        double ang_diff;
                        calc_orientation_pre(jdn);

                        alpha = 268.08  -   0.009*T     + 1.086*::sin(J4) + 0.060*::sin(J5) + 0.015*::sin(J6) + 0.009*::sin(J7);
                        delta =  64.51  +   0.003*T     + 0.468*::cos(J4) + 0.026*::cos(J5) + 0.007*::cos(J6) + 0.002*::cos(J7);
                        W     =  36.022 + (ang_diff = 101.3747235*d - 0.980*::sin(J4) - 0.054*::sin(J5) - 0.014*::sin(J6) - 0.008*::sin(J7));

                        calc_orientation_aux(alpha, delta, W, orientation);
                        calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
                    } // calc_orientation()

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::jupiter::europa);
                }; // class europa

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::jupiter::europa);


                class ganymede : public jupiter_rotator
                {
                public:
                    ganymede(const config_record &) : jupiter_rotator() {}

                    virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
                    {
                        double ang_diff;
                        calc_orientation_pre(jdn);

                        alpha = 268.20  -  0.009*T     - 0.037*::sin(J4) + 0.431*::sin(J5) + 0.091*::sin(J6);
                        delta =  64.57  +  0.003*T     - 0.016*::cos(J4) + 0.186*::cos(J5) + 0.039*::cos(J6);
                        W     =  44.064 + (ang_diff = 50.3176081*d + 0.033*::sin(J4) - 0.389*::sin(J5) - 0.082*::sin(J6));

                        calc_orientation_aux(alpha, delta, W, orientation);
                        calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
                    } // calc_orientation()


                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::jupiter::ganymede);
                }; // class ganymede

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::jupiter::ganymede);


                class callisto : public jupiter_rotator
                {
                public:
                    callisto(const config_record &) : jupiter_rotator() {}

                    virtual void calc_orientation(double jdn, transform & orientation, vector & angular_velocity)
                    {
                        double ang_diff;
                        calc_orientation_pre(jdn);

                        alpha = 268.72 -  0.009*T     - 0.068*::sin(J5) + 0.590*::sin(J6) + 0.010*::sin(J8);
                        delta =  64.83 +  0.003*T     - 0.029*::cos(J5) + 0.254*::cos(J6) - 0.004*::cos(J8);
                        W     = 259.51 + (ang_diff = 21.5710715*d + 0.061*::sin(J5) - 0.533*::sin(J6) - 0.009*::sin(J8));

                        calc_orientation_aux(alpha, delta, W, orientation);
                        calc_angular_velocity_aux(ang_diff, d, orientation, angular_velocity);
                    } // calc_orientation()

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::jupiter::callisto);
                }; // class callisto

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::jupiter::callisto);

            } // namespace jupiter


            namespace saturn
            {

                class saturn : public major_planet_rotator
                {
                public:
                    saturn(const config_record &)
                        : major_planet_rotator(&major_planet_data[6])
                    {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::saturn::saturn);
                }; // class saturn

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::saturn::saturn);

            } // namespace saturn


            namespace uranus
            {

                class uranus : public major_planet_rotator
                {
                public:
                    uranus(const config_record &)
                        : major_planet_rotator(&major_planet_data[7])
                    {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::uranus::uranus);
                }; // class uranus

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::uranus::uranus);
            }


            namespace neptune
            {

                class neptune : public major_planet_rotator
                {
                public:
                    neptune(const config_record &)
                        : major_planet_rotator(&major_planet_data[8])
                    {}

                    BROKER_DECLARE_CREATOR(periapsis::space::rotator::neptune::neptune);
                }; // class neptune

                BROKER_DEFINE_CREATOR(periapsis::space::rotator::neptune::neptune);
            }


            namespace pluto
            {
            }


        } // namespace rotator


        //////////////////////////////////////////////////////////////

        rotating_body::rotating_body(const gsgl::string & name, gsgl::scenegraph::node *parent, body_rotator *rotator)
            : physics_frame(name, parent), rotator(rotator)
        {
        } // rotating_body::rotating_body()


        rotating_body::~rotating_body()
        {
            delete rotator;
        } // rotating_body::rotating_body()


        //


        void rotating_body::init(context *c)
        {
            if (rotator)
                rotator->calc_orientation(c->julian_cur, get_orientation(), get_angular_velocity());

            assert(get_linear_velocity().mag() == 0);
        } // rotating_body::init()


        void rotating_body::update(context *c)
        {
            if (rotator)
                rotator->calc_orientation(c->julian_cur, get_orientation(), get_angular_velocity());

            assert(get_linear_velocity().mag() == 0);
        } // update()


    } // namespace space

} // namespace periapsis
