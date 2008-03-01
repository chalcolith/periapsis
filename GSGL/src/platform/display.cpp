//
// $Id: display.cpp 319 2008-03-01 20:40:39Z Gordon $
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

#include "platform/display.hpp"
#include "platform/lowlevel.hpp"
#include "platform/extensions.hpp"

#include <cmath>

namespace gsgl
{

    using namespace data;
    using namespace math;

    namespace platform
    {

        config_variable<int> display::DISPLAY_WIDTH(L"display/width", 1028);
        config_variable<int> display::DISPLAY_HEIGHT(L"display/height", 768);

    
        //
        display::display(const int & width, const int & height, bool is_console) 
            : surface(0), is_console(is_console)
        {
            unsigned int rmask, gmask, bmask, amask;
            
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
            rmask = 0xff000000;
            gmask = 0x00ff0000;
            bmask = 0x0000ff00;
            amask = 0x000000ff;
#else
            
            rmask = 0x000000ff;
            gmask = 0x0000ff00;
            bmask = 0x00ff0000;
            amask = 0xff000000;
#endif
            
            if (is_console)
            {
                // create the window
                SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
                SDL_GL_SetAttribute(SDL_GL_SWAP_CONTROL, 0);
                
                surface = SDL_SetVideoMode(width, height, 32, SDL_OPENGL);

                if (surface)
                    gsgl::platform::init_extensions();

                // center the window
                center_console_window(width, height);
            }
            else
            {
                surface = SDL_CreateRGBSurface(SDL_HWSURFACE | SDL_SRCALPHA, width, height, 32, rmask, gmask, bmask, amask);
            }
        } // display::display()
        
        display::~display()
        {
            if (!is_console)
                SDL_FreeSurface(surface);
        } // display::~display()
        
        int display::get_width() const
        {
            return surface->w;
        } // display::get_width()
        
        int display::get_height() const
        {
            return surface->h;
        } // display::get_height()


        gsgl::real_t display::get_aspect_ratio() const
        {
            return static_cast<gsgl::real_t>(surface->w) / static_cast<gsgl::real_t>(surface->h);
        } // display::get_aspect_ratio()
        

        void display::draw_rectangle(float x1, float y1, float x2, float y2,
                                     float s1, float t1, float s2, float t2)
        {
            glBegin(GL_TRIANGLE_STRIP);

            // upper left
            glNormal3f(0, 0, 1);
            glTexCoord2f(s1, t2);
            glVertex2f(x1, y2);

            // lower left
            glNormal3f(0, 0, 1);
            glTexCoord2f(s1, t1);
            glVertex2f(x1, y1);

            // upper right
            glNormal3f(0, 0, 1);
            glTexCoord2f(s2, t2);
            glVertex2f(x2, y2);

            // lower right
            glNormal3f(0, 0, 1);
            glTexCoord2f(s2, t1);
            glVertex2f(x2, y1);

            glEnd();                                                                                                CHECK_GL_ERRORS();
        } // display::draw_rectangle()

        
        void display::record_3d_text_info()
        {
            glGetIntegerv(GL_VIEWPORT, text_draw_viewport);                                                         CHECK_GL_ERRORS();
            glGetFloatv(GL_MODELVIEW_MATRIX, text_draw_modelview.ptr());                                            CHECK_GL_ERRORS();
            glGetFloatv(GL_PROJECTION_MATRIX, text_draw_projection.ptr());                                          CHECK_GL_ERRORS();
            text_draw_pm = text_draw_projection * text_draw_modelview;                                              CHECK_GL_ERRORS();
        } // display::record_3d_text_info()


        void display::draw_text_start()
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                       CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                          CHECK_GL_ERRORS();
            
            glMatrixMode(GL_PROJECTION);                                                                            CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                       CHECK_GL_ERRORS();
            glOrtho(0.0, surface->w, 0.0, surface->h, -1, 1);

            glMatrixMode(GL_MODELVIEW);                                                                             CHECK_GL_ERRORS();
            glPushMatrix();
            glLoadIdentity();                                                                                       CHECK_GL_ERRORS();
        } // display::draw_text_start()


        void display::draw_2d_text(const gsgl::real_t x, const gsgl::real_t y, font *f, const string & str)
        {
            glMatrixMode(GL_MODELVIEW);                                                                             CHECK_GL_ERRORS();
            glLoadIdentity();                                                                                       CHECK_GL_ERRORS();
            glTranslatef(x, y, 0);                                                                                  CHECK_GL_ERRORS();
            f->draw(str);
        } // display::draw_2d_text()


        void display::draw_3d_text(const vector & p, font *f, const string & str, const gsgl::real_t x_offset, const gsgl::real_t y_offset)
        {
            vector p_in_clip_space = text_draw_pm * p;
            
            if (p_in_clip_space.get_z() >= 0)
            {
                gsgl::real_t wx, wy;
                wx = text_draw_viewport[0] + text_draw_viewport[2]*(p_in_clip_space.get_x() + 1)/2;
                wy = text_draw_viewport[1] + text_draw_viewport[3]*(p_in_clip_space.get_y() + 1)/2;

                if (wx >= -100 && wx < surface->w && wy >= -10 && wy < surface->h+10)
                    draw_2d_text(wx + x_offset, wy + y_offset, f, str);
            }
        } // display::draw_3d_text()


        void display::draw_text_stop()
        {
            glMatrixMode(GL_MODELVIEW);
            glPopMatrix();

            glPopClientAttrib();                                                                                    CHECK_GL_ERRORS();
            glPopAttrib();                                                                                          CHECK_GL_ERRORS();
        } // display::draw_text_stop()


#ifdef WIN32

        void display::center_console_window(const int & width, const int & height)
        {
            SDL_SysWMinfo info;
            SDL_VERSION(&info.version);

            if (SDL_GetWMInfo(&info))
            {
                HWND desktop = GetDesktopWindow();
                RECT rect;
                if (desktop && GetWindowRect(desktop, &rect))
                {
                    int left = ((rect.right - rect.left) < 2*(rect.bottom - rect.top)) ? (rect.left + rect.right - width)/2 : (3*rect.left + rect.right - 2*width)/4;
                    int top = (rect.top + rect.bottom - height)/2;

                    SetWindowPos(info.window, HWND_TOP, left, top, width, height, SWP_SHOWWINDOW);
                }
            }
        } // display::center_console_window()

#else
#error You must define display::center_console_window() for your platform!
#endif

    } // namespace platform
    
} // namespace gsgl
