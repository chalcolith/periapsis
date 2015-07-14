#ifndef periapsis_app_H
#define periapsis_app_H

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

#include "main_window.hpp"

#include "data/pointer.hpp"
#include "framework/application.hpp"

namespace gsgl
{
    namespace scenegraph
    {
        class simulation_context;
    }
}

namespace periapsis
{
    namespace space
    {
        class space_drawing_context;
    }


    //

    class periapsis_app
        : public gsgl::framework::application
    {
        gsgl::scenegraph::simulation_context *sim_context;
        space::space_drawing_context   *draw_context;

    public:
        periapsis_app(const gsgl::string & title, const int & argc, const char **argv);
        ~periapsis_app();

        gsgl::scenegraph::simulation_context *get_sim_context() { return sim_context; }
        space::space_drawing_context   *get_draw_context() { return draw_context; }

        virtual void init();
        virtual void update();
        virtual void cleanup();
    }; // class periapsis_app

} // namespace periapsis

#endif
