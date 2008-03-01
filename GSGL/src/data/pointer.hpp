#ifndef GSGL_DATA_POINTER_H
#define GSGL_DATA_POINTER_H

//
// $Id: pointer.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "data/shared.hpp"

namespace gsgl
{

    namespace data
    {

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

            const T *ptr() const;
            T * ptr();

            const T & value() const;
            T & value();

            const T & operator* () const;
            T & operator* ();

            const T * operator-> () const;
            T *operator-> ();

            operator const T * () const;
            operator T * ();

            bool operator== (const smart_pointer & p);
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

        template <typename T, bool ARRAY_PTR>
        const T *smart_pointer<T,ARRAY_PTR>::ptr() const
        {
            return data;
        } // smart_pointer<T,ARRAY_PTR>::ptr()

        template <typename T, bool ARRAY_PTR>
        T *smart_pointer<T,ARRAY_PTR>::ptr()
        {
            return data;
        } // smart_pointer<T,ARRAY_PTR>::ptr()

        template <typename T, bool ARRAY_PTR>
        const T & smart_pointer<T,ARRAY_PTR>::value() const
        {
            return *data;
        } // smart_pointer<T,ARRAY_PTR>::value()

        template <typename T, bool ARRAY_PTR>
        T & smart_pointer<T,ARRAY_PTR>::value()
        {
            return *data;
        } // smart_pointer<T,ARRAY_PTR>::value()

        template <typename T, bool ARRAY_PTR>
        const T & smart_pointer<T,ARRAY_PTR>::operator* () const
        {
            return *data;
        } // smart_pointer<T,ARRAY_PTR>::operator* ()

        template <typename T, bool ARRAY_PTR>
        T & smart_pointer<T,ARRAY_PTR>::operator* ()
        {
            return *data;
        } // smart_pointer<T,ARRAY_PTR>::operator* ()

        template <typename T, bool ARRAY_PTR>
        const T *smart_pointer<T,ARRAY_PTR>::operator-> () const
        {
            return data;
        } // smart_pointer<T,ARRAY_PTR>::operator-> ()

        template <typename T, bool ARRAY_PTR>
        T *smart_pointer<T,ARRAY_PTR>::operator-> ()
        {
            return data;
        } // smart_pointer<T,ARRAY_PTR>::operator-> ()

        template <typename T, bool ARRAY_PTR>
        smart_pointer<T,ARRAY_PTR>::operator const T * () const
        {
            return data;
        } // smart_pointer<T,ARRAY_PTR>::operator const T * ()

        template <typename T, bool ARRAY_PTR>
        smart_pointer<T,ARRAY_PTR>::operator T * ()
        {
            return data;
        } // smart_pointer<T,ARRAY_PTR>::operator T * ()


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


        /// A smart pointer class with reference counting.
        /// When the last shared pointer goes out of scope, the object it's pointing to will be deleted.
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
            
            const T * ptr() const;
            T * ptr();

            const T & value() const;
            T & value();

            const T & operator* () const;
            T & operator* ();

            const T * operator-> () const;
            T *operator-> ();

            operator const T * () const;
            operator T * ();

            bool operator== (const shared_pointer & p) const;
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
        

        template <typename T, bool ARRAY_PTR>
        const T *shared_pointer<T,ARRAY_PTR>::ptr() const
        {
            return impl->data;
        } // shared_pointer<T>::ptr()


        template <typename T, bool ARRAY_PTR>
        T *shared_pointer<T,ARRAY_PTR>::ptr()
        {
            return impl->data;
        } // shared_pointer<T>::ptr()


        template <typename T, bool ARRAY_PTR>
        const T & shared_pointer<T,ARRAY_PTR>::value() const
        {
            return *impl->data;
        } // shared_pointer<T>::value()


        template <typename T, bool ARRAY_PTR>
        T & shared_pointer<T,ARRAY_PTR>::value()
        {
            return *impl->data;
        } // shared_pointer<T>::value()


        template <typename T, bool ARRAY_PTR>
        const T & shared_pointer<T,ARRAY_PTR>::operator* () const
        {
            return *impl->data;
        } // shared_pointer<T>::operator* ()


        template <typename T, bool ARRAY_PTR>
        T & shared_pointer<T,ARRAY_PTR>::operator* ()
        {
            return *impl->data;
        } // shared_pointer<T>::operator* ()


        template <typename T, bool ARRAY_PTR>
        const T *shared_pointer<T,ARRAY_PTR>::operator-> () const
        {
            return impl->data;
        } // shared_pointer<T>::operator-> ()
        

        template <typename T, bool ARRAY_PTR>
        T *shared_pointer<T,ARRAY_PTR>::operator-> ()
        {
            return impl->data;
        } // shared_pointer<T>::operator-> ()
        

        template <typename T, bool ARRAY_PTR>
        shared_pointer<T,ARRAY_PTR>::operator const T * () const
        {
            return impl->data;
        } // shared_pointer<T>::operator ()


        template <typename T, bool ARRAY_PTR>
        shared_pointer<T,ARRAY_PTR>::operator T * ()
        {
            return impl->data;
        } // shared_pointer<T>::operator ()
        

        template <typename T, bool ARRAY_PTR>
        bool shared_pointer<T,ARRAY_PTR>::operator== (const shared_pointer & p) const
        {
            return impl->data == p.impl->data;
        } // shared_pointer<T,ARRAY_PTR>::operator== ()


        //

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

        
    } // namespace data
    
} // namespace gsgl

#endif
