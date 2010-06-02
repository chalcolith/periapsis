#ifndef GSGL_DATA_STACK_H
#define GSGL_DATA_STACK_H

//
// $Id: stack.hpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "data.hpp"
#include "array.hpp"

namespace gsgl
{

    namespace data
    {

        /// The usual FILO stack structure.  This should only be used for storing simple types, as items will be copied around in memory.
        template <typename T>
        class simple_stack
            : public simple_array<T>
        {
        public:
            simple_stack();
            simple_stack(const simple_stack & s);
            simple_stack & operator= (const simple_stack & s);
            virtual ~simple_stack();

            /// \name simple_stack Functionality
            /// @{
            const T & top() const;
            T & top();
            void push(const T & item);
            void pop();
            /// @}

        }; // class simple_stack
        

        // implementation
        
        template <typename T> 
        simple_stack<T>::simple_stack() 
            : simple_array<T>()
        {
        } // simple_stack<T>::simple_stack()

        
        template <typename T> 
        simple_stack<T>::simple_stack(const simple_stack & s) 
            : simple_array<T>(s)
        {
        } // simple_stack<T>::simple_stack()
        

        template <typename T> 
        simple_stack<T> & simple_stack<T>::operator= (const simple_stack & s)
        {
            *dynamic_cast<simple_array<T> *>(this) = s;
            return *this;
        } // simple_stack<T>::operator= ()


        template <typename T>
        simple_stack<T>::~simple_stack()
        {
        } // simple_stack<T>::~simple_stack()
                

        template <typename T> 
        const T & simple_stack<T>::top() const
        {
            if (size())
                return item(size()-1);
            else
                throw memory_exception(__FILE__, __LINE__, L"Stack underflow!");
        } // simple_stack<T>::top()
        

        template <typename T> 
        T & simple_stack<T>::top()
        {
            if (size())
                return item(size()-1);
            else
                throw memory_exception(__FILE__, __LINE__, L"Stack underflow!");
        } // simple_stack<T>::top()
        

        template <typename T> 
        void simple_stack<T>::push(const T & item)
        {
            append(item);
        } // simple_stack<T>::push()
        

        template <typename T> 
        void simple_stack<T>::pop()
        {
            if (size())
            {
                resize(size()-1);
            }
            else
            {
                throw memory_exception(__FILE__, __LINE__, L"Stack underflow!");
            }
        } // simple_stack<T>::pop()
        

    } // namespace data
    
} // namespace gsgl

#endif
