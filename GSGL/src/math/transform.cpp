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

#include "math/transform.hpp"
#include "math/quaternion.hpp"
#include "math/units.hpp"
#include "data/string.hpp"
#include "data/list.hpp"

#include <cstring>

namespace gsgl
{

    namespace math
    {
    
        transform::transform() 
            : matrix<4,4,gsgl::real_t>()
        {
        } // transform::transform()
        

        transform::transform(const gsgl::real_t *data) 
            : matrix<4,4,gsgl::real_t>(data)
        {
        } // transform::transform()
        

        transform::transform(const transform & t) 
            : matrix<4,4,gsgl::real_t>(t)
        {
        } // transform::transform()
        

        transform::transform(const quaternion & q)
        {
            *this = q;
        } // transform::transform()


        transform::transform(const matrix<4,4,gsgl::real_t> & m) 
            : matrix<4,4,gsgl::real_t>(m)
        {
        } // transform::transform()
        

        transform & transform::operator= (const transform & t)
        {
            ::memcpy(data, t.data, 16 * sizeof(gsgl::real_t));
            return *this;
        } // transform::operator= ()


        transform & transform::operator= (const matrix<4,4,gsgl::real_t> & m)
        {
            ::memcpy(data, m.ptr(), 16 * sizeof(gsgl::real_t));
            return *this;
        } // transform::operator= ()


        transform & transform::operator= (const quaternion & q)
        {
            data[0]  = static_cast<gsgl::real_t>(1 - 2*q.y*q.y - 2*q.z*q.z);
            data[1]  = static_cast<gsgl::real_t>(2*q.x*q.y + 2*q.w*q.z);
            data[2]  = static_cast<gsgl::real_t>(2*q.x*q.z - 2*q.w*q.y);
            data[3]  = 0;
            
            data[4]  = static_cast<gsgl::real_t>(2*q.x*q.y - 2*q.w*q.z);
            data[5]  = static_cast<gsgl::real_t>(1 - 2*q.x*q.x - 2*q.z*q.z);
            data[6]  = static_cast<gsgl::real_t>(2*q.y*q.z + 2*q.w*q.x);
            data[7]  = 0;
            
            data[8]  = static_cast<gsgl::real_t>(2*q.x*q.z + 2*q.w*q.y);
            data[9]  = static_cast<gsgl::real_t>(2*q.y*q.z - 2*q.w*q.x);
            data[10] = static_cast<gsgl::real_t>(1 - 2*q.x*q.x - 2*q.y*q.y);
            data[11] = 0;
            
            data[12] = 0;
            data[13] = 0;
            data[14] = 0;
            data[15] = 1;

            return *this;
        } // transform::operator= ()

        
        transform::~transform()
        {
        } // transform::~transform()
        

        //
        
        vector transform::translation_part() const
        {
            return vector(data[12], data[13], data[14]);
        } // transform::translation_part()
        

        transform transform::rotation_part() const
        {
            transform res = IDENTITY;

            memcpy(res.data + 0, data + 0, sizeof(gsgl::real_t) * 3);
            memcpy(res.data + 4, data + 4, sizeof(gsgl::real_t) * 3);
            memcpy(res.data + 8, data + 8, sizeof(gsgl::real_t) * 3);

            //res.data[0] = data[0];
            //res.data[1] = data[1];
            //res.data[2] = data[2];
            //
            //res.data[4] = data[4];
            //res.data[5] = data[5];
            //res.data[6] = data[6];
            //
            //res.data[8] = data[8];
            //res.data[9] = data[9];
            //res.data[10] = data[10];
            
            return res;
        } // transform::rotation_part()
        
        //

        vector transform::get_basis_x() const
        {
            return vector(data[0], data[1], data[2]);
        } // transform::basis_x()


        vector transform::get_basis_y() const
        {
            return vector(data[4], data[5], data[6]);
        } // transform::basis_y()


        vector transform::get_basis_z() const
        {
            return vector(data[8], data[9], data[10]);
        } // transform::basis_z()


        void transform::set_basis_x(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z)
        {
            data[0] = x;
            data[1] = y;
            data[2] = z;
        } // transform::set_basis_x()

        void transform::set_basis_x(const vector & v)
        {
            set_basis_x(v.get_x(), v.get_y(), v.get_z());
        } // transform::set_basis_x()


        void transform::set_basis_y(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z)
        {
            data[4] = x;
            data[5] = y;
            data[6] = z;
        } // transform::set_basis_y()

        void transform::set_basis_y(const vector & v)
        {
            set_basis_y(v.get_x(), v.get_y(), v.get_z());
        } // transform::set_basis_y()


        void transform::set_basis_z(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z)
        {
            data[8] = x;
            data[9] = y;
            data[10] = z;
        } // transform::set_basis_z()

        void transform::set_basis_z(const vector & v)
        {
            set_basis_z(v.get_x(), v.get_y(), v.get_z());
        } // transform::set_basis_z()



        // This is from an Intel technote: Streaming SIMD Extensions - Inverse of a 4x4 Matrix

        /************************************************************
         *
         * input:
         * mat - pointer to array of 16 floats (source matrix)
         * output:
         * dst - pointer to array of 16 floats (invert matrix)
         *
         *************************************************************/
        static void invert(float *mat, float *dst)
        {
            float tmp[12]; /* temp array for pairs */
            float src[16]; /* array of transpose source matrix */
            float det; /* determinant */

            /* transpose matrix */
            for (int i = 0; i < 4; ++i) {
                src[i] = mat[i*4];
                src[i + 4] = mat[i*4 + 1];
                src[i + 8] = mat[i*4 + 2];
                src[i + 12] = mat[i*4 + 3];
            }

            /* calculate pairs for first 8 elements (cofactors) */
            tmp[0] = src[10] * src[15];
            tmp[1] = src[11] * src[14];
            tmp[2] = src[9] * src[15];
            tmp[3] = src[11] * src[13];
            tmp[4] = src[9] * src[14];
            tmp[5] = src[10] * src[13];
            tmp[6] = src[8] * src[15];
            tmp[7] = src[11] * src[12];
            tmp[8] = src[8] * src[14];
            tmp[9] = src[10] * src[12];
            tmp[10] = src[8] * src[13];
            tmp[11] = src[9] * src[12];
            
            /* calculate first 8 elements (cofactors) */
            dst[0] = tmp[0]*src[5] + tmp[3]*src[6] + tmp[4]*src[7];
            dst[0] -= tmp[1]*src[5] + tmp[2]*src[6] + tmp[5]*src[7];
            dst[1] = tmp[1]*src[4] + tmp[6]*src[6] + tmp[9]*src[7];
            dst[1] -= tmp[0]*src[4] + tmp[7]*src[6] + tmp[8]*src[7];
            dst[2] = tmp[2]*src[4] + tmp[7]*src[5] + tmp[10]*src[7];
            dst[2] -= tmp[3]*src[4] + tmp[6]*src[5] + tmp[11]*src[7];
            dst[3] = tmp[5]*src[4] + tmp[8]*src[5] + tmp[11]*src[6];
            dst[3] -= tmp[4]*src[4] + tmp[9]*src[5] + tmp[10]*src[6];
            dst[4] = tmp[1]*src[1] + tmp[2]*src[2] + tmp[5]*src[3];
            dst[4] -= tmp[0]*src[1] + tmp[3]*src[2] + tmp[4]*src[3];
            dst[5] = tmp[0]*src[0] + tmp[7]*src[2] + tmp[8]*src[3];
            dst[5] -= tmp[1]*src[0] + tmp[6]*src[2] + tmp[9]*src[3];
            dst[6] = tmp[3]*src[0] + tmp[6]*src[1] + tmp[11]*src[3];
            dst[6] -= tmp[2]*src[0] + tmp[7]*src[1] + tmp[10]*src[3];
            dst[7] = tmp[4]*src[0] + tmp[9]*src[1] + tmp[10]*src[2];
            dst[7] -= tmp[5]*src[0] + tmp[8]*src[1] + tmp[11]*src[2];
            
            /* calculate pairs for second 8 elements (cofactors) */
            tmp[0] = src[2]*src[7];
            tmp[1] = src[3]*src[6];
            tmp[2] = src[1]*src[7];
            tmp[3] = src[3]*src[5];
            tmp[4] = src[1]*src[6];
            tmp[5] = src[2]*src[5];
            tmp[6] = src[0]*src[7];
            tmp[7] = src[3]*src[4];
            tmp[8] = src[0]*src[6];
            tmp[9] = src[2]*src[4];
            tmp[10] = src[0]*src[5];
            tmp[11] = src[1]*src[4];

            /* calculate second 8 elements (cofactors) */
            dst[8] = tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
            dst[8] -= tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
            dst[9] = tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
            dst[9] -= tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
            dst[10] = tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
            dst[10]-= tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
            dst[11] = tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
            dst[11]-= tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
            dst[12] = tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
            dst[12]-= tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
            dst[13] = tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
            dst[13]-= tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
            dst[14] = tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
            dst[14]-= tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
            dst[15] = tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
            dst[15]-= tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];

            /* calculate determinant */
            det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
            
            /* calculate matrix inverse */
            det = 1.0f/det;
            for (int j = 0; j < 16; ++j)
                dst[j] *= det;
        }


        transform transform::inverse() const
        {
            transform res;
            invert(const_cast<float *>(this->data), const_cast<float *>(res.data));
            return res;
        } // transform::inverse()

        
        transform transform::transpose() const
        {
            transform res;
            for (int i = 0; i < 4; ++i)
                for (int j = 0; j < 4; ++j)
                    res.item(i,j) = this->item(j,i);
            return res;
        } // transform::transpose()

        
        transform transform::translation_transform(const vector & v)
        {
            transform res = IDENTITY;
            res.data[12] = v.get_x();
            res.data[13] = v.get_y();
            res.data[14] = v.get_z();
            return res;
        } // transform::translation_transform()


        transform transform::scale(const gsgl::real_t & s)
        {
            return scale(s, s, s);
        } // transform::sacle()


        transform transform::scale(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z)
        {
            transform res = IDENTITY;
            res.data[0] *= x;
            res.data[5] *= y;
            res.data[10] *= z;
            return res;
        } // transform::scale()
        

        transform transform::parse(const string & str)
        {
            transform res;

            data::list<string> tokens = str.split(L" ,");

            int num = 0;
            for (data::list<string>::iterator i = tokens.iter(); i.is_valid(); ++i)
            {
                if (!i->is_empty())
                    res[num++] = units::parse(*i);
            }

            return res;
        } // transform::parse()

        //
        
        static const gsgl::real_t identity_data[] = { 1, 0, 0, 0,  0, 1, 0, 0,  0, 0, 1, 0,  0, 0, 0, 1 };
        
        transform transform::IDENTITY(identity_data);
        
    } // namespace math
    
} // namespace gsgl
