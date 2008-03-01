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

#include "platform/mapped_file.hpp"
#include "data/exception.hpp"
#include "data/string.hpp"
#include "data/file.hpp"

#include "platform/lowlevel.hpp"


namespace gsgl
{

    using namespace io;

    namespace platform
    {

#ifdef WIN32
        static string get_windows_error()
        {
            void *buf;
            FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, 0, GetLastError(), 0, (LPWSTR) &buf, 512, 0);
            string res(static_cast<wchar_t *>(buf));
            LocalFree(buf);

            return res;
        }
#endif



        static int NUM_MAPPED_FILES = 0;


        mapped_file::mapped_file(const string & fname, unsigned int io_open_mode, unsigned int create_size)
            : file_handle(0), map_handle(0), map_pointer(0)
        {
#ifdef WIN32
            string full_path = io::file::get_full_path(fname);

            // open file handle
            unsigned int win_open_mode = GENERIC_READ;

            if (io_open_mode & io::FILE_OPEN_WRITE)
                win_open_mode |= GENERIC_WRITE;

            if (file::exists(full_path))
            {
                file_handle = CreateFileW(full_path.w_string(), win_open_mode, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

                if (file_handle != INVALID_HANDLE_VALUE)
                    create_size = GetFileSize(file_handle, 0);
            }
            else
            {
                if (create_size == 0)
                    throw internal_exception(__FILE__, __LINE__, L"Must specify size for creating memory-mapped files.");

                file_handle = CreateFileW(full_path.w_string(), win_open_mode, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
                
                if (file_handle != INVALID_HANDLE_VALUE)
                {
                    unsigned char buf = 0;
                    unsigned int written;
                    SetFilePointer(file_handle, create_size-1, 0, FILE_BEGIN);                    
                    WriteFile(file_handle, &buf, 1, (LPDWORD) &written, 0);
                    SetFilePointer(file_handle, 0, 0, FILE_BEGIN);
                }
            }
            
            if (file_handle == INVALID_HANDLE_VALUE)
                throw io_exception(L"Unable to open %ls: %ls", full_path.w_string(), get_windows_error().w_string());

            // create mapping
            string map_name = string::format(L"periapsis_mapped_file_%d", NUM_MAPPED_FILES++);

            if (io_open_mode & io::FILE_OPEN_WRITE)
                win_open_mode = PAGE_READWRITE;
            else
                win_open_mode = PAGE_READONLY;

            map_handle = CreateFileMappingW(file_handle, 0, win_open_mode, 0, create_size, map_name.w_string());

            if (!map_handle)
                throw io_exception(L"Unable to create memory map for %ls: %ls", full_path.w_string(), get_windows_error().w_string());

            // map file
            if (io_open_mode & io::FILE_OPEN_WRITE)
                win_open_mode = FILE_MAP_ALL_ACCESS;
            else
                win_open_mode = FILE_MAP_READ;

            map_pointer = MapViewOfFile(map_handle, win_open_mode, 0, 0, 0);

            if (!map_pointer)
                throw io_exception(L"Unable to create memory map for %ls: %ls", full_path.w_string(), get_windows_error().w_string());

            map_size = create_size;
#else
#error Implement memory-mapped files!
#endif
        } // mapped_file::mapped_file()


        mapped_file::~mapped_file()
        {
#ifdef WIN32
            if (map_pointer)
            {
                UnmapViewOfFile(map_pointer);
                map_pointer = 0;
            }

            if (map_handle)
            {
                CloseHandle(map_handle);
                map_handle = 0;
            }

            if (file_handle)
            {
                CloseHandle(file_handle);
                file_handle = 0;
            }
#else
#error Implement memory-mapped files!
#endif
        } // mapped_file::~mapped_file()


        unsigned int mapped_file::get_size()
        {
            return map_size;
        } // mapped_file::get_size()


        void *mapped_file::get_pointer()
        {
            return map_pointer;
        } // mapped_file::get_pointer()


    } // namespace platform

} // namespace gsgl
