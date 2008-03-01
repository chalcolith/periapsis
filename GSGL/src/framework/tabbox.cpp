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

#include "framework/tabbox.hpp"
#include "framework/textbox.hpp"
#include "framework/application.hpp"

#include "platform/font.hpp"
#include "platform/display.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace data;
    using namespace platform;

    namespace framework
    {

        tabbox::tabbox(widget *parent, 
                       int x, int y, int w, int h, 
                       const color & fg, const color & bg,
                       const string & tab_font_face, 
                       const int tab_font_size,
                       const int tab_bar_height)
            : widget(parent, x, y, w, h, fg, bg), active_tab_index(-1), 
              tab_font(new font(tab_font_face, tab_font_size, fg)), tab_bar_height(tab_bar_height)
        {
            //LOG_BASIC(L"ui: creating tab box");
        } // tabbox::tabbox()


        tabbox::~tabbox()
        {
            //LOG_BASIC(L"ui: destroying tab box");

            delete tab_font;

            for (list<tab_rec>::iterator i = tabs.iter(); i.is_valid(); ++i)
            {
                delete i->title_bar;
                // the contents will be deleted as one of the children...
            }
        } // tabbox::~tabbox()


        void tabbox::add_tab(const string & name, widget *contents)
        {
            textbox *title_bar = new textbox(0, 0, 0, 0, 0, get_foreground(), get_background(), tab_font->get_face(), tab_font->get_size());
            title_bar->get_text() = name;

            contents->get_x() = 0;
            contents->get_y() = 0;
            contents->get_w() = get_w();
            contents->get_h() = get_h() - tab_bar_height;
            add_child(contents);

            tab_rec tr;
            tr.name = name;
            tr.title_bar = title_bar;
            tr.contents = contents;
            tabs.append(tr);
        }; // tabbox::add_tab()

        
        void tabbox::draw()
        {
            // initialize ourselves if we aren't already
            if (active_tab_index == -1)
            {
                if (tabs.size())
                {
                    active_tab_index = 0;
                }
                else
                {
                    //widget::draw();
                    return;
                }   
            }

            // draw tab bar
            int tab_bar_width = get_w() / tabs.size();

            for (gsgl::index_t i = 0; i < tabs.size(); ++i)
            {
                widget *title_bar = tabs[i].title_bar;
                assert(title_bar);

                title_bar->get_w() = tab_bar_width;
                title_bar->get_h() = tab_bar_height;

                title_bar->get_x() = i * title_bar->get_w();
                title_bar->get_y() = get_h() - tab_bar_height;
            
                // draw (graying out inactive tabs)
                glMatrixMode(GL_MODELVIEW);
                glPushMatrix();
                glTranslatef(static_cast<GLfloat>(title_bar->get_x()), static_cast<GLfloat>(title_bar->get_y()), 0);

                title_bar->draw();

                glPopMatrix();

                // also make sure of contents flags
                widget *contents = tabs[i].contents;

                if (i == active_tab_index)
                    contents->set_flags(WIDGET_INVISIBLE | WIDGET_INACTIVE, false);
                else
                    contents->set_flags(WIDGET_INVISIBLE | WIDGET_INACTIVE, true);
            }

            // draw outline
            glDisable(GL_BLEND);

            get_foreground().set();
            glLineWidth(1.0f);

            glBegin(GL_LINE_STRIP);
            glVertex2i(0, 0);
            glVertex2i(0, get_h() - tab_bar_height);
            glVertex2i(active_tab_index * tab_bar_width, get_h() - tab_bar_height);
            glVertex2i(active_tab_index * tab_bar_width, get_h());
            glVertex2i((active_tab_index+1) * tab_bar_width, get_h());
            glVertex2i((active_tab_index+1) * tab_bar_width, get_h() - tab_bar_height);
            glVertex2i(get_w(), get_h() - tab_bar_height);
            glVertex2i(get_w(), 0);
            glVertex2i(0, 0);
            glEnd();

            // contents widget will be drawn normally as it is a child...
        } // tabbox::draw()


        bool tabbox::handle_event(const SDL_Event & e)
        {
            switch (e.type)
            {
            case SDL_MOUSEBUTTONUP:

                if (e.button.button == SDL_BUTTON_LEFT)
                {
                    int mouse_x = e.button.x;
                    int mouse_y = application::global_instance()->get_console()->get_height() - e.button.y;

                    get_local(mouse_x, mouse_y);

                    for (int i = 0; i < tabs.size(); ++i)
                    {
                        int tab_x = tabs[i].title_bar->get_x();
                        int tab_y = tabs[i].title_bar->get_y();
                        int tab_w = tabs[i].title_bar->get_w();
                        int tab_h = tabs[i].title_bar->get_h();

                        if ((tab_x <= mouse_x && mouse_x <= tab_x + tab_w)
                            && (tab_y <= mouse_y && mouse_y <= tab_y + tab_h))
                        {
                            active_tab_index = i;
                            return true;
                        }
                    }
                }

                break;
            default:
                break;
            }

            return false;
        } // tabbox::handle_event()

    } // namespace framework

} // namespace gsgl
