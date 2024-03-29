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

#include "framework/application.hpp"
#include "framework/widget.hpp"

#include "data/log.hpp"
#include "data/exception.hpp"
#include "data/list.hpp"
#include "data/file.hpp"
#include "data/directory.hpp"
#include "data/broker.hpp"

#include "platform/thread.hpp"
#include "platform/budget.hpp"
#include "platform/display.hpp"
#include "platform/texture.hpp"

#include "scenegraph/event_map.hpp"
#include "scenegraph/simulation.hpp"
#include "scenegraph/model.hpp"

#include "platform/lowlevel.hpp"


#ifndef WIN32
#include <dlfcn.h>
#endif

#if defined (__OpenBSD__)
#define DLOPEN_FLAG DL_LAZY
#elif defined (__linux__)
#define DLOPEN_FLAG RTLD_NOW
#elif defined (WIN32)
#elif defined (__APPLE__)
#define DLOPEN_FLAG RTLD_NOW
#else
#error You must specify the correct flag to dlopen() for your OS.
#endif


namespace gsgl
{

    using namespace data;
    using namespace io;
    using namespace platform;
    using namespace scenegraph;
    

    // global application instance
    framework::application *framework::application::instance = 0;


    namespace framework
    {

        config_variable<string> application::PROGRAM_PATH(L"paths/program", L".");
        config_variable<string> application::SYS_DATA_PATH(L"paths/sys_data", L"data");
        config_variable<string> application::USER_DATA_PATH(L"paths/user_data", L"user");
        config_variable<string> application::EVENT_MAP_PATH(L"paths/event_map", L"EventMap.cfg");
        config_variable<string> application::USER_CONFIG_PATH(L"paths/user_config", L"UserConfig.cfg");

        
        /// Creates the global application object.
        /// This will also create the global application broker, event map and console objects.
        /// It will parse the command line to determine the correct system and user data directories.
        application::application(const string & title, const int & argc, const char **argv) 
            : framework_object(), singleton<application>(),
              state(APP_NO_STATE), title(title),
              splash_screen(0), focus_widget(0), 
              global_sim_context(0), global_draw_context(0),
              global_scenery(0), global_simulation(0),
              global_console(0), global_mapper(0),
              global_budget(0), budget_font(0)
        {
            // override global config
            get_config_overrides(argc, argv);

            // global budget
            global_budget = new budget();

            // init event mapper
            global_mapper = new event_map(EVENT_MAP_PATH);

            // init SDL
            if (TTF_Init() == -1)
                throw runtime_exception(L"SDL Font System Error: %hs.", TTF_GetError());

            if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_JOYSTICK) == 0)
            {
                try
                {
                    SDL_WM_SetCaption(title.c_string(), title.c_string());
                    SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
                    SDL_EnableUNICODE(1);

                    global_console = new display(platform::display::DISPLAY_WIDTH, platform::display::DISPLAY_HEIGHT, true);
                }
                catch (exception & e)
                {
                    throw runtime_exception(L"Error: %s.", e.get_message());
                }
            }
            else
            {
                throw runtime_exception(L"Unable to initialize SDL: %hs", SDL_GetError());
            }

            // load splash screen if present
            string splashscreen_path = SYS_DATA_PATH + L"splashscreen.png";
            if (io::file::exists(splashscreen_path))
                splash_screen = new texture(L"splashscreen", splashscreen_path, texture::TEXTURE_ENV_REPLACE);

            // load default package
            string pkg_path = SYS_DATA_PATH + L"Default.package";
            if (io::file::exists(pkg_path))
                load_package(pkg_path);

            // load user packages
            const config_record & packages_config = const_cast<config_record &>(global_config::get_config()).get_child(L"packages");
            for (list<config_record>::const_iterator pkg = packages_config.get_children().iter(); pkg.is_valid(); ++pkg)
            {
                if (pkg->get_name() == L"package")
                {
                    const string & filename = pkg->get_attribute(L"filename");
                    if (!filename.is_empty())
                    {
                        load_package(SYS_DATA_PATH + filename);
                    }
                }
            }
        } // application::application()


        application::~application()
        {
            // clean up SDL (also destroys the OpenGL context)
            TTF_Quit();
            SDL_Quit();
        } // application::~application()


        void application::load_package(const string & fname)
        {
            for (list<package *>::iterator i = loaded_packages.iter(); i.is_valid(); ++i)
            {
                if ((*i)->get_fname() == fname)
                    throw runtime_exception(L"Attempted to load duplicate package %ls", fname.w_string());
            }

            loaded_packages.append(new package(fname));
        } // application::load_package()


        void application::load_and_run_simulation(const string & fname, gsgl::scenegraph::simulation_context *sim_context, gsgl::scenegraph::drawing_context *draw_context)
        {
            assert(global_scenery);
            assert(sim_context);
            assert(draw_context);

            try
            {
                if (global_simulation)
                    unload_and_quit_simulation();

                gsgl::log(string(L"application: loading simulation ") + fname);

                state = APP_SIM_LOADING;
                config_record sim_config(fname);

                global_sim_context = sim_context;
                global_draw_context = draw_context;
                global_simulation = new simulation(sim_config, global_console, global_sim_context, global_draw_context, global_scenery);
                global_simulation->init();

                if (widgets.size())
                {
                    widgets.top()->set_flags(widget::WIDGET_INVISIBLE, true);
                }

                state = APP_SIM_RUNNING;
            }
            catch (gsgl::exception &)
            {
                quit_application();
                throw;
            }
        } // application::load_and_run_simulation()


        void application::unload_and_quit_simulation()
        {
            gsgl::log(L"application: unloading simulation");

            // free simulation
            if (global_simulation)
            {
                global_simulation->cleanup();
                delete global_simulation;
                global_simulation = 0;
            }

            // calling code will free context
            if (global_sim_context)
            {
                global_sim_context = 0;
            }

            if (global_draw_context)
            {
                global_draw_context = 0;
            }

            // remove viewpoint node
            if (global_scenery)
            {
                remove_viewpoint_nodes(global_scenery);
            }

            // activate main widget
            if (widgets.size())
            {
                widgets.top()->set_flags(widget::WIDGET_INVISIBLE, false);
            }

            state = APP_UI_RUNNING;
        } // application::unload_and_quit_simulation()


        void application::quit_application()
        {
            if (global_simulation)
                unload_and_quit_simulation();

            state = APP_QUITTING;
            cleanup();

            // delete all widgets
            while (widgets.size())
            {
                delete widgets.top();
                widgets.pop();
            }

            // clean up
            delete global_simulation; global_simulation = 0;
            delete global_scenery;    global_scenery = 0;

            delete splash_screen;     splash_screen = 0;

            delete global_console;    global_console = 0;
            delete global_mapper;     global_mapper = 0;

            delete budget_font;       budget_font = 0;
            delete global_budget;     global_budget = 0;

            // this stuff is here rather than in the destructor because these might throw
            model::clear_cache(L"__ALL__");
            material::clear_cache(L"__ALL__");
            font::clear_cache();
            texture::clear_cache(L"__ALL__");
        } // application::quit_application()


        void application::run()
        {
            if (state != APP_NO_STATE)
                throw runtime_exception(L"You cannot call application::run() twice!");

            // initialize
            state = APP_INITIALIZING;
            this->init();

            // main loop
            state = APP_UI_RUNNING;
            bool should_draw_budget = false;
            unsigned int start_tick, end_tick;
            global_budget->reset();
            start_tick = SDL_GetTicks();

            while (state != APP_QUITTING)
            {
                // draw
                try
                {
                    // clear screen
                    display::scoped_viewport view(*global_console);
                    global_console->clear(display::CLEAR_COLOR | display::CLEAR_DEPTH);

                    // call draw function or default sim draw function...
                    if (!this->draw())
                    {
                        if (state == APP_SIM_RUNNING && global_simulation)
                        {
                            global_simulation->pre_draw();
                            global_simulation->draw();
                        }
                        else if (splash_screen)
                        {
                            BUDGET_SCOPE(L"application: splash screen");

                            draw_splash_screen();
                        }
                    }
                }
                catch (gsgl::exception &)
                {
                    quit_application();
                    throw;
                }

                // draw global UI elements
                if ((state == APP_UI_RUNNING || state == APP_SIM_RUNNING))
                {
                    BUDGET_SCOPE(L"application: user interface");

                    // draw from bottom up
                    int i, num = widgets.size();
                    for (i = 0; i < num; ++i)
                    {
                        if ((widgets[i]->get_flags() & widget::WIDGET_INVISIBLE) == 0)
                        {
                            display::scoped_state state(*global_console, display::ENABLE_ORTHO_2D);

                            display::scoped_ortho ortho(*global_console);
                            display::scoped_modelview mv(*global_console, &math::transform::IDENTITY);

                            mv.translate(0.375f, 0.375f, 0);
                            draw_ui(widgets[i]);
                        }
                    }
                }

                // draw budget
                if (should_draw_budget)
                {
                    end_tick = SDL_GetTicks();
                    draw_budget(end_tick - start_tick);
                    global_budget->reset();
                    start_tick = SDL_GetTicks();
                }

                // swap buffers
                {
                    BUDGET_SCOPE(L"application: buffer swap");
                    SDL_GL_SwapBuffers();
                }

                // update sim
                try
                {
                    if (state != APP_QUITTING && global_simulation)
                    {
                        if (global_simulation->is_running())
                            global_simulation->update();
                        else
                            unload_and_quit_simulation();
                    }
                }
                catch (gsgl::exception &)
                {
                    quit_application();
                    throw;
                }

                // get events
                {
                    BUDGET_SCOPE(L"application: event handling");

                    SDL_Event e;
                    while (SDL_PollEvent(&e))
                    {
                        switch (e.type)
                        {
                        case SDL_QUIT:
                            quit_application();
                            break;
                        case SDL_KEYDOWN:
                            if (e.key.keysym.sym == SDLK_b && (e.key.keysym.mod & (KMOD_CTRL | KMOD_ALT)))
                            {
                                should_draw_budget = !should_draw_budget;
                                break;
                            }
                            else if (e.key.keysym.sym == SDLK_w && (e.key.keysym.mod & (KMOD_CTRL | KMOD_ALT)))
                            {
                                if (global_simulation)
                                    global_draw_context->render_flags ^= scenegraph::drawing_context::RENDER_WIREFRAME;
                                break;
                            }
                            else if (e.key.keysym.sym == SDLK_t && (e.key.keysym.mod & (KMOD_CTRL | KMOD_ALT)))
                            {
                                if (global_simulation)
                                    global_draw_context->render_flags ^= scenegraph::drawing_context::RENDER_NO_TEXTURES;
                                break;
                            }
                            else if (e.key.keysym.sym == SDLK_l && (e.key.keysym.mod & (KMOD_CTRL | KMOD_ALT)))
                            {
                                if (global_simulation)
                                    global_draw_context->render_flags ^= scenegraph::drawing_context::RENDER_NO_LIGHTING;
                                break;
                            }

                            // fall through
                        default:
                            if (widgets.size() && handle_ui_event(e, widgets.top()))
                                continue;

                            if (this->handle_event(e))
                                continue;

                            if (global_simulation)
                                global_mapper->handle_event(e, global_simulation);

                            break;
                        }
                    }
                }

                // update app
                try
                {
                    if (state != APP_QUITTING)
                        this->update();
                }
                catch (gsgl::exception &)
                {
                    quit_application();
                    throw;
                }
            }

            // clean up
            state = APP_DEAD;
        } // application::run()


        //

        void application::draw_splash_screen()
        {
            float width = static_cast<float>(global_console->get_width());
            float height = static_cast<float>(global_console->get_height());

            display::scoped_state state(*global_console, display::ENABLE_ORTHO_2D);

            display::scoped_ortho ortho(*global_console);
            display::scoped_modelview mv(*global_console, &math::transform::IDENTITY);

            display::scoped_texture t(*global_console, splash_screen);

            global_console->draw_rect_2d(0, 0, width, height);
        } // application::draw_splash_screen()


        static const int BUDGET_FONT_SIZE = 12;
        static const color BUDGET_COLOR(1.0f, 0, 0, 1.0f);
        static const int BUDGET_BAR_WIDTH = 256;
        static const wchar_t *BUDGET_FORMAT = L"%6u";

        void application::draw_budget(unsigned int ticks)
        {
            if (!budget_font)
                budget_font = new font(L"Sans", BUDGET_FONT_SIZE, BUDGET_COLOR);

            // get width & highest value
            int step = BUDGET_FONT_SIZE * 4 / 3;
            int num = global_budget->get_data().size();
            int widest = 0;
            unsigned int highest = 0;
            int sum = 0;

            for (dictionary<unsigned int, string>::iterator i = global_budget->get_data().iter(); i.is_valid(); ++i)
            {
                int width = static_cast<int>(budget_font->calc_width(i.get_index()));
                if (width > widest)
                    widest = width;

                if (*i > highest)
                    highest = *i;

                sum += *i;
            }

            // draw text
            {
                display::scoped_text td(*global_console);
                display::scoped_color sc(*global_console, BUDGET_COLOR);

                td.draw_2d(0, static_cast<float>((num+1)*step), budget_font, L"TOTAL");
                td.draw_2d(static_cast<float>(widest), static_cast<float>((num+1)*step), budget_font, string::format(BUDGET_FORMAT, ticks));

                int n = 0;
                for (dictionary<unsigned int, string>::iterator i = global_budget->get_data().iter(); i.is_valid(); ++i, ++n)
                {
                    int y = (num-n)*step;

                    td.draw_2d(0, static_cast<float>(y), budget_font, i.get_index());
                    td.draw_2d(static_cast<float>(widest), static_cast<float>(y), budget_font, string::format(BUDGET_FORMAT, *i));

                    int w = static_cast<int>(BUDGET_BAR_WIDTH * static_cast<double>(*i)/static_cast<double>(highest));
                    global_console->draw_rect_2d(static_cast<float>(widest+64), static_cast<float>(y+2), static_cast<float>(widest+64+w), static_cast<float>(y+8));
                }

                td.draw_2d(0, 0, budget_font, L"other");
                td.draw_2d(static_cast<float>(widest), 0, budget_font, string::format(BUDGET_FORMAT, ticks - sum));

                int w = static_cast<int>(BUDGET_BAR_WIDTH * static_cast<double>(ticks - sum)/static_cast<double>(highest));
                global_console->draw_rect_2d(static_cast<float>(widest+64), 2, static_cast<float>(widest+64+w), 10);
            }
        } // application::draw_budget()


        void application::draw_ui(widget *w)
        {
            assert(w);

            display::scoped_modelview mv(*global_console);
            mv.translate(static_cast<float>(w->get_x()), static_cast<float>(w->get_y()), 0);

            if ((w->get_flags() & widget::WIDGET_INVISIBLE) == 0)
            {
                w->draw();

                // don't use the iterator; it goes from the top down
                const int num = w->get_children().size();
                for (int i = 0; i < num; ++i)
                {
                    draw_ui(w->get_children()[i]);
                }
            }
        } // application::draw_ui()


        /// Implement this to handle SDL events other than through the default scene graph event handler.
        /// If it returns true, the scene graph event handler will NOT be called.  If it returns false, and the
        /// current simulation pointer is valid, then the event mapper will be called with the simulation's
        /// set event targets.
        bool application::handle_event(const SDL_Event &)
        {
            return false;
        } // application::handle_event()
        
        
        /// Implement this if initialization is needed prior to the main loop.
        void application::init()
        {
        } // application::init()
        

        /// Implement this to draw the simulation in a custom manner.  
        /// If this is not implemented or returns false, the sim will be updated and drawn automatically.
        bool application::draw()
        {
            return false;
        } // application::draw()
        

        void application::update()
        {
        } // application::update()


        /// Implement this if cleanup is needed after the main loop terminates.
        void application::cleanup()
        {
        } // application::cleanup()
        

        //

        void application::remove_viewpoint_nodes(node *n)
        {
            list<node *> children_to_remove;

            // go through tree, marking viewpoint nodes to delete
            for (simple_array<node *>::iterator i = n->get_children().iter(); i.is_valid(); ++i)
            {
                node *child = *i;
                
                if (dynamic_cast<freeview *>(child))
                    children_to_remove.append(child);
                else
                    remove_viewpoint_nodes(child);
            }

            // remove viewpoint nodes
            for (list<node *>::iterator i = children_to_remove.iter(); i.is_valid(); ++i)
            {
                for (simple_array<node *>::iterator j = n->get_children().iter(); j.is_valid(); ++j)
                {
                    if (*i == *j)
                    {
                        n->get_children().remove(j);
                        break;
                    }
                }
            }
        } // application::remove_viewpoint_nodes()


        //

        bool application::get_cmdline_dir(const int & argc, const char **argv, const int & pos, const string & arg_key, config_record & conf, const string & conf_key)
        {
            string arg(argv[pos]);

            if (arg == arg_key)
            {
                if (pos+1 < argc)
                {
                    string val(argv[pos+1]);
                    io::directory dir(val);
                    conf.get_child(conf_key).get_text() = dir.get_full_path();
                    return true;
                }
                else
                {
                    throw runtime_exception(L"You must specify a directory after %ls on the command line.", arg_key.w_string());
                }
            }

            return false;
        } // application::get_cmdline_dir()


        void application::get_config_overrides(const int & argc, const char **argv)
        {
            // override global config root node's name with our title, so overrides will work...
            const_cast<config_record &>(global_config::get_config()).get_name() = title;

            // override config from the command line
            config_record cmd_line_config;
            cmd_line_config.get_name() = title;

            config_record & program = cmd_line_config.get_child(L"paths/program");
            program.get_text() = get_program_dir(string(argv[0]));

            for (int i = 1; i < argc; ++i)
            {
                string arg(argv[i]);

                // system data
                if (get_cmdline_dir(argc, argv, i, L"-sys_data", cmd_line_config, L"paths/sys_data"))
                {
                    ++i;
                    continue;
                }

                // user data
                if (get_cmdline_dir(argc, argv, i, L"-user_data", cmd_line_config, L"paths/user_data"))
                {
                    ++i;
                    continue;
                }
            }

            config_record & sys_data = cmd_line_config.get_child(L"paths/sys_data");
            if (sys_data.get_text().is_empty())
                sys_data.get_text() = program.get_text() + SYS_DATA_PATH;

            config_record & user_data = cmd_line_config.get_child(L"paths/user_data");
            if (user_data.get_text().is_empty())
                user_data.get_text() = get_user_dir();

            config_record & event_map = cmd_line_config.get_child(L"paths/event_map");
            if (event_map.get_text().is_empty())
                event_map.get_text() = user_data.get_text() + EVENT_MAP_PATH;

            config_record & user_config = cmd_line_config.get_child(L"paths/user_config");
            if (user_config.get_text().is_empty())
                user_config.get_text() = user_data.get_text() + title + L".cfg";

            // update the config variables with the new values
            global_config::override_with(cmd_line_config);

            // create directories if necessary
            if (!io::directory::exists(SYS_DATA_PATH))
                throw runtime_exception(L"Invalid system data directory: %ls", SYS_DATA_PATH.get_value().w_string());

            if (!io::directory::exists(USER_DATA_PATH))
                io::directory::create(USER_DATA_PATH);

            // copy files if necessary
            if (!io::file::exists(USER_CONFIG_PATH))
                io::file::copy(SYS_DATA_PATH + L"Sample " + title + L".cfg", USER_CONFIG_PATH);

            if (!io::file::exists(EVENT_MAP_PATH))
                io::file::copy(SYS_DATA_PATH + L"Sample " + title + L" EventMap.cfg", EVENT_MAP_PATH);

            // load user config & override with command line config again
            config_record user_record(USER_CONFIG_PATH);
            global_config::override_with(user_record);
            global_config::override_with(cmd_line_config);

            // save global config
            global_config::save(USER_CONFIG_PATH);
        } // application::get_user_config()


        //

        node *application::load_objects(const config_record & obj_config, synchronized<string> & status_string)
        {
            node *result = 0;
            data::broker *b = data::broker::global_instance();
            assert(b);

            const string & type_name = obj_config.get_name();

            status_string = string::format(L"Loading %ls...", type_name.w_string());

            try
            {
                if (b->has_object(type_name))
                {
                    try
                    {
                        result = dynamic_cast<node *>(b->create_object(type_name, obj_config));

                        if (!result)
                            throw runtime_exception(L"Unable to create object of type %ls referenced in %ls.", obj_config.get_name(), obj_config.get_file().get_full_path().w_string());

                        // get children
                        for (list<config_record>::const_iterator child = obj_config.get_children().iter(); child.is_valid(); ++child)
                        {
                            if (child->get_name() != L"property")
                            {
                                node *child_node = load_objects(*child, status_string);
                                if (child_node)
                                    result->add_child(child_node);
                            }
                        }
                    }
                    catch (runtime_exception & rte)
                    {
                        log(rte.get_message());
                        status_string = string(rte.get_message());
                        result = 0;
                    }
                }
                else
                {
                    throw runtime_exception(L"Unknown object type %ls in %ls.", obj_config.get_name().w_string(), obj_config.get_file().get_full_path().w_string());
                }
            }
            catch (runtime_exception & rte)
            {
                log(rte.get_message());
                status_string = string(rte.get_message());
                result = 0;
            }

            return result;
        } // load_objects()


        void application::unload_scenery()
        {
            delete global_scenery;
            global_scenery = 0;
        } // application::unload_scenery()


        void application::load_scenery(synchronized<string> & status_string)
        {
            // read scenery files
            list<node *> all_nodes;

            for (list<package *>::iterator package = loaded_packages.iter(); package.is_valid(); ++package)
            {
                for (list<pkg_scenery *>::const_iterator scenery = (*package)->get_loaded_sceneries().iter(); scenery.is_valid(); ++scenery)
                {
                    const string & fname = (*scenery)->get_fname();
                    if (!fname.is_empty())
                    {
                        config_record scn_config(fname);
                        if (scn_config.get_name() == L"scenery")
                        {
                            for (list<config_record>::iterator i = scn_config.get_children().iter(); i.is_valid(); ++i)
                            {
                                if (i->get_name() != L"description")
                                {
                                    node *obj = load_objects(*i, status_string);
                                    if (obj)
                                        all_nodes.append(obj);
                                }
                            }
                        }
                        else
                        {
                            throw runtime_exception(L"Invalid scenery specification in %ls.", fname.w_string());
                        }
                    }
                }
            }

            // make root node
            node *scenery = new node(L"scenery", 0);

            // add all parentless nodes to the root
            list<node *> non_roots;

            for (list<node *>::iterator n = all_nodes.iter(); n.is_valid(); ++n)
            {
                if ((*n)->get_parent_name().is_empty())
                {
                    scenery->add_child(*n);
                }
                else
                {
                    non_roots.append(*n);
                }
            }

            if (scenery->get_children().size() == 0)
            {
                throw runtime_exception(L"There must be at least one scenery node without a parent.");
            }

            // add others to the hierarchy
            list<node *> nodes_to_check, nodes_not_added;

            nodes_to_check = non_roots;

            while (true)
            {
                // pick a non-rooted node
                for (list<node *>::iterator child = non_roots.iter(); child.is_valid(); ++child)
                {
                    bool found_root = false;

                    // try to find a root for it
                    for (list<node *>::iterator parent = all_nodes.iter(); parent.is_valid(); ++parent)
                    {
                        if (*child != *parent && (*child)->get_parent_name() == (*parent)->get_name())
                        {
                            (*parent)->add_child(*child);
                            found_root = true;
                            break;
                        }
                    }

                    if (found_root)
                        break;
                    else
                        nodes_not_added.append(*child);
                }

                if (nodes_not_added.size() == 0)
                {
                    break;
                }
                else if (nodes_not_added.size() == nodes_to_check.size())
                {
                    // can't happen
                    throw runtime_exception(L"Found a parentless node: %ls", nodes_not_added[0]->get_name().w_string());
                }
                else
                {
                    nodes_to_check = nodes_not_added;
                    nodes_not_added.clear();
                }
            }

            global_scenery = scenery;
        } // load_scenery()


        //

        bool application::handle_ui_event(const SDL_Event & e, widget *w)
        {
            assert(w);

            if (e.type == SDL_MOUSEBUTTONDOWN)
            {
                mouse_button_pressed = e.button.button;
                mouse_button_x = e.button.x;
                mouse_button_y = global_console->get_height() - e.button.y;
            }

            return handle_ui_event(e, w, e.button.x, global_console->get_height() - e.button.y);
        } // application::handle_ui_event()


        bool application::handle_ui_event(const SDL_Event & e, widget *w, int mouse_x, int mouse_y)
        {
            assert(w);

            if (!(w->get_flags() & (widget::WIDGET_INVISIBLE | widget::WIDGET_INACTIVE)))
            {
                int widget_x = w->get_x();
                int widget_w = w->get_w();
                int widget_y = w->get_y();
                int widget_h = w->get_h();

                if ((widget_x <= mouse_x && mouse_x <= (widget_x + widget_w))
                    && (widget_y <= mouse_y && mouse_y <= (widget_y + widget_h)))
                {
                    if (w->get_event_handler() && w->get_event_handler()(w, e))
                        return true;
                    if (w->handle_event(e))
                        return true;

                    for (int i = 0; i < w->get_children().size(); ++i)
                    {
                        if (handle_ui_event(e, w->get_children()[i], mouse_x - widget_x, mouse_y - widget_y))
                            return true;
                    }
                }
            }

            return false;
        } // handle_ui_event()


        void application::get_mousedown_info(int & button, int & x, int & y)
        {
            button = mouse_button_pressed;
            x = mouse_button_x;
            y = mouse_button_y;
        } // application::get_mousedown_info()
        


        // utilities
#ifdef WIN32

#pragma warning (disable : 4996)

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <shlobj.h>
#include <direct.h>
#endif


#ifdef WIN32
        static const wchar_t *SLASH = L"\\";
#else
#error Must define the actual directory separator!
#endif


        string application::get_program_dir(const string & arg)
        {
            string result;
            int last_slash_pos = arg.find_reverse(SLASH);

            if (last_slash_pos != -1)
            {
                result = arg.substring(0, last_slash_pos);
            }
            else
            {
                io::directory cwd; // defaults to current directory
                result = cwd.get_full_path();
            }

            io::directory progdir(result);
            return progdir.get_full_path();
        } // application::get_program_dir()


        string application::get_user_dir()
        {
#ifdef WIN32
            smart_pointer<wchar_t, true> buf(new wchar_t[MAX_PATH]);

            if (SUCCEEDED(SHGetFolderPath(0, CSIDL_LOCAL_APPDATA | CSIDL_FLAG_CREATE, 0, 0, buf)))
            {
                string path(buf);
                io::directory userdir(path);

                return userdir.get_full_path() + title + io::directory::SEPARATOR;
            }
            else
            {
                throw io_exception(L"Unable to find user data directory.");
            }
#else
#error get_user_dir() unimplemented!
#endif
        } // application::get_user_dir()


    } // namespace framework
    
} // namespace gsgl
