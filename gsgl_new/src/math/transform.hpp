#ifndef GSGL_MATH_TRANSFORM_H
#define GSGL_MATH_TRANSFORM_H

//
// $Id: transform.hpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "math.hpp"
#include "matrix.hpp"
#include "vector.hpp"

namespace gsgl
{

    namespace math
    {

        class quaternion;

    
        /// A 4x4 matrix used in 3d coordinate system transformations.
        class MATH_API transform 
            : public matrix<4,4,gsgl::real_t>
        {
        public:
            transform();
            explicit transform(const gsgl::real_t *);
            explicit transform(const quaternion &);
            transform(const transform &);
            transform(const matrix<4,4,gsgl::real_t> &);

            transform & operator= (const transform &);
            transform & operator= (const matrix<4,4,gsgl::real_t> &);
            transform & operator= (const quaternion &);
            
            virtual ~transform();
            
            //

            inline const gsgl::real_t & operator[] (const gsgl::index_t & i) const { return data[i]; }
            inline gsgl::real_t & operator[] (const gsgl::index_t & i) { return data[i]; }

            //
            vector get_basis_x() const;
            vector get_basis_y() const;
            vector get_basis_z() const;

            void set_basis_x(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z);
            void set_basis_x(const vector &);
            void set_basis_y(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z);
            void set_basis_y(const vector &);
            void set_basis_z(const gsgl::real_t & x, const gsgl::real_t & y, const gsgl::real_t & z);
            void set_basis_z(const vector &);


            transform transpose() const;
            transform inverse() const;

            vector    translation_part() const;
            transform rotation_part() const;

            //

            inline vector operator* (const vector & v) const
            {
                vector res;
                matrix_utils::multiply<4,4,4,1,gsgl::real_t> (*this, v, res);
                res.normalize_h();
                return res;
            } // operator * ()

            inline transform operator* (const transform & t) const
            {
                transform res;
                matrix_utils::multiply<4,4,4,4,gsgl::real_t> (*this, t, res);
                return res;
            } // operator * ()

            //
            
            static transform parse(const gsgl::string &);
            static transform translation_transform(const vector &);
            
            static transform scale(const gsgl::real_t & s);
            static transform scale(const gsgl::real_t &, const gsgl::real_t &, const gsgl::real_t &);
            
            static transform IDENTITY;
        }; // class transform
        
    } // namespace math
    
} // namespace gsgl

#endif
