//
// $Id: gas_body.cpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "space/gas_body.hpp"
#include "space/gas_body_atmosphere.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::scenegraph;

namespace periapsis
{

    namespace space
    {

        BROKER_DEFINE_CREATOR(periapsis::space::gas_body);


        gas_body::gas_body(const config_record & obj_config)
            : celestial_body(obj_config)
        {
            // the rotator will be deleted by its parent frame
            body_rotator *rotator = !obj_config[L"rotator"].is_empty() ? rotator = dynamic_cast<body_rotator *>(broker::global_instance()->create_object(obj_config[L"rotator"], obj_config)) : 0;
            get_rotating_frame() = get_atmosphere() = new gas_body_atmosphere(get_name() + L" atmosphere [rotating frame]", this, rotator);
        } // gas_body::gas_body()


        gas_body::~gas_body()
        {
        } // gas_body::~gas_body()


    } // namespace space

} // namespace periapsis
