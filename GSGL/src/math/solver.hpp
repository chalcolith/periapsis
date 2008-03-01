#ifndef GSGL_MATH_SOLVER_H
#define GSGL_MATH_SOLVER_H

//
// $Id: solver.hpp 314 2008-03-01 16:33:47Z Gordon $
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

    namespace math
    {
        

        /// Base class for numerical equation solvers, where the equation is x' = f(t, x).
        /// State classes need to implement operator+(state) const, operator*(double) const, and derivative(double) const.
        template <typename S>
        class solver
        {
        public:
            virtual S next(const S & x, const double & t, const double & dt) = 0;
        }; // class solver


        /// Euler's method solver.
        template <typename S>
        class euler_solver
            : public solver<S>
        {
        public:
            S next(const S & x, const double & t, const double & dt)
            {
                return x + x.derivative(t) * dt;
            }
        }; // class euler_solver

        
        /// The venerable Runge-Kutta solver.
        template <typename S> 
        class runge_kutta_solver 
            : public solver<S>
        {
        public:
            S next(const S & x, const double & t, const double & dt)
            {
                S k1 = x.derivative(t);
                S k2 = (x + k1*(dt*0.5)).derivative(t + dt*0.5);
                S k3 = (x + k2*(dt*0.5)).derivative(t + dt*0.5);
                S k4 = (x + k3*dt).derivative(t + dt);
                
                return x + (k1 + k2*2.0 + k3*2.0 + k4)*(dt/6.0);
            } // next()
        }; // class runge_kutta_solver
        

    } // namespace math
    
} // namespace math

#endif
