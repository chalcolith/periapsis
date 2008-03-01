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

#include "scenegraph/event_map.hpp"
#include "scenegraph/event.hpp"
#include "scenegraph/simulation.hpp"

#include "data/exception.hpp"
#include "data/string.hpp"
#include "data/config.hpp"
#include "framework/application.hpp"

#include "platform/lowlevel.hpp"

//

#define SG_EVENT(name) sg_event_codes[L#name] = scenegraph::sg_event::name

#define SDL_EVENT(name) sdl_event_codes[L#name] = SDLK_##name

//

namespace gsgl
{

    using namespace data;

    scenegraph::event_map *scenegraph::event_map::instance = 0;


    namespace scenegraph
    {
            
        static data::dictionary<int, string> sg_event_codes;
        static data::dictionary<int, string> sdl_event_codes;
        

        static void init_dictionaries()
        {
            // scenegraph event codes
            SG_EVENT(SIM_QUIT);

            SG_EVENT(VIEW_ROT_X_POS);
            SG_EVENT(VIEW_ROT_X_NEG);
            SG_EVENT(VIEW_ROT_Y_POS);
            SG_EVENT(VIEW_ROT_Y_NEG);
            SG_EVENT(VIEW_ROT_Z_POS);
            SG_EVENT(VIEW_ROT_Z_NEG);
            SG_EVENT(VIEW_TOGGLE_ROT_MODE);
            SG_EVENT(VIEW_ZOOM_IN);
            SG_EVENT(VIEW_ZOOM_OUT);
            SG_EVENT(VIEW_RESET);

            SG_EVENT(TIME_INC_SCALE);
            SG_EVENT(TIME_DEC_SCALE);
            SG_EVENT(TIME_RESET_SCALE);
            SG_EVENT(TIME_PAUSE);

            SG_EVENT(RENDER_TOGGLE_LABELS);
            SG_EVENT(RENDER_TOGGLE_COORD_SYSTEMS);
            
            // SDL event codes

            SDL_EVENT(BACKSPACE);
            SDL_EVENT(TAB);
            SDL_EVENT(CLEAR);
            SDL_EVENT(RETURN);
            SDL_EVENT(PAUSE);
            SDL_EVENT(ESCAPE);
            SDL_EVENT(SPACE);
            SDL_EVENT(EXCLAIM);
            SDL_EVENT(QUOTEDBL);
            SDL_EVENT(HASH);
            SDL_EVENT(DOLLAR);
            SDL_EVENT(AMPERSAND);
            SDL_EVENT(QUOTE);
            SDL_EVENT(LEFTPAREN);
            SDL_EVENT(RIGHTPAREN);
            SDL_EVENT(ASTERISK);
            SDL_EVENT(PLUS);
            SDL_EVENT(COMMA);
            SDL_EVENT(MINUS);
            SDL_EVENT(PERIOD);
            SDL_EVENT(SLASH);
            SDL_EVENT(0);
            SDL_EVENT(1);
            SDL_EVENT(2);
            SDL_EVENT(3);
            SDL_EVENT(4);
            SDL_EVENT(5);
            SDL_EVENT(6);
            SDL_EVENT(7);
            SDL_EVENT(8);
            SDL_EVENT(9);
            SDL_EVENT(COLON);
            SDL_EVENT(SEMICOLON);
            SDL_EVENT(LESS);
            SDL_EVENT(EQUALS);
            SDL_EVENT(GREATER);
            SDL_EVENT(QUESTION);
            SDL_EVENT(AT);
            SDL_EVENT(LEFTBRACKET);
            SDL_EVENT(BACKSLASH);
            SDL_EVENT(RIGHTBRACKET);
            SDL_EVENT(CARET);
            SDL_EVENT(UNDERSCORE);
            SDL_EVENT(BACKQUOTE);
            SDL_EVENT(a);
            SDL_EVENT(b);
            SDL_EVENT(c);
            SDL_EVENT(d);
            SDL_EVENT(e);
            SDL_EVENT(f);
            SDL_EVENT(g);
            SDL_EVENT(h);
            SDL_EVENT(i);
            SDL_EVENT(j);
            SDL_EVENT(k);
            SDL_EVENT(l);
            SDL_EVENT(m);
            SDL_EVENT(n);
            SDL_EVENT(o);
            SDL_EVENT(p);
            SDL_EVENT(q);
            SDL_EVENT(r);
            SDL_EVENT(s);
            SDL_EVENT(t);
            SDL_EVENT(u);
            SDL_EVENT(v);
            SDL_EVENT(w);
            SDL_EVENT(x);
            SDL_EVENT(y);
            SDL_EVENT(z);
            SDL_EVENT(DELETE);
            SDL_EVENT(KP0);
            SDL_EVENT(KP1);
            SDL_EVENT(KP2);
            SDL_EVENT(KP3);
            SDL_EVENT(KP4);
            SDL_EVENT(KP5);
            SDL_EVENT(KP6);
            SDL_EVENT(KP7);
            SDL_EVENT(KP8);
            SDL_EVENT(KP9);
            SDL_EVENT(KP_PERIOD);
            SDL_EVENT(KP_DIVIDE);
            SDL_EVENT(KP_MULTIPLY);
            SDL_EVENT(KP_MINUS);
            SDL_EVENT(KP_PLUS);
            SDL_EVENT(KP_ENTER);
            SDL_EVENT(KP_EQUALS);
            SDL_EVENT(UP);
            SDL_EVENT(DOWN);
            SDL_EVENT(RIGHT);
            SDL_EVENT(LEFT);
            SDL_EVENT(INSERT);
            SDL_EVENT(HOME);
            SDL_EVENT(END);
            SDL_EVENT(PAGEUP);
            SDL_EVENT(PAGEDOWN);
            SDL_EVENT(F1);
            SDL_EVENT(F2);
            SDL_EVENT(F3);
            SDL_EVENT(F4);
            SDL_EVENT(F5);
            SDL_EVENT(F6);
            SDL_EVENT(F7);
            SDL_EVENT(F8);
            SDL_EVENT(F9);
            SDL_EVENT(F10);
            SDL_EVENT(F11);
            SDL_EVENT(F12);
            SDL_EVENT(F13);
            SDL_EVENT(F14);
            SDL_EVENT(F15);
            SDL_EVENT(NUMLOCK);
            SDL_EVENT(CAPSLOCK);
            SDL_EVENT(SCROLLOCK);
            SDL_EVENT(RSHIFT);
            SDL_EVENT(LSHIFT);
            SDL_EVENT(RCTRL);
            SDL_EVENT(LCTRL);
            SDL_EVENT(RALT);
            SDL_EVENT(LALT);
            SDL_EVENT(RMETA);
            SDL_EVENT(LMETA);
            SDL_EVENT(LSUPER);
            SDL_EVENT(RSUPER);
            SDL_EVENT(MODE);
            SDL_EVENT(COMPOSE);
            SDL_EVENT(HELP);
            SDL_EVENT(PRINT);
            SDL_EVENT(SYSREQ);
            SDL_EVENT(BREAK);
            SDL_EVENT(MENU);
            SDL_EVENT(POWER);
            SDL_EVENT(EURO);
            SDL_EVENT(UNDO);
        } // init_dictionaries()
        
        //
        
        event_map::event_map(const string & fname)
            : scenegraph_object(), data::singleton<event_map>()
        {
            init_dictionaries();
            load_events(fname);            
        } // event_map::event_map()
        
        event_map::~event_map()
        {
        } // event_map::~event_map()
        
        //
        
        bool event_map::handle_event(const SDL_Event & e, simulation *sim)
        {
            scenegraph::sg_event sge;
            
            switch (e.type)
            {
            case SDL_KEYDOWN:
            {
                SDLKey sdlsym = e.key.keysym.sym;
                SDLMod sdlmod = e.key.keysym.mod;
                
                int mod = 0;
                if (sdlmod & KMOD_CTRL)
                    mod |= CTRL_MODIFIER;
                if (sdlmod & KMOD_ALT)
                    mod |= ALT_MODIFIER;
                if (sdlmod & KMOD_SHIFT)
                    mod |= SHIFT_MODIFIER;
                    
                sge.get_code() = static_cast<scenegraph::sg_event::event_code>(key_events[sdlsym][mod]);
                break;
            }
            default:
                break;
            }
            
            if (sge.get_code())
            {
                return sim->handle_event(sge);
            }
            
            return false;
        } // event_map::handle_event()



        void event_map::load_events(const string & fname)
        {
            config_record conf(fname);

            if (conf.get_name() != L"event_map")
                throw runtime_exception(L"Invalid event map configuration format.");

            for (list<config_record>::iterator child = conf.get_children().iter(); child.is_valid(); ++child)
            {
                if (child->get_name() == L"keyboard")
                {
                    load_keyboard_events(*child);
                }
                else
                {
                    throw runtime_exception(L"Unknown event map section type '%ls'.", child->get_name().w_string());
                }
            }
        } // event_map::load_events()


        void event_map::load_keyboard_events(config_record & keyboard_conf)
        {
            for (list<config_record>::iterator child = keyboard_conf.get_children().iter(); child.is_valid(); ++child)
            {
                if (child->get_name() == L"event")
                {
                    string name = child->get_attribute(L"name");
                    string key = child->get_attribute(L"key");

                    if (!name.is_empty() && !key.is_empty())
                    {
                        scenegraph::sg_event::event_code code = static_cast<scenegraph::sg_event::event_code>(sg_event_codes[name]);

                        if (!code)
                            throw runtime_exception(L"Unknown event code '%ls'", name.w_string());

                        int modifiers = 0;

                        string sdlsim = key;
                        sdlsim.trim();
                        while (sdlsim.size())
                        {                            
                            if (sdlsim.left_substring(5) == L"CTRL_")
                            {
                                modifiers |= CTRL_MODIFIER;
                                sdlsim = sdlsim.substring(5);
                                continue;
                            }
                            
                            if (sdlsim.left_substring(4) == L"ALT_")
                            {
                                modifiers |= ALT_MODIFIER;
                                sdlsim = sdlsim.substring(4);
                                continue;
                            }
                            
                            if (sdlsim.left_substring(6) == L"SHIFT_")
                            {
                                modifiers |= SHIFT_MODIFIER;
                                sdlsim = sdlsim.substring(6);
                                continue;
                            }
                            
                            if (sdl_event_codes.contains_index(sdlsim))
                            {
                                int keysim = sdl_event_codes[sdlsim];
                                key_events[keysim][modifiers] = code;
                                break;
                            }
                            else
                            {
                                throw runtime_exception(L"Unknown keyboard code '%ls'", key.w_string());
                            }
                        }

                    }
                    else
                    {
                        throw runtime_exception(L"Invalid keyboard event specified.");
                    }
                }
            }
        } // event_map::load_keyboard_events()

                
    } // namespace scenegraph
    
} // namespace gsgl
