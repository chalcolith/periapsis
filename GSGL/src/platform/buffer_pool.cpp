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

#include "platform/buffer_pool.hpp"


namespace gsgl
{

    using namespace data;

    namespace platform
    {

        buffer_pool::buffer_pool(const vbuffer::update_mode & mode, const gsgl::index_t & num_objects_per_bucket, const gsgl::index_t & num_vertices_per_object, const gsgl::index_t & num_indices_per_object)
            : mode(mode), num_objects_per_bucket(num_objects_per_bucket),
              num_vertices_per_object(num_vertices_per_object), num_indices_per_object(num_indices_per_object)
        {
        } // buffer_pool::buffer_pool()


        buffer_pool::~buffer_pool()
        {
            for (simple_array<bucket *>::iterator i = buckets.iter(); i.is_valid(); ++i)
                delete *i;
        } // buffer_pool::~buffer_pool()

        
        buffer_pool::object_record buffer_pool::allocate_object()
        {
            buffer_pool::object_record result;

            if (available_objects.size())
            {
                result = available_objects.top();
                available_objects.pop();
            }
            else
            {
                // allocate a new bucket, return the first object, and free the rest (go backwards so earlier objects will appear sooner)
                bucket *new_bucket = new bucket(mode, num_objects_per_bucket * num_vertices_per_object, num_objects_per_bucket * num_indices_per_object);

                for (int i = num_objects_per_bucket-1; i >= 0; --i)
                {
                    object_record rec;

                    rec.parent = new_bucket;
                    rec.pos_in_vertices = i * num_vertices_per_object;
                    rec.pos_in_indices  = i * num_indices_per_object;

                    if (i == 0)
                        result = rec;
                    else
                        free_object(rec);
                }
            }

            return result;
        } // buffer_pool::allocate_object()


        void buffer_pool::free_object(const object_record & obj)
        {
            available_objects.push(obj);
        } // buffer_pool::free_object()


        void buffer_pool::load()
        {
            for (simple_array<bucket *>::iterator i = buckets.iter(); i.is_valid(); ++i)
            {
                (*i)->vertices.load();
                (*i)->indices.load();
            }
        } // buffer_pool::load()


        void buffer_pool::unload()
        {
            for (simple_array<bucket *>::iterator i = buckets.iter(); i.is_valid(); ++i)
            {
                (*i)->vertices.unload();
                (*i)->vertices.unload();
            }
        } // buffer_pool::unload()


    } // namespace platform

} // namespace gsgl
