#ifndef GSGL_FRAMEWORK_TEXTBOX_H
#define GSGL_FRAMEWORK_TEXTBOX_H

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

#include "framework/framework.hpp"
#include "framework/widget.hpp"

namespace gsgl
{

    namespace platform
    {
        class font;
    } // namespace platform


    namespace framework
    {

        class FRAMEWORK_API textbox
            : public widget
        {
            platform::font *text_font;
            gsgl::string text;

        public:

            enum text_align 
            { 
                ALIGN_LEFT, 
                ALIGN_RIGHT, 
                ALIGN_CENTER, 
                ALIGN_JUSTIFY 
            };

            enum textbox_flags             
            {               
                NO_FLAGS    = 0,
                TEXT_WRAP   = (1 << 0), 
                TEXT_SCROLL = (1 << 1) 
            };

            textbox(widget *parent, 
                    const int x, const int y, const int w, const int h, 
                    const platform::color & fg, const platform::color & bg,
                    const gsgl::string & font_face, const int font_size, 
                    text_align align = ALIGN_LEFT, gsgl::flags_t text_flags = NO_FLAGS);
            virtual ~textbox();

            text_align & get_align() { return align; }
            gsgl::flags_t & get_text_flags() { return text_flags; }
            gsgl::string & get_text() { return text; }

            virtual void draw();

        private:
            text_align align;
            gsgl::flags_t text_flags;
        }; // class textbox

    } // namespace framework

} // namespace gsgl

#endif
