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

#include "scenegraph/simulation.hpp"
#include "scenegraph/freeview.hpp"
#include "scenegraph/event.hpp"
#include "scenegraph/utils.hpp"

#include "data/exception.hpp"
#include "data/broker.hpp"
#include "math/time.hpp"
#include "math/vector.hpp"
#include "math/transform.hpp"
#include "physics/vehicle.hpp"

#include "platform/budget.hpp"
#include "platform/lowlevel.hpp"

#include <cmath>

#ifdef WIN32
#include <ctime>
#else
#include <sys/time.h>
#endif

namespace gsgl
{

    using namespace data;
    using namespace io;
    using namespace math;
    using namespace platform;
    
    namespace scenegraph
    {

        static config_variable<int> NUM_FRAME_DELTAS(L"scenegraph/simulation/num_frame_deltas", 100);

        //


        simulation::simulation(const config_record & sim_config, 
                               display *console, 
                               simulation_context *sim_context, 
                               drawing_context *draw_context,
                               node *scenery)
            : scenegraph_object(), running(true),
              console(console), sim_context(sim_context), draw_context(draw_context), scenery(scenery), 
              start_time(0), time_scale(1), info_font(new platform::font(L"Sans", 18, platform::color(1, 0, 0))), frame_deltas(NUM_FRAME_DELTAS)
        {
            assert(console);
            assert(sim_context);
            assert(scenery);

            if (sim_config.get_name() != L"simulation")
                throw runtime_exception(L"Invalid simulation configuration file %ls", sim_config.get_file().get_full_path().w_string());

            // get simulation parameters
            double time = 0;

            for (list<config_record>::const_iterator param = sim_config.get_children().iter(); param.is_valid(); ++param)
            {
                if (param->get_name() == L"start_time")
                {
                    time = param->get_text().to_double();
                }
                else if (param->get_name() == L"viewpoint")
                {
                    broker *b = broker::global_instance();
                    assert(b);

                    // create free view node
                    view = dynamic_cast<freeview *>(b->create_object(L"gsgl::scenegraph::freeview", *param));
                    view->get_name() = L"Default View";
                    non_scenery_nodes.append(view);

                    //////////////////////////////////////////////////////////////////////////////
                    //// uncomment this to get a box with a spaceship inside it
                    //string box_name(L"Checkered Box");

                    //utils::checkered_box *box = new utils::checkered_box(box_name, 0, 100);
                    //box->get_parent_name() = view->get_parent_name();
                    //box->get_translation().get_y() = -50000000.0f;
                    //scenery->connect(box);

                    ////
                    //string vc_fname = global_config::get_config().get_child(L"paths/sys_data").get_text() + io::directory::SEPARATOR + L"Vehicles" + io::directory::SEPARATOR + L"Ship1" + io::directory::SEPARATOR + L"Ship1.vehicle_prefab";
                    //data::config_record vc(vc_fname);
                    //node *ship = dynamic_cast<node *>(b->create_object(L"periapsis::space::spacecraft", vc));
                    //assert(ship);

                    //ship->get_name() = string::format(L"Default Ship: %ls", ship->get_name().w_string());
                    //ship->get_parent_name() = box->get_parent_name();
                    //ship->get_translation().get_y() = -50000000.0f;
                    //scenery->connect(ship);

                    ////
                    //view->get_parent_name() = box->get_name();
                    ////
                    //////////////////////////////////////////////////////////////////////////////
                }
                else if (param->get_name() == L"vehicle")
                {
                    throw runtime_exception(L"Loading vehicles is not yet implemented.");
                }
                else
                {
                    throw runtime_exception(L"Unknown simulation parameter %ls in %ls.", param->get_name().w_string(), sim_config.get_file().get_full_path().w_string());
                }
            }

            if (time != 0)
            {
                julian_day jd(time);
                start_time = jd.to_time_t();
            }
            else
            {
                throw runtime_exception(L"No time specified for simulation!");
            }

            // add viewpoint to scenery
            if (scenery->connect(view))
                view->reset();
            else
                throw runtime_exception(L"Unable to add viewpoint to simulation.");

            //
            for (int i = 0; i < NUM_FRAME_DELTAS; ++i)
                frame_deltas[i] = 1.0f / 60.0f;
        } // simulation::simulation()
        

        simulation::~simulation()
        {
            for (list<node *>::iterator i = non_scenery_nodes.iter(); i.is_valid(); ++i)
            {
                (*i)->detach();
                delete *i;
            }
            non_scenery_nodes.clear();
        } // simulation::~simulation()


        void simulation::init_context()
        {
            sim_context->sim = this;
            sim_context->scenery = scenery;

            //
            sim_context->time_scale = time_scale;

            sim_context->frame = 0;

            sim_context->start_t_time = start_time;
            sim_context->cur_t_time = sim_context->start_t_time;

            sim_context->start_tick = SDL_GetTicks();
            sim_context->cur_tick = sim_context->start_tick;
            sim_context->delta_tick = 0;

            sim_context->start_time = static_cast<double>(sim_context->start_t_time);
            sim_context->cur_time = static_cast<double>(sim_context->cur_t_time);
            sim_context->delta_time = 0;

            math::julian_day jdn(sim_context->start_t_time);
            sim_context->julian_start = jdn.get_jdn();
            sim_context->julian_cur = sim_context->julian_start;
            sim_context->julian_dt = 0;

            //
            draw_context->console = console;
            draw_context->screen = console;
            draw_context->view = view;
            draw_context->cam = view->get_camera();
            draw_context->num_lights = 0;
            draw_context->render_flags = drawing_context::RENDER_NO_FLAGS;
        } // simulation::init_context()


        void simulation::update_context()
        {
            sim_context->sim = this;
            sim_context->scenery = scenery;

            //

            sim_context->time_scale = time_scale;

            ++sim_context->frame;

            unsigned int prev_tick = sim_context->cur_tick;
            sim_context->cur_tick = SDL_GetTicks();
            sim_context->delta_tick = sim_context->cur_tick - prev_tick;

            // maximum frame time is 200 milliseconds (helps with debugging)
            if (sim_context->delta_tick > 200)
                sim_context->delta_tick = 200;

            sim_context->delta_time = time_scale * static_cast<double>(sim_context->delta_tick) / 1000.0;
            sim_context->cur_time += sim_context->delta_time;

            sim_context->cur_t_time = static_cast<time_t>(sim_context->cur_time);

            sim_context->julian_dt = sim_context->delta_time / math::julian_day::JDAY;
            sim_context->julian_cur += sim_context->julian_dt;

            //
            draw_context->console = console;
            draw_context->screen = console;
            draw_context->view = view;
            draw_context->cam = view->get_camera();
            draw_context->num_lights = 0;
        } // simulation::update_context()


        void simulation::cleanup_context()
        {
        } // simulation::cleanup_context()

        
        void simulation::init()
        {
            init_context();
            
            // init world
            if (scenery)
                init_node(scenery);
        } // simulation::init()
        

        void simulation::pre_draw()
        {
            budget_record br(L"pre-render");
            node::pre_draw_scene(sim_context, draw_context, pre_rec);
        } // simulation::pre_draw()


        void simulation::draw()
        {
            // draw screens that need to be rendered first

            // draw main console scene
            draw_context->screen = console;
            draw_context->view = view;
            draw_context->cam = view->get_camera();

            node::draw_scene(sim_context, draw_context, pre_rec);

            {
                budget_record br(L"sim info");

                // update fps info
                gsgl::real_t avg_delta = 0.0f;
                for (int i = 0; i < NUM_FRAME_DELTAS; ++i)
                    avg_delta += frame_deltas[i];
                avg_delta /= NUM_FRAME_DELTAS;

                // draw info text
                int fps = static_cast<int>(::ceil(1.0f / avg_delta));
                float height = info_font->calc_height(L"Wtj");

                console->draw_text_start();

                glMatrixMode(GL_MODELVIEW);                                                                         CHECK_GL_ERRORS();
                glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
                glTranslatef(1.0f, console->get_height() - height, 0.0f);                                           CHECK_GL_ERRORS();
                info_font->draw(L"Periapsis Spaceflight Simulation");

                glMatrixMode(GL_MODELVIEW);                                                                         CHECK_GL_ERRORS();
                glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
                glTranslatef(1.0f, console->get_height() - 2*height, 0.0f);                                         CHECK_GL_ERRORS();
                string rel = string::format(L"View Mode: %ls", view->is_relative() ? L"RELATIVE" : L"ABSOLUTE");
                info_font->draw(rel);

                glMatrixMode(GL_MODELVIEW);                                                                         CHECK_GL_ERRORS();
                glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
                glTranslatef(1.0f, console->get_height() - 3*height, 0.0f);                                         CHECK_GL_ERRORS();
                struct tm *gmt = ::gmtime(&sim_context->cur_t_time);
                string utc(::asctime(gmt));
                string date = string::format(L"%ls UTC", utc.trim().w_string());
                info_font->draw(date);

                glMatrixMode(GL_MODELVIEW);                                                                         CHECK_GL_ERRORS();
                glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
                glTranslatef(1.0f, console->get_height() - 4*height, 0.0f);                                         CHECK_GL_ERRORS();
                string info = string::format(L"Time: %dx FPS: %d", static_cast<int>(time_scale), fps);
                info_font->draw(info);

                console->draw_text_stop();
            }
        } // simulation::draw()


        void simulation::update()
        {
            update_context();

            frame_deltas[sim_context->frame % NUM_FRAME_DELTAS] = static_cast<gsgl::real_t>(sim_context->delta_tick) / 1000.0f;
            
            // update world
            if (time_scale != 0.0f && scenery)
                update_node(scenery);
        } // simulation::update()
        
        
        void simulation::cleanup()
        {
            cleanup_context();

            // cleanup world
            if (scenery)
                cleanup_node(scenery);
        } // simulation::cleanup()
        

        static config_variable<gsgl::real_t> MAX_TIME_SCALE(L"scenegraph/simulation/max_time_scale", 1000000000.0f);


        bool simulation::handle_event(sg_event & e)
        {
            switch (e.get_code())
            {
            case sg_event::SIM_QUIT:
                running = false;
                return true;

            case sg_event::TIME_INC_SCALE:
                time_scale *= 10.0f;

                if (time_scale > MAX_TIME_SCALE)
                    time_scale = MAX_TIME_SCALE;

                return true;

            case sg_event::TIME_DEC_SCALE:
                time_scale /= 10.0f;

                if (time_scale < 1.0f)
                    time_scale = 1.0f;
                
                return true;

            case sg_event::TIME_RESET_SCALE:
                time_scale = 1.0f;
                return true;

            case sg_event::TIME_PAUSE:
                time_scale = 0.0f;
                return true;

            case sg_event::RENDER_TOGGLE_LABELS:
                draw_context->render_flags ^= drawing_context::RENDER_LABELS;
                return true;

            case sg_event::RENDER_TOGGLE_COORD_SYSTEMS:
                draw_context->render_flags ^= drawing_context::RENDER_COORD_SYSTEMS;
                return true;

            default:
                break;
            }

            // pass events to nodes...
            if (scenery)
                return handle_event(e, scenery);
            else
                return false;
        } // simulation::handle_event()


        void simulation::init_node(node *n)
        {
            assert(n);

            n->init(sim_context);
            
            // update children
            for (simple_array<node *>::iterator i = n->get_children().iter(); i.is_valid(); ++i)
                init_node(*i);
        } // simulation::init_node()
        
        
        static const string PHYSICS_CATEGORY = L"physics: ";

        void simulation::update_node(node *n)
        {
            assert(n);

            {
                platform::budget_record br(PHYSICS_CATEGORY + n->get_type_name());
                n->update(sim_context);
            }
                        
            //
            for (simple_array<node *>::iterator i = n->get_children().iter(); i.is_valid(); ++i)
                update_node(*i);
        } // simulation::update_node()


        void simulation::cleanup_node(node *n)
        {
            assert(n);

            n->cleanup(sim_context);

            for (simple_array<node *>::iterator i = n->get_children().iter(); i.is_valid(); ++i)
                cleanup_node(*i);
        } // simulation::cleanup_node()
        

        bool simulation::handle_event(sg_event & e, node *n)
        {
            assert(n);

            if (n->handle_event(sim_context, e))
                return true;

            for (simple_array<node *>::iterator i = n->get_children().iter(); i.is_valid(); ++i)
            {
                if (handle_event(e, *i))
                    return true;
            }

            return false;
        } // simulation::pass_event()


    } // namespace scenegraph
    
} // namespace gsgl
