//
// $Id: stream.cpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "data/stream.hpp"
#include "data/exception.hpp"
#include "data/string.hpp"

#include <cstdio>
#include <cwchar>
#include <cctype>

#ifdef WIN32
#pragma warning (disable : 4996)
#define snprintf _snprintf
#define snscanf _snscanf
#endif

namespace gsgl
{

    namespace io
    {
    
        static const int BUF_LEN = 64;
        
        text_stream::text_stream()
        {}
        
        text_stream::~text_stream()
        {}
        
        text_stream & text_stream::operator<< (const wchar_t * str)
        {
            this->write(str, (gsgl::index_t) ::wcslen(str));
            return *this;
        }
        
        text_stream & text_stream::operator<< (const wchar_t & ch)
        {
            /// \todo Implement UTF coding.
            this->write(&ch, 1);
            return *this;
        } // text_stream::operator<< ()
        
        text_stream & text_stream::operator<< (const int & n)
        {
            wchar_t buf[BUF_LEN];
            buf[BUF_LEN-1] = 0;
            swprintf(buf, BUF_LEN-1, L"%d", n);
            this->write(buf, (gsgl::index_t) ::wcslen(buf));
            return *this;
        } // text_stream::operator<< ()
        
        text_stream & text_stream::operator<< (const double & n)
        {
            wchar_t buf[BUF_LEN];
            buf[BUF_LEN-1] = 0;
            swprintf(buf, BUF_LEN-1, L"%f", n);
            this->write(buf, (gsgl::index_t) ::wcslen(buf));
            return *this;
        } // text_stream::operator<< ()

        text_stream & text_stream::operator<< (const float & n)
        {
            wchar_t buf[BUF_LEN];
            buf[BUF_LEN-1] = 0;
            swprintf(buf, BUF_LEN-1, L"%f", n);
            this->write(buf, (gsgl::index_t) ::wcslen(buf));
            return *this;
        } // text_stream::operator<< ()        

        text_stream & text_stream::operator<< (const char *str)
        {
            return *this << gsgl::string(str);
        } // text_stream::operator<< ()


        text_stream & operator<< (text_stream & s, const printable & p)
        {
            p.to_stream(s);
            return s;
        } // text_stream::operator<< ()


        //

        text_stream & text_stream::operator>> (wchar_t & ch)
        {
            /// \todo Implement UTF coding.
            this->read(&ch, 1);
            return *this;
        } // text_stream::operator>> ()
        
        text_stream & text_stream::operator>> (int & n)
        {
            wchar_t buf[BUF_LEN];
            int pos = 0;
            buf[pos] = 0;
            
            do
            {
                this->read(&buf[pos++], 1);
            }
            while (pos < BUF_LEN && !this->at_end() && isdigit(buf[pos-1]));
            buf[pos] = 0;
            
            swscanf(buf, L"%d", &n);
            return *this;
        } // text_stream::operator>> ()
        
        text_stream & text_stream::operator>> (double & n)
        {
            wchar_t buf[BUF_LEN];
            int pos = 0;
            buf[pos] = 0;
            
            do
            {
                this->read(&buf[pos++], 1);
            }
            while (pos < BUF_LEN && !this->at_end() && !isspace(buf[pos-1]));
            buf[pos] = 0;
            
            swscanf(buf, L"%f", &n);
            return *this;
        } // text_stream::operator>> ()

        text_stream & text_stream::operator>> (float & n)
        {
            double dd;
            *this >> dd;
            n = (float) dd;
            return *this;
        } // text_stream::oeprator>> ()
        

        text_stream & operator>> (text_stream & s, printable & p)
        {
            p.from_stream(s);
            return s;
        } // text_stream::operator>> ()


        //////////////////////////////////////////////////////////////

        data_stream::data_stream()
        {
        } // data_stream::data_stream()


        data_stream::~data_stream()
        {
        } // data_stream::~data_stream()


        data_stream & data_stream::operator<< (const unsigned char & c)
        {
            const size_t bytes_written = write(&c, sizeof(unsigned char));

            if (bytes_written != sizeof(unsigned char))
                throw io_exception(L"Error writing char.");
            return *this;
        } // data_stream::operator<< ()


        data_stream & data_stream::operator<< (const int & n)
        {
            const size_t bytes_written = write(reinterpret_cast<const unsigned char *>(&n), sizeof(int));

            if (bytes_written != sizeof(int))
                throw io_exception(L"Error writing int.");

            return *this;
        } // data_stream::operator<< ()


        data_stream & data_stream::operator<< (const double & n)
        {
            const size_t bytes_written = write(reinterpret_cast<const unsigned char *>(&n), sizeof(double));

            if (bytes_written != sizeof(double))
                throw io_exception(L"Error writing double.");
            return *this;
        } // data_stream::operator<< ()


        data_stream & data_stream::operator<< (const float & n)
        {
            const size_t bytes_written = write(reinterpret_cast<const unsigned char *>(&n), sizeof(float));

            if (bytes_written != sizeof(float))
                throw io_exception(L"Error writing float.");
            return *this;
        } // data_stream::operator<< ()


        data_stream & operator<< (data_stream & st, const serializable & ss)
        {
            ss.to_stream(st);
            return st;
        } // data_stream::operator<< ()


        //

        data_stream & data_stream::operator>> (unsigned char & ch)
        {
            const size_t bytes_read = read(&ch, sizeof(unsigned char));

            if (bytes_read != sizeof(unsigned char))
                throw io_exception(L"Error reading char.");

            return *this;
        } // data_stream::operator>> ()


        data_stream & data_stream::operator>> (int & n)
        {
            const size_t bytes_read = read(reinterpret_cast<unsigned char *>(&n), sizeof(int));

            if (bytes_read != sizeof(int))
                throw io_exception(L"Error reading int.");
            return *this;
        } // data_stream::operator>> ()


        data_stream & data_stream::operator>> (double & n)
        {
            const size_t bytes_read = read(reinterpret_cast<unsigned char *>(&n), sizeof(double));

            if (bytes_read != sizeof(double))
                throw io_exception(L"Error reading double.");
            return *this;
        } // data_stream::operator>> ()


        data_stream & data_stream::operator>> (float & n)
        {
            const size_t bytes_read = read(reinterpret_cast<unsigned char *>(&n), sizeof(float));

            if (bytes_read != sizeof(float))
                throw io_exception(L"Error reading float.");

            return *this;
        } // data_stream::operator>> ()


        data_stream & operator>> (data_stream & st, serializable & ss)
        {
            ss.from_stream(st);
            return st;
        } // data_stream::operator>> ()


    } // namespace io
    
} // namespace gsgl
