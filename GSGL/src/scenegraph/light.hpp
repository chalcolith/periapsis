#ifndef GSGL_SCENEGRAPH_LIGHT_H
#define GSGL_SCENEGRAPH_LIGHT_H

//
// $Id: light.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "scenegraph/node.hpp"
#include "platform/color.hpp"

namespace gsgl
{

    namespace scenegraph
    {

        /// A point light.
        class SCENEGRAPH_API light
            : public node
        {
            platform::color ambient, diffuse, specular;
            gsgl::real_t attenuation_constant, attenuation_linear, attenuation_quadratic;

        public:
            light(node *parent);
            virtual ~light();

            platform::color & get_ambient() { return ambient; }
            platform::color & get_diffuse() { return diffuse; }
            platform::color & get_specular() { return specular; }

            gsgl::real_t & get_attenuation_constant() { return attenuation_constant; }
            gsgl::real_t & get_attenuation_linear() { return attenuation_linear; }
            gsgl::real_t & get_attenuation_quadratic() { return attenuation_quadratic; }

            virtual void bind(int light_number);
        }; // class light

    } // namespace scenegraph

} // namespace gsgl

#endif
