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

#include "data/log.hpp"
#include "data/fstream.hpp"


#ifdef WIN32
// forward declaration of utility function to print to windows debug output
static void win32_debug_print(const wchar_t *str);
#endif



namespace gsgl
{

    using namespace io;

    namespace data
    {

        logger *logger::instance = 0;
        int logger::global_log_level = logger::LOG_LEVEL_BASIC;

        static logger global_logger;

		//

		logger::logger()
		{
#ifdef DEBUG
#ifdef WIN32
				win32_debug_print(L"logger: creating global logger\n");
#else
				io::ft_stream::out << L"logger: creating global logger\n";
#endif
#endif
		} // logger::logger()


		logger::~logger()
		{
#ifdef DEBUG
#ifdef WIN32
				win32_debug_print(L"logger: destroying global logger\n");
#else
				io::ft_stream::out << L"logger: destroying global logger\n";
#endif
#endif
		} // logger::~logger()



        void logger::print_line(int log_level, const gsgl::string & msg)
        {
            if (instance && log_level <= global_log_level)
            {
                for (list<log_target *>::iterator i = instance->log_targets.iter(); i.is_valid(); ++i)
                    (*i)->print_line(msg);
            }
        } // logger::print_line()


        void logger::set_global_log_level(int new_log_level)
        {
            global_log_level = new_log_level;
        } // logger::set_global_log_level()


        //////////////////////////////////////////

        log_target::log_target()
            : data_object()
        {
            if (logger::instance)
                logger::instance->log_targets.append(this);
            else
                throw internal_exception(__FILE__, __LINE__, L"You must define a global logger object.");
        } // log_target::log_target()


        log_target::~log_target()
        {
            if (logger::instance)
            {
                list<log_target *>::iterator i = logger::instance->log_targets.find_value(this);
                if (i.is_valid())
                    logger::instance->log_targets.remove(i);
                else
                    throw internal_exception(__FILE__, __LINE__, L"Trying to remove a log target that is not registered!");
            }
        } // log_target::~log_target()


        //////////////////////////////////////////

        file_log_target::file_log_target(const gsgl::string & fname)
            : log_target(), f(new ft_stream(fname, io::FILE_OPEN_WRITE))
        {
#ifdef DEBUG
			string msg = string::format(L"file_log_target: creating new log target on %ls\n", fname.w_string());

#ifdef WIN32
			win32_debug_print(msg.w_string());
#else
			io::ft_stream::out << msg;
#endif
#endif
        } // file_log_target::file_log_target()


        file_log_target::~file_log_target()
        {
			string msg = string::format(L"file_log_target: destroying log target on %ls\n", f->get_fname().w_string());
#ifdef DEBUG
#ifdef WIN32
			win32_debug_print(msg.w_string());
#else
			io::ft_stream::out << msg;
#endif
#endif

            delete f;
        } // file_log_target::~file_log_target()


        void file_log_target::print_line(const string & msg)
        {
            *f << msg << L"\n";
        } // file_log_target::print_line()


        //////////////////////////////////////////

        debug_log_target *debug_log_target::instance;


        debug_log_target::debug_log_target()
            : singleton<debug_log_target>(), log_target()
        {
#ifdef DEBUG
#ifdef WIN32
			win32_debug_print(L"debug_log_target: creating debug log target\n");
#else
			io::ft_stream::out << L"debug_log_target: creating debug log target");
#endif
#endif
        } // debug_log_target::debug_log_target()


        debug_log_target::~debug_log_target()
        {
#ifdef DEBUG
#ifdef WIN32
			win32_debug_print(L"debug_log_target: destroying debug log target\n");
#else
			io::ft_stream::out << L"debug_log_target: destroying debug log target");
#endif
#endif
        } // debug_log_target::~debug_log_target()


        void debug_log_target::print_line(const string & msg)
        {
#ifdef WIN32
            string msg_ln(msg);
            msg_ln += L"\n";

            win32_debug_print(msg_ln.w_string());
#else
#error Debug log target is not implemented on this platform!
#endif
        } // debug_log_target::print_line()


    } // namespace data


} // namespace gsgl


#ifdef WIN32
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>

static void win32_debug_print(const wchar_t *str)
{
    OutputDebugString(str);
} // win32_debug_print()

#endif
