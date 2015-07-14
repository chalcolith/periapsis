#ifndef GSGL_FRAMEWORK_WIDGET_H
#define GSGL_FRAMEWORK_WIDGET_H

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
#include "scenegraph/event_map.hpp"
#include "data/stack.hpp"

#include "platform/color.hpp"
#include "platform/texture.hpp"
#include "platform/font.hpp"
#include "platform/display.hpp"


union SDL_Event;

namespace gsgl
{

    namespace framework
    {

        class FRAMEWORK_API widget
            : public framework_object
        {
            platform::display & screen;
            gsgl::flags_t flags;

            widget *parent, *next_tab, *prev_tab;
            data::simple_stack<widget *> children;

            int x, y, w, h;
            platform::color foreground, background;
            platform::texture *tex;

        public:
            enum widget_flags
            {
                NO_FLAGS = 0,
                WIDGET_INVISIBLE = (1 << 0),  ///< The widget is invisible but still active.
                WIDGET_INACTIVE  = (1 << 1),  ///< The widget is neither visible nor active.
                WIDGET_CAN_FOCUS = (1 << 2),  ///< The widget is capable of handling keyboard events.
            };


            widget(platform::display & screen, 
                   widget *parent, const int x, const int y, const int w, const int h,
                   const platform::color & foreground, const platform::color & background);
            virtual ~widget();

            /// \name Accessors
            /// @{
            platform::display & get_screen() { return screen; }

            gsgl::flags_t & get_flags() { return flags; }
            void set_flags(const gsgl::flags_t f, bool flag_on);

            widget * & get_next_tab() { return next_tab; }
            widget * & get_prev_tab() { return prev_tab; }
            
            widget * get_parent() { return parent; }
            data::simple_stack<widget *> & get_children() { return children; }

            int & get_x() { return x; }
            int & get_y() { return y; }
            int & get_w() { return w; }
            int & get_h() { return h; }

            platform::color & get_foreground() { return foreground; }
            platform::color & get_background() { return background; }
            platform::texture * & get_texture() { return tex; }
            /// @}

            void add_child(widget *);
            void remove_child(widget *);

            virtual void draw();
            virtual bool handle_event(const SDL_Event &);

            /// \name Event callbacks
            /// @{

            typedef bool (*event_handler)(widget *this_widget, const SDL_Event &);

            /// The event handler pointer is provided to override the default event handler of a particular widget object
            /// without needing to go to the trouble of creating a whole new derived class.  If the event handler is non-null,
            /// it will be called instead of handle_event(); if it returns false, then handle_event() will be called as well.
            event_handler & get_event_handler() { return handler; }

            /// @}

            /// \name Utilities
            /// @{

            /// Transforms the given local coordinates to absolute window coordinates.
            void get_abs(int & x, int & y);

            /// Transforms the given absolute window coordinates into local widget coordinates.
            void get_local(int & x, int & y);

            /// Determines whether or not the latest mouse-down was in this widget.
            bool button_down_here(int button);

            /// Draws a line (in local coordinates).
            void draw_line(int x0, int y0, int x1, int y1);

            /// Draws a box (in local coordinates).
            void draw_box(int x, int y, int w, int h);

            /// @}

        private:
            event_handler handler;
        }; // class widget


    } // namespace framework

} // namespace gsgl

#endif
