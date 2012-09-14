#ifndef GSGL_DATA_INDEXABLE_H
#define GSGL_DATA_INDEXABLE_H

//
// $Id: indexable.hpp 2 2008-03-01 20:58:50Z kulibali $
//
// Copyright (c) 2008-2010, The Periapsis Project. All rights reserved. 
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

#include "data.hpp"

namespace gsgl
{

    namespace data
    {
    
        /// A collection whose elements may be indexed by a particular type.
        template <typename T, typename I>
        class indexable
        {
        public:
            indexable();
            virtual ~indexable();
            
            virtual const T & item(const I & index) const = 0;
            virtual T & item(const I & index) = 0;
            
            inline const T & operator[] (const I & index) const { return item(index); }
            inline T & operator[] (const I & index) { return item(index); }

            virtual bool contains_index(const I &) const = 0;
        }; // class indexable
        
        //
        
        template <typename T, typename I> 
        indexable<T,I>::indexable() 
        {
        } // indexable<T,I>::indexable()
        
        template <typename T, typename I> 
        indexable<T,I>::~indexable()
        {
        } // indexable<T,I>::indexable()
                
    } // namespace data
    
} // namespace gsgl

#endif
