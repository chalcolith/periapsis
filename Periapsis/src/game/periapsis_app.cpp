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

#include "periapsis_app.hpp"
#include "main_window.hpp"

#include "data/broker.hpp"
#include "data/directory.hpp"
#include "data/file.hpp"

#include "platform/texture.hpp"

#include "scenegraph/context.hpp"
#include "scenegraph/simulation.hpp"

#include "space/space_context.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::io;
using namespace gsgl::platform;
using namespace gsgl::framework;

namespace periapsis
{

    periapsis_app::periapsis_app(const string & title, const int & argc, const char **argv)
        : application(title, argc, argv),
          sim_context(0), draw_context(0)
    {
        sim_context = new gsgl::scenegraph::simulation_context();
        draw_context = new space::space_drawing_context();
    } // periapsis_app::periapsis_app()


    periapsis_app::~periapsis_app()
    {
        // main window will be deleted in application destructor

        delete sim_context;
        delete draw_context;
    } // periapsis_app::~periapsis_app()


    void periapsis_app::init()
    {
        // initialize main window
        int sw = get_console()->get_width();
        int sh = get_console()->get_height();
        int x = (sw / 2) - (main_window::WIDTH / 2);
        int y = (sh / 2) - (main_window::HEIGHT / 2);

        widgets.push(new main_window(*global_console, get_title(), x, y));
    } // periapsis_app::init()


    void periapsis_app::update()
    {
    } // periapsis_app::update()


    void periapsis_app::cleanup()
    {
        delete sim_context;
        sim_context = 0;
        
        delete draw_context;
        draw_context = 0;
    } // periapsis_app::cleanup()

} // namespace periapsis
