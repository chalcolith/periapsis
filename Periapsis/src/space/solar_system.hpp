#ifndef PERIAPSIS_SPACE_SOLAR_SYSTEM_H
#define PERIAPSIS_SPACE_SOLAR_SYSTEM_H

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
#include "scenegraph/node.hpp"
#include "scenegraph/utils.hpp"

namespace periapsis
{

    namespace space
    {

        class SPACE_API solar_system
            : public gsgl::scenegraph::node
        {
            gsgl::scenegraph::utils::coord_system *cs;

        public:
            solar_system(const gsgl::data::config_record & conf);
            virtual ~solar_system();

            virtual gsgl::real_t draw_priority(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);
            virtual void init(const gsgl::scenegraph::simulation_context *);
            virtual void draw(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);

            BROKER_DECLARE_CREATOR(periapsis::space::solar_system);
        }; // class solar_system

    } // namespace space

} // namespace periapsis

#endif
