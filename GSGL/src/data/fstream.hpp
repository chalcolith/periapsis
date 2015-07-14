#ifndef GSGL_DATA_FSTREAM_H
#define GSGL_DATA_FSTREAM_H

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
#include "data/string.hpp"
#include "data/stream.hpp"

namespace gsgl
{

    class string;
    
    namespace io
    {
    
        /// Modes for opening files.
        enum open_mode 
        { 
            FILE_OPEN_READ   = 1 << 0, 
            FILE_OPEN_WRITE  = 1 << 1, 
            FILE_OPEN_APPEND = 1 << 2,
            FILE_OPEN_TEXT   = 1 << 3,
            FILE_OPEN_BINARY = 1 << 4  // bloody windows
        };


        /// Base class for streams on a file.
        class DATA_API file_stream
            : public data_object
        {
        protected:
            gsgl::string fname;

            void *fp;
            int mode;

            file_stream(void *fp, gsgl::flags_t mode);

            bool at_end() const;

        public:
            file_stream(const gsgl::string & fname, gsgl::flags_t mode = FILE_OPEN_READ);
            virtual ~file_stream();

			const gsgl::string & get_fname() const { return fname; }
        }; // class file_stream

            
        /// A text stream on a file.
        class DATA_API ft_stream
            : public file_stream, public text_stream
        {
            ft_stream(void *fp, gsgl::flags_t mode);
        public:
            ft_stream(const gsgl::string & fname, gsgl::flags_t mode = FILE_OPEN_READ);
            virtual ~ft_stream();
            
            wchar_t peek();
            wchar_t get();
            void unget(wchar_t);
            bool at_end() const;

            gsgl::index_t read(wchar_t *, const gsgl::index_t);
            gsgl::index_t write(const wchar_t *, const gsgl::index_t);
            
            static ft_stream out;
            static ft_stream err;
            static ft_stream in;
        }; // class fstream
        

        /// A data stream on a file.
        class DATA_API fd_stream
            : public file_stream, public data_stream
        {
        public:
            fd_stream(const gsgl::string & fname, gsgl::flags_t mode = FILE_OPEN_READ);
            virtual ~fd_stream();

            bool at_end() const;

            gsgl::index_t read(unsigned char *, const gsgl::index_t);
            gsgl::index_t write(const unsigned char *, const gsgl::index_t);
        }; // class data_stream

    } // namespace io
    
} // namespace gsgl

#endif
