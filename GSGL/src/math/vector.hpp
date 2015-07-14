#ifndef GSGL_math_VECTOR4_H
#define GSGL_math_VECTOR4_H

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
#include "math/matrix.hpp"

namespace gsgl
{

    class string;
    
    namespace math
    {
    
        /// A vector for storing homogenous coordinates.
        ///
        /// In this implementation the homogenous coordinate is always 1.0, 
        /// so the first three values may be used for 3-d coordinates.

        class MATH_API vector 
            : public matrix<4,1,gsgl::real_t>
        {
        public:
            vector(gsgl::real_t x = 0, gsgl::real_t y = 0, gsgl::real_t z = 0);
            vector(const vector &);
            vector(const matrix<4,1,gsgl::real_t> &);

            explicit vector(const gsgl::real_t *);
            explicit vector(const gsgl::string &);
            
            vector & operator= (const vector &);
            vector & operator= (const matrix<4,1,gsgl::real_t> &);
            virtual ~vector();
                        
            inline const gsgl::real_t & get_x() const { return data[0]; }
            inline const gsgl::real_t & get_y() const { return data[1]; }
            inline const gsgl::real_t & get_z() const { return data[2]; }
            inline const gsgl::real_t & get_w() const { return data[3]; }

            inline gsgl::real_t & get_x() { return data[0]; }
            inline gsgl::real_t & get_y() { return data[1]; }
            inline gsgl::real_t & get_z() { return data[2]; }
            inline gsgl::real_t & get_w() { return data[3]; }

            /// Normalize the 3d vector (make its length equal to 1).
            void normalize();
            
            /// Makes sure the homogenous coordinate is 1.0 (does not make the length equal to 1).
            void normalize_h();
            
            gsgl::real_t mag() const;  ///< Returns the magnitide of the vector.
            gsgl::real_t mag2() const; ///< Returns the magnitude squared of the vector.
            
            vector & operator+ () { return *this; }
            vector operator- () const;
            vector operator+ (const vector &) const;
            vector operator- (const vector &) const;

            vector & operator+= (const vector &);
            vector & operator-= (const vector &);

            vector & operator*= (const gsgl::real_t);
            vector operator* (const gsgl::real_t);
            
            gsgl::real_t dot(const vector &) const;
            vector cross(const vector &) const;
            
            static vector interpolate(const vector & start, const vector & end, const gsgl::real_t & percent);

            static vector parse(const gsgl::string &);

            static const vector ZERO;
            static const vector X_AXIS;
            static const vector Y_AXIS;
            static const vector Z_AXIS;
            static const vector NEG_X_AXIS;
            static const vector NEG_Y_AXIS;
            static const vector NEG_Z_AXIS;
        }; // class vector
        

        /// Multiplies a vector by a scalar.
        inline vector operator* (gsgl::real_t n, const vector & v)
        {
            return vector(n*v.get_x(), n*v.get_y(), n*v.get_z());
        }
        

        /// Multiplies a vector by a scalar.
        inline vector operator* (const vector & v, gsgl::real_t n)
        {
            return vector(n*v.get_x(), n*v.get_y(), n*v.get_z());
        }


        /// Divides a vector by a scalar.
        inline vector operator/ (const vector & v, gsgl::real_t n)
        {
            return v * (static_cast<gsgl::real_t>(1) / n);
        }
        
    } // namespace math
    
} // namespace gsgl

#endif
