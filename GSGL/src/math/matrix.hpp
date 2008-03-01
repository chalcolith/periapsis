#ifndef GSGL_MATH_MATRIX_H
#define GSGL_MATH_MATRIX_H

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

#include "data/exception.hpp"
#include "data/string.hpp"
#include "data/stream.hpp"

#include <cstring>

namespace gsgl
{

    namespace math
    {
    
        /// A generic matrix class.
        /// Implements an RxC matrix, where R is the number of rows and C is the number of columns.
        template <int R, int C, typename T = gsgl::real_t> 
        class matrix
            : public math_object
        {
        protected:
            T data[R*C];
        public:
            matrix();
            explicit matrix(const T *); ///< Initializes the matrix from an array in column-major order.
            matrix(const matrix<R,C,T> &);
            matrix & operator= (const matrix<R,C,T> &);
            virtual ~matrix();
            
            const T & item (int i, int j) const { return data[j*R+i]; }
            T & item (int i, int j) { return data[j*R+i]; }
            
            /// Returns an array containing the matrix's data, in column-major order.
            const T *ptr() const { return data; }
            T * ptr() { return data; }

            void to_stream(io::text_stream & s) const;
            void from_stream(io::text_stream & s);

            bool operator== (const matrix<R,C,T> & m) const;
        }; // class matrix


        //
        
        template <int R, int C, typename T> 
        matrix<R,C,T>::matrix()
            : math_object()
        {
            ::memset(data, 0, sizeof(T) * R*C);
        } // matrix<R,C,T>::matrix()
        
        template <int R, int C, typename T> 
        matrix<R,C,T>::matrix(const T *ptr)
            : math_object()
        {
            ::memcpy(data, ptr, sizeof(T) * R*C);
        } // matrix<R,C,T>::matrix()
        
        template <int R, int C, typename T> 
        matrix<R,C,T>::matrix(const matrix<R,C,T> & m)
            : math_object()
        {
            ::memcpy(data, m.data, sizeof(T) * R*C);
        } // matrix<R,C,T>::matrix()
        
        template <int R, int C, typename T> 
        matrix<R,C,T> & matrix<R,C,T>::operator= (const matrix<R,C,T> & m)
        {
            ::memcpy(data, m.data, sizeof(T) * R*C);
            return *this;
        } // matrix<R,C,T>::operator= ()
        
        template <int R, int C, typename T> 
        matrix<R,C,T>::~matrix()
        {
        } // matrix<R,C,T>::~matrix()
        
        //

        template <int R, int C, typename T>
        bool matrix<R,C,T>::operator== (const matrix<R,C,T> & m) const
        {
            for (int i = 0; i < R*C; ++i)
                if (data[i] != m.data[i])
                    return false;
            return true;
        } // matrix<R,C,T>::operator== ()


        //
        
        template <int R, int C, typename T> 
        void matrix<R,C,T>::to_stream(io::text_stream & s) const
        {
            for (int i = 0; i < R; ++i)
            {
                for (int j = 0; j < C; ++j)
                {
                    if (j > 0)
                        s << L' ';
                    s << item(i, j);
                }
                s << L'\n';
            }
        } // matrix<R,C,T>::to_stream()
        
        template <int R, int C, typename T> 
        void matrix<R,C,T>::from_stream(io::text_stream & s)
        {
            for (int i = 0; i < R; ++i)
            {
                for (int j = 0; j < C; ++j)
                {
                    s >> item(i, j);
                }
            }
        } // matrix<R,C,T>::from_stream()
                


        //


        namespace matrix_utils
        {
            /// Multiplies two arbitrary matrices.  CA must equal RB.

            template <int RA, int CA, int RB, int CB, typename T>
            void multiply(const matrix<RA,CA,T> & a, const matrix<RB,CB,T> & b, matrix<RA,CB,T> & result)
            {
                assert(CA == RB);

                gsgl::real_t *pa = const_cast<gsgl::real_t *>(a.ptr());
                gsgl::real_t *pb = const_cast<gsgl::real_t *>(b.ptr());
                gsgl::real_t *pr = const_cast<gsgl::real_t *>(result.ptr());

                for (int i = 0; i < RA; ++i)
                {
                    for (int j = 0; j < CB; ++j)
                    {
                        pr[j*RA+i] = 0;

                        for (int r = 0; r < CA; ++r)
                        {
                            pr[j*RA+i] += pa[r*RA+i] * pb[j*RB+r];
                        }
                    }
                }
            } // multiply()

        } // namespace matrix_utils


    } // namespace math


} // namespace gsgl

#endif
