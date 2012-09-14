#ifndef GSGL_DATA_DATA_H
#define GSGL_DATA_DATA_H

//
// $Id: data.hpp 28 2008-11-12 01:41:40Z kulibali $
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

#include "base.hpp"

namespace gsgl
{

    /// The type used as indices into data collections.
    typedef int index_t;

    /// \name Flag utilities.
    /// \{

    /// Used for storing flag bits.
    typedef unsigned int flags_t;

    /// Set one or more flags.
    BASE_API inline void set_flags(flags_t & flags, const flags_t & flags_to_set)
    {
        flags |= flags_to_set;
    }

    /// Unset one or more flags.
    BASE_API inline void unset_flags(flags_t & flags, const flags_t & flags_to_unset)
    {
        flags &= ~flags_to_unset;
    }

    /// Check if a flag is set.
    BASE_API inline bool flag_is_set(const flags_t & flags, const flags_t & flags_to_test)
    {
        return (flags & flags_to_test) != 0;
    }

    namespace data
    {

        /// Base class for data data object types.
        class BASE_API data_object
        {
        public:
            data_object() {}
            virtual ~data_object() {}
        }; // data_object

    }

    namespace io
    {

        class BASE_API io_object
        {
        public:
            io_object() {}
            virtual ~io_object() {}
        }; // io_object

    }

    /// \}

    //////////////////////////////////////////////////////////////////

    /// \name Utility functions
    /// \{

    template <typename T>
    const T & min_val(const T & a, const T & b)
    {
        return a < b ? a : b;
    } // min_val()

    template <typename T>
    const T & max_val(const T & a, const T & b)
    {
        return a < b ? b : a;
    } // max_val()

    /// \}

} // namespace gsgl

#endif
