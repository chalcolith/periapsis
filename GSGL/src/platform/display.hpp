#ifndef GSGL_FRAMEWORK_SCREEN_H
#define GSGL_FRAMEWORK_SCREEN_H

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

#include "platform/platform.hpp"
#include "platform/font.hpp"
#include "data/config.hpp"
#include "math/math.hpp"
#include "math/transform.hpp"

// forward declarations
struct SDL_Surface;

namespace gsgl
{

    namespace platform
    {

        //
        class texture;

    
        /// This class represents a screen on which to draw.
        class PLATFORM_API display
        {
            struct SDL_Surface *surface;
            bool is_console;

            math::transform text_draw_modelview;
            math::transform text_draw_projection;
            math::transform text_draw_pm;
            int text_draw_viewport[4];

        public:
            display(const int & width, const int & height, bool is_console = false);
            virtual ~display();
            
            static data::config_variable<int> DISPLAY_WIDTH;
            static data::config_variable<int> DISPLAY_HEIGHT;

            int get_width() const;
            int get_height() const;
            gsgl::real_t get_aspect_ratio() const;

            /// Draws a possibly textured rectangle (a suitable projection must already be set up).
            /// Operates in the current OpenGL context, not in any particular display.
            static void draw_rectangle(float x1, float y1, float x2, float y2,
                                       float s1 = 0, float t1 = 0, float s2 = 1, float t2 = 1);

            /// Records the 3D modelview, projection and viewport information for subsequent calls to draw_3d_text().
            void record_3d_text_info();

            /// Sets up an orthogonal view frustum for drawing text.
            void draw_text_start();

            /// Draws text at the given screen coordinates.
            void draw_2d_text(const gsgl::real_t x, const gsgl::real_t y, const font *f, const gsgl::string & str);

            /// Draws text at the given world coordinates.
            void draw_3d_text(const math::vector & p, const font *f, const gsgl::string & str, const gsgl::real_t x_offset = 0, const gsgl::real_t y_offset = 0);

            /// Tears down the orthogonal view frustum for drawing text.
            void draw_text_stop();

        private:
            void center_console_window(const int & width, const int & height);
        }; // class display
        
    } // namespace platform
    
} // namespace gsgl

#endif
