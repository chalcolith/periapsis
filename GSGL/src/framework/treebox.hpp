#ifndef GSGL_FRAMEWORK_TREEBOX_H
#define GSGL_FRAMEWORK_TREEBOX_H

//
// $Id: treebox.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "framework/textbox.hpp"
#include "framework/scrollbar.hpp"
#include "data/array.hpp"
#include "data/list.hpp"

namespace gsgl
{

    namespace platform
    {
        class font;
    }


    namespace framework
    {

        class treebox;
        class treebox_plus;


        class FRAMEWORK_API treebox_node
            : public widget
        {
            treebox      *parent_treebox;
            treebox_node *parent_node;

            treebox_plus *plus_widget;
            textbox *text_widget;

            bool expanded;
            int indent;

            data::list<treebox_node *> tree_nodes;

            void *user_data;

        public:

            treebox_node(treebox *parent_treebox, treebox_node *parent_node, 
                         const platform::color & fg, const platform::color & bg, 
                         const gsgl::string & text, void *user_data);
            virtual ~treebox_node();

            treebox *get_parent_treebox() { return parent_treebox; }

            bool & get_expanded() { return expanded; }
            int & get_indent() { return indent; }

            data::list<treebox_node *> & get_tree_nodes() { return tree_nodes; }
            void clear_tree_nodes();

            /// Get or set user data for this node.
            void *& get_user_data() { return user_data; }

            //
            virtual void draw();
            virtual bool handle_event(const SDL_Event &);
        }; // class treebox_node


        class FRAMEWORK_API treebox
            : public widget
        {
            platform::font *text_font;

            scrollbar *side_scroll;
            data::list<treebox_node *> tree_nodes;

            treebox_node *selected_node;

        public:
            treebox(widget *parent, int x, int y, int w, int h, const platform::color & fg, const platform::color & bg, const gsgl::string & font_face, const int font_size);
            virtual ~treebox();

            platform::font *get_text_font() { return text_font; }
            data::list<treebox_node *> & get_tree_nodes() { return tree_nodes; }
            void clear_tree_nodes();

            treebox_node *& get_selected_node() { return selected_node; }

            virtual void draw();

        private:
            data::simple_array<treebox_node *> nodes_to_draw;

            void mark_expanded_nodes(treebox_node *n, bool visible, int indent);
        }; // class treebox


    } // namespace framework

} // namespace gsgl

#endif
