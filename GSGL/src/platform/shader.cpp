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

#include "platform/shader.hpp"
#include "platform/extensions.hpp"

#include "data/log.hpp"
#include "data/pointer.hpp"
#include "data/array.hpp"
#include "data/file.hpp"
#include "data/fstream.hpp"

namespace gsgl
{

    using namespace data;
    using namespace io;

    namespace platform
    {

        shader_uniform_base::shader_uniform_base(shader_program & parent_program, const string & name)
            : parent_program(parent_program), name(name), opengl_loc(-1)
        {
        } // shader_uniform_base::shader_uniform_base()


        shader_uniform_base::~shader_uniform_base()
        {
        } // shader_uniform_base::~shader_uniform_base()


        void shader_uniform_base::set(const int & i)
        {
            if (opengl_loc != -1 || bind())
                glUniform1i(opengl_loc, i);
        } // shader_uniform_base::set()


        void shader_uniform_base::set(const float & f)
        {
            if (opengl_loc != -1 || bind())
                glUniform1f(opengl_loc, f);
        } // shader_uniform_base::set()


        void shader_uniform_base::set(const float ff[4])
        {
            if (opengl_loc != -1 || bind())
                glUniform4fv(opengl_loc, 1, ff);
        } // shader_uniform_base::set()


        void shader_uniform_base::set(const bool & b)
        {
            if (opengl_loc != -1 || bind())
                glUniform1i(opengl_loc, b);
        } // shader_uniform_base::set()


        bool shader_uniform_base::bind()
        {
            if (opengl_loc == -1)
            {
                if (parent_program.opengl_id == 0)
                    throw runtime_exception(L"Cannot set uniform value '%hs' when the shader is not bound.", name.c_string());

                opengl_loc = glGetUniformLocation(parent_program.opengl_id, name.c_string());
                if (opengl_loc == -1)
                    throw runtime_exception(L"Unable to set uniform value '%hs' in shader program!", name.c_string());
            }

            return true;
        } // shader_uniform_base::bind()


        //////////////////////////////////////////////////////////////


        static const int INFO_BUF_SIZE = 4096;

        //

        class shader_base
        {
        protected:
            const string fname;
            int opengl_id;
            list<string> lines;

        public:
            shader_base(const string & fname);
            virtual ~shader_base();

            int get_id() { return opengl_id; }
            void compile();

            void read_file(const string & fname);
            void unload();

        protected:
            virtual void create() = 0;
        }; // class shader_base


        //

        
        class vertex_shader
            : public shader_base
        {
        public:
            vertex_shader(const gsgl::string & fname);

        protected:
            void create();
        }; // class vertex_shader


        //


        class fragment_shader
            : public shader_base
        {
        public:
            fragment_shader(const gsgl::string & fname);

        protected:
            void create();
        }; // class fragment_shader


        //


        shader_base::shader_base(const string & fname)
            : fname(fname), opengl_id(0)
        {
            read_file(fname);
        } // shader_base::shader_base()


        shader_base::~shader_base()
        {
            unload();
        } // shader_base::~shader_base()


        void shader_base::read_file(const string & fname)
        {
            ft_stream f(fname);
            string line;

            for (f >> line; !f.at_end(); f >> line)
            {
                lines.append(line + L"\n");
            }
        } // shader_base::read_file()


        void shader_base::unload()
        {
            if (opengl_id)
            {
                glDeleteShader(opengl_id);                                                                          CHECK_GL_ERRORS();
                opengl_id = 0;
            }
        } // shader_base::unload()


        void shader_base::compile()
        {
            if (!opengl_id)
            {
                this->create();

                simple_array<char *> src_lines;

                for (list<string>::iterator i = lines.iter(); i.is_valid(); ++i)
                {
                    if (i->size())
                        src_lines.append(::strdup(i->c_string()));
                }

                glShaderSource(opengl_id, src_lines.size(), (const GLchar **) src_lines.ptr(), 0);                  CHECK_GL_ERRORS();

                for (int i = 0; i < src_lines.size(); ++i)
                    ::free(src_lines[i]);
            }

            glCompileShader(opengl_id);                                                                             CHECK_GL_ERRORS();

            GLint compile_status;
            glGetShaderiv(opengl_id, GL_COMPILE_STATUS, &compile_status);                                           CHECK_GL_ERRORS();

            if (!compile_status)
            {
                int len;
                smart_pointer<char, true> buf(new char[INFO_BUF_SIZE]);
                glGetShaderInfoLog(opengl_id, INFO_BUF_SIZE, &len, buf);                                            CHECK_GL_ERRORS();

                string error_msg = fname + L"\n\n" + string(buf);

                throw runtime_exception(error_msg.w_string());
            }
        } // shader_base::compile()

        //


        vertex_shader::vertex_shader(const string & fname)
            : shader_base(fname)
        {
        } // vertex_shader::vertex_shader()


        void vertex_shader::create()
        {
            if (!opengl_id)
            {
                opengl_id = glCreateShader(GL_VERTEX_SHADER);                                                       CHECK_GL_ERRORS();

                if (!opengl_id)
                    throw runtime_exception(L"Unable to create vertex shader %ls", fname.w_string());
            }
        } // vertex_shader::compile()


        //


        fragment_shader::fragment_shader(const string & fname)
            : shader_base(fname)
        {
        } // fragment_shader::fragment_shader()


        void fragment_shader::create()
        {
            if (!opengl_id)
            {
                opengl_id = glCreateShader(GL_FRAGMENT_SHADER);                                                     CHECK_GL_ERRORS();

                if (!opengl_id)
                    throw runtime_exception(L"Unable to create fragment shader %ls", fname.w_string());
            }
        } // fragment_shader::create()


        //

        shader_program::shader_program()
            : opengl_id(0)
        {
        } // shader_program::shader_program()

        
        shader_program::~shader_program()
        {
            unload();

            for (gsgl::data::dictionary<shader_uniform_base *, string>::iterator i = uniforms.iter(); i.is_valid(); ++i)
                delete *i;
            uniforms.clear();

            for (gsgl::data::list<shader_base *>::iterator i = shaders.iter(); i.is_valid(); ++i)
                delete *i;
            shaders.clear();
        } // shader_program::~shader_program()


        void shader_program::add_vertex_shader(const string & fname)
        {
            shaders.append(new vertex_shader(io::file::get_full_path(fname)));
        } // shader_program::add_Vertex_shader()


        void shader_program::add_fragment_shader(const string & fname)
        {
            shaders.append(new fragment_shader(io::file::get_full_path(fname)));
        } // shader_program::add_fragment_shader()


        void shader_program::load()
        {
            if (!opengl_id)
            {
                opengl_id = glCreateProgram();                                                                      CHECK_GL_ERRORS();

                if (!opengl_id && opengl_id != GL_INVALID_VALUE)
                    throw runtime_exception(L"Unable to create shader program.");

                // compile shaders
                for (list<shader_base *>::iterator i = shaders.iter(); i.is_valid(); ++i)
                {
                    (*i)->compile();
                }

                // add shaders
                for (list<shader_base *>::iterator i = shaders.iter(); i.is_valid(); ++i)
                {
                    glAttachShader(opengl_id, (*i)->get_id());                                                      CHECK_GL_ERRORS();
                }

                // validate
                int len, status;
                smart_pointer<char, true> buf(new char[INFO_BUF_SIZE]);

                glValidateProgram(opengl_id);                                                                       CHECK_GL_ERRORS();
                glGetProgramiv(opengl_id, GL_VALIDATE_STATUS, &status);

                if (!status)
                {
                    glGetProgramInfoLog(opengl_id, INFO_BUF_SIZE, &len, buf);
                    throw runtime_exception(string(buf).w_string());
                }

                // link
                glLinkProgram(opengl_id);                                                                           CHECK_GL_ERRORS();
                glGetProgramiv(opengl_id, GL_LINK_STATUS, &status);

                if (!status)
                {
                    glGetProgramInfoLog(opengl_id, INFO_BUF_SIZE, &len, buf);
                    throw runtime_exception(string(buf).w_string());
                }
            }
        } // shader_program::load()


        void shader_program::unload()
        {
            if (opengl_id)
            {
                for (dictionary<shader_uniform_base *, string>::iterator i = uniforms.iter(); i.is_valid(); ++i)
                    (*i)->opengl_loc = -1;

                for (list<shader_base *>::iterator i = shaders.iter(); i.is_valid(); ++i)
                    (*i)->unload();

                glDeleteProgram(opengl_id);                                                                         CHECK_GL_ERRORS();
                opengl_id = 0;
            }
        } // shader_program::unload()


        void shader_program::bind()
        {
            // use shader
            if (opengl_id)
            {
                glUseProgram(opengl_id);                                                                            CHECK_GL_ERRORS();
            }
            else
            {
                throw internal_exception(__FILE__, __LINE__, L"Unable to use shader program.");
            }
        } // shader_program::bind()


        void shader_program::unbind()
        {
            for (dictionary<shader_uniform_base *, string>::iterator i = uniforms.iter(); i.is_valid(); ++i)
                (*i)->opengl_loc = -1;

            glUseProgram(0);                                                                                        CHECK_GL_ERRORS();
        } // shader_program::unbind()


    } // namespace platform

} // namespace gsgl
