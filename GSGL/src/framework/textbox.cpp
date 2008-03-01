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

#include "framework/textbox.hpp"
#include "platform/font.hpp"

#include "platform/lowlevel.hpp"


namespace gsgl
{

    using namespace data;
    using namespace platform;

    namespace framework
    {


        textbox::textbox(widget *parent, 
                         int x, int y, int w, int h, 
                         const color & fg, const color & bg, 
                         const string & font_face, const int font_size, 
                         text_align align, gsgl::flags_t text_flags)
            : widget(parent, x, y, w, h, fg, bg), text_font(new font(font_face, font_size, fg)), align(align), text_flags(text_flags)
        {
            //LOG_BASIC(L"ui: creating text box");
        } // textbox::textbox()


        textbox::~textbox()
        {
            //LOG_BASIC(L"ui: destroying text box");
            delete text_font;
        } // texbox::~textbox()


        static void get_lines(const int width, const string & text, font *text_font, list<string> & lines, int & max_w)
        {

        } // get_lines()


        void textbox::draw()
        {
            if ((get_flags() & WIDGET_INACTIVE) == 0)
                widget::draw();

            // get lines
            int text_width = static_cast<int>(text_font->calc_width(text));
            int y = 1;
            int x;

            switch (align)
            {
            case ALIGN_LEFT:
                x = 1;
                break;
            case ALIGN_RIGHT:
                x = (get_w() - text_width) - 1;
                break;
            case ALIGN_CENTER:
                x = get_w()/2 - text_width/2;
                break;
            case ALIGN_JUSTIFY:
                throw internal_exception(__FILE__, __LINE__, L"ALIGN_JUSTIFY not implemented.");
                break;
            }

            glMatrixMode(GL_MODELVIEW);
            glPushMatrix();

            glTranslatef(static_cast<GLfloat>(x), static_cast<GLfloat>(y), 0);
            text_font->draw(text);

            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();
        } // textbox::draw()


    } // namespace framework

} // namespace gsgl
