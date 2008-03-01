#ifndef GSGL_DATA_CACHE_H
#define GSGL_DATA_CACHE_H

//
// $Id: cache.hpp 314 2008-03-01 16:33:47Z Gordon $
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

#include "data/data.hpp"
#include "data/log.hpp"
#include "data/singleton.hpp"
#include "data/dictionary.hpp"
#include "data/string.hpp"

namespace gsgl
{

    namespace data
    {

        /// Instances of this class's subclasses will automatically register themselves with the global cache, if the cache() constructer is included in the initializer list of the subclass.
        template <typename T>
        class cache
            : public singleton< cache<T> >
        {
            gsgl::string name;
            dictionary<T *, gsgl::string> items;

        public:
            cache(const gsgl::string & name);
            virtual ~cache();

            bool contains_index(const gsgl::string & key) const { return items.contains_index(key); }
            T * & operator[] (const gsgl::string & key) { return items[key]; }
        }; // class cache


        template <typename T>
        cache<T>::cache(const gsgl::string & name)
            : singleton(), name(name)
        {
        } // cache<T>::cache()


        template <typename T>
        cache<T>::~cache()
        {
            for (dictionary<T *, gsgl::string>::iterator i = items.iter(); i.is_valid(); ++i)
            {
                T *item = *i;

                if (item)
                {
                    if (item->get_ref_count() > 1)
                        gsgl::log(string(L"cache: MEMORY LEAK: ") + name + L": dangling reference for " + i.get_index());
                    item->detach();
                }
                else
                {
                    gsgl::log(string(L"cache: ") + name + L": cache error: null pointer for " + i.get_index());
                }
            }
            
            items.clear();
        } // cache<T>::~cache()


    } // namespace data

} // namespace gsgl

#endif
