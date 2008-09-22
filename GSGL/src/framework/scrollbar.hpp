#ifndef GSGL_FRAMEWORK_SCROLLBAR_H
#define GSGL_FRAMEWORK_SCROLLBAR_H

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

#include "framework/framework.hpp"
#include "framework/widget.hpp"
#include "data/config.hpp"

namespace gsgl
{

    namespace framework
    {

        class FRAMEWORK_API scrollbar
            : public widget
        {
            static gsgl::data::config_variable<int> ARROW_SIZE;

            int min_val, max_val;
            int pos, extent;

        public:
            enum bar_mode
            { 
                VERTICAL,
                HORIZONTAL
            };

            scrollbar(platform::display & screen, widget *parent, 
                      const int x, const int y, const int w, const int h,
                      const platform::color & foreground, 
                      const platform::color & background,
                      bar_mode mode = VERTICAL);
            virtual ~scrollbar();


            bar_mode & get_mode() { return mode; }

            /// Get or set the minimum value of the scrollbar.
            int & get_min() { return min_val; }

            /// Get or set the maximum value of the scrollbar.
            int & get_max() { return max_val; }

            /// Get or set the current beginning position of the scrollbar.
            int & get_pos() { return pos; }

            /// Get or set the current extent of the scrollbar.
            int & get_extent() { return extent; }


            virtual void draw();

        private:
            bar_mode mode;
        }; // class scrollbar

    } // namespace framework

} // namespace gsgl

#endif