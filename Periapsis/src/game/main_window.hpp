#ifndef PERIAPSIS_MAIN_WINDOW_H
#define PERIAPSIS_MAIN_WINDOW_H

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

#include "data/singleton.hpp"
#include "data/string.hpp"
#include "data/config.hpp"
#include "framework/widget.hpp"

namespace gsgl
{
    namespace framework
    {
        class textbox;
        class button;
        class tabbox;
    }
}


namespace periapsis
{

    class periapsis_app;
    class simulation_tab;
    class settings_tab;

    class load_scenery_thread;


    class main_window
        : public gsgl::framework::widget, public gsgl::data::singleton<main_window>
    {
        gsgl::framework::textbox *title_box, *status_bar;
        gsgl::framework::button  *quit_button;
        gsgl::framework::tabbox  *tab_box;
        simulation_tab *sim_tab;
        settings_tab   *set_tab;

        bool need_to_load_scenery;
        load_scenery_thread *loading_thread;

    public:
        main_window(const gsgl::string & title, const int x, const int y);
        virtual ~main_window();

        //
        void set_status(const gsgl::string & message);

        //
        virtual void draw();

        //

        static gsgl::data::config_variable<int>                   WIDTH;
        static gsgl::data::config_variable<int>                   HEIGHT;
        static gsgl::data::config_variable<gsgl::platform::color> FOREGROUND;
        static gsgl::data::config_variable<gsgl::platform::color> BACKGROUND;
        static gsgl::data::config_variable<gsgl::string>          FONT_FACE;
        static gsgl::data::config_variable<int>                   FONT_SIZE;

        static gsgl::data::config_variable<int> TITLE_BOX_HEIGHT;
        static gsgl::data::config_variable<int> STATUS_BAR_HEIGHT;
        static gsgl::data::config_variable<int> TAB_BOX_SPACE;
        static gsgl::data::config_variable<int> QUIT_BUTTON_WIDTH;
    }; // class main_window

} // namespace periapsis

#endif
