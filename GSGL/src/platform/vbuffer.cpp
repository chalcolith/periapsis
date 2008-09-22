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

#include "platform/vbuffer.hpp"
#include "platform/lowlevel.hpp"

#include <cstring>

namespace gsgl
{

    using namespace data;

    namespace platform
    {

        vbuffer_base::vbuffer_base(const int & target, const int & gl_mode)
            : target(target), gl_mode(gl_mode), opengl_id(0), 
              prev_size(0), lowest_dirty_index(INT_MAX), highest_dirty_index(-1)
        {
        } // vbuffer_base::vbuffer_base()


        vbuffer_base::~vbuffer_base()
        {
            unload();
        } // vbuffer_base::~vbuffer_base()


        void vbuffer_base::load()
        {
            glGenBuffers(1, (GLuint *) &opengl_id);                                                                 CHECK_GL_ERRORS();
            if (!opengl_id)
                throw opengl_exception(__FILE__, __LINE__, L"Unable to generate buffer id: %hs", gluErrorString(glGetError()));
        } // vbuffer_base::load()


        void vbuffer_base::unload()
        {
            if (opengl_id)
            {
                glDeleteBuffers(1, (GLuint *) &opengl_id);                                                          CHECK_GL_ERRORS();
                opengl_id = 0;
                prev_size = 0;
            }
        } // vbuffer_base::unload()


        void vbuffer_base::bind()
        {
            if (!opengl_id)
                load();

            glBindBuffer(target, opengl_id);                                                                        CHECK_GL_ERRORS();

            // update the video card if the buffer is dirty
            if (buffer_size() != prev_size)
            {
                glBufferData(target, buffer_size() * element_size(), get_ptr(), gl_mode);                                                   CHECK_GL_ERRORS();
                prev_size = buffer_size();
            }
            else if (lowest_dirty_index != INT_MAX || highest_dirty_index != -1)
            {
                if (lowest_dirty_index == INT_MAX)
                    lowest_dirty_index = 0;
                if (highest_dirty_index == -1)
                    highest_dirty_index = buffer_size()-1;

                if (gl_mode == GL_STATIC_DRAW)
                {
                    glBufferData(target, prev_size, get_ptr(), gl_mode);                                                                   CHECK_GL_ERRORS();
                }
                else
                {
                    glBufferSubData(target, 
                                    lowest_dirty_index * element_size(), 
                                    ((highest_dirty_index - lowest_dirty_index)+1) * element_size(), 
                                    ((char *)get_ptr()) + (lowest_dirty_index * element_size()));                                       CHECK_GL_ERRORS();
                }
            }

            highest_dirty_index = -1;
            lowest_dirty_index = INT_MAX;
        } // vbuffer_base::bind()


        void vbuffer_base::unbind()
        {
            glBindBuffer(target, 0);
        } // vbuffer_base::unbind()


        //

        index_buffer::index_buffer(const vbuffer::update_mode & mode, const gsgl::index_t initial_capacity)
            : vbuffer_mixin<unsigned int>(GL_ELEMENT_ARRAY_BUFFER, mode == vbuffer::STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, initial_capacity)
        {
        } // index_buffer::index_buffer()


        index_buffer::~index_buffer()
        {
        } // index_buffer::~index_buffer()


        //

        vertex_buffer::vertex_buffer(const vbuffer::update_mode & mode, const gsgl::index_t initial_capacity)
            : vbuffer_mixin<float>(GL_ARRAY_BUFFER, mode == vbuffer::STATIC ? GL_STATIC_DRAW : GL_DYNAMIC_DRAW, initial_capacity)
        {
        } // vertex_buffer::vertex_buffer()


        vertex_buffer::~vertex_buffer()
        {
        } // vertex_buffer::~vertex_buffer()



        namespace vbuffer
        {


        } // namespace vbuffer


    } // namespace platform

} // namespace gsgl

