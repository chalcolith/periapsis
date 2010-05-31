#ifndef GSGL_DATA_DIR_H
#define GSGL_DATA_DIR_H

//
// $Id: directory.hpp 2 2008-03-01 20:58:50Z kulibali $
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
#include "data/list.hpp"
#include "data/string.hpp"

namespace gsgl
{

    namespace data
    {
        template <typename T> class list;
    }

    namespace io
    {

        class file;
    
        /// Represents a directory on disk.
        class BASE_API directory
            : public data_object, public data::comparable
        {
            gsgl::string name, full_path;

            mutable data::list<file> files;
            mutable data::list<directory> dirs;
        public:
            directory();
            directory(const string & name);
            ~directory();
            
            directory(const directory &);
            directory & operator= (const directory &);
            
            virtual int compare(const comparable &) const;

            /// \return The path that was given upon creation of the directory object.
            const gsgl::string & get_name() const;

            /// \return The full (absolute) path of the directory.  Guaranteed to end in a path separator.
            const gsgl::string & get_full_path() const;
            
            /// \return A list of the files in the directory.
            const data::list<file> & get_files() const;

            /// \return A list of subdirectories in the directory.
            const data::list<directory> & get_dirs() const;

            /// Returns true if the particular directory exists.
            static bool exists(const gsgl::string & pathname);

            static void create(const gsgl::string & pathname);

            /// The system-dependent directory separator.
            static const gsgl::string SEPARATOR;

        private:
            void get_paths();
        }; // class directory
        
    } // namespace io
    
} // namespace gsgl

#endif
