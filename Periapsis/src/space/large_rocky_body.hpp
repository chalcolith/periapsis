#ifndef PERIAPSIS_SPACE_LARGE_ROCKY_BODY_H
#define PERIAPSIS_SPACE_LARGE_ROCKY_BODY_H

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

#include "space/space.hpp"
#include "space/celestial_body.hpp"


namespace periapsis
{

    namespace space
    {


        class SPACE_API large_rocky_body
            : public celestial_body
        {
        public:
            large_rocky_body(const gsgl::data::config_record & obj_config);
            virtual ~large_rocky_body();

            virtual gsgl::real_t draw_priority(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);

            virtual void init(const gsgl::scenegraph::simulation_context *);
            virtual void draw(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);
            virtual void update(const gsgl::scenegraph::simulation_context *);
            virtual void cleanup(const gsgl::scenegraph::simulation_context *);

            BROKER_DECLARE_CREATOR(periapsis::space::large_rocky_body);
        }; // class large_rocky_body


    } // namespace space

} // namespace periapsis

#endif
