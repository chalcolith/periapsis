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

#include "simulation_tab.hpp"
#include "main_window.hpp"
#include "periapsis_app.hpp"

#include "data/exception.hpp"
#include "data/pointer.hpp"
#include "data/config.hpp"
#include "data/file.hpp"
#include "data/directory.hpp"
#include "math/time.hpp"
#include "math/vector.hpp"
#include "scenegraph/node.hpp"
#include "scenegraph/simulation.hpp"
#include "space/space_context.hpp"

#include "framework/textbox.hpp"
#include "framework/button.hpp"
#include "framework/treebox.hpp"
#include "framework/application.hpp"


using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::io;
using namespace gsgl::math;
using namespace gsgl::framework;
using namespace gsgl::scenegraph;

namespace periapsis
{

    static config_variable<int> BUTTON_SPACE(L"ui/simulation_tab/button_space", 10);


    simulation_tab::simulation_tab(platform::display & screen)
        : widget(screen, 0, 0, 0, 0, 0, main_window::FOREGROUND, main_window::BACKGROUND),
          title_box(0), time_box(0), view_box(0), vehicle_box(0),
          load_button(0), save_button(0), go_button(0), 
          waiting_for_scenery(false),
          waiting_for_vehicles(false),
          sim_is_ready(false),
          current_sim_record(0)
    {
    } // simulation_tab::simulation_tab()


    simulation_tab::~simulation_tab()
    {
        // children are deleted automatically
    } // simulation_tab::~simulation_tab()


    static void handle_run_simulation(button *);

    void simulation_tab::draw()
    {
        int eighth = get_h() / 8;

        // initialize children on first draw (since the parent has handled sizing...)
        if (!title_box)
        {
            int baseline = 7 * eighth;

            title_box = new textbox(get_screen(), this, 
                                    BUTTON_SPACE, baseline + BUTTON_SPACE, 
                                    get_w() - 2*BUTTON_SPACE, eighth - 2*BUTTON_SPACE, 
                                    main_window::FOREGROUND, main_window::BACKGROUND, 
                                    main_window::FONT_FACE, main_window::FONT_SIZE);
            title_box->get_text() = L"<simulation description>";
        }


        if (!time_box)
        {
            int baseline = 6 * eighth + eighth/2;

            time_box = new datetime_box(get_screen(), this,
                                        BUTTON_SPACE, baseline + BUTTON_SPACE,
                                        get_w() - 2*BUTTON_SPACE, main_window::FONT_SIZE*4/3,
                                        main_window::FOREGROUND, main_window::BACKGROUND);                                        

            //time_box->get_text() = L"1988-6-19 12:0:0";
            //time_box->get_jdn() = 2026871.8;
            //time_box->get_jdn() = julian_day::now().get_jdn();
            time_box->set_text(L"2007-03-20 12:0:0"); // vernal equinox, for testing
        }

        
        if (!view_box)
        {
            int baseline = eighth/2 + BUTTON_SPACE/2;

            view_box = new sim_view_box(get_screen(), this,
                                        BUTTON_SPACE, baseline + BUTTON_SPACE,
                                        get_w()/2 - 2*BUTTON_SPACE, 6*eighth - 2*BUTTON_SPACE,
                                        main_window::FOREGROUND, main_window::BACKGROUND);
            treebox *view_tree = view_box->get_scenery_box();
            treebox_node *temp_node = new treebox_node(get_screen(), view_tree, 0, main_window::FOREGROUND, main_window::BACKGROUND, L"Loading...", 0);

            waiting_for_scenery = true;
        }

        //if (!vehicle_box)
        //{
        //    int baseline = eight/2 + BUTTON_SPACE/2;

        //    vehicle_box = new sim_vehicle_box(this,
        //                                      BUTTON_SPACE, baseline + BUTTON_SPACE,
        //                                      get_w() / 2 - 2 * BUTTON_SPACE, 6 * eighth - 2*BUTTON_SPACE,
        //                                      main_window::FOREGROUND, main_window::BACKGROUND);
        //    treebox *vehicle_tree = vehicle_box->get_vehicle_box();
        //    treebox_node *temp_node = new treebox_node(vehicle_tree, 0, main_window::FOREGROUND, main_window::BACKGROUND, L"Loading...", 0);

        //    waiting_for_vehicles = true;
        //}


        if (!load_button)
        {
            load_button = new button(get_screen(), this, 
                                     BUTTON_SPACE, BUTTON_SPACE, 
                                     (get_w() / 3) - 2*BUTTON_SPACE, main_window::FONT_SIZE * 4 / 3, 
                                     main_window::FOREGROUND, main_window::BACKGROUND, 
                                     main_window::FONT_FACE, main_window::FONT_SIZE, 
                                     L"Load Simulation");
        }

        if (!save_button)
        {
            save_button = new button(get_screen(), this,
                                     (get_w() / 3) + BUTTON_SPACE, BUTTON_SPACE,
                                     (get_w() / 3) - 2*BUTTON_SPACE, main_window::FONT_SIZE * 4 / 3,
                                     main_window::FOREGROUND, main_window::BACKGROUND,
                                     main_window::FONT_FACE, main_window::FONT_SIZE,
                                     L"Save Simulation");
        }

        if (!go_button)
        {
            go_button = new button(get_screen(), this,
                                   (get_w() * 2 / 3) + BUTTON_SPACE, BUTTON_SPACE,
                                   (get_w() / 3) - 2*BUTTON_SPACE, main_window::FONT_SIZE * 4 / 3,
                                   main_window::FOREGROUND, main_window::BACKGROUND,
                                   main_window::FONT_FACE, main_window::FONT_SIZE,
                                   L"Run Simulation");
            go_button->set_on_click_handler(&handle_run_simulation);
        }

        // check for scenery & sim
        if (waiting_for_scenery)
        {
            if (application::global_instance()->get_global_scenery())
            {
                view_box->load_scenery_info();
                waiting_for_scenery = false;
            }
        }

        // check for vehicles
        if (waiting_for_vehicles)
        {

        }

        // check for simulation
        sim_is_ready = (time_box->get_jdn() != 0) && (view_box->get_scenery_box()->get_selected_node() != 0);

        if (sim_is_ready)
        {
            go_button->set_flags(widget::WIDGET_INACTIVE, false);
        }
        else
        {
            go_button->set_flags(widget::WIDGET_INACTIVE, true);
        }

        // now draw per usual
        //widget::draw();
    } // simulation_tab::draw()


    static string get_temp_sim_fname()
    {
        return application::USER_DATA_PATH + L"Currently Running.sim";
    } // simulation_tab::get_temp_sim_fname()


    static void handle_run_simulation(button *b)
    {
        simulation_tab *stab = dynamic_cast<simulation_tab *>(b->get_parent());
        if (!stab)
            throw internal_exception(__FILE__, __LINE__, L"Run Simulation button is not correctly parented.");

        // create new simulation if necessary
        config_record *sim_rec = stab->current_sim_record;
        if (!sim_rec)
        {
            string fname = get_temp_sim_fname();
            if (file::exists(fname))
                file::remove(fname);

            // initialize file
            {
                file f(fname);
                smart_pointer<ft_stream> s(f.open_text(io::FILE_OPEN_WRITE));
                *s << L"<simulation name=\"Running Simulation\"></simulation>";
            }

            sim_rec = new config_record(fname);
        }

        // insert time node if necessary
        config_record & time_param = sim_rec->get_child(L"start_time");
        if (time_param.get_text().is_empty())
        {
            time_param.get_text() = string::format(L"%f", stab->get_time_box()->get_jdn());
        }

        // insert view node if necessary
        config_record & view_param = sim_rec->get_child(L"viewpoint");
        if (view_param.get_text().is_empty())
        {
            node *n = reinterpret_cast<node *>(stab->get_view_box()->get_scenery_box()->get_selected_node()->get_user_data());
            assert(n);

            view_param[L"parent"] = n->get_name();
        }

        // save simulation record, and run
        string fname = sim_rec->get_file().get_full_path();
        sim_rec->save();
        delete sim_rec;
        sim_rec = 0;

        periapsis_app *app = dynamic_cast<periapsis_app *>(application::global_instance());
        if (app)
        {
            app->load_and_run_simulation(fname, app->get_sim_context(), app->get_draw_context());
        }
        else
        {
            throw runtime_exception(L"You cannot run a Periapsis simulation within a different application!");
        }
    } // handle_run_simulation()


} // namespace periapsis
