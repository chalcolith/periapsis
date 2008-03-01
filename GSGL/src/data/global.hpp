#ifndef GSGL_DATA_GLOBAL_H
#define GSGL_DATA_GLOBAL_H

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

#include "data/data.hpp"
#include "data/exception.hpp"
#include "data/singleton.hpp"
#include "data/list.hpp"

namespace gsgl
{

    namespace data
    {

        /// Base class for global resource registers.  They are singleton objects which delete themselves when their last registered resource unregisters itself.
        /// The resource class must implement a \c cleanup() function.
        template <typename R, typename L = gsgl::data::list<R *> >
        class global_register
            : public singleton<global_register<R,L> >
        {
        protected:
            L registered_resources;

            global_register();
            virtual ~global_register();

            /// Reimplemented in order to auto-create.
            static global_register<R,L> *global_instance();

        public:
            void register_resource(R *);
            void unregister_resource(R *);
        }; // class global_register


        template <typename R, typename L>
        global_register<R,L>::global_register()
            : singleton<global_register<R,L> >()
        {
        } // global_register<R,L>::global_register()


        template <typename R, typename L>
        global_register<R,L>::~global_register()
        {
        } // global_register<R,L>::~global_register()


        template <typename R, typename L>
        void global_register<R,L>::register_resource(R *r)
        {
            global_register_resource_aux<R>(registered_resources, r);
        } // global_register<R,L>::register_resource()

        
        /// Auxiliary function to get around the inability to partially specialize template member functions.
        template <typename R>
        void global_register_resource_aux(gsgl::data::list<R *> & l, R *r)
        {
            assert(r);
            l.append(r);
        } // global_register_resource_aux()


        template <typename R, typename L>
        void global_register<R,L>::unregister_resource(R *r)
        {
            global_unregister_resource_aux<R>(registered_resources, r);
            
            if (registered_resources.is_empty())
                delete this;
        } // global_register<R,L>::unregister_resource()

        
        template <typename R>
        void global_unregister_resource_aux(gsgl::data::list<R *> & l, R *r)
        {
            assert(r);

            gsgl::data::list<R *>::iterator i = l.find_value(r);

            if (i.is_valid())
                l.remove(i);
            else
                throw internal_exception(__FILE__, __LINE__, L"Attempted to unregister an invalid resource.");
        } // global_unregister_resource_aux()


    } // namespace data

} // namespace gsgl


#endif
