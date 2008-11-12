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

uniform bool UseHeightmap;
uniform sampler2D Heightmap;
uniform vec4 HeightmapBounds;
uniform float HeightmapMax;


void get_texture_coords(inout vec2 tex_coords)
{
    tex_coords.s = (gl_MultiTexCoord0.s - TextureBounds.x) / (TextureBounds.z - TextureBounds.x);
    tex_coords.t = (gl_MultiTexCoord0.t - TextureBounds.y) / (TextureBounds.w - TextureBounds.y);
}

void apply_texture_coords(in int i, vec2 tex_coords)
{
    gl_TexCoord[i].st = tex_coords;
}

void get_heightmap_coords(inout vec2 hm_coords)
{
    hm_coords.s = (gl_MultiTexCoord0.s - HeightmapBounds.x) / (HeightmapBounds.z - HeightmapBounds.x);
    hm_coords.t = (gl_MultiTexCoord0.t - HeightmapBounds.y) / (HeightmapBounds.w - HeightmapBounds.y);
}

void get_vertex_pos(in vec2 hm_coords, inout vec4 vertex_pos)
{
    if (UseHeightmap)
    {
        float height = texture2D(Heightmap, hm_coords).x;
        vec4 normal = normalize(vec4(gl_Normal, 1.0));
        vertex_pos = vertex_pos + normal * (height * HeightmapMax * 100.0);
    }
}

void apply_vertex_pos(in vec4 vertex_pos)
{
    gl_Position = gl_ModelViewProjectionMatrix * vertex_pos;
}


void get_point_light(in int i, in vec3 eye_dir, in vec3 pos3, in vec3 normal, inout vec4 ambient, inout vec4 diffuse, inout vec4 specular)
{
    float n_dot_vp; // normal . light dir
    float n_dot_hv; // normal . light half vector
    float pf;       // power factor
    float att;      // attenuation
    float d;        // distance to light source
    vec3  vp;       // direction from surface to light
    vec3  hv;       // highlight direction

    // direction from surface to light
    vp = gl_LightSource[i].position.xyz - pos3;
    d = length(vp);
    vp = normalize(vp);
    
    // attenuation
    att = 1.0 / (gl_LightSource[i].constantAttenuation + gl_LightSource[i].linearAttenuation * d + gl_LightSource[i].quadraticAttenuation * d*d);
    
    // products
    hv = normalize(vp + eye_dir);
    n_dot_vp = max(0.0, dot(normal, vp));
    n_dot_hv = max(0.0, dot(normal, hv));
    
    pf = n_dot_vp == 0.0 ? 0.0 : pow(n_dot_hv, gl_FrontMaterial.shininess);
    
    ambient += gl_LightSource[i].ambient;
    diffuse += gl_LightSource[i].diffuse * n_dot_vp * att;
    specular += gl_LightSource[i].specular * pf * att;
}


void apply_lighting(in vec4 pos, in vec3 normal)
{
    vec3 pos3 = pos.xyz / pos.w;
    vec3 eye_dir = vec3(0.0, 0.0, 1.0);
    
    vec4 ambient, diffuse, specular;
    
    for (int i = 0; i < NumLights; ++i)
    {
        get_point_light(i, eye_dir, pos3, normal, ambient, diffuse, specular);
    }

    vec4 color = gl_FrontLightModelProduct.sceneColor 
        + (ambient * gl_FrontMaterial.ambient)
        + (diffuse * gl_FrontMaterial.diffuse) 
        + (specular * gl_FrontMaterial.specular);
    
    gl_FrontColor = clamp(color, 0.0, 1.0);
}


void main(void)
{
    vec4 vertex_pos = gl_Vertex;

    // texture and heightmap
    vec2 tex_coords, hm_coords;
    
    get_texture_coords(tex_coords);
    get_heightmap_coords(hm_coords);    
    get_vertex_pos(hm_coords, vertex_pos);
    
    apply_texture_coords(0, tex_coords);
    apply_vertex_pos(vertex_pos);

    // lighting
    vec4 pos_in_eye_space = gl_ModelViewMatrix * vertex_pos;
    vec3 normal_in_eye_space = normalize(gl_NormalMatrix * gl_Normal);
    
    if (NumLights > 0)
    {
        apply_lighting(pos_in_eye_space, normal_in_eye_space);
    }
    else
    {
        gl_FrontColor = gl_Color;
    }
} // main()
