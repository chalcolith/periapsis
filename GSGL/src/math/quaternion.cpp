//
// $Id: quaternion.cpp 314 2008-03-01 16:33:47Z Gordon $
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

#include "math/quaternion.hpp"
#include "math/transform.hpp"

#include <cmath>

namespace gsgl
{

    namespace math
    {
    
        /// Creates a quaternion w + xi + yj + zk.
        quaternion::quaternion(const double w, const double x, const double y, const double z) 
            : math_object(), w(w), x(x), y(y), z(z)
        {
        } // quaternion::quaternion()
        

        /// Creates a quaternion representing a rotation around the given axis of \em a radians.
        quaternion::quaternion(const vector & v, double a)
            : math_object()
        {
            w = ::cos(a/2.0);
            x = v.get_x() * ::sin(a/2.0);
            y = v.get_y() * ::sin(a/2.0);
            z = v.get_z() * ::sin(a/2.0);

            normalize();
        } // quaternion::quaternion()
        

        /// Copy constructor; creates a copy of the given quaternion.
        quaternion::quaternion(const quaternion & q)
            : math_object()
        {
            x = q.x;
            y = q.y;
            z = q.z;
            w = q.w;
        } // quaternion::quaternion()


        /// Intialize the quaternion from a string of numbers "x y z w"; the resulting quaternion is w + xi + yj + zk.
        quaternion::quaternion(const gsgl::string & s)
            : math_object()
        {
            vector v(s);

            x = v.get_x();
            y = v.get_y();
            z = v.get_z();
            w = v.get_w();
        } // quaternion::quaternion()


        /// Creates a quaternion from a rotation matrix (ignores any translation component in the transform).
        quaternion::quaternion(const transform & t)
            : math_object()
        {
            *this = t;
        } // quaternion::quaternion()


        /// Sets the quaternion from a rotation matrix (ignores any translation component in the transform).
        quaternion & quaternion::operator= (const transform & t)
        {
            const gsgl::real_t *data = t.ptr();

            gsgl::real_t tr, s;
            
            tr = data[0] + data[5] + data[10];

            if (tr >= 0)
            {
                s = ::sqrt(tr + 1.0f);
                w = s * 0.5f;
                s = 0.5f / s;
                x = (data[9] - data[6]) * s;
                y = (data[2] - data[9]) * s;
                z = (data[4] - data[1]) * s;
            }
            else
            {
                int i = 0;
                
                if (data[5] > data[0])
                    i = 1;

                if (i == 0)
                {
                    if (data[10] > data[0])
                        i = 2;
                }
                else
                {
                    if (data[10] > data[5])
                        i = 2;
                }

                switch (i)
                {
                case 0:
                    s = ::sqrt( (data[0] - (data[5] + data[10])) + 1.0f );
                    x = s * 0.5f;
                    s = 0.5f / s;
                    y = (data[1] + data[4]) * s;
                    z = (data[8] + data[2]) * s;
                    w = (data[0] - data[6]) * s;
                    break;
                case 1:
                    s = ::sqrt( (data[5] - (data[10] + data[0])) + 1.0f );
                    y = s * 0.5f;
                    s = 0.5f / s;
                    z = (data[6] + data[9]) * s;
                    x = (data[1] + data[4]) * s;
                    w = (data[2] - data[8]) * s;
                    break;
                case 2:
                    s = ::sqrt( (data[10] - (data[0] + data[5])) + 1.0f );
                    z = s * 0.5f;
                    s = 0.5f / s;
                    x = (data[8] + data[2]) * s;
                    y = (data[6] + data[9]) * s;
                    w = (data[4] - data[1]) * s;
                    break;
                }
            }

            normalize();

            return *this;
        } // quaternion::quaternion()

        
        /// Assignment operator; sets the quaternion equal to the given quaternion.
        quaternion & quaternion::operator= (const quaternion & q)
        {
            x = q.x;
            y = q.y;
            z = q.z;
            w = q.w;
            return *this;
        } // quaternion::operator= ()

        
        quaternion::~quaternion()
        {
        } // quaternion::~quaternion()
        

        //        

        /// Returns the norm of the quaterion: sqrt(w*w + x*x + y*y + z*z).
        double quaternion::norm() const
        {
            return ::sqrt(w*w + x*x + y*y + z*z);
        } // quaternion::norm()


        /// Returns the conjugate of the quaternion: w - xi - yj - zk.
        quaternion quaternion::conjugate() const
        {
            return quaternion(w, -x, -y, -z);
        } // quaternion::conjugate()
        

        /// Returns the inverse of the quaternion: inverse / norm.
        quaternion quaternion::inverse() const
        {
            double n = norm();

            if (n != 0)
                return quaternion(w/n, -x/n, -y/n, -z/n);
            else
                throw runtime_exception(L"Division by zero finding a quaternion inverse.");
        } // quaternion::inverse()
        

        /// Normalizes the quaternion; sets w*w + x*x + y*y + z*z == 1.
        void quaternion::normalize()
        {
            double m = norm();

            if (m != 0)
            {
                w /= m;
                x /= m;
                y /= m;
                z /= m;
            }
            else
            {
                throw runtime_exception(L"Division by zero finding a quaternion normal.");
            }
        } // quaternion::normalize()

        
        //
        
        quaternion quaternion::operator+ (const quaternion & q) const
        {
            return quaternion(w+q.w, x+q.x, y+q.y, z+q.z);
        } // quaternion::operator+ ()
        

        quaternion quaternion::operator* (const quaternion & q) const
        {
            return quaternion(w*q.w - x*q.x - y*q.y - z*q.z,
                              w*q.x + x*q.w + y*q.z - z*q.y,
                              w*q.y - x*q.z + y*q.w + z*q.x,
                              w*q.z + x*q.y - y*q.x + z*q.w);
        } // quaternion::operator* ()
        

        quaternion quaternion::operator* (const double & n) const
        {
            return quaternion(w*n, x*n, y*n, z*n);
        } // quaterion::operator* ()


        double quaternion::dot(const quaternion & q) const
        {
            return w*q.w + x*q.x + y*q.y + z*q.z;
        } // quaternion::dot()


        quaternion quaternion::interpolate(const quaternion & start, const quaternion & end, const double & percent)
        {
            quaternion result;

            double theta = ::acos(start.dot(end));
            result = ( (start * ::sin((1.0f - percent) * theta)) + (end * ::sin(percent * theta)) ) * (1.0 / ::sin(theta));
            result.normalize();

            return result;
        } // quaternion::interpolate()


    } // namespace math
    
} // namespace math
