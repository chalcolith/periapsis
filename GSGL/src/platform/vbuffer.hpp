#ifndef GSGL_PLATFORM_VBUFFER_H
#define GSGL_PLATFORM_VBUFFER_H

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
#include "data/array.hpp"
#include "math/vector.hpp"

namespace gsgl
{

    namespace platform
    {

        namespace vbuffer
        {

            typedef unsigned int index_t;
            typedef float real_t;

            /// Used to specify whether a buffer is dynamic or static.
            enum update_mode { STATIC = 0, DYNAMIC = 1 };

            /// Returns a pointer that represents a byte offset for use with the OpenGL array drawing functions.
            /// The template parameter T should be the type of the elements in the array; this is used to get the size.
            template <typename T>
            void *VBO_OFFSET(const vbuffer::index_t & num_elements_to_offset)
            {
                return static_cast<char *>(0) + (num_elements_to_offset * sizeof(T));
            }

        } // namespace vbuffer


        //////////////////////////////////////////////////////////////

        /// An intermediate class so we don't pollute the header namespaces with OpenGL stuff.
        class PLATFORM_API vbuffer_base
        {
            const int target, gl_mode;
            int opengl_id;

            size_t prev_size;

            static int last_bound_buffer;

        public:
            vbuffer_base(const int & target, const int & mode);
            virtual ~vbuffer_base();

            void load();   ///< Loads the buffer object into the video card.
            void bind();   ///< Binds the buffer object for use with OpenGL.
            void unbind(); ///< Unbinds the buffer object for use with OpenGL.
            void unload(); ///< Unloads the buffer object from the video card.            
            
        protected:
            int lowest_dirty_index, highest_dirty_index;

            virtual gsgl::index_t buffer_size() = 0;
            virtual size_t element_size() = 0;
            virtual void *get_ptr() = 0;
        }; // class vbuffer_base


        //////////////////////////////////////////////////////////////

        /// Mixes in an array of the appropriate data type.
        template <typename T>
        class vbuffer_mixin
            : public vbuffer_base
        {
            gsgl::data::simple_array<T> buffer;

        public:
            vbuffer_mixin(const int & target, const int & gl_mode, const gsgl::index_t initial_capacity = 0) 
                : vbuffer_base(target, gl_mode), buffer(initial_capacity) {}
            virtual ~vbuffer_mixin() {}

            gsgl::data::simple_array<T> & get_buffer() { return buffer; }

            gsgl::index_t size() const { return buffer.size(); }
            void append(const T & t) { buffer.append(t); }

            inline const T & operator[] (const gsgl::index_t & i) const { return buffer[i]; }
            inline T & operator[] (const gsgl::index_t & i) { if (i < lowest_dirty_index) lowest_dirty_index = i; if (i > highest_dirty_index) highest_dirty_index = i; return buffer[i]; }

        protected:
            virtual gsgl::index_t buffer_size() { return buffer.size(); }
            virtual size_t element_size() { return sizeof(T); }
            virtual void *get_ptr() { return buffer.ptr(); }
        }; // class vbuffer_mixin


        //

        /// Encapsulates an index buffer object.
        class PLATFORM_API index_buffer
            : public vbuffer_mixin<vbuffer::index_t>
        {
        public:
            index_buffer(const vbuffer::update_mode & mode, const gsgl::index_t inital_capacity = 0);
            virtual ~index_buffer();
        }; // class index_buffer


        /// Encapsulates a vertex buffer object.
        class PLATFORM_API vertex_buffer
            : public vbuffer_mixin<vbuffer::real_t>
        {
        public:
            vertex_buffer(const vbuffer::update_mode & mode, const gsgl::index_t initial_capacity = 0);
            virtual ~vertex_buffer();

            /// \name Vertex component access.
            /// These functions assume 3 components per vertex.
            /// @{
            inline gsgl::math::vector get_vector(const gsgl::index_t & i) const { return gsgl::math::vector( (*this)[i*3+0], (*this)[i*3+1], (*this)[i*3+2] ); }

            inline const vbuffer::real_t & get_x(const gsgl::index_t & i) const { return (*this)[i*3+0]; }
            inline const vbuffer::real_t & get_y(const gsgl::index_t & i) const { return (*this)[i*3+1]; }
            inline const vbuffer::real_t & get_z(const gsgl::index_t & i) const { return (*this)[i*3+2]; }

            inline vbuffer::real_t & get_x(const gsgl::index_t & i) { return (*this)[i*3+0]; }
            inline vbuffer::real_t & get_y(const gsgl::index_t & i) { return (*this)[i*3+1]; }
            inline vbuffer::real_t & get_z(const gsgl::index_t & i) { return (*this)[i*3+2]; }
            /// @}

            /// \name Texture coordinate access.
            /// These functions assume 2 components per coordinate.
            /// @{
            inline const vbuffer::real_t & get_s(const gsgl::index_t & i) const { return (*this)[i*2+0]; }
            inline const vbuffer::real_t & get_t(const gsgl::index_t & i) const { return (*this)[i*2+1]; }

            inline vbuffer::real_t & get_s(const gsgl::index_t & i) { return (*this)[i*2+0]; }
            inline vbuffer::real_t & get_t(const gsgl::index_t & i) { return (*this)[i*2+1]; }
            /// @}
        }; // class vertex_buffer


    } // namespace platform

} // namespace gsgl

#endif
