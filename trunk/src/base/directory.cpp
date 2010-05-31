//
// $Id: directory.cpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "data/directory.hpp"
#include "data/pointer.hpp"
#include "data/list.hpp"
#include "data/file.hpp"

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

#ifdef WIN32
        const string directory::SEPARATOR(L"\\");
#else
#error Must define directory separator!
#endif

        //
    
        directory::directory()
            : data_object()
        {
            data::smart_pointer<char, true> buf(new char[MAX_PATH_SIZE]);
            ::_getcwd(buf, MAX_PATH_SIZE);

            name = string(buf);
            get_paths();
        } // directory::directory()
        
        directory::directory(const string & name)
            : data_object(), name(name)
        {
            get_paths();
        } // directory::directory()
        
        directory::directory(const directory & d)
            : data_object(), name(d.name)
        {
            get_paths();
        } // directory::directory()
        
        directory::~directory()
        {
        } // directory::~directory()
        
        directory & directory::operator= (const directory & d)
        {
            files.clear();
            dirs.clear();

            name = d.name;
            get_paths();

            return *this;
        } // directory::operator= ()


        int directory::compare(const comparable & d) const
        {
            const directory *dp = dynamic_cast<const directory *>(&d);
            if (dp)
            {
                return full_path.compare(dp->full_path);
            }
            else
            {
                throw internal_exception(__FILE__, __LINE__, L"Comparing different types.");
            }
        } // directory::compare()


        void directory::get_paths() 
        {
#ifdef WIN32
            data::smart_pointer<wchar_t, true> buf(new wchar_t[MAX_PATH_SIZE]);
            wchar_t *fname;
            GetFullPathName(name.w_string(), MAX_PATH_SIZE, buf, &fname);

            full_path = string(buf);

            string end = full_path.right_substring(1);

            if (!(end == L"\\" || end == L"/"))
            {
                full_path += SEPARATOR;
            }
#endif
        } // directory::get_paths()


        const string & directory::get_name() const
        {
            return name;
        } // directory::get_name()


        const string & directory::get_full_path() const
        {
            return full_path;
        } // directory::get_full_path()
        

        const data::list<file> & directory::get_files() const
        {
			files.clear();

#ifdef WIN32
            string wild = full_path + L"*";

            WIN32_FIND_DATA data;
            HANDLE fh = FindFirstFile(wild.w_string(), &data);
            bool finding = fh != INVALID_HANDLE_VALUE;
            while (finding)
            {
                if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0)
                    files.append(file(full_path + string(data.cFileName)));
                finding = FindNextFile(fh, &data) != 0;
            }
            if (fh != INVALID_HANDLE_VALUE)
                FindClose(fh);
#else
#error directory::get_files() not implemented
#endif

            return files;
        } // directory::get_files()


        const data::list<directory> & directory::get_dirs() const
        {
			dirs.clear();

#ifdef WIN32
            string wild = full_path + L"*";

            WIN32_FIND_DATA data;
            HANDLE fh = FindFirstFile(wild.w_string(), &data);
            bool finding = fh != INVALID_HANDLE_VALUE;
            while (finding)
            {
                string entry_name(data.cFileName);

                if ((data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0 && entry_name != string(L".") && entry_name != string(L".."))
                    dirs.append(directory(full_path + string(data.cFileName)));
                finding = FindNextFile(fh, &data) != 0;
            }
            if (fh != INVALID_HANDLE_VALUE)
                FindClose(fh);
#else
#error directory::get_dirs() not implemented
#endif

            return dirs;
        } // directory::get_dirs()


        bool directory::exists(const string & path)
        {
            return ::access(path.c_string(), 00) == 0;
        } // directory::exists()


        void directory::create(const string & dir)
        {
            if (::mkdir(dir.c_string()) == -1)
                throw io_exception(L"unable to create directory %ls: %hs", dir.w_string(), ::strerror(errno));
        } // directory::create()

    } // namespace io
    
} // namespace gsgl
