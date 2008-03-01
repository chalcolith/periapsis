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

#include "datetime_box.hpp"
#include "main_window.hpp"
#include "data/config.hpp"
#include "math/math.hpp"
#include "math/time.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::platform;
using namespace gsgl::framework;


namespace periapsis
{

    static config_variable<int> LABEL_WIDTH(L"ui/datetime_box/label_width", 128);
    static config_variable<int> JULIAN_WIDTH(L"ui/datetime_box/julian_width", 256);
    static config_variable<int> SPACING(L"ui/datetime_box/spacing", 8);


    datetime_box::datetime_box(widget *parent, int x, int y, int w, int h, const color & fg, const color & bg)
        : widget(parent, x, y, w, h, fg, bg), label(0), julian_input(0), text_input(0), jdn(0), mode(RECALC_NONE)
    {
        label = new textbox(this, 0, 0, LABEL_WIDTH, h, fg, bg, main_window::FONT_FACE, main_window::FONT_SIZE);
        label->get_background()[color::COMPONENT_ALPHA] = 0;
        label->get_text() = L"Date & Time:";

        julian_input = new textbox(this, LABEL_WIDTH, 0, JULIAN_WIDTH + SPACING, h, fg, bg, main_window::FONT_FACE, main_window::FONT_SIZE);

        text_input = new textbox(this, LABEL_WIDTH + JULIAN_WIDTH + SPACING*2, 0, w - (LABEL_WIDTH+JULIAN_WIDTH+SPACING*2), h, fg, bg, main_window::FONT_FACE, main_window::FONT_SIZE);
        text_input->get_text() = L"<no date & time specified>";
    } // datetime_box::datetime_box()


    datetime_box::~datetime_box()
    {
        // children will be automatically deleted
    } // datetime_box::~datetime_box()


    const double & datetime_box::get_jdn() const
    {
        return jdn;
    } // datetime_box::get_jdn()


    void datetime_box::set_jdn(const double & n)
    {
        mode = RECALC_BOTH;
        jdn = n;
    } // datetime_box::get_jdn()


    const string & datetime_box::get_text() const
    {
        return text_input->get_text();
    } // datetime_box::get_text()


    void datetime_box::set_text(const string & str)
    {
        mode = RECALC_JDN;
        text_input->get_text() = str;
    } // datetime_box::get_text()


    void datetime_box::draw()
    {
        switch (mode)
        {
        case RECALC_TEXT:
            recalc_text();
            mode = RECALC_NONE;
            break;
        case RECALC_JDN:
            recalc_jdn();
            mode = RECALC_NONE;
            break;
        case RECALC_BOTH:
            julian_input->get_text() = string::format(L"%f", jdn);
            recalc_text();
            mode = RECALC_NONE;
            break;
        default:
            break;
        }

        if (jdn == 0.0)
            julian_input->get_text() = L"<invalid>";
    } // datetime_box::draw()


    void datetime_box::recalc_text()
    {
        try
        {
            jdn = julian_input->get_text().to_double();
            julian_day jd(jdn);
            text_input->get_text() = jd.to_gregorian_string();
        }
        catch (...)
        {
            jdn = 0;
            text_input->get_text() = L"<invalid>";
        }
    } // datetime_box::recalc_text()


    void datetime_box::recalc_jdn()
    {
        try
        {
            julian_day jd;

            jd.from_gregorian_string(text_input->get_text());
            jdn = jd.get_jdn();
            julian_input->get_text() = string::format(L"%f", jdn);
        }
        catch (...)
        {
            jdn = 0;
        }
    } // datetime_box::recalc_jdn()


} // namespace periapsis
