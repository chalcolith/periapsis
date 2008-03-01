#ifndef GSGL_SG_EVENT_H
#define GSGL_SG_EVENT_H

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

namespace gsgl
{

    namespace scenegraph
    {
    
        class SCENEGRAPH_API sg_event
            : public scenegraph_object
        {
        public:
            enum event_code {
                NULL_EVENT = 0,

                SIM_QUIT,

                VIEW_ROT_X_POS,
                VIEW_ROT_X_NEG,
                VIEW_ROT_Y_POS,
                VIEW_ROT_Y_NEG,
                VIEW_ROT_Z_POS,
                VIEW_ROT_Z_NEG,
                VIEW_TOGGLE_ROT_MODE,
                VIEW_ZOOM_IN,
                VIEW_ZOOM_OUT,
                VIEW_RESET,

                TIME_INC_SCALE,
                TIME_DEC_SCALE,
                TIME_RESET_SCALE,
                TIME_PAUSE,

                RENDER_TOGGLE_LABELS,
                RENDER_TOGGLE_COORD_SYSTEMS,

                NUM_EVENT_CODES
            }; // enum event_code
            
        private:
            event_code code;
            
            union
            {
                void *param;
                int n;
                short a[2];
            };
            
        public:
            sg_event();
            sg_event(event_code code, void *param);
            sg_event(event_code code, int n);
            sg_event(event_code code, short a[2]);
            
            event_code & get_code();
        }; // class sg_event
        
    } // namespace scenegraph
    
} // namespace gsgl

#endif
