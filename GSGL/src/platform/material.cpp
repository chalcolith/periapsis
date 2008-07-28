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

#include "platform/material.hpp"

#include "data/log.hpp"
#include "data/config.hpp"

#include "platform/lowlevel.hpp"


namespace gsgl
{

    using namespace data;
    using namespace io;
    using namespace platform;


    namespace platform
    {

        class material_file
        {
        public:
            string category;
            dictionary<shared_pointer<material_impl>, string> materials;

            material_file(const string & category, const string & fname);
            virtual ~material_file();

        private:
            void load_mtl_file(const string & fname);
        }; // class material_file


        //////////////////////////////////////////////////////////////

        material_file::material_file(const string & category, const string & fname)
            : category(category)
        {
            string full_path = io::file::get_full_path(fname);

            string ext = full_path.right_substring(4);
            if (ext.make_lower() == L".mtl")
                load_mtl_file(full_path);
            else
                throw runtime_exception(L"%ls: unknown material file format!", full_path.w_string());
        } // material_file::material_file()


        material_file::~material_file()
        {
        } // material_file::~material_file()


        void material_file::load_mtl_file(const string & fname)
        {
            ft_stream fs(fname);

            material_impl *cur = 0;

            string line;
            int line_number = 0;

            for (fs >> line; !fs.at_end(); fs >> line)
            {
                line_number++;

                line.trim();
                if (line.size() == 0 || line[0] == L'#')
                    continue;

                list<string> tokens = line.split(L" ");

                if (tokens[0] == L"newmtl")
                {
                    if (materials.contains_index(tokens[1]))
                        throw runtime_exception(L"Duplicate material '%ls' in %ls.", tokens[1].w_string(), fname.w_string());

                    cur = new material_impl();
                    materials[tokens[1]] = cur;
                }
                else if (tokens[0] == L"Ka")
                {
                    cur->ambient[color::COMPONENT_RED]   = static_cast<float>(tokens[1].to_double());
                    cur->ambient[color::COMPONENT_GREEN] = static_cast<float>(tokens[2].to_double());
                    cur->ambient[color::COMPONENT_BLUE]  = static_cast<float>(tokens[3].to_double());
                    cur->ambient[color::COMPONENT_ALPHA] = 1.0f;
                }
                else if (tokens[0] == L"Kd")
                {
                    cur->diffuse[color::COMPONENT_RED]   = static_cast<float>(tokens[1].to_double());
                    cur->diffuse[color::COMPONENT_GREEN] = static_cast<float>(tokens[2].to_double());
                    cur->diffuse[color::COMPONENT_BLUE]  = static_cast<float>(tokens[3].to_double());
                    cur->diffuse[color::COMPONENT_ALPHA] = 1.0f;
                }
                else if (tokens[0] == L"Ks")
                {
                    cur->specular[color::COMPONENT_RED]   = static_cast<float>(tokens[1].to_double());
                    cur->specular[color::COMPONENT_GREEN] = static_cast<float>(tokens[2].to_double());
                    cur->specular[color::COMPONENT_BLUE]  = static_cast<float>(tokens[3].to_double());
                    cur->specular[color::COMPONENT_ALPHA] = 1.0f;
                }
                else if (tokens[0] == L"d")
                {
                    float alpha = static_cast<float>(tokens[1].to_double());

                    cur->ambient[color::COMPONENT_ALPHA]  = alpha;
                    cur->diffuse[color::COMPONENT_ALPHA]  = alpha;
                    cur->specular[color::COMPONENT_ALPHA] = alpha;
                    cur->emissive[color::COMPONENT_ALPHA] = alpha;
                }
                else if (tokens[0] == L"Tr")
                {
                    float alpha = 1.0f - static_cast<float>(tokens[1].to_double());

                    cur->ambient[color::COMPONENT_ALPHA]  = alpha;
                    cur->diffuse[color::COMPONENT_ALPHA]  = alpha;
                    cur->specular[color::COMPONENT_ALPHA] = alpha;
                    cur->emissive[color::COMPONENT_ALPHA] = alpha;
                }
                else if (tokens[0] == L"illum")
                {
                    int code = tokens[1].to_int();
                    if (code == 1)
                        cur->render_flat = true;
                }
                else if (tokens[0] == L"Ns")
                {
                    cur->shininess = static_cast<float>(tokens[1].to_double());
                    if (cur->shininess > 128.0)
                        cur->shininess = 128.0;
                }
                else if (tokens[0] == L"map_Ka" || tokens[0] == L"map_Kd")
                {
                    file f(fname);
                    cur->color_map = new texture(category, f.get_dir_name() + tokens[1]);
                }
                else
                {
                    throw runtime_exception(L"Syntax error in MTL file %ls:%d", fname.w_string(), line_number);
                }
            }
        } // material_file::load_mtl_file()


        //////////////////////////////////////////////////////////////

        typedef data::dictionary<data::shared_pointer<material_impl>, gsgl::string> material_cache;
        typedef data::dictionary<data::shared_pointer<material_file>, gsgl::string> material_file_cache;

        static dictionary<material_cache, string>      non_file_materials;
        static dictionary<material_file_cache, string> material_files;

        //

        static color HOT_PINK(0xff69b4ff);
        static color GREEN(0x32cd32ff);


        material_impl::material_impl()
            : ambient(HOT_PINK), // hot pink
              diffuse(HOT_PINK), 
              specular(color::BLACK), 
              emissive(color::BLACK),

              color_map(0),
              normal_map(0),
              height_map(0),
              shader(0),

              shininess(128), 
              render_flat(false)
        {
        } // material_impl::material_impl()


        material_impl::material_impl(const config_record & conf)
            : ambient(HOT_PINK), // hot pink
              diffuse(HOT_PINK), 
              specular(color::BLACK), 
              emissive(color::BLACK),

              color_map(0),
              normal_map(0),
              height_map(0),
              shader(0),

              shininess(128), 
              render_flat(false)
        {
            if (!conf[L"color_map"].is_empty())
            {
                string fname = conf.get_directory().get_full_path() + conf[L"color_map"];
                color_map = new texture(L"material", fname, texture::TEXTURE_NO_FLAGS, texture::TEXTURE_COLORMAP, 0);
            }
        } // material_impl::material_impl()
        

        material_impl::~material_impl()
        {
        } // material_impl::~material_impl()


        //////////////////////////////////////////////////////////////

        material::material(const string & category, const config_record & conf)
            : platform_object(), impl(0), draw_flags(DRAW_NO_FLAGS)
        {
            string mat_key = string::format(L"%ls::%ls::%d", category.w_string(), conf.get_file().get_full_path().w_string(), conf.get_line_number());

            if (non_file_materials[category].contains_index(mat_key))
            {
                gsgl::log(string::format(L"material: loading material '%s'", mat_key.w_string()));
                impl = non_file_materials[category][mat_key];
            }
            else
            {
                gsgl::log(string::format(L"material: creating material '%s'", mat_key.w_string()));
                impl = new material_impl(conf);
                non_file_materials[category][mat_key] = impl;
            }

            // set up flags from what's in the material
            if (impl->specular != color::BLACK)
                draw_flags |= DRAW_SPECULAR;
            if (impl->emissive != color::BLACK)
                draw_flags |= DRAW_EMISSIVE;

            if (impl->color_map.ptr())
                draw_flags |= DRAW_COLOR_MAP;
            if (impl->normal_map.ptr())
                draw_flags |= DRAW_NORMAL_MAP;
            if (impl->height_map.ptr())
                draw_flags |= DRAW_HEIGHT_MAP;
            if (impl->shader.ptr())
                draw_flags |= DRAW_USE_SHADER;
        } // material::create_material()


        material::material(const string & category, const string & fname, const string & mat_name)
            : platform_object(), impl(0), draw_flags(DRAW_NO_FLAGS)
        {
            // get material key in case we've already loaded it
            io::file ff(fname);
            string full_path = ff.get_full_path();

            // check for material file
            material_file *mat_file = material_files[category][full_path].ptr();
            if (!mat_file)
            {
                gsgl::log(string::format(L"material: creating new material file record '%ls::%ls'.", category.w_string(), full_path.w_string()));
                mat_file = new material_file(category, full_path);
                material_files[category][full_path] = mat_file;
            }

            // get material in file
            if (mat_file->materials.contains_index(mat_name))
            {
                gsgl::log(string::format(L"material: loading material '%ls::%ls'.", category.w_string(), mat_name.w_string()));
                impl = mat_file->materials[mat_name];
            }
            else
            {
                throw runtime_exception(L"There is no material '%ls' in %ls!", mat_name.w_string(), full_path.w_string());
            }

            // set up flags from what's in the material
            if (impl->specular != color::BLACK)
                draw_flags |= DRAW_SPECULAR;
            if (impl->emissive != color::BLACK)
                draw_flags |= DRAW_EMISSIVE;

            if (impl->color_map.ptr())
                draw_flags |= DRAW_COLOR_MAP;
            if (impl->normal_map.ptr())
                draw_flags |= DRAW_NORMAL_MAP;
            if (impl->height_map.ptr())
                draw_flags |= DRAW_HEIGHT_MAP;
            if (impl->shader.ptr())
                draw_flags |= DRAW_USE_SHADER;
        } // material::create_material()


        material::~material()
        {
        } // material::~material()


        const platform::color & material::get_ambient() const
        {
            assert(impl.ptr());
            return impl->ambient;
        } // material::get_ambient()


        const platform::color & material::get_diffuse() const
        {
            assert(impl.ptr());
            return impl->diffuse;
        } // material::get_diffuse()


        const platform::color & material::get_specular() const
        {
            assert(impl.ptr());
            return impl->specular;
        } // material::get_specular()


        const platform::color & material::get_emissive() const
        {
            assert(impl.ptr());
            return impl->emissive;
        } // material::get_emissive()


        const platform::texture *material::get_color_map() const
        {
            assert(impl.ptr());
            return impl->color_map.ptr();
        } // material::get_color_map()


        const platform::texture *material::get_normal_map() const
        {
            assert(impl.ptr());
            return impl->normal_map.ptr();
        } // material::get_normal_map()


        const platform::texture *material::get_height_map() const
        {
            assert(impl.ptr());
            return impl->height_map.ptr();
        } // material::get_height_map()


        const platform::shader_program *material::get_shader() const
        {
            assert(impl.ptr());
            return impl->shader.ptr();
        } // material::shader()


        void material::load()
        {
            assert(impl.ptr());

            if ((draw_flags & DRAW_COLOR_MAP) && impl->color_map.ptr())
                impl->color_map->load();

            if ((draw_flags & DRAW_NORMAL_MAP) && impl->normal_map.ptr())
                impl->normal_map->load();

            if ((draw_flags & DRAW_HEIGHT_MAP) && impl->height_map.ptr())
                impl->height_map->load();

            if ((draw_flags & DRAW_USE_SHADER) && impl->shader.ptr())
                impl->shader->load();
        } // material::load()


        void material::unload()
        {
            assert(impl.ptr());

            if ((draw_flags & DRAW_COLOR_MAP) && impl->color_map.ptr())
                impl->color_map->unload();

            if ((draw_flags & DRAW_NORMAL_MAP) && impl->normal_map.ptr())
                impl->normal_map->unload();

            if ((draw_flags & DRAW_HEIGHT_MAP) && impl->height_map.ptr())
                impl->height_map->unload();

            if ((draw_flags & DRAW_USE_SHADER) && impl->shader.ptr())
                impl->shader->unload();
        } // material::unload()


        void material::bind(gsgl::flags_t render_flags) const
        {
            assert(impl.ptr());

            if (draw_flags & DRAW_DIFFUSE)
            {
                glColor4fv(impl->diffuse.get_val());                                                                    CHECK_GL_ERRORS();
            }

            if (draw_flags & DRAW_AMBIENT)
            {
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, impl->ambient.get_val());                                   CHECK_GL_ERRORS();
            }

            if (draw_flags & DRAW_DIFFUSE)
            {
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, impl->diffuse.get_val());                                   CHECK_GL_ERRORS();
            }

            if (draw_flags & DRAW_SPECULAR)
            {
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, impl->specular.get_val());                                 CHECK_GL_ERRORS();
            }

            if (draw_flags & DRAW_EMISSIVE)
            {
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, impl->emissive.get_val());                                 CHECK_GL_ERRORS();
            }

            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, impl->shininess);                                          CHECK_GL_ERRORS();

            if (impl->render_flat)
            {
                glShadeModel(GL_FLAT);                                                                              CHECK_GL_ERRORS();
            }
            else
            {
                glShadeModel(GL_SMOOTH);                                                                            CHECK_GL_ERRORS();
            }

            //
            if ((draw_flags & DRAW_COLOR_MAP) && impl->color_map.ptr())
                impl->color_map->bind();

            if ((draw_flags & DRAW_NORMAL_MAP) && impl->normal_map.ptr())
                impl->normal_map->bind();

            if ((draw_flags & DRAW_HEIGHT_MAP) && impl->height_map.ptr())
                impl->height_map->bind();

            if ((draw_flags & DRAW_USE_SHADER) && impl->shader.ptr())
                impl->shader->bind();
        } // material::bind()


        void material::unbind() const
        {
            assert(impl.ptr());

            if (draw_flags & DRAW_DIFFUSE)
                glColor4fv(GREEN.get_val());

            if (draw_flags & DRAW_AMBIENT)
                glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, GREEN.get_val());

            if (draw_flags & DRAW_DIFFUSE)
                glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, GREEN.get_val());

            if (draw_flags & DRAW_SPECULAR)
                glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, GREEN.get_val());

            if (draw_flags & DRAW_EMISSIVE)
                glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, GREEN.get_val());

            if ((draw_flags & DRAW_COLOR_MAP) && impl->color_map.ptr())
                impl->color_map->unbind();

            if ((draw_flags & DRAW_NORMAL_MAP) && impl->normal_map.ptr())
                impl->normal_map->unbind();

            if ((draw_flags & DRAW_HEIGHT_MAP) && impl->height_map.ptr())
                impl->height_map->unbind();

            if ((draw_flags & DRAW_USE_SHADER) && impl->shader.ptr())
                impl->shader->unbind();
        } // material::unbind()


        bool material::is_opaque() const
        {
            assert(impl.ptr());

            return impl->ambient[color::COMPONENT_ALPHA] == 1.0f && impl->diffuse[color::COMPONENT_ALPHA] == 1.0f;
        } // material::is_opaque()


        void material::clear_cache(const string & category)
        {
            // clear non-file materials
            for (dictionary<material_cache, string>::iterator cache = non_file_materials.iter(); cache.is_valid(); ++cache)
            {
                if (category == L"__ALL__" || cache.get_index() == category)
                {
                    for (material_cache::iterator i = cache->iter(); i.is_valid(); ++i)
                    {
                        if (i->get_ref_count() > 1)
                            throw memory_exception(__FILE__, __LINE__, L"Dangling reference to material %ls.", i.get_index().w_string());
                    }

                    cache->clear();
                }
            }
            
            // clear material files
            for (dictionary<material_file_cache, string>::iterator files = material_files.iter(); files.is_valid(); ++files)
            {
                if (category == L"__ALL__" || files.get_index() == category)
                {
                    for (material_file_cache::iterator i = files->iter(); i.is_valid(); ++i)
                    {
                        for (dictionary<shared_pointer<material_impl>, string>::iterator j = (*i)->materials.iter(); j.is_valid(); ++j)
                        {
                            if (j->get_ref_count() > 1)
                                throw memory_exception(__FILE__, __LINE__, L"Dangling reference to material %ls::%ls::%ls.", category, i.get_index(), j.get_index());
                        }
                    }

                    files->clear();
                }
            }
        } // material::clear_cache()


    } // namespace platform

} // namespace gsgl
