#ifndef GSGL_PLATFORM_COLOR_H
#define GSGL_PLATFORM_COLOR_H

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
#include "data/config.hpp"

namespace gsgl
{

    namespace platform
    {


        class PLATFORM_API color
            : public platform_object
        {
            float val[4];

        public:
            enum component { COMPONENT_RED = 0, COMPONENT_GREEN = 1, COMPONENT_BLUE = 2, COMPONENT_ALPHA = 3, NUM_COMPONENTS = 4 };
            
            color()
            {
                unsigned int hex_val = 0xa020f0ff;

                val[COMPONENT_RED]   = static_cast<float>((hex_val & 0xff000000) >> 24) / 255.0f;
                val[COMPONENT_GREEN] = static_cast<float>((hex_val & 0x00ff0000) >> 16) / 255.0f;
                val[COMPONENT_BLUE]  = static_cast<float>((hex_val & 0x0000ff00) >> 8)  / 255.0f;
                val[COMPONENT_ALPHA] = static_cast<float>((hex_val & 0x000000ff) >> 0)  / 255.0f;
            } // color()


            color(float red, float green, float blue, float alpha = 1.0f)
            {
                val[COMPONENT_RED]   = red;
                val[COMPONENT_GREEN] = green;
                val[COMPONENT_BLUE]  = blue;
                val[COMPONENT_ALPHA] = alpha;
            } // color()

            explicit color(const unsigned int & hex_val)
            {
                val[COMPONENT_RED]   = static_cast<float>((hex_val & 0xff000000) >> 24) / 255.0f;
                val[COMPONENT_GREEN] = static_cast<float>((hex_val & 0x00ff0000) >> 16) / 255.0f;
                val[COMPONENT_BLUE]  = static_cast<float>((hex_val & 0x0000ff00) >> 8)  / 255.0f;
                val[COMPONENT_ALPHA] = static_cast<float>((hex_val & 0x000000ff) >> 0)  / 255.0f;
            } // color()

            color(const color & c)
            {
                for (int i = 0; i < NUM_COMPONENTS; ++i)
                    val[i] = c.val[i];
            } // color()

            color & operator= (const color & c)
            {
                for (int i = 0; i < NUM_COMPONENTS; ++i)
                    val[i] = c.val[i];
                return *this;
            } // operator= ()

            //
            inline bool operator== (const color & c) const { return val[0] == c.val[0] && val[1] == c.val[1] && val[2] == c.val[2] && val[3] == c.val[3]; }
            inline bool operator!= (const color & c) const { return val[0] != c.val[0] || val[1] != c.val[1] || val[2] != c.val[2] || val[3] != c.val[3]; }

            //
            inline const float & get_val(const component c) const { return val[c]; }
            inline float & get_val(const component c) { return val[c]; }

            inline const float & operator[] (const component c) const { return get_val(c); }
            inline float & operator[] (const component c) { return get_val(c); }

            /// Sets the current OpenGL color to the value of this color.
            void bind() const;

            const float *get_val() const { return val; }

            // 
            static const color BLACK;
            static const color WHITE;
        }; // class color


    } // namespace platform


    namespace data
    {

        template <>
        inline gsgl::string gsgl::data::config_variable<gsgl::platform::color>::get_string() const 
        {
            return gsgl::string::format(L"%f %f %f %f", value.get_val()[0], value.get_val()[1], value.get_val()[2], value.get_val()[3]);
        } // config_variable<gsgl::platform::color>::get_string()


        template <>
        inline void gsgl::data::config_variable<gsgl::platform::color>::assign_from_string(const gsgl::string & s)
        {
            gsgl::data::list<gsgl::string> numbers = s.split(L" ");
            int pos = 0;
            for (gsgl::data::list<gsgl::string>::iterator i = numbers.iter(); pos < 4 && i.is_valid(); ++i)
            {
                if (!i->is_empty())
                    const_cast<float *>(value.get_val())[pos++] = static_cast<float>(i->to_double());
            }
        } // config_variable<gsgl::platform::color>::assign_from_string()

    }

} // namespace gsgl

#endif
