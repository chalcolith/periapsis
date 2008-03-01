//
// $Id: freeview.cpp 319 2008-03-01 20:40:39Z Gordon $
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

#include "scenegraph/freeview.hpp"
#include "math/quaternion.hpp"

namespace gsgl
{

    using namespace data;
    using namespace math;
    
    namespace scenegraph
    {

        BROKER_DEFINE_CREATOR(gsgl::scenegraph::freeview);

        //

    
        freeview::freeview(const string & name, node *parent) 
            : node(name, parent), cam(0), last_code(sg_event::NULL_EVENT), relative(false),
              transition_end_tick(0), transition_angular_velocity(0), transition_axis(vector::Z_AXIS), transition_linear_velocity(0), transition_path(vector::ZERO)
        {
            cam = new camera(this);
        } // freeview::freeview()
        

        freeview::freeview(const data::config_record & obj_conf) 
            : node(obj_conf), cam(0), last_code(sg_event::NULL_EVENT), relative(false),
              transition_end_tick(0), transition_angular_velocity(0), transition_axis(vector::Z_AXIS), transition_linear_velocity(0), transition_path(vector::ZERO)
        {
            cam = new camera(this);

            if (!obj_conf[L"field_of_view"].is_empty())
            {
                cam->get_field_of_view() = static_cast<gsgl::real_t>(obj_conf[L"field_of_view"].to_double());
            }
            else
            {
                cam->get_field_of_view() = 60;
            }
        } // freeview::freeview()

        
        freeview::~freeview()
        {
        } // freeview::~freeview()
        
        
        static config_variable<gsgl::real_t> ROTATION_STEP(L"scenegraph/freeview/rotation_step", 1.0f);
        static config_variable<gsgl::real_t> TRANSITION_TIME(L"scenegraph/freeview/transition_time", 0.5f);


        void freeview::reset(context *c)
        {
            assert(get_parent());

            last_code = sg_event::NULL_EVENT;
            transition_end_tick = 0;
            
            transition_angular_velocity = 0;
            transition_axis = vector::Z_AXIS;

            transition_linear_velocity = 0;
            transition_path = vector::ZERO;

            get_translation() = vector(0, -get_parent()->default_view_distance(), 0);
            get_orientation() = transform::IDENTITY;

            if (get_translation().mag() > 0)
                relative = true;
        } // freeview::reset()


        void freeview::update(context *c)
        {
            if (c->cur_tick < transition_end_tick)
            {
                if (transition_angular_velocity)
                {
                    quaternion rq(transition_axis, transition_angular_velocity * c->delta_tick / 1000.0f);
                    transform rt(rq);
                    get_orientation() = rt * get_orientation();

                    if (relative)
                        get_translation() = rt * get_translation();
                }
                else if (transition_linear_velocity)
                {
                    get_translation() += transition_path * (transition_linear_velocity * c->delta_tick / 1000.0f);
                }
            }
        } // freeview::update()


        bool freeview::rot_absolute(sg_event::event_code code, context *c)
        {
            if (code != last_code)
            {
                vector axis;
                gsgl::real_t velocity = static_cast<gsgl::real_t>(ROTATION_STEP * math::DEG2RAD / TRANSITION_TIME);
                
                switch (code)
                {
                case sg_event::VIEW_ROT_X_POS:
                    axis = vector::X_AXIS;
                    break;
                case sg_event::VIEW_ROT_X_NEG:
                    axis = vector::X_AXIS;
                    velocity *= -1;
                    break;
                case sg_event::VIEW_ROT_Y_POS:
                    axis = vector::Y_AXIS;
                    break;
                case sg_event::VIEW_ROT_Y_NEG:
                    axis = vector::Y_AXIS;
                    velocity *= -1;
                    break;
                case sg_event::VIEW_ROT_Z_POS:
                    axis = vector::Z_AXIS;
                    break;
                case sg_event::VIEW_ROT_Z_NEG:
                    axis = vector::Z_AXIS;
                    velocity *= -1;
                    break;
                default:
                    return false;
                }

                last_code = code;

                transition_axis = get_orientation() * axis;
                transition_angular_velocity = velocity;
            }

            if (transition_angular_velocity != 0)
            {
                transition_end_tick = c->cur_tick + static_cast<unsigned long>(1000.0f * TRANSITION_TIME);
            }
            
            return true;
        } // freeview::rot_absolute()

        
        bool freeview::rot_relative(sg_event::event_code code, context *c)
        {
            if (code != last_code)
            {
                vector axis;
                gsgl::real_t velocity = static_cast<gsgl::real_t>(ROTATION_STEP * math::DEG2RAD / TRANSITION_TIME);
                
                switch (code)
                {
                case sg_event::VIEW_ROT_X_POS:
                    axis = vector::X_AXIS;
                    velocity *= -1;
                    break;
                case sg_event::VIEW_ROT_X_NEG:
                    axis = vector::X_AXIS;
                    break;
                case sg_event::VIEW_ROT_Y_POS:
                    axis = vector::Y_AXIS;
                    break;
                case sg_event::VIEW_ROT_Y_NEG:
                    axis = vector::Y_AXIS;
                    velocity *= -1;
                    break;
                case sg_event::VIEW_ROT_Z_POS:
                    axis = vector::Z_AXIS;
                    velocity *= -1;
                    break;
                case sg_event::VIEW_ROT_Z_NEG:
                    axis = vector::Z_AXIS;
                    break;
                default:
                    return false;
                }
                
                last_code = code;

                transition_axis = get_orientation() * axis;
                transition_angular_velocity = velocity;
            }

            if (transition_angular_velocity != 0)
            {
                transition_end_tick = c->cur_tick + static_cast<unsigned long>(1000.0f * TRANSITION_TIME);
            }
            
            return true;
        } // freeview::rot_relative()

        
        bool freeview::handle_event(context *c, sg_event & e)
        {
            assert(c->view == this);

            switch (e.get_code())
            {
            case sg_event::VIEW_ROT_X_POS:
            case sg_event::VIEW_ROT_X_NEG:
            case sg_event::VIEW_ROT_Y_POS:
            case sg_event::VIEW_ROT_Y_NEG:
            case sg_event::VIEW_ROT_Z_POS:
            case sg_event::VIEW_ROT_Z_NEG:
                if (relative)
                    return rot_relative(e.get_code(), c);
                else
                    return rot_absolute(e.get_code(), c);

            case sg_event::VIEW_TOGGLE_ROT_MODE:
                // don't let us switch to relative unless we're orbiting something...
                if (get_translation().mag2())
                {
                    relative = !relative;
                    last_code = sg_event::NULL_EVENT;
                }
                return true;

            case sg_event::VIEW_ZOOM_IN:
            case sg_event::VIEW_ZOOM_OUT:
                if (relative)
                {
                    transition_angular_velocity = 0;

                    if (transition_end_tick < c->cur_tick)
                    {
                        double dist = get_translation().mag();
                        if (dist > 0)
                        {
                            double surf = get_parent()->minimum_view_distance();
                            double alt = dist - surf;
                            if (alt < 0.0)
                                alt = 0.0;
                            double new_alt = alt * (e.get_code() == sg_event::VIEW_ZOOM_IN ? 0.5 : 2.0);
                            double new_dist = surf + new_alt;
                            double ratio = new_dist / dist;

                            vector desired_position = get_translation() * static_cast<gsgl::real_t>(ratio);
                            vector path = desired_position - get_translation();

                            transition_linear_velocity = path.mag() / TRANSITION_TIME;
                            transition_path = path; transition_path.normalize();

                            transition_end_tick = c->cur_tick + static_cast<unsigned long>(1000.0f * TRANSITION_TIME);
                        }
                    }
                }
                return true;

            case sg_event::VIEW_RESET:
                reset(c);
                return true;

            default:
                break;
            }

            return false;
        } // freeview::handle_event()

        
    } // namespace scenegraph
    
} // namespace gsgl
