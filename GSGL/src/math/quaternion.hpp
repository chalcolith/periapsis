#ifndef GSGL_MATH_QUATERNION_H
#define GSGL_MATH_QUATERNION_H

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
#include "math/vector.hpp"

namespace gsgl
{

    namespace math
    {

        class transform;

    
        /// Quaternion class.
        class MATH_API quaternion
            : public math_object
        {
        public:
            double w,x,y,z;

            quaternion(const double w = 0, const double x = 0, const double y = 0, const double z = 0);
            quaternion(const vector & v, double a);
            quaternion(const quaternion &);

            explicit quaternion(const gsgl::string &);
            explicit quaternion(const transform &);
            
            quaternion & operator= (const quaternion &);
            quaternion & operator= (const transform &);
            virtual ~quaternion();
            
            double norm() const;
            quaternion conjugate() const;
            quaternion inverse() const;
            void normalize();
            
            quaternion operator+ (const quaternion &) const;
            quaternion operator* (const quaternion &) const;
            quaternion operator* (const double &) const;

            double dot(const quaternion &) const;

            static quaternion interpolate(const quaternion & start, const quaternion & end, const double & t);
        }; // class quaternion
        

    } // namespace math
    
} // namespace gsgl

#endif
