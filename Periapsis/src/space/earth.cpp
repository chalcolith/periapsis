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

#include "space/earth.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;


namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::planet_earth);

        const gsgl::real_t planet_earth::BARYSYSTEM_OFFSET = 4700000.0f; // distance from the earth's center to the barycentre of the earth-moon system


        planet_earth::planet_earth(const config_record & obj_config)
            : large_rocky_body(obj_config), moon(0)
        {
        } // earth::earth()


        planet_earth::~planet_earth()
        {
            moon = 0;
        } // earth::~earth()


        void planet_earth::update(context *c)
        {
            large_rocky_body::update(c);

            // find the moon if we don't already have it
            if (!moon)
            {
                for (simple_array<node *>::iterator i = get_parent()->get_children().iter(); i.is_valid(); ++i)
                {
                    if (*i && (*i)->get_name() == L"Moon")
                    {
                        moon = dynamic_cast<large_rocky_body *>(*i);
                        break;
                    }
                }
            }

            // offset the center of the planet to the real position of the earth relative to the earth-moon barysystem
            // we're one frame off, though, which is unfortunate
            if (moon)
            {
                vector moon_dir = moon->get_translation(); // the moon's position in the frame of the earth-moon barysystem
                moon_dir.normalize();

                get_translation() = moon_dir * -BARYSYSTEM_OFFSET;
            }
        } // earth::update()


    } // namespace space

} // namespace periapsis
