//
// $Id: light.cpp 314 2008-03-01 16:33:47Z Gordon $
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

#include "scenegraph/light.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace platform;


    namespace scenegraph
    {

        static int num_lights = 0;


        light::light(node *parent)
            : node(string::format(L"light_%d", num_lights++), parent)
        {
        }; // light::light()


        light::~light()
        {
        } // light::~light()


        void light::bind(int light_number)
        {
            glEnable(light_number);
            glLightfv(light_number, GL_AMBIENT, ambient.get_val());
            glLightfv(light_number, GL_DIFFUSE, diffuse.get_val());
            glLightfv(light_number, GL_SPECULAR, specular.get_val());
            glLightfv(light_number, GL_POSITION, math::vector::ZERO.ptr());

            glLightf(light_number, GL_CONSTANT_ATTENUATION, attenuation_constant);
            glLightf(light_number, GL_LINEAR_ATTENUATION, attenuation_linear);
            glLightf(light_number, GL_QUADRATIC_ATTENUATION, attenuation_quadratic);
        } // light::bind()


    } // namespace scenegraph

} // namespace gsgl
