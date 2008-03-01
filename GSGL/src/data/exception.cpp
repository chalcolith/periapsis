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

#include "data/exception.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cwchar>
#include <cstdarg>


#ifdef WIN32
#pragma warning (disable:4251)
#pragma warning (disable:4996)
#endif


namespace gsgl
{

#define BUF_SIZE 1024

#define MAKE_MESSAGE() \
    { \
        wchar_t buf[BUF_SIZE]; \
        va_list args; \
        va_start(args, format); \
        ::vswprintf(buf, BUF_SIZE, format, args); \
        va_end(args); \
        message = ::wcsdup(buf); \
    }

#define MAKE_MESSAGE_INTERNAL() \
    { \
        wchar_t buf[BUF_SIZE]; \
        va_list args; \
        va_start(args, format); \
        ::vswprintf(buf, BUF_SIZE, format, args); \
        va_end(args); \
        message = ::wcsdup(buf); \
        ::swprintf(buf, BUF_SIZE, L"%hs: %d: %ls \n(%ls).", fname, line, message, L"Please contact the developer"); \
        ::free(message); \
        message = ::wcsdup(buf); \
    }
 
    //

    exception::exception()
        : message(0)
    {
    } // exception::exception()

    exception::exception(const wchar_t *format, ...)
        : message(0)
    {
        MAKE_MESSAGE();
    } // exception::exception()
    
    exception::~exception()
    {
        ::free(message);
    } // exception::~exception()
    
    const wchar_t *exception::get_message() const
    {
        return message;
    } // exception::get_message()
    
    //

    internal_exception::internal_exception(const char *fname, const int line)
        : exception(), fname(::strdup(fname)), line(line)
    {
    } // internal_exception::internal_exception()

    internal_exception::internal_exception(const char *fname, const int line, const wchar_t *format, ...) 
        : exception(), fname(::strdup(fname)), line(line)
    {
        MAKE_MESSAGE_INTERNAL();
    } // internal_exception::internal_exception()
        
    internal_exception::~internal_exception()
    {
        ::free(fname);
    } // internal_exception::~internal_exception()
    
    //

    memory_exception::memory_exception(const char *fname, const int line, const wchar_t *format, ...) 
        : internal_exception(fname, line)
    {
        MAKE_MESSAGE_INTERNAL();
    } // memory_exception::memory_exception()

    //

    assert_exception::assert_exception(const char *fname, const int line, const char *message)
        : internal_exception(fname, line, L"%hs", message)
    {
    } // assert_exception::assert_exception()

    //
    
    opengl_exception::opengl_exception(const char *fname, const int line, const wchar_t *format, ...) 
        : internal_exception(fname, line)
    {
        MAKE_MESSAGE_INTERNAL();
    } // opegnl_exception::opengl_exception()
    
    //

    runtime_exception::runtime_exception()
        : exception()
    {
    }; // runtime_exception::runtime_exception()

    runtime_exception::runtime_exception(const wchar_t *format, ...) 
        : exception()
    {
        MAKE_MESSAGE();
    } // runtime_exception::runtime_exception()
    
    //

    io_exception::io_exception(const wchar_t *format, ...) 
        : runtime_exception()
    {
        MAKE_MESSAGE();
    } // io_exception::io_exception()
    
} // namespace gsgl
