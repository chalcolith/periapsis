#ifndef GSGL_DATA_LOG_H
#define GSGL_DATA_LOG_H

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
#include "data/singleton.hpp"
#include "data/string.hpp"
#include "data/list.hpp"

namespace gsgl
{

    namespace io
    {
        class ft_stream;
    }


    namespace data
    {

        class log_target;


        /// Global logger that sends log lines to the various log targets.
        /// The global logger object will be created on the first call to \c global_logger(), and will destroy itself when the last log target goes out of scope.
        class DATA_API logger
            : public data::singleton<logger>
        {
            static int global_log_level;

            data::list<log_target *> log_targets;

			logger();

        public:
			virtual ~logger();


            enum
            {
                LOG_LEVEL_NONE   = 0,
                LOG_LEVEL_BASIC  = 1,
                LOG_LEVEL_MEDIUM = 5,
                LOG_LEVEL_HIGH   = 10,
                LOG_LEVEL_ULTRA  = 100
            };

            static void print_line(int log_level, const gsgl::string &);
            static void set_global_log_level(int new_log_level);

            friend class log_target;
        }; // class logger


        /// Base class for log targets.
        class DATA_API log_target
            : public data_object
        {
        public:
            log_target();
            virtual ~log_target();

            virtual void print_line(const gsgl::string &) = 0;
        }; // class log_target


        /// A log target that prints to a file.
        class DATA_API file_log_target
            : public log_target
        {
            io::ft_stream *f;

        public:
            file_log_target(const gsgl::string & fname);
            virtual ~file_log_target();

            void print_line(const gsgl::string &);
        }; // class file_log_target


        /// A log target that prints to a console (or debug output).
        class DATA_API debug_log_target
            : public data::singleton<debug_log_target>, public log_target
        {
        public:
            debug_log_target();
            virtual ~debug_log_target();

            void print_line(const gsgl::string &);
        }; // class debug_log_target


    } // namespace data


    DATA_API inline void log(int log_level, const gsgl::string & s)
    {
        gsgl::data::logger::print_line(log_level, s);
    } // log()


    DATA_API inline void log(const gsgl::string & s)
    {
        gsgl::data::logger::print_line(gsgl::data::logger::LOG_LEVEL_BASIC, s);
    } // log()


} // namespage gsgl

#endif
