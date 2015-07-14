#ifndef GSGL_SCENEGRAPH_EVENT_MAP_H
#define GSGL_SCENEGRAPH_EVENT_MAP_H

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

#include "scenegraph/scenegraph.hpp"

#include "data/singleton.hpp"
#include "data/dictionary.hpp"

// forward declarations
union SDL_Event;

namespace gsgl
{

    class string;

    
    namespace data
    {
        template <typename T> class list;
        class config_record;
    } // namespace data

    
    namespace scenegraph
    {
        class simulation;


        enum key_modifier {
            NO_MODIFIER    = 0,
            CTRL_MODIFIER  = 1 << 0,
            ALT_MODIFIER   = 1 << 1,
            SHIFT_MODIFIER = 1 << 2
        };


        /// Maps SDL events to internal game events.
        class SCENEGRAPH_API event_map
            : public scenegraph_object, public data::singleton<event_map>
        {
            // keys[sdl_keysim][modifier_bitmap] = scenegraph::event_code
            data::dictionary<data::dictionary< int, int >, int> key_events;

        public:
            event_map(const string & fname);
            ~event_map();

            bool handle_event(const SDL_Event & e, scenegraph::simulation *sim);
            
        private:
            void load_events(const string & fname);
            void load_keyboard_events(gsgl::data::config_record &);
        }; // class event_map
        
    } // namespace scenegraph
    
} // namespace gsgl

#endif
