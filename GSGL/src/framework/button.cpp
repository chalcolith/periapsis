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

#include "framework/button.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace platform;

    namespace framework
    {


        button::button(widget *parent, 
                       int x, int y, int w, int h, 
                       const color & fg, const color & bg, 
                       const string & font_face, int font_size,
                       const string & text)
            : textbox(parent, 
                      x, y, w, h, fg, bg,
                      font_face, font_size, 
                      textbox::ALIGN_CENTER, 
                      textbox::NO_FLAGS),
              down_tick(0),
              on_click_handler(0)
        {
            get_text() = text;
            //LOG_BASIC(L"ui: creating button %ls", text.w_string());
        } // button::button()


        button::~button()
        {
            //LOG_BASIC(L"ui: destroying button %ls", get_text().w_string());
        } // button::~button()


        void button::set_on_click_handler(void (*handler)(button *))
        {
            on_click_handler = handler;
        } // button::set_on_click_handler()


        void button::draw()
        {
            color old_fg = get_foreground();
            color old_bg = get_background();

            if (down_tick)
            {
                if ((SDL_GetTicks() - down_tick) < 100)
                {
                    get_foreground() = old_bg;
                    get_background() = old_fg;
                }
                else
                {
                    down_tick = 0;
                }
            }

            textbox::draw();

            get_foreground().set();
            glLineWidth(1.0f);

            glBegin(GL_LINE_STRIP);
            glVertex2i(0, 0);
            glVertex2i(get_w(), 0);
            glVertex2i(get_w(), get_h());
            glVertex2i(0, get_h());
            glVertex2i(0, 0);
            glEnd();

            if (down_tick)
            {
                get_foreground() = old_fg;
                get_background() = old_bg;
            }
        } // button::draw()


        bool button::handle_event(const SDL_Event & e)
        {
            switch (e.type)
            {
            case SDL_MOUSEBUTTONDOWN:
                down_tick = SDL_GetTicks();
                return true;
            case SDL_MOUSEBUTTONUP:
                {
                    if (button_down_here(e.button.button))
                    {
                        if (on_click_handler)
                        {
                            on_click_handler(this);
                            return true;
                        }
                    }
                }
                break;
            default:
                break;
            }

            return false;
        } // button::handle_event()


    } // namespace framework

} // namespace gsgl
