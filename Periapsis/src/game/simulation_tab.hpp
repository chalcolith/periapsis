#ifndef PERIAPSIS_SIMULATION_TAB_H
#define PERIAPSIS_SIMULATION_TAB_H

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

#include "datetime_box.hpp"
#include "sim_view_box.hpp"

namespace gsgl
{

    namespace data
    {
        class config_record;
    }

    namespace framework
    {
        class textbox;
        class button;
    }

    namespace scenegraph
    {
        class node;
    }

} // namespace gsgl



namespace periapsis
{

    namespace space
    {
        class space_context;
    }


    class simulation_tab
        : public gsgl::framework::widget
    {
        gsgl::framework::textbox *title_box;
        datetime_box *time_box;
        sim_view_box *view_box;

        gsgl::framework::button *load_button;
        gsgl::framework::button *save_button;
        gsgl::framework::button *go_button;

        bool waiting_for_scenery;
        bool sim_is_ready;

        gsgl::data::config_record *current_sim_record;

    public:
        simulation_tab();
        virtual ~simulation_tab();

        datetime_box *get_time_box() { return time_box; }
        sim_view_box *get_view_box() { return view_box; }
        bool & get_waiting_for_scenery() { return waiting_for_scenery; }
        bool & get_sim_is_ready() { return sim_is_ready; }

        virtual void draw();

    private:
        friend void handle_run_simulation(gsgl::framework::button *);
    }; // class simulation_tab

} // namespace periapsis

#endif
