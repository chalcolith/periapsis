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

#include "data/fstream.hpp"
#include "platform/texture.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::io;
using namespace gsgl::math;
using namespace gsgl::platform;


//////////////////////////////////////////////////////////////////////

struct patch_rec
{
    double left_longitude, right_longitude;
    double top_latitude, bottom_latitude;

    rgba_buffer *buffer;
}; // struct patch_rec



static void build_planet_patches(const string & input_image_fname, const string & output_patches_fname)
{
    texture *planet_tex = new texture(input_image_fname);
    
    
} // build_planet_patches()


//////////////////////////////////////////////////////////////////////

int main(int argc, char **argv)
{
    try
    {
        string input_fname, output_fname;

        if (argc > 1)
            input_fname = string(argv[1]);
        if (argc > 2)
            output_fname = string(argv[2]);

        if (!(input_fname.is_empty() || output_fname.is_empty()))
            build_planet_patches(input_fname, output_fname);
        else
            throw runtime_exception(L"usage: createplanetpatches input_texture_fname output_patch_file_fname");
    }
    catch (gsgl::exception & e)
    {
        ft_stream::err << e.get_message() << L"\n";

        return 1;
    }

    return 0;
} // main()
