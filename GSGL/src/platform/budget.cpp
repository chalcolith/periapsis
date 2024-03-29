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

#include "platform/budget.hpp"

#include "data/exception.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace data;

    // global budget
    platform::budget *platform::budget::instance = 0;

    namespace platform
    {


        budget::budget()
            : platform_object(), singleton<budget>()
        {
        } // budget::budget()


        budget::~budget()
        {
        } // budget::~budget()


        void budget::reset()
        {
            for (data::dictionary<unsigned int, string>::iterator i = data.iter(); i.is_valid(); ++i)
            {
                *i = 0;
            }
        } // budget::reset()


        budget_record::budget_record(const string & category)
            : parent(budget::global_instance()), category(category)
        {
#ifdef DEBUG
            assert(parent);
            start_tick = SDL_GetTicks();
#endif
        } // budget_record::budget_record()


        budget_record::~budget_record()
        {
#ifdef DEBUG
            unsigned int end_tick = SDL_GetTicks();
            if (parent->global_instance())
                parent->get_data()[category] += end_tick - start_tick;
#endif
        } // budget_record::~budget_record()


    } // namespace platform

} // namespace gsgl
