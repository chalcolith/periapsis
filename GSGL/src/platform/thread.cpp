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

#include "platform/thread.hpp"
#include "data/exception.hpp"
#include "platform/lowlevel.hpp"

namespace gsgl
{

    namespace platform
    {

        mutex::mutex()
            : m(0)
        {
            m = SDL_CreateMutex();
        } // mutex::mutex()


        mutex::~mutex()
        {
            if (m)
                SDL_DestroyMutex(m);
        } // mutex::~mutex()


        void mutex::lock()
        {
            if (m)
                SDL_LockMutex(m);
        } // mutex::lock()


        void mutex::unlock()
        {
            if (m)
                SDL_UnlockMutex(m);
        } // mutex::unlock()


        //////////////////////////////////////////////////////////////

        static int run_thread(void *data)
        {
            thread *th = (thread *) data;

            if (th)
                return th->run();
            else
                return -1;
        } // run_thread()


        thread::thread()
            : t(0)
        {
        } // thread::thread()


        thread::~thread()
        {
            if (t)
                SDL_WaitThread(t, 0);
        } // thread::~thread()


        bool thread::is_running()
        {
            return t != 0;
        } // thread::is_running()


        void thread::start()
        {
            if (!t)
                t = SDL_CreateThread(run_thread, this);
            else
                throw runtime_exception(L"You cannot run a thread that is already running!");
        } // thread::start()


        void thread::kill()
        {
            if (t)
                SDL_KillThread(t);
        } // thread:kill()


        void thread::wait()
        {
            if (t)
            {
                SDL_WaitThread(t, 0);
                t = 0;
            }
        } // thread::wait()

    } // namespace platform

} // namespace gsgl
