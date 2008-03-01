//
// $Id: vehicle_module.cpp 314 2008-03-01 16:33:47Z Gordon $
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

#include "physics/vehicle_module.hpp"
#include "math/units.hpp"

namespace gsgl
{

    using namespace data;
    using namespace math;
    using namespace scenegraph;

    namespace physics
    {

        static int num_modules = 0;


        vehicle_module::vehicle_module(const config_record & obj_config)
            : node(obj_config), module_mass(0)
        {
            get_name() = string::format(L"vehicle_module_%d (%ls)", num_modules++, get_name().w_string());

            if (!obj_config[L"mass"].is_empty())
                module_mass = units::parse(obj_config[L"mass"]);
            if (module_mass <= 0)
                throw runtime_exception(L"Vehicle module %ls has a zero or negative mass!", get_name().w_string());

            for (list<config_record>::const_iterator i = obj_config.get_children().iter(); i.is_valid(); ++i)
            {
                if (i->get_name() == L"model")
                {
                    model *m = new model(*i);

                    m->get_name() = string::format(L"%ls: %ls", get_name().w_string(), m->get_name().w_string());
                    models.append(m);
                    add_child(m);
                }
            }
        } // vehicle_module::vehicle_module()


        vehicle_module::~vehicle_module()
        {
        } // vehicle_module::~vehicle_module()

    } // namespace physics

} // namespace gsgl
