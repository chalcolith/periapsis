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

#include "platform/platform.hpp"
#include "platform/color.hpp"
#include "platform/lowlevel.hpp"


namespace gsgl
{

    namespace platform
    {

        const color color::BLACK(0, 0, 0, 1);
        const color color::WHITE(1, 1, 1, 1);
        const color color::HOT_PINK(0xff69b4ff);


        void color::bind() const
        {
            glColor4fv(val);                                                                                        CHECK_GL_ERRORS();
        } // color::bind()


        void color::unbind() const
        {
            glColor4fv(HOT_PINK.val);
        } // color::unbind()


        color color::parse(const string & s)
        {
            color res(BLACK);

            data::list<string> tokens = s.split(L" ,\t");
            
            int num = 0;
            for (data::list<string>::iterator i = tokens.iter(); num < 4 && i.is_valid(); ++i)
            {
                if (!i->is_empty())
                    res.val[num++] = static_cast<float>(i->to_double());
            }

            return res;
        } // color::parse()

    } // namespace gsgl

} // namespace gsgl

