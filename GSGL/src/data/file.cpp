//
// $Id: file.cpp 314 2008-03-01 16:33:47Z Gordon $
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

#include "data/file.hpp"
#include "data/exception.hpp"
#include "data/pointer.hpp"
#include "data/fstream.hpp"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <direct.h>
#include <io.h>
#endif

namespace gsgl
{

    namespace io
    {
    
        const gsgl::index_t MAX_PATH_SIZE = 1024;
        
        file::file()
            : data_object()
        {
        } // file::file()

        file::file(const string & fname)
            : data_object(), name(fname)
        {
#ifdef WIN32
            data::smart_pointer<wchar_t, true> buf(new wchar_t[MAX_PATH_SIZE]);
            wchar_t *base;

            GetFullPathName(fname.w_string(), MAX_PATH_SIZE, buf, &base);
            if (!base)
                throw runtime_exception(L"no file in path %ls", fname.w_string());

            base_name = string(base);
            full_path = string(buf);

            *base = 0;
            dir_name = string(buf);
            string last = dir_name.right_substring(1);
            if (!last.is_empty() && !(last == L"\\" || last == L"/") )
                dir_name += directory::SEPARATOR;

            dir = directory(dir_name);
#else
#error file::file() not implemented
#endif
        } // file::file()
        
        file::file(const file & f)
            : data_object(), name(f.name), base_name(f.base_name), dir_name(f.dir_name), full_path(f.full_path), dir(f.dir)
        {
        } // file::file()
        
        file::~file()
        {
        } // file::~file()
        
        file & file::operator= (const file & f)
        {
            name      = f.name;
            base_name = f.base_name;
            dir_name  = f.dir_name;
            full_path = f.full_path;

            dir       = f.dir;

            return *this;
        } // file::operator= ()
        

        bool file::operator== (const file & f) const
        {
            return full_path == f.full_path;
        } // file::operator== ()


        bool file::operator!= (const file & f) const
        {
            return full_path != f.full_path;
        } // file::operator!=


        ft_stream *file::open_text(gsgl::flags_t mode)
        {
            return new ft_stream(name, mode);
        } // file::open()

        //ft_stream *file::open_text(const string & fname, gsgl::flags_t mode)
        //{
        //    return new ft_stream(fname, mode);
        //} // file::open()
        
        const string & file::get_name() const
        {
            return name;
        } // file::get_name()

        const string & file::get_base_name() const
        {
            return base_name;
        } // file::get_base_name()

        const string & file::get_dir_name() const
        {
            return dir_name;
        } // file::get_dir_name()

        const string & file::get_full_path() const
        {
            return full_path;
        } // file::get_full_path()

        const directory & file::get_directory() const
        {
            return dir;
        } // file::dir()

        //

        string file::get_full_path(const string & fname)
        {
            file f(fname);
            return f.get_full_path();
        } // file::get_full_path()

        
        bool file::exists(const string & fname)
        {
            return ::access(fname.c_string(), 00) == 0;
        } // file::exists()
        

        void file::remove(const string & fname)
        {
            if (::unlink(fname.c_string()) != 0)
                throw io_exception(L"Unable to delete file %ls: %hs.", fname.w_string(), ::strerror(errno));
        } // file::remove()


        static const gsgl::index_t BUF_LEN = 4096;

        void file::copy(const string & src_path, const string & dest_path)
        {
            data::smart_pointer<char, true> buf(new char[BUF_LEN]);
            
            FILE *fin = ::fopen(src_path.c_string(), "rb");
            if (!fin)
                throw io_exception(L"Unable to open %hs: %hs.", src_path.c_string(), ::strerror(errno));

            FILE *fout = ::fopen(dest_path.c_string(), "wb");
            if (!fout)
                throw io_exception(L"Unable to open %hs: %hs.", dest_path.c_string(), ::strerror(errno));

            size_t num_read;
            while ((num_read = ::fread(buf, sizeof(char), BUF_LEN, fin)))
            {
                size_t num_written = ::fwrite(buf, sizeof(char), num_read, fout);
                if (num_written != num_read)
                {
                    fclose(fout);
                    fclose(fin);
                    throw io_exception(L"Error copying %hs to %hs: %hs.", src_path.c_string(), dest_path.c_string(), ::strerror(errno));
                }
            }

            fclose(fout);
            fclose(fin);
        } // file::copy()

    } // namespace io
    
} // namespace gsgl
