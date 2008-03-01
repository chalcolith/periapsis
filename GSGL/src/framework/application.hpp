#ifndef GSGL_FRAMEWORK_APPLICATION_H
#define GSGL_FRAMEWORK_APPLICATION_H

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

#include "framework/framework.hpp"
#include "framework/package.hpp"
#include "framework/widget.hpp"

#include "data/singleton.hpp"
#include "data/list.hpp"
#include "data/string.hpp"
#include "data/config.hpp"
#include "data/stack.hpp"

// forward declarations
union SDL_Event;

namespace gsgl
{

    namespace data
    {
        class log;
        class broker;
    } // namespace data

    
    namespace platform
    {
        class budget;
        class display;
        class texture;

        template <typename T>
        class synchronized;
    } // namespace platform


    namespace scenegraph
    {
        class event_map;
        class context;
        class simulation;
        class node;
    } // namespace scenegraph


    namespace framework
    {

        /// Base class for applications.
        class FRAMEWORK_API application
            : public framework_object, public gsgl::data::singleton<application>
        {
            data::list<gsgl::data_object *> global_caches;

        protected:
            enum app_state
            {
                APP_NO_STATE = 0,
                APP_INITIALIZING,
                APP_UI_RUNNING,
                APP_SIM_LOADING,
                APP_SIM_RUNNING,
                APP_QUITTING,
                APP_DEAD
            };

            app_state state;

            gsgl::string title;
            
            //
            data::list<package *> loaded_packages;            
            platform::texture *splash_screen;

            //
            data::simple_stack<widget *> widgets;
            widget *focus_widget;

            //
            scenegraph::context    *global_context;
            scenegraph::node       *global_scenery;
            scenegraph::simulation *global_simulation;
            
            platform::display      *global_console;
            scenegraph::event_map  *global_mapper;

            platform::budget       *global_budget;

        public:

            application(const string & title, const int & argc, const char **argv);
            virtual ~application();

            /// \name System Configuration Variables
            /// \{

            static data::config_variable<gsgl::string> PROGRAM_PATH;      ///< The path that the program is running in.
            static data::config_variable<gsgl::string> SYS_DATA_PATH;     ///< The directory of the system data.
            static data::config_variable<gsgl::string> USER_DATA_PATH;    ///< The user's data directory.
            static data::config_variable<gsgl::string> EVENT_MAP_PATH;    ///< The program's event map.
            static data::config_variable<gsgl::string> USER_CONFIG_PATH;  ///< The program's user config file.

            /// \}

            /// \name Accessors
            /// @{

            string & get_title() { return title; }

            scenegraph::node           *get_global_scenery() { return global_scenery; }
            platform::display          *get_console()        { return global_console; }
            scenegraph::simulation     *get_simulation()     { return global_simulation; }
            platform::budget           *get_global_budget()  { return global_budget; }

            data::simple_stack<widget *> & get_widgets() { return widgets; }
            widget *get_focus_widget() { return focus_widget; } ///< Return the widget with the current keyboard focus.

            void get_mousedown_info(int & button, int & x, int & y);
            /// @}

            /// \name Game Lifecycle Functions
            /// @{

            /// Load a game package containing specifications for shared libraries, scenery, and vehicles.
            void load_package(const string & fname);
            const gsgl::data::list<package *> & get_loaded_packages() const { return loaded_packages; }

            void load_scenery(platform::synchronized<string> & status_string);
            void unload_scenery();

            void load_and_run_simulation(const string & fname, gsgl::scenegraph::context *c); ///< Loads and runs a simulation.  This will set the top widget to be invisible.
            void unload_and_quit_simulation();                  ///< Unloads and quits a simulation.  This will set the top widget to be visible, so make sure that any extra widgets have been popped off the stack.
            void quit_application();                            ///< Quits the application.  Unloads and quits any simulation loaded.

            ///< This contains the main event loop of the application.  It clears the screen, calls draw(), draws UI elements, and flips the buffers.
            void run();
            
            /// @}


            /// \name Virtual Functions
            /// @{
            virtual void init();    ///< This function is called by the constructor.  Do not call it yourself.
            virtual bool draw();    ///< This function is called for every frame.  Do not call it yourself.  If it returns false and the sim pointer is valid, the sim's draw function is called.
            virtual void update();  ///< This function is called after every frame.  Do not call it yourself.
            virtual void cleanup(); ///< This function when the application ends.  Do not call it yourself.
            
            virtual bool handle_event(const SDL_Event & e); ///< Handles SDL events.  Does not normally need to be reimplemented, as UI elements and scenegraph events will propagate by default.
            /// @}
            
        private:
            void get_config_overrides(const int & argc, const char **argv);
            bool get_cmdline_dir(const int & argc, const char **argv, const int & pos, const gsgl::string & arg_key, gsgl::data::config_record & conf, const gsgl::string & conf_key);
            gsgl::string get_program_dir(const gsgl::string & arg);
            gsgl::string get_user_dir();

            //
            scenegraph::node *load_objects(const data::config_record & obj_config, platform::synchronized<string> & status_string);
            void remove_viewpoint_nodes(scenegraph::node *);

            //
            void draw_splash_screen();
            void draw_ui(framework::widget *);
            void draw_budget(unsigned int ticks);

            //
            int mouse_button_pressed;
            int mouse_button_x, mouse_button_y;

            bool handle_ui_event(const SDL_Event & e, widget *w);
            bool handle_ui_event(const SDL_Event & e, widget *w, int mouse_x, int mouse_y);
        }; // class application
        
    } // namespace framework
    
} // namespace gsgl

#endif
