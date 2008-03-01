#ifndef GSGL_DATA_STREAM_H
#define GSGL_DATA_STREAM_H

//
// $Id: stream.hpp 314 2008-03-01 16:33:47Z Gordon $
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
#include "data/printable.hpp"
#include "data/serializable.hpp"

namespace gsgl
{

    namespace io
    {
    
        /// A text stream.
        class DATA_API text_stream
        {
        protected:
            text_stream();
        public:
            virtual ~text_stream();
            
            virtual wchar_t peek() = 0;
            virtual wchar_t get() = 0;
            virtual void unget(wchar_t) = 0;
            virtual bool at_end() const = 0;
            
            virtual gsgl::index_t read(wchar_t *, const gsgl::index_t num) = 0;
            virtual gsgl::index_t write(const wchar_t *, const gsgl::index_t num) = 0;
            
            text_stream & operator<< (const wchar_t *);
            text_stream & operator<< (const wchar_t &);
            text_stream & operator<< (const int &);

            text_stream & operator<< (const float &);
            text_stream & operator<< (const double &);
            text_stream & operator<< (const char *);
            
            text_stream & operator>> (wchar_t &);
            text_stream & operator>> (int &);

            text_stream & operator>> (float &);
            text_stream & operator>> (double &);

        }; // class stream
        

        DATA_API text_stream & operator<< (text_stream &, const printable &);
        DATA_API text_stream & operator>> (text_stream &, printable &);


        /// A data stream.  This is used for serializing data in binary format.
        /// \todo Implement endian check.
        class DATA_API data_stream
        {
        protected:
            data_stream();

        public:
            virtual ~data_stream();

            virtual bool at_end() const = 0;

            virtual gsgl::index_t read(unsigned char *, const gsgl::index_t num) = 0;
            virtual gsgl::index_t write(const unsigned char *, const gsgl::index_t num) = 0;

            data_stream & operator<< (const unsigned char &);
            data_stream & operator<< (const int &);
            data_stream & operator<< (const float &);
            data_stream & operator<< (const double &);
            
            data_stream & operator>> (unsigned char &);
            data_stream & operator>> (int &);
            data_stream & operator>> (float &);
            data_stream & operator>> (double &);
        }; // class data_stream


        DATA_API data_stream & operator<< (data_stream &, const serializable &);
        DATA_API data_stream & operator>> (data_stream &, serializable &);

    } // namespace io
    
} // namespace gsgl

#endif
