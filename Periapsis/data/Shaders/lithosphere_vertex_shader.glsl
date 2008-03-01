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

uniform int NumLights;

uniform vec4 TextureBounds;

//uniform bool UseHeightmap;
//uniform sampler2D Heightmap;
//uniform vec4 HeightmapBounds;
//uniform float HeightmapMax;

void main(void)
{
    // texture map lookup
    gl_TexCoord[0].s = (gl_MultiTexCoord0.s - TextureBounds.x) / (TextureBounds.z - TextureBounds.x);
    gl_TexCoord[0].t = (gl_MultiTexCoord0.t - TextureBounds.y) / (TextureBounds.w - TextureBounds.y);

    // height map lookup
    vec4 pos = gl_Vertex;

    //if (UseHeightmap)
    //{
    //    vec2 coords;
    //    coords.s = (gl_MultiTexCoord0.s - HeightmapBounds.x) / (HeightmapBounds.z - HeightmapBounds.x);
    //    coords.t = (gl_MultiTexCoord0.t - HeightmapBounds.y) / (HeightmapBounds.w - HeightmapBounds.y);

    //    float height = texture2D(Heightmap, coords).x;
    //    vec3 normal_in_world_space = normalize(vec4(gl_Normal, 1.0));
    //    pos.xyz = pos.xyz + normal_in_world_space * (height * HeightmapMax * 100.0);
    //}
    
    gl_Position = gl_ModelViewProjectionMatrix * pos;

    // lighting
    if (NumLights > 0)
    {
        vec4 pos_in_clip_space = gl_ModelViewMatrix * pos;
        vec3 normal_in_clip_space = normalize(gl_NormalMatrix * gl_Normal);

        vec3 light_pos = normalize(gl_LightSource[0].position - pos_in_clip_space);
        float diffuse_factor = dot(normal_in_clip_space, light_pos);

        gl_FrontColor = gl_Color * vec4(max(0.0, diffuse_factor));
    }
    else
    {
        gl_FrontColor = gl_Color;
    }
} // main()
