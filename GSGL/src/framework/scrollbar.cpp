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

#include "framework/scrollbar.hpp"
#include "math/vector.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace data;
    using namespace math;
    using namespace platform;

    namespace framework
    {
        
        config_variable<int> scrollbar::ARROW_SIZE(L"framework/scrollbar/arrow_size", 10);


        scrollbar::scrollbar(widget *parent, 
                             int x, int y, int w, int h, 
                             const color & fg, const color & bg, 
                             bar_mode mode)
            : widget(parent, x, y, w, h, fg, bg), min_val(0), max_val(0), pos(0), extent(0), mode(mode)
        {
            //LOG_BASIC(L"ui: creating scrollbar");
        } // scrollbar::scrollbar()


        scrollbar::~scrollbar()
        {
            //LOG_BASIC(L"ui: destroying scrollbar");
        } // scrollbar::~scrollbar()


        void scrollbar::draw()
        {
            assert(max_val >= min_val);
            assert(pos >= min_val);
            assert(pos <= max_val);
            assert(pos + extent <= max_val);

            widget::draw();

            glPushAttrib(GL_ALL_ATTRIB_BITS);
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);

            get_foreground().bind();
            glLineWidth(3.0f);

            if (mode == VERTICAL)
            {
                // draw up arrow
                
                // draw down arrow
            }
            else
            {
            }

            glPopClientAttrib();
            glPopAttrib();
        } // scrollbar::draw()

    } // namespace framework

} // namespace gsgl
