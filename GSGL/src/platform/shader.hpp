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

        class PLATFORM_API shader_uniform_base
        {
        protected:
            friend class shader_program;

            shader_program & parent_program;
            const gsgl::string name;
            int opengl_loc;

        public:
            shader_uniform_base(shader_program & parent_program, const gsgl::string & name);
            virtual ~shader_uniform_base();

        protected:
            void set(const int & i);
            void set(const float & f);
            void set(const float ff[4]);
            void set(const bool & b);

        private:
            bool bind();
        }; // class shader_uniform_base


        template <typename T>
        class shader_uniform
            : public shader_uniform_base
        {
        public:
            shader_uniform(shader_program & parent_program, const gsgl::string & name)
                : shader_uniform_base(parent_program, name) {}
            virtual ~shader_uniform() {}

            void set(const T & val)
            {
                shader_uniform_base::set(val);
            }
        }; // shader_uniform


        //////////////////////////////////////////////////////////////

        class shader_base;

        class PLATFORM_API shader_program
        {
            int opengl_id;

            gsgl::data::list<shader_base *> shaders;

            friend class shader_uniform_base;
            gsgl::data::dictionary<shader_uniform_base *, gsgl::string> uniforms;

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

            /// Returns a uniform that represents the given type.
            template <typename T>
            shader_uniform<T> *get_uniform(const gsgl::string & name)
            {
                shader_uniform<T> *u = 0;

                if (uniforms.contains_index(name))
                {
                    u = dynamic_cast<shader_uniform<T> *>(uniforms[name]);
                    if (!u)
                        throw internal_exception(__FILE__, __LINE__, L"Duplicate shader name '%ls'", name.w_string());
                }
                else
                {
                    u = new shader_uniform<T>(*this, name);
                    uniforms[name] = u;
                }

                return u;
            } // get_uniform()
        }; // class shader_program

    } // namespace platform

} // namespace gsgl

#endif
