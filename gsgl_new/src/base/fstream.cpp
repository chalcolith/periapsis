//
// $Id: fstream.cpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "stdafx.h"
#include "fstream.hpp"
#include "pointer.hpp"
#include "exception.hpp"
#include "string.hpp"

#ifdef WIN32
#pragma warning (disable : 4996)
#endif

namespace gsgl
{

    namespace io
    {

        //

        file_stream::file_stream(void *fp, gsgl::flags_t mode)
            : data::data_object(), fname(L"???"), fp(fp), mode(mode)
        {
        } // file_stream::file_stream()


        file_stream::file_stream(const string & fname, gsgl::flags_t mode)
            : data::data_object(), fname(fname), fp(0), mode(mode)
        {
            string mode_string;

            if (mode & FILE_OPEN_READ)
            {
                if (mode & FILE_OPEN_WRITE)
                {
                    if (mode & FILE_OPEN_APPEND)
                        mode_string = L"a+";
                    else
                        mode_string = L"w+";
                }
                else
                {
                    mode_string = L"r+";
                }
            }
            else if (mode & FILE_OPEN_WRITE)
            {
                if (mode & FILE_OPEN_APPEND)
                    mode_string = L"a";
                else
                    mode_string = L"w";
            }
            else
            {
                throw runtime_exception(L"invalid file open mode %ls opening %ls", mode_string.w_string(), fname.w_string());
            }

            if (mode & FILE_OPEN_BINARY && mode & FILE_OPEN_TEXT)
                throw runtime_exception(L"You cannot open a file in both text and binary mode.");

            if (mode & FILE_OPEN_BINARY)
                mode_string.append(L"b");
            else if (mode & FILE_OPEN_TEXT)
                mode_string.append(L"t");

            FILE *f = ::fopen(fname.c_string(), mode_string.c_string());

            if (f && !::ferror(f))
            {
                fp = f;
            }
            else
            {
                throw runtime_exception(L"Unable to open %ls: %hs", fname.w_string(), strerror(errno));
            }
        } // file_stream::file_stream()


        file_stream::~file_stream()
        {
            if (fp)
                ::fclose(static_cast<FILE *>(fp));
        } // file_stream::~file_stream()


        bool file_stream::at_end() const
        {
            return ::feof(static_cast<FILE *>(fp)) != 0;
        } // file_stream::at_end()

        //

        ft_stream ft_stream::out(stdout, FILE_OPEN_WRITE | FILE_OPEN_APPEND | FILE_OPEN_TEXT);
        ft_stream ft_stream::err(stderr, FILE_OPEN_WRITE | FILE_OPEN_APPEND | FILE_OPEN_TEXT);
        ft_stream ft_stream::in(stdin, FILE_OPEN_READ | FILE_OPEN_TEXT);


        //

        ft_stream::ft_stream(void *fp, gsgl::flags_t mode) 
            : file_stream(fp, mode | FILE_OPEN_TEXT), text_stream()
        {
        } // ft_stream::ft_stream()


        ft_stream::ft_stream(const string & fname, gsgl::flags_t mode) 
            : file_stream(fname, mode | FILE_OPEN_TEXT), text_stream()
        {
        } // ft_stream::ft_stream()

        ft_stream::~ft_stream()
        {
        } // ft_stream::~ft_stream()

        wchar_t ft_stream::peek()
        {
            if (mode & FILE_OPEN_READ)
            {
                wchar_t ch = ::fgetwc(static_cast<FILE *>(fp));
                ::ungetwc(ch, static_cast<FILE *>(fp));
                return ch;
            }
            else
            {
                throw io_exception(L"Cannot read from write-only stream %ls", fname.w_string());
            }
        } // ft_stream::peek()

        wchar_t ft_stream::get()
        {
            if (mode & FILE_OPEN_READ)
            {
                wchar_t ch = ::fgetwc(static_cast<FILE *>(fp));
                return ch;
            }
            else
            {
                throw io_exception(L"cannot read from a write-only file");
            }
        } // ft_stream::get()

        void ft_stream::unget(wchar_t ch)
        {
            if (mode & FILE_OPEN_READ)
            {
                ::ungetwc(ch, static_cast<FILE *>(fp));
            }
            else
            {
                throw io_exception(L"cannot read from a write-only file");
            }
        } // ft_stream::unget()

        bool ft_stream::at_end() const
        {
            return file_stream::at_end();
        } // ft_stream::at_end()

        gsgl::index_t ft_stream::read(wchar_t *buf, const gsgl::index_t num)
        {
            if (mode & FILE_OPEN_READ)
            {
                /// \todo Implement UTF-8 for file streams.
                data::smart_pointer<char, true> cbuf(new char[num+1]);
                int num_read = (gsgl::index_t) ::fread(cbuf.ptr(), 1, num, static_cast<FILE *>(fp));
                for (int i = 0; i < num_read; ++i)
                    buf[i] = cbuf.ptr()[i];
                return num_read;
            }
            else
            {
                throw io_exception(L"cannot read from a write-only file");
            }
        } // ft_stream::read()

        gsgl::index_t ft_stream::write(const wchar_t *buf, const gsgl::index_t num)
        {
            if (mode & FILE_OPEN_WRITE)
            {
                /// \todo Implement UTF-8 for file streams.
                data::smart_pointer<char, true> cbuf (new char[num+1]);
                for (int i = 0; i < num; ++i)
                    cbuf.ptr()[i] = (char) buf[i];

                return (gsgl::index_t) ::fwrite(cbuf.ptr(), 1, num, static_cast<FILE *>(fp));
            }
            else
            {
                throw io_exception(L"cannot write to a read-only file");
            }
        } // ft_stream::write()


        //


        fd_stream::fd_stream(const string & fname, gsgl::flags_t mode)
            : file_stream(fname, mode | FILE_OPEN_BINARY), data_stream()
        {
        } // fd_stream::fd_stream()


        fd_stream::~fd_stream()
        {
        } // fd_stream::~fd_stream()


        bool fd_stream::at_end() const
        {
            return file_stream::at_end();
        } // ft_stream::at_end()

        gsgl::index_t fd_stream::read(unsigned char *ptr, gsgl::index_t num)
        {
            return static_cast<gsgl::index_t>(::fread(ptr, 1, num, static_cast<FILE *>(fp)));
        } // fd_stream::read()


        gsgl::index_t fd_stream::write(const unsigned char * ptr, gsgl::index_t num)
        {
            return static_cast<gsgl::index_t>(::fwrite(ptr, 1, num, static_cast<FILE *>(fp)));
        } // fd_stream::write()

    } // namespace io

} // namespace gsgl
