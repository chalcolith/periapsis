#ifndef GSGL_PLATFORM_SHADER_H
#define GSGL_PLATFORM_SHADER_H

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

#include "platform/platform.hpp"
#include "data/string.hpp"
#include "data/list.hpp"
#include "data/dictionary.hpp"

namespace gsgl
{

    namespace platform
    {

        class shader_base;

        class PLATFORM_API shader_program
        {
            int opengl_id;

            gsgl::data::list<shader_base *> shaders;
            gsgl::data::dictionary<int, gsgl::string> uniforms;

        public:
            shader_program();
            ~shader_program();

            void add_vertex_shader(const gsgl::string & fname);
            void add_fragment_shader(const gsgl::string & fname);

            /// Loads and compiles the shader program.  This should only be called in the main thread with a valid OpenGL context.
            void load();

            /// Unloads the shader program.
            void unload();

            /// Tells OpenGL to use the shader program.
            void bind();

            /// Tells OpenGL to used the default shaders.
            void unbind();

            void set_uniform(const gsgl::string & name, const int & i);
            void set_uniform(const gsgl::string & name, const float & f);
            void set_uniform(const gsgl::string & name, const float ff[4]);
            void set_uniform(const gsgl::string & name, const bool b);

            int get_id() { return opengl_id; }

        private:
            int get_uniform_loc(const gsgl::string & name);
        }; // class shader_program

    } // namespace platform

} // namespace gsgl

#endif
