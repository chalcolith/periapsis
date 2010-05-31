#ifndef GSGL_DATA_FILE_H
#define GSGL_DATA_FILE_H

//
// $Id: file.hpp 2 2008-03-01 20:58:50Z kulibali $
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

#include "data/data.hpp"
#include "data/string.hpp"
#include "data/fstream.hpp"
#include "data/directory.hpp"

namespace gsgl
{

    namespace io
    {
    
        extern BASE_API const gsgl::index_t MAX_PATH_SIZE;
        
        /// Represents a file on disk.
        /// \todo Implement file information.
        class BASE_API file
            : public data_object
        {
            string name;
            string base_name;
            string dir_name;
            string full_path;

            directory dir;
        public:
            file();
            file(const gsgl::string & fname);
            ~file();
            
            file(const file & f);
            file & operator= (const file & f);
            
            bool operator== (const file & f) const;
            bool operator!= (const file & f) const;

            /// Returns a pointer to a stream opened on the file.  Calling code must delete the stream.
            gsgl::io::ft_stream *open_text(gsgl::flags_t mode = FILE_OPEN_READ);

            /// Returns a pointer to a stream opened on a file.  Calling code must delete the stream.
            //static gsgl::io::ft_stream *open_text(const gsgl::string & fname, unsigned int mode = FILE_OPEN_READ);
            
            /// Returns the name that the file was initialized with.
            const string & get_name() const;

            /// Returns the name without any path information.
            const string & get_base_name() const;
            
            /// Returns the name of the file's parent directory.  Guaranteed to end in a path separator.
            const string & get_dir_name() const;
                        
            /// Returns the full path name of the file.
            const string & get_full_path() const;
            
            /// Returns the directory in which the file resides.
            const directory & get_directory() const;
            
            /// Returns the full path name of the file.
            static string get_full_path(const gsgl::string & fname);

            /// Returns true if the file exists.
            static bool exists(const gsgl::string & fname);

            /// Delete a file on disk.
            static void remove(const gsgl::string & fname);

            /// Copy a file.
            static void copy(const gsgl::string & src_path, const gsgl::string & dest_path);
        }; // class file
        
    } // namespace io
    
} // namespace gsgl

#endif
