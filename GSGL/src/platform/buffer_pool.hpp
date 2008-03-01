#ifndef GSGL_PLATFORM_BUFFER_POOL_H
#define GSGL_PLATFORM_BUFFER_POOL_H

//
// $Id: buffer_pool.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "platform/vbuffer.hpp"
#include "data/array.hpp"
#include "data/stack.hpp"

namespace gsgl
{

    namespace platform
    {

        class PLATFORM_API buffer_pool
        {
            const vbuffer::update_mode mode;
            const gsgl::index_t num_objects_per_bucket;
            const gsgl::index_t num_vertices_per_object;
            const gsgl::index_t num_indices_per_object;

        public:
            struct bucket
            {
                vertex_buffer vertices;
                index_buffer indices;

                bucket(const vbuffer::update_mode & mode, const gsgl::index_t num_vertices = 0, const gsgl::index_t num_indices = 0)
                    : vertices(mode, num_vertices), indices(mode, num_indices) {}
            };

            struct object_record
            {
                vbuffer::index_t pos_in_vertices;
                vbuffer::index_t pos_in_indices;
                bucket *parent;

                bool operator== (const object_record & or) const
                {
                    return pos_in_vertices == or.pos_in_vertices && pos_in_indices == or.pos_in_indices && parent == or.parent;
                }
            };

        private:
            data::simple_array<bucket *> buckets;
            data::simple_stack<object_record> available_objects;

        public:
            buffer_pool(const vbuffer::update_mode & mode, const gsgl::index_t & num_objects_per_bucket, const gsgl::index_t & num_vertices_per_object, const gsgl::index_t & num_indices_per_object);
            virtual ~buffer_pool();

            object_record allocate_object();
            void free_object(const object_record &);

            void load();
            void unload();
        }; // class buffer_pool

    } // namespace platform

} // namespace gsgl

#endif
