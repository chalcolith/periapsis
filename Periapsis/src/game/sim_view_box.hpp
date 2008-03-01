#ifndef PERIAPSIS_SIM_VIEW_BOX_H
#define PERIAPSIS_SIM_VIEW_BOX_H

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
#include "framework/textbox.hpp"
#include "framework/treebox.hpp"

namespace gsgl
{
    namespace scenegraph
    {
        class node;
    }
}

namespace periapsis
{

    class sim_view_box
        : public gsgl::framework::widget
    {
        gsgl::framework::textbox *title_box;
        gsgl::framework::treebox *scenery_box;
    public:
        sim_view_box(gsgl::framework::widget *parent,
                     int x, int y, int w, int h,
                     const gsgl::platform::color & fg, const gsgl::platform::color & bg);
        virtual ~sim_view_box();

        gsgl::framework::treebox *get_scenery_box() { return scenery_box; }


        //
        void load_scenery_info();

    private:
        gsgl::framework::treebox_node *load_scenery_info(gsgl::scenegraph::node *n, gsgl::framework::treebox *box, gsgl::framework::treebox_node *parent_node);
    }; // class sim_view_box

} // namespace periapsis

#endif
