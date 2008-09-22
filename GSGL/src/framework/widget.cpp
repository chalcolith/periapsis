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

#include "framework/widget.hpp"
#include "framework/application.hpp"

#include "platform/display.hpp"


namespace gsgl
{

    using namespace data;
    using namespace platform;

    namespace framework
    {

        widget::widget(display & screen, widget *parent, const int x, const int y, const int w, const int h, const color & fg, const color & bg)
            : screen(screen), flags(NO_FLAGS), parent(parent), next_tab(0), prev_tab(0), x(x), y(y), w(w), h(h), foreground(fg), background(bg), tex(0), handler(0)
        {
            if (parent)
                parent->add_child(this);
        } // widget::widget()


        widget::~widget()
        {
            for (simple_stack<widget *>::iterator i = children.iter(); i.is_valid(); ++i)
                delete *i;
        } // widget::~widget()


        void widget::set_flags(const gsgl::flags_t f, bool flag_on)
        {
            if (flag_on)
                flags |= f;
            else
                flags &= ~f;
        } // widget::set_flags()


        void widget::add_child(widget *child)
        {
            children.push(child);
            child->parent = this;
        } // widget::add_child()


        void widget::remove_child(widget *child)
        {
            for (simple_stack<widget *>::iterator i = children.iter(); i.is_valid(); ++i)
            {
                if (*i == child)
                {
                    child->parent = 0;
                    children.remove(i);
                    break;
                }
            }
        } // widget::remove_child()


        void widget::draw()
        {
            display::scoped_state state(get_screen(), display::ENABLE_ORTHO_2D);
            display::scoped_color bg(get_screen(), get_background());
            display::scoped_texture t(get_screen(), tex);

            get_screen().draw_rect_2d(0, 0, static_cast<float>(w), static_cast<float>(h));
        } // widget::draw()


        bool widget::handle_event(const SDL_Event & e)
        {
            return false;
        } // widget::handle_event()


        // 
        void widget::get_abs(int & x, int & y)
        {
            int res_x = x;
            int res_y = y;

            for (widget *w = parent; w; w = w->parent)
            {
                res_x += w->x;
                res_y += w->y;
            }

            x = res_x;
            y = res_y;
        } // widget::get_abs()


        void widget::get_local(int & x, int & y)
        {
            int res_x = x;
            int res_y = y;

            for (widget *w = this; w; w = w->parent)
            {
                res_x -= w->x;
                res_y -= w->y;
            }

            x = res_x;
            y = res_y;
        } // widget::get_local()


        bool widget::button_down_here(int button)
        {
            int prev_button, prev_x, prev_y;
            application::global_instance()->get_mousedown_info(prev_button, prev_x, prev_y);
            get_local(prev_x, prev_y);
            return prev_button == button
                && prev_x >= 0 && prev_x < get_w()
                && prev_y >= 0 && prev_y < get_h();
        } // widget::button_down_here()


        void widget::draw_line(int x0, int y0, int x1, int y1)
        {
            get_abs(x0, y0);
            get_abs(x1, y1);
            get_screen().draw_line_2d(x0, y0, x1, y1);
        } // widget::draw_line()


        void widget::draw_box(int x, int y, int w, int h)
        {
            get_abs(x, y);
            get_screen().draw_rect_2d(static_cast<float>(x), static_cast<float>(y), static_cast<float>(x+w), static_cast<float>(y+h));
        } // widget::draw_box()

    } // namespace framework

} // namespace gsgl
