//
// $Id: vector.cpp 17 2008-07-28 04:17:44Z kulibali $
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
#include "vector.hpp"
#include "units.hpp"
#include "base/string.hpp"
#include "base/list.hpp"

namespace gsgl
{

    namespace math
    {
    
        const vector vector::ZERO(0, 0, 0);
        const vector vector::X_AXIS(1, 0, 0);
        const vector vector::Y_AXIS(0, 1, 0);
        const vector vector::Z_AXIS(0, 0, 1);
        const vector vector::NEG_X_AXIS(-1, 0, 0);
        const vector vector::NEG_Y_AXIS(0, -1, 0);
        const vector vector::NEG_Z_AXIS(0, 0, -1);
        
        vector::vector(gsgl::real_t x, gsgl::real_t y, gsgl::real_t z) 
            : matrix<4,1,gsgl::real_t>()
        {
            data[0] = x;
            data[1] = y;
            data[2] = z;
            data[3] = 1;
        } // vector::vector()
        
        vector::vector(const gsgl::real_t *ptr) 
            : matrix<4,1,gsgl::real_t>(ptr)
        {
        } // vector::vector()
        
        vector::vector(const vector & v) 
            : matrix<4,1,gsgl::real_t>(v)
        {
        } // vector::vector()
        
        vector::vector(const matrix<4,1,gsgl::real_t> & v) 
            : matrix<4,1,gsgl::real_t>(v)
        {
            if (data[3] != static_cast<gsgl::real_t>(1))
                normalize_h();
        } // vector::vector()
        
        vector & vector::operator= (const vector & v)
        {
            ::memcpy(data, v.data, sizeof(gsgl::real_t) * 4);
            return *this;
        } // vector::operator= ()
        
        vector & vector::operator= (const matrix<4,1,gsgl::real_t> & m)
        {
            ::memcpy(data, m.ptr(), sizeof(gsgl::real_t) * 4);
            return *this;
        } // vector::operator= ()
        
        vector::~vector()
        {
        } // vector::~vector()
        
        vector::vector(const string & s) 
            : matrix<4,1,gsgl::real_t>()
        {
            data[3] = 1;
            
            data::list<string> components = s.split(L" ");
            int index = 0;
            for (data::list<string>::iterator i = components.iter(); i.is_valid(); ++i)
            {
                if (i->size())
                    data[index++] = static_cast<gsgl::real_t>(i->to_double());
                if (index == 4)
                    break;
            }
        } // vector::vector()
        
        void vector::normalize()
        {
            gsgl::real_t m = mag();
            if (m > 0)
            {
                data[0] /= m;
                data[1] /= m;
                data[2] /= m;
            }
        } // normalize()
        
        void vector::normalize_h()
        {
            gsgl::real_t m = data[3];
            if (m > 0)
            {
                data[0] /= m;
                data[1] /= m;
                data[2] /= m;
                data[3] /= m;
            }
        } // normalize_h()
        
        /// Returns the magnitude of the 3-d vector.
        gsgl::real_t vector::mag() const
        {
            gsgl::real_t result = 0.0;
            for (size_t i = 0; i < 3; ++i)
                result += data[i]*data[i];
            return ::sqrt(result);
        } // vector::mag()
        
        /// Returns the square of the magnitude of the 3-d vector (more efficient than vector::mag()).
        gsgl::real_t vector::mag2() const
        {
            gsgl::real_t result = 0.0;
            for (size_t i = 0; i < 3; ++i)
                result += data[i]*data[i];
            return result;
        } // vector::mag2()
        

        /// \note This does not negate the homogeneous element.
        vector vector::operator- () const
        {
            return vector(-data[0], -data[1], -data[2]);
        } // vector::operator- ()
        
        /// \note This does not add the homogeneous element.
        vector vector::operator+ (const vector & v) const
        {
            return vector(data[0] + v.data[0], data[1] + v.data[1], data[2] + v.data[2]);
        } // vector::operator+ ()
        
        /// \note This does not subtract the homogeneous element.
        vector vector::operator- (const vector & v) const
        {
            return vector(data[0] - v.data[0], data[1] - v.data[1], data[2] - v.data[2]);
        } // vector::operator- ()


        /// \note Does not add the homogeneous element.
        vector & vector::operator+= (const vector & v)
        {
            for (int i = 0; i < 3; ++i)
                data[i] += v.data[i];
            return *this;
        } // vector::operator+= ()


        /// \note Does not add the homogeneous element.
        vector & vector::operator-= (const vector & v)
        {
            for (int i = 0; i < 3; ++i)
                data[i] -= v.data[i];
            return *this;
        } // vector::operator -= ()
        

        /// \note Does not multiply the homogeneous element.
        vector & vector::operator*= (const gsgl::real_t n)
        {
            data[0] *= n;
            data[1] *= n;
            data[2] *= n;
            return *this;
        } // vector::operator*= ()


        /// \note Does not multiply the homogeneous element.
        vector vector::operator* (const gsgl::real_t n)
        {
            return vector(data[0]*n, data[1]*n, data[2]*n);
        } // vector::operator* ()


        gsgl::real_t vector::dot(const vector & v) const
        {
            gsgl::real_t result = 0.0;
            for (size_t i = 0; i < 3; ++i)
                result += data[i]*v.data[i];
            return result;
        } // vector::dot()
        
        vector vector::cross(const vector & v) const
        {
            return vector(data[1]*v.data[2] - data[2]*v.data[1], 
                          data[2]*v.data[0] - data[0]*v.data[2], 
                          data[0]*v.data[1] - data[1]*v.data[0]);
        } // vector::cross()


        vector vector::interpolate(const vector & start, const vector & end, const gsgl::real_t & percent)
        {
            return start + (end - start) * percent;
        } // vector::interpolate()
        

        vector vector::parse(const string & str)
        {
            vector res(vector::ZERO);

            data::list<string> tokens = str.split(L" ,\t");
            
            int num = 0;
            for (data::list<string>::iterator i = tokens.iter(); num < 4 && i.is_valid(); ++i)
            {
                if (!i->is_empty())
                    res.data[num++] = units::parse(*i);
            }

            return res;
        } // vector::parse()

    } // namespace math
    
} // namespace gsgl
