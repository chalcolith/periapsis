#ifndef GSGL_PLATFORM_HARDWARE_H
#define GSGL_PLATFORM_HARDWARE_H

//
// $Id: lowlevel.hpp 319 2008-03-01 20:40:39Z Gordon $
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

#include "platform/platform.hpp"
#include "data/string.hpp"
#include "data/log.hpp"

#if defined(__APPLE__)

#include <OpenGL/GL.h>
#include <OpenGL/GLU.h>
#include <OpenGL/glext.h>

#include <AGL/agl.h>
#include <Carbon/Carbon.h>

#include <SDL/SDL.h>
#include <SDL/SDL_syswm.h>
#include <SDL/SDL_thread.h>
#include <SDL_ttf/SDL_ttf.h>
#include <SDL_image/SDL_image.h>

#elif defined(WIN32)

#ifndef WINVER				// Allow use of features specific to Windows XP or later.
#define WINVER 0x0501		// Change this to the appropriate value to target other versions of Windows.
#endif

#ifndef _WIN32_WINNT		// Allow use of features specific to Windows XP or later.                   
#define _WIN32_WINNT 0x0501	// Change this to the appropriate value to target other versions of Windows.
#endif						

#ifndef _WIN32_WINDOWS		// Allow use of features specific to Windows 98 or later.
#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
#endif

#ifndef _WIN32_IE			// Allow use of features specific to IE 6.0 or later.
#define _WIN32_IE 0x0600	// Change this to the appropriate value to target other versions of IE.
#endif

#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <shlobj.h>

#include <GL/glew.h>
#include <GL/GL.h>
#include <GL/GLU.h>

#include <SDL.h>
#include <SDL_syswm.h>
#include <SDL_thread.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#endif


#ifdef DEBUG
#define CHECK_GL_ERRORS() \
{ \
    gsgl::string _gl_err_msg_; \
	char *_sdl_err_; \
    int _gl_err_code_; \
    glFlush(); \
    while ((_gl_err_code_ = glGetError()) != GL_NO_ERROR) \
    { \
        _gl_err_msg_ += gsgl::string::format(L"%d: %ls.\n", _gl_err_code_, gsgl::string((const char *)gluErrorString(_gl_err_code_)).w_string()); \
    } \
	_sdl_err_ = SDL_GetError(); \
	if (_sdl_err_ && *_sdl_err_) \
		_gl_err_msg_ += gsgl::string::format(L"%hs.\n", gsgl::string(_sdl_err_)); \
    if (_gl_err_msg_.size()) \
    { \
        dynamic_cast<gsgl::data::logger *>(gsgl::data::logger::global_instance())->print_line(gsgl::data::logger::LOG_LEVEL_BASIC, _gl_err_msg_.w_string()); \
		throw gsgl::opengl_exception(__FILE__, __LINE__, L"\n\nOpenGL errors: %ls", _gl_err_msg_.w_string()); \
    } \
}
#else
#define CHECK_GL_ERRORS()
#endif

#endif
