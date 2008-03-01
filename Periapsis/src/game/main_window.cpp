//
// $Id: main_window.cpp 315 2008-03-01 16:33:59Z Gordon $
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


#include "platform/platform.hpp"
#include "platform/thread.hpp"
#include "framework/textbox.hpp"
#include "framework/button.hpp"
#include "framework/tabbox.hpp"

#include "main_window.hpp"
#include "periapsis_app.hpp"
#include "simulation_tab.hpp"
#include "scenery_tab.hpp"
#include "settings_tab.hpp"

#include "platform/lowlevel.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::platform;
using namespace gsgl::framework;


// main window instance
namespace gsgl
{
    namespace data
    {
        periapsis::main_window *periapsis::main_window::instance = 0;
    }
}


namespace periapsis
{

    class load_scenery_thread
        : public gsgl::platform::thread
    {
    public:
        synchronized<bool> loading_failed;
        synchronized<string> loading_message;

    protected:
        virtual int run();
    }; // class load_scenery_thread


    //

    int load_scenery_thread::run()
    {
        try
        {
            loading_message = string(L"Loading scenery...");
            application::global_instance()->load_scenery(loading_message);
            loading_message = string(L"Finished loading scenery.");
            return 1;
        }
        catch (gsgl::exception & e)
        {
            loading_message = string(e.get_message()).copy();
            loading_failed = true;
            return 0;
        }
    } // load_scenery_thread::run()


    //////////////////////////////////////////////////////////////////

    //
    config_variable<int>    main_window::WIDTH      (L"ui/main_window/width",      800);
    config_variable<int>    main_window::HEIGHT     (L"ui/main_window/height",     600);
    config_variable<color>  main_window::FOREGROUND (L"ui/main_window/foreground", color(1, 1, 1, 1));
    config_variable<color>  main_window::BACKGROUND (L"ui/main_window/background", color(0, 0, 0.6f, 0.6f));
    config_variable<string> main_window::FONT_FACE  (L"ui/main_window/font_face",  L"Sans");
    config_variable<int>    main_window::FONT_SIZE  (L"ui/main_window/font_size",  18);

    //
    config_variable<int> main_window::TITLE_BOX_HEIGHT  (L"ui/main_window/spacing/title_box_height",  32);
    config_variable<int> main_window::STATUS_BAR_HEIGHT (L"ui/main_window/spacing/status_bar_height", 24);
    config_variable<int> main_window::TAB_BOX_SPACE     (L"ui/main_window/spacing/tab_box_space",     4);
    config_variable<int> main_window::QUIT_BUTTON_WIDTH (L"ui/main_window/spacing/quit_button_width", 64);

    //
    static void handle_quit(button *);

    //

    main_window::main_window(const string & title, const int x, const int y)
        : widget(0, x, y, WIDTH, HEIGHT, FOREGROUND, BACKGROUND), gsgl::data::singleton<main_window>(),
          title_box(0), status_bar(0), tab_box(0), sim_tab(0), set_tab(0),
          need_to_load_scenery(true), loading_thread(0)
    {
        title_box = new textbox(this, 
                                0, HEIGHT - TITLE_BOX_HEIGHT, 
                                WIDTH, TITLE_BOX_HEIGHT, 
                                FOREGROUND, BACKGROUND, 
                                FONT_FACE, FONT_SIZE * 4 / 3);
        title_box->get_text() = title;

        //
        quit_button = new button(this, WIDTH - QUIT_BUTTON_WIDTH, HEIGHT - TITLE_BOX_HEIGHT*3/4, QUIT_BUTTON_WIDTH, TITLE_BOX_HEIGHT * 3 / 4, FOREGROUND, BACKGROUND, FONT_FACE, FONT_SIZE, L"Quit");
        quit_button->set_on_click_handler(&handle_quit);

        //
        status_bar = new textbox(this, 0, 0, WIDTH, STATUS_BAR_HEIGHT, FOREGROUND, BACKGROUND, FONT_FACE, FONT_SIZE);
        status_bar->get_text() = L"Welcome to Periapsis, the spaceflight simulator.";

        //
        sim_tab = new simulation_tab();
        set_tab = new settings_tab();

        tab_box = new tabbox(this, 
                             2*TAB_BOX_SPACE, STATUS_BAR_HEIGHT+2*TAB_BOX_SPACE, 
                             WIDTH-4*TAB_BOX_SPACE, HEIGHT - (TITLE_BOX_HEIGHT + STATUS_BAR_HEIGHT + 4*TAB_BOX_SPACE), 
                             FOREGROUND, BACKGROUND, 
                             FONT_FACE, FONT_SIZE, 
                             TITLE_BOX_HEIGHT * 2 / 3);
        tab_box->add_tab(L" Simulation", sim_tab);
        tab_box->add_tab(L" Settings", set_tab);
    } // main_window::main_window()


    main_window::~main_window()
    {
        delete loading_thread;
        // children are automatically deleted
    } // main_window::~main_window()


    void main_window::set_status(const string & message)
    {
        status_bar->get_text() = message;
    } // main_window::set_status()


    //

    void main_window::draw()
    {
        // start scenery loading thread if necessary
        if (need_to_load_scenery)
        {
            need_to_load_scenery = false;

            if (application::global_instance()->get_global_scenery())
                application::global_instance()->unload_scenery();

            if (loading_thread)
                delete loading_thread;
            
            loading_thread = new load_scenery_thread();
            loading_thread->loading_failed = false;
            loading_thread->start();
        }

        // check for scenery loading errors
        if (loading_thread && loading_thread->is_running())
        {
            if (loading_thread->loading_failed)
            {
                string temp = string(loading_thread->loading_message).copy();
                application::global_instance()->unload_scenery();
                throw runtime_exception(temp.w_string());
            }

            // update status
            set_status(loading_thread->loading_message);
        }

        // draw
        widget::draw();
    } // main_window::draw()


    //

    static void handle_quit(button *)
    {
        application::global_instance()->quit_application();
    } // handle_quit()


} // namespace periapsis
