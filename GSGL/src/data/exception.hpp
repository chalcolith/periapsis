#ifndef GSGL_DATA_EXCEPTION_H
#define GSGL_DATA_EXCEPTION_H

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


#ifdef WIN32
#ifdef GSGL_DATA_EXPORTS
#define DATA_API __declspec(dllexport)
#else
#define DATA_API __declspec(dllimport)
#endif
#else
#define DATA_API 
#endif


namespace gsgl
{

    class DATA_API exception
    {
    protected:
        wchar_t *message;
        exception();
    public:
        exception(const wchar_t *format, ...);
        virtual ~exception();
        
        const wchar_t *get_message() const;
    }; // struct exception
    

    //
    
    class DATA_API internal_exception 
        : public exception
    {
    protected:
        char *fname;
        int line;
        internal_exception(const char *fname, const int line);
    public:
        internal_exception(const char *fname, const int line, const wchar_t *format, ...);
        virtual ~internal_exception();
    }; // struct internal_exception
    

    class DATA_API memory_exception 
        : public internal_exception
    {
    public:
        memory_exception(const char *fname, const int line, const wchar_t *format, ...);
    }; // class memory_exception


	//

    class DATA_API opengl_exception
        : public internal_exception
    {
    public:
        opengl_exception(const char *fname, const int line, const wchar_t *format, ...);
    }; // class opengl_exception


    //

    class DATA_API assert_exception
        : public internal_exception
    {
    public:
        assert_exception(const char *fname, const int line, const char *message);
    }; // class assert_exception


	//
    
    class DATA_API runtime_exception 
        : public exception
    {
    protected:
        runtime_exception();
    public:
        runtime_exception(const wchar_t *format, ...);
    }; // struct user_error


    class DATA_API io_exception 
        : public runtime_exception
    {
    public:
        io_exception(const wchar_t *format, ...);
    }; // struct io_error
    

    //
    
#ifdef assert
#undef assert
#endif
    
#ifdef DEBUG
#define assert(test) { if (!(test)) throw gsgl::assert_exception(__FILE__, __LINE__, #test); }
#else
#define assert(test)
#endif

} // namespace gsgl

#endif
