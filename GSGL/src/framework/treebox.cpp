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

#include "treebox.hpp"
#include "data/config.hpp"
#include "platform/font.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace data;
    using namespace platform;

    namespace framework
    {

        static config_variable<int> PLUS_WIDTH(L"framework/treebox/plus_width", 8);


        class treebox_plus
            : public widget
        {
            treebox_node *parent_node;
        public:
            treebox_plus(treebox_node *parent, const color & fg, const color & bg);
            virtual ~treebox_plus();

            virtual void draw();
            virtual bool handle_event(const SDL_Event &);
        }; // class treebox_plus


        treebox_plus::treebox_plus(treebox_node *parent, const color & fg, const color & bg)
            : widget(parent, 0, 0, 0, 0, fg, bg), parent_node(parent)
        {
            //LOG_BASIC(L"ui: creating treebox plus");
        } // treebox_plus::treebox_plus()


        treebox_plus::~treebox_plus()
        {
            //LOG_BASIC(L"ui: destroying treebox plus");
        } // treebox_plus::~treebox_plus()


        void treebox_plus::draw()
        {
            if (parent_node->get_tree_nodes().size())
            {
                get_foreground().bind();
                glLineWidth(3.0f);

                glBegin(GL_LINES);

                glVertex2i(get_x(), get_y() + get_h()/2);
                glVertex2i(get_x() + get_w(), get_y() + get_h()/2);

                if (!parent_node->get_expanded())
                {
                    glVertex2i(get_x() + get_w()/2, get_y() + get_h()/2 - PLUS_WIDTH/2);
                    glVertex2i(get_x() + get_w()/2, get_y() + get_h()/2 + PLUS_WIDTH/2);
                }

                glEnd();
            }
        } // treebox_plus::draw()


        bool treebox_plus::handle_event(const SDL_Event & e)
        {
            if (e.type == SDL_MOUSEBUTTONUP)
            {
                if (parent_node->get_children().size())
                    parent_node->get_expanded() = !parent_node->get_expanded();
                return true;
            }
            return false;
        } // treebox_plus::handle_event()

        //

        treebox_node::treebox_node(treebox *parent_treebox, treebox_node *parent_node, const color & fg, const color & bg, const string & text, void *user_data)
            : widget(parent_treebox, 0, 0, 0, 0, fg, bg), parent_treebox(parent_treebox), parent_node(parent_node), plus_widget(0), text_widget(0), expanded(false), indent(0), user_data(user_data)
        {
            //LOG_BASIC(L"ui: creating treebox");

            if (parent_treebox && !parent_node)
                parent_treebox->get_tree_nodes().append(this);
            if (parent_node)
                parent_node->get_tree_nodes().append(this);

            plus_widget = new treebox_plus(this, fg, bg);
            text_widget = new textbox(this, 0, 0, 0, 0, fg, bg, 
                                      parent_treebox->get_text_font()->get_face(), 
                                      parent_treebox->get_text_font()->get_size());
            text_widget->get_text() = text;
        } // treebox_node::treebox_node()


        treebox_node::~treebox_node()
        {
            //LOG_BASIC(L"ui: destroying treebox");
            // children are deleted automatically
        } // treebox_node::~treebox_node()


        void treebox_node::clear_tree_nodes()
        {
            for (list<treebox_node *>::iterator i = tree_nodes.iter(); i.is_valid(); ++i)
            {
                (*i)->clear_tree_nodes();
                parent_treebox->remove_child(*i);
                delete *i;
            }

            tree_nodes.clear();
        } // treebox_node::clear_tree_nodes()


        void treebox_node::draw()
        {
            plus_widget->get_x() = 0;
            plus_widget->get_y() = 0;
            plus_widget->get_w() = PLUS_WIDTH;
            plus_widget->get_h() = get_h();

            text_widget->get_x() = PLUS_WIDTH;
            text_widget->get_y() = 0;
            text_widget->get_w() = get_w() - PLUS_WIDTH;
            text_widget->get_h() = get_h();

            if (this == parent_treebox->get_selected_node())
            {
                glDisable(GL_BLEND);
                get_foreground().bind();
                glLineWidth(1.0f);

                glBegin(GL_LINE_STRIP);
                glVertex2i(PLUS_WIDTH, 0);
                glVertex2i(get_w(), 0);
                glVertex2i(get_w(), get_h());
                glVertex2i(PLUS_WIDTH, get_h());
                glVertex2i(PLUS_WIDTH, 0);
                glEnd();
            }
        } // treebox_node::draw()


        bool treebox_node::handle_event(const SDL_Event & e)
        {
            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                if (button_down_here(e.button.button))
                {
                    parent_treebox->get_selected_node() = this;
                }

                // fall through -- let events pass to the plus widget
            }

            return false;
        } // treebox_node::handle_event()


        //
        static config_variable<int> SCROLL_WIDTH(L"framework/treebox/scroll_width", 16);


        treebox::treebox(widget *parent, int x, int y, int w, int h, const color & fg, const color & bg, const string & font_face, const int font_size)
            : widget(parent, x, y, w, h, fg, bg), text_font(0), side_scroll(0), selected_node(0)
        {
            text_font = new font(font_face, font_size, fg);
            side_scroll = new scrollbar(this, w-SCROLL_WIDTH, 0, SCROLL_WIDTH, h, fg, bg);
            side_scroll->set_flags(WIDGET_INVISIBLE, true);
        } // treebox::treebox()


        treebox::~treebox()
        {
            delete text_font;
        } // treebox::~treebox()


        void treebox::clear_tree_nodes()
        {
            simple_array<treebox_node *> nodes_to_remove;

            for (list<treebox_node *>::iterator i = tree_nodes.iter(); i.is_valid(); ++i)
            {
                treebox_node *n = *i;
                n->clear_tree_nodes();
                nodes_to_remove.append(n);
            }

            for (simple_array<treebox_node *>::iterator i = nodes_to_remove.iter(); i.is_valid(); ++i)
            {
                treebox_node *n = *i;
                remove_child(n);
                delete n;
            }

            tree_nodes.clear();
        } // treebox::clear_tree_nodes()


        void treebox::draw()
        {
            const int list_item_height = text_font->get_size() * 4 / 3;

            // go through nodes, building a list of the expanded ones (even if out of the visible box)
            nodes_to_draw.clear();

            for (list<treebox_node *>::iterator n = tree_nodes.iter(); n.is_valid(); ++n)
            {
                mark_expanded_nodes(*n, true, 0);
            }

            // readjust scroll bar
            side_scroll->get_min() = 0;
            side_scroll->get_max() = nodes_to_draw.size();
            side_scroll->get_extent() = min_val(get_h() / list_item_height, nodes_to_draw.size());

            // now get the index of the first node to draw, and the number of nodes to draw
            bool draw_scroll_bar = nodes_to_draw.size() > side_scroll->get_extent();
            int start_index = side_scroll->get_pos();
            int num_to_draw = draw_scroll_bar ? side_scroll->get_extent() : nodes_to_draw.size();

            // mark all the other nodes invisible and shift y coordinates
            for (int i = 0; i < nodes_to_draw.size(); ++i)
            {
                if (i < start_index || i > start_index+num_to_draw)
                {
                    nodes_to_draw[i]->set_flags(WIDGET_INVISIBLE, true);
                }
                else
                {
                    treebox_node *n = nodes_to_draw[i];

                    n->set_flags(WIDGET_INVISIBLE, false);
                    n->get_x() = get_x() + n->get_indent() * PLUS_WIDTH;
                    n->get_y() = get_y() + get_h() - (list_item_height * (1 + i - start_index));
                    n->get_w() = get_w() - (n->get_indent()*PLUS_WIDTH + (draw_scroll_bar ? SCROLL_WIDTH : 0));
                    n->get_h() = list_item_height;
                }
            }

            // we are done; the nodes are children and will be drawn automatically
        } // treebox::draw()


        void treebox::mark_expanded_nodes(treebox_node *n, bool visible, int indent)
        {
            if (visible)
                nodes_to_draw.append(n);

            n->set_flags(WIDGET_INVISIBLE, !visible);
            n->get_indent() = indent;

            for (list<treebox_node *>::iterator child = n->get_tree_nodes().iter(); child.is_valid(); ++child)
            {
                mark_expanded_nodes(*child, visible && n->get_expanded(), indent+1);
            }
        } // treebox::mark_expanded_nodes()

    } // namespace framework

} // namespace gsgl
