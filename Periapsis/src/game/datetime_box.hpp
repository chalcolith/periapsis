#ifndef PERIAPSIS_DATETIME_BOX_H
#define PERIAPSIS_DATETIME_BOX_H

//
// $Id: datetime_box.hpp 315 2008-03-01 16:33:59Z Gordon $
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

#include "framework/widget.hpp"
#include "framework/textbox.hpp"

namespace periapsis
{

    class datetime_box
        : public gsgl::framework::widget
    {
        gsgl::framework::textbox *label;
        gsgl::framework::textbox *julian_input;
        gsgl::framework::textbox *text_input;

        double jdn;

        enum recalc_mode
        {
            RECALC_NONE = 0,
            RECALC_TEXT = 1,
            RECALC_JDN  = 2,
            RECALC_BOTH = 3
        };

        recalc_mode mode;

    public:
        datetime_box(widget *parent, int x, int y, int w, int h, const gsgl::platform::color & fg, const gsgl::platform::color & bg);
        virtual ~datetime_box();

        const double & get_jdn() const;
        void set_jdn(const double &);

        const gsgl::string & get_text() const;
        void set_text(const gsgl::string &);

        virtual void draw();

    private:
        void recalc_text();
        void recalc_jdn();
    }; // class datetime_box

} // namespace periapsis

#endif
