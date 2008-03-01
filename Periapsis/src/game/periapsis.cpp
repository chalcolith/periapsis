// 
// $Id: $
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

#include "periapsis_app.hpp"

#include "data/exception.hpp"
#include "data/log.hpp"
#include "data/broker.hpp"
#include "data/cache.hpp"

//

#ifdef WIN32
#pragma warning (disable : 4996)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

//

extern "C" {

    int SDL_main(int argc, char *argv[])
    {
        int res = 0;

        try
        {
#ifdef DEBUG
            // print debug output by default
            gsgl::data::debug_log_target debug_log;
#endif

            // create the application
            periapsis::periapsis_app app(L"Periapsis", argc, const_cast<const char **>(argv));

            // run the application
            app.run();
        }
        catch (gsgl::exception & e)
        {
#if defined(__APPLE__)
            short code;
            StandardAlert(kAlertStopAlert, "\pError", msg.p_string(), 0, &code);
#elif defined(WIN32)
            MessageBox(0, e.get_message(), L"Error", MB_OK | MB_ICONERROR);
#else
#error Alert message box unimplemented!
#endif

            res = 1;
        }
        catch (...)
        {
            res = 1;
        }

        return res;
    } // SDL_main()   

} // extern "C"
