#ifndef GSGL_DATA_POINTER_H
#define GSGL_DATA_POINTER_H

//
// $Id: pointer.hpp 15 2008-07-20 17:36:39Z kulibali $
//
// Copyright (c) 2008-2010, The Periapsis Project. All rights reserved. 
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

namespace gsgl
{

    namespace data
    {

        /// A simple smart pointer meant to be used for RAII in a single scope -- it cannot be copied.
        template <typename T, bool ARRAY_PTR = false>
        class smart_pointer
            : public data_object
        {
            T * data;

            smart_pointer(const smart_pointer & sp);
            smart_pointer & operator= (const smart_pointer & sp);
        public:
            explicit smart_pointer(T *data = 0);
            ~smart_pointer();

            const T *ptr() const { return data; }
            T * ptr() { return data; }

            const T & value() const { return *data; }
            T & value() { return *data; }

            const T & operator* () const { return *data; }
            T & operator* () { return *data; }

            const T * operator-> () const { return data; }
            T *operator-> () { return data; }

            operator const T * () const { return data; }
            operator T * () { return data; }

            bool operator== (const smart_pointer & p) { return data == p.data; }
        }; // class smart_pointer


        template <typename T, bool ARRAY_PTR>
        smart_pointer<T,ARRAY_PTR>::smart_pointer(T *data)
            : data_object(), data(data)
        {
        } // smart_pointer<T,ARRAY_PTR>::smart_pointer()


        template <typename T, bool ARRAY_PTR>
        smart_pointer<T,ARRAY_PTR>::smart_pointer(const smart_pointer & sp)
        {
            throw internal_exception(__FILE__, __LINE__, L"Cannot assign a smart pointer.");
        } // smart_pointer<T,ARRAY_PTR>::smart_pointer()


        template <typename T, bool ARRAY_PTR>
        smart_pointer<T,ARRAY_PTR> & smart_pointer<T,ARRAY_PTR>::operator= (const smart_pointer & sp)
        {
            throw internal_exception(__FILE__, __LINE__, L"Cannot assign a smart pointer.");
        } // smart_pointer<T,ARRAY_PTR>::smart_pointer()

        template <typename T, bool ARRAY_PTR>
        smart_pointer<T,ARRAY_PTR>::~smart_pointer()
        {
            if (ARRAY_PTR)
                delete [] data;
            else
                delete data;
        } // smart_pointer<T,ARRAY_PTR>::~smart_pointer()


        //////////////////////////////////////////////////////////////

        /// A base class for reference-counted shared objects.
        class BASE_API shared_object
            : public data_object
        {
            int ref_count;
        public:
            shared_object() : data_object(), ref_count(0) {}
            virtual ~shared_object() {}

            void attach() { ++ref_count; }

            /// Returns true if the object deleted itself.
            bool detach() 
            {
                if (--ref_count == 0)
                {
                    delete this;
                    return true;
                }

                return false;
            }

            const int & get_ref_count() const { return ref_count; }
        }; // class shared_object


        //////////////////////////////////////////////////////////////
        
        template <typename T, bool ARRAY_PTR = false>
        class shared_pointer_impl 
            : public shared_object
        {
            T *data;
        public:
            shared_pointer_impl(T *data);
            virtual ~shared_pointer_impl();

            template <typename T, bool ARRAY_PTR>
            friend class shared_pointer;
        }; // class shared_pointer_impl


        template <typename T, bool ARRAY_PTR>
        shared_pointer_impl<T,ARRAY_PTR>::shared_pointer_impl(T *data)
            : shared_object(), data(data)
        {
        } // shared_pointer_impl<T,AP>::shared_pointer_impl()


        template <typename T, bool ARRAY_PTR>
        shared_pointer_impl<T,ARRAY_PTR>::~shared_pointer_impl()
        {
            if (ARRAY_PTR)
                delete [] data;
            else
                delete data;
        } // shared_pointer_impl<T,AP>::~shared_pointer_impl()


        //////////////////////////////////////////////////////////////

        /// A smart pointer class with reference counting.
        /// When the last shared pointer goes out of scope, the object it points to will be deleted.
        /// Assigning a new pointer value to a shared pointer object will break its link to the old object.
        template <typename T, bool ARRAY_PTR = false>
        class shared_pointer
            : public data_object
        {
            shared_pointer_impl<T, ARRAY_PTR> *impl;            

        public:
            shared_pointer(T *data = 0);
            shared_pointer(const shared_pointer & p);
            
            shared_pointer & operator= (const shared_pointer & p);
            shared_pointer & operator= (T *data);
            
            ~shared_pointer();
            
            inline const T * ptr() const { return impl->data; }
            inline T * ptr() { return impl->data; }

            inline const T & value() const { return *impl->data; }
            inline T & value() { return *impl->data; }

            const T & operator* () const { return *impl->data; }
            T & operator* () { return *impl->data; }

            const T * operator-> () const { return impl->data; }
            T *operator-> () { return impl->data; }

            //operator const T * () const { return impl->data; }
            //operator T * () { return impl->data; }

            bool operator== (const shared_pointer & p) const { return impl->data == p.impl->data; }

            const int & get_ref_count() const { return impl->get_ref_count(); }
        }; // class shared_pointer
        

        //
        
        template <typename T, bool ARRAY_PTR>
        shared_pointer<T,ARRAY_PTR>::shared_pointer(T * data)
            : data_object(), impl(new shared_pointer_impl<T,ARRAY_PTR>(data))
        {
            impl->attach();
        } // shared_pointer<T>::shared_pointer()
        

        template <typename T, bool ARRAY_PTR>
        shared_pointer<T,ARRAY_PTR>::shared_pointer(const shared_pointer & p)
            : data_object(), impl(p.impl)
        {
            impl->attach();
        } // shared_pointer<T>::shared_pointer()
        

        template <typename T, bool ARRAY_PTR>
        shared_pointer<T,ARRAY_PTR> & shared_pointer<T,ARRAY_PTR>::operator= (const shared_pointer & p)
        {
            impl->detach();
            impl = p.impl;
            impl->attach();

            return *this;
        } // shared_pointer<T>::shared_pointer= ()
        

        template <typename T, bool ARRAY_PTR>
        shared_pointer<T,ARRAY_PTR> & shared_pointer<T,ARRAY_PTR>::operator= (T *data)
        {
            impl->detach();    
            impl = new shared_pointer_impl<T,ARRAY_PTR>(data);
            impl->attach();

            return *this;
        } // shared_pointer<T>::operator= ()
        

        template <typename T, bool ARRAY_PTR>
        shared_pointer<T,ARRAY_PTR>::~shared_pointer()
        {
            impl->detach();
        } // shared_pointer<T>::~shared_pointer()

        
    } // namespace data
    
} // namespace gsgl

#endif
