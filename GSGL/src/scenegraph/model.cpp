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

#include "scenegraph/model.hpp"

#include "data/list.hpp"
#include "data/dictionary.hpp"
#include "data/fstream.hpp"
#include "data/file.hpp"
#include "data/cache.hpp"

#include "platform/vbuffer.hpp"
#include "platform/lowlevel.hpp"

#include <cmath>

namespace gsgl
{

    using namespace data;
    using namespace io;
    using namespace platform;
    
    namespace scenegraph
    {


        class material_impl
        {
        public:
            string name;

            color ambient;
            color diffuse;
            color specular;
            color emissive;

            float shininess;
            bool  flat;

            texture *tex;

            material_impl();
            ~material_impl();
        }; // class material_impl


        material_impl::material_impl()
            : ambient(0.8f, 0.8f, 0.8f, 1),
              diffuse(0.8f, 0.8f, 0.8f, 1),
              specular(0.0f, 0.0f, 0.0f, 1),
              emissive(0.0f, 0.0f, 0.0f, 1),
              shininess(128.0f),
              flat(false),
              tex(0)
        {
        } // material_impl::material_impl()

    
        material_impl::~material_impl()
        {
            delete tex;
        } // material_impl::~material_impl()


        //////////////////////////////////////////

        class material_file
            : public shared_object
        {
        public:
            dictionary<material_impl *, string> materials;

            material_file(const string & fname);
            ~material_file();

        private:
            void load_mtl_file(const string & fname);
        }; // class material_file


        material_file::material_file(const string & fname)
            : shared_object()
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
            for (dictionary<material_impl *, string>::iterator i = materials.iter(); i.is_valid(); ++i)
                delete *i;

            materials.clear();
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
                }
                else if (tokens[0] == L"Kd")
                {
                    cur->diffuse[color::COMPONENT_RED]   = static_cast<float>(tokens[1].to_double());
                    cur->diffuse[color::COMPONENT_GREEN] = static_cast<float>(tokens[2].to_double());
                    cur->diffuse[color::COMPONENT_BLUE]  = static_cast<float>(tokens[3].to_double());
                }
                else if (tokens[0] == L"Ks")
                {
                    cur->specular[color::COMPONENT_RED]   = static_cast<float>(tokens[1].to_double());
                    cur->specular[color::COMPONENT_GREEN] = static_cast<float>(tokens[2].to_double());
                    cur->specular[color::COMPONENT_BLUE]  = static_cast<float>(tokens[3].to_double());
                }
                else if (tokens[0] == L"d")
                {
                    float alpha = static_cast<float>(tokens[1].to_double());

                    cur->ambient[color::COMPONENT_ALPHA]  = alpha;
                    cur->diffuse[color::COMPONENT_ALPHA]  = alpha;
                    cur->specular[color::COMPONENT_ALPHA] = alpha;
                }
                else if (tokens[0] == L"Tr")
                {
                    float alpha = 1.0f - static_cast<float>(tokens[1].to_double());

                    cur->ambient[color::COMPONENT_ALPHA]  = alpha;
                    cur->diffuse[color::COMPONENT_ALPHA]  = alpha;
                    cur->specular[color::COMPONENT_ALPHA] = alpha;
                }
                else if (tokens[0] == L"illum")
                {
                    int code = tokens[1].to_int();
                    if (code == 1)
                        cur->flat = true;
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
                    cur->tex = new platform::texture(f.get_dir_name() + tokens[1], TEXTURE_LOAD_NO_PARAMS);
                }
                else
                {
                    throw runtime_exception(L"Syntax error in MTL file %ls:%d", fname.w_string(), line_number);
                }
            }
        } // material_file::load_mtl_file()


        //////////////////////////////////////////////////////////////

        typedef cache<material_file> material_cache;


        material::material(const string & filename, const string & material_name)
            : impl(0)
        {
            material_cache & global_material_files = *material_cache::global_instance();

            material_file *mf = 0;
            if (global_material_files.contains_index(filename))
            {
                mf = global_material_files[filename];
            }
            else
            {
                mf = new material_file(filename);
                global_material_files[filename] = mf; // assign this after in case the creator throws
                mf->attach(); // keep in the cache
            }
            mf->attach();

            if (!(impl = mf->materials[material_name]))
            {
                throw runtime_exception(L"Unable to find material '%ls' in material file '%ls'.", material_name.w_string(), filename.w_string());
            }
        } // material::material()


        material::~material()
        {
        } // material::~material()


        string & material::get_name()     { assert(impl); return impl->name; }
        color & material::get_ambient()   { assert(impl); return impl->ambient; }
        color & material::get_diffuse()   { assert(impl); return impl->diffuse; }
        color & material::get_specular()  { assert(impl); return impl->specular; }
        float & material::get_shininess() { assert(impl); return impl->shininess; }
        bool & material::get_flat()       { assert(impl); return impl->flat; }
        texture *material::get_texture()  { assert(impl); return impl->tex; }


        bool material::is_opaque()
        {
            assert(impl);
            return impl->ambient[color::COMPONENT_ALPHA] == 1.0f && impl->diffuse[color::COMPONENT_ALPHA] == 1.0f;
        } // material::is_opaque()


        /// \todo Implement differing texture flags.
        void material::bind(gsgl::flags_t render_flags)
        {
            assert(impl);

            glColor4fv(impl->diffuse.get_val());                                                                    CHECK_GL_ERRORS();

            glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, impl->ambient.get_val());                                   CHECK_GL_ERRORS();
            glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, impl->diffuse.get_val());                                   CHECK_GL_ERRORS();
            //glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, impl->specular.get_val());                                 CHECK_GL_ERRORS();
            glMaterialfv(GL_FRONT_AND_BACK, GL_EMISSION, impl->emissive.get_val());                                 CHECK_GL_ERRORS();
            glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, impl->shininess);                                          CHECK_GL_ERRORS();

            if (impl->flat)
            {
                glShadeModel(GL_FLAT);                                                                              CHECK_GL_ERRORS();
            }
            else
            {
                glShadeModel(GL_SMOOTH);                                                                            CHECK_GL_ERRORS();
            }

            if (impl->tex && !(render_flags & context::RENDER_UNTEXTURED))
            {
                glEnable(GL_TEXTURE_2D);

                impl->tex->bind();

                glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);                                        CHECK_GL_ERRORS();
               
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);                                       CHECK_GL_ERRORS();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);                                       CHECK_GL_ERRORS();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);                                   CHECK_GL_ERRORS();
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);                                   CHECK_GL_ERRORS();
            }
        } // material::bind()


        /// Provided only so that it can be added to the global metacache in the right order.
        gsgl::data_object *material::create_global_material_cache()
        {
            return new material_cache(L"material cache");
        } // material::global_material_cache()


        //////////////////////////////////////////////////////////////

        /// A submesh is part of a mesh with a particular material.  It is only for internal use.
        /// A submesh_node will collect one or more submeshes to use in a model part.
        class submesh
        {
        public:
            material *mat;

            // we are not bothering to share vertices
            vertex_buffer point_vertices;
            vertex_buffer line_vertices;
            vertex_buffer triangle_vertices;
            vertex_buffer triangle_texcoords;
            vertex_buffer triangle_normals;

            submesh();
            ~submesh();

            void load();
            void unload();
            void draw(gsgl::flags_t render_flags = 0);
        }; // class submesh


        submesh::submesh()
            : mat(0), 
              point_vertices(vbuffer::STATIC),
              line_vertices(vbuffer::STATIC),
              triangle_vertices(vbuffer::STATIC),
              triangle_texcoords(vbuffer::STATIC), 
              triangle_normals(vbuffer::STATIC)
        {
        } // submesh::submesh()


        submesh::~submesh()
        {
            unload();
            // don't delete the material; it's in the cache!
        } // submesh::~submesh()


        void submesh::load()
        {
            point_vertices.load();
            line_vertices.load();
            triangle_vertices.load();
            triangle_texcoords.load();
            triangle_normals.load();
        } // submesh::load()


        void submesh::unload()
        {
            point_vertices.unload();
            line_vertices.unload();
            triangle_vertices.unload();
            triangle_texcoords.unload();
            triangle_normals.unload();
        } // submesh::unload()


        void submesh::draw(gsgl::flags_t render_flags)
        {
            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                       CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                          CHECK_GL_ERRORS();

            glEnable(GL_POINT_SMOOTH);                                                                              CHECK_GL_ERRORS();
            glEnable(GL_LINE_SMOOTH);                                                                               CHECK_GL_ERRORS();
            glEnable(GL_POLYGON_SMOOTH);                                                                            CHECK_GL_ERRORS();

            glEnableClientState(GL_VERTEX_ARRAY);                                                                   CHECK_GL_ERRORS();
            glEnableClientState(GL_NORMAL_ARRAY);                                                                   CHECK_GL_ERRORS();
            glEnableClientState(GL_TEXTURE_COORD_ARRAY);                                                            CHECK_GL_ERRORS();

            // lighting and material
            if (!(render_flags & context::RENDER_UNLIT))
            {
                glEnable(GL_LIGHTING);                                                                          CHECK_GL_ERRORS();

                glLightModelf(GL_LIGHT_MODEL_LOCAL_VIEWER, GL_TRUE);                                            CHECK_GL_ERRORS();
                glLightModelf(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);                                               CHECK_GL_ERRORS();
                //glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
            }

            mat->bind(render_flags);

            // points
            if (point_vertices.size())
            {
                point_vertices.bind();
                glVertexPointer(3, GL_FLOAT, 0, 0);                                                                 CHECK_GL_ERRORS();

                glDrawArrays(GL_POINTS, 0, point_vertices.size()/3);                                                CHECK_GL_ERRORS();
            }

            // lines
            if (line_vertices.size())
            {
                line_vertices.bind();
                glVertexPointer(3, GL_FLOAT, 0, 0);                                                                 CHECK_GL_ERRORS();

                glDrawArrays(GL_LINES, 0, line_vertices.size()/3);                                                  CHECK_GL_ERRORS();
            }

            // triangles
            glEnable(GL_CULL_FACE);                                                                                 CHECK_GL_ERRORS();

            if (triangle_vertices.size())
            {
                if (render_flags & context::RENDER_WIREFRAME)
                {
                    glPolygonMode(GL_FRONT, GL_LINE);                                                               CHECK_GL_ERRORS();
                }
                else
                {
                    glPolygonMode(GL_FRONT, GL_FILL);                                                               CHECK_GL_ERRORS();
                }

                // sanity check
                if (triangle_normals.size() < triangle_vertices.size())
                    triangle_normals[triangle_vertices.size() - 1] = 0;
                if (triangle_texcoords.size()/2 < triangle_vertices.size()/3)
                    triangle_texcoords[(triangle_vertices.size()/3)*2 - 1] = 0;
                
                triangle_normals.bind();
                glNormalPointer(GL_FLOAT, 0, 0);                                                                    CHECK_GL_ERRORS();

                triangle_texcoords.bind();
                glTexCoordPointer(2, GL_FLOAT, 0, 0);                                                               CHECK_GL_ERRORS();

                triangle_vertices.bind();
                glVertexPointer(3, GL_FLOAT, 0, 0);                                                                 CHECK_GL_ERRORS();

                glDrawArrays(GL_TRIANGLES, 0, triangle_vertices.size()/3);                                          CHECK_GL_ERRORS();
            }

            glPopClientAttrib();                                                                                    CHECK_GL_ERRORS();
            glPopAttrib();                                                                                          CHECK_GL_ERRORS();
        } // submesh::draw()


        //////////////////////////////////////////

        /// A mesh is a named part of a mesh file.  Only for internal use.
        class mesh
        {
            string name;
            simple_array<submesh *> submeshes;

        public:
            mesh(const string & name);
            ~mesh();

            string & get_name() { return name; }
            simple_array<submesh *> & get_submeshes() { return submeshes; }
        }; // class mesh


        mesh::mesh(const string & name)
            : name(name)
        {
        } // mesh::mesh()


        mesh::~mesh()
        {
            for (simple_array<submesh *>::iterator i = submeshes.iter(); i.is_valid(); ++i)
                delete *i;
            submeshes.clear();
        } // mesh::~mesh()


        //////////////////////////////////////////

        class mesh_file
            : public shared_object
        {
            dictionary<mesh *, string> meshes;
        public:
            mesh_file(const string & fname);
            ~mesh_file();

            dictionary<mesh *, string> & get_meshes() { return meshes; }

        private:
            void load_obj_file(const string & fname);
        }; // class mesh_file


        //

        mesh_file::mesh_file(const string & fname)
            : shared_object()
        {
            string full_path = io::file::get_full_path(fname);

            string ext = full_path.right_substring(4);
            if (ext.make_lower() == L".obj")
                load_obj_file(full_path);
            else
                throw runtime_exception(L"%ls: invalid mesh file format.", full_path.w_string());
        } // mesh_file::mesh_file()


        mesh_file::~mesh_file()
        {
            for (dictionary<mesh *, string>::iterator i = meshes.iter(); i.is_valid(); ++i)
                delete *i;
            meshes.clear();
        } // mesh_file::~mesh_file()


        //

        static void add_triangle_vertex(submesh *sm, const string & s,
                                        simple_array<gsgl::real_t> & vertices, 
                                        simple_array<gsgl::real_t> & texcoords, 
                                        simple_array<gsgl::real_t> & normals)
        {
            // parse vertex indices
            int indices[3];
            int & vvv = indices[0];
            int & ttt = indices[1];
            int & nnn = indices[2];

            for (int i = 0; i < 3; ++i)
                indices[i] = 0;

            list<string> index_strs = s.split(L"/");
            list<string>::iterator index_str = index_strs.iter();
            for (int i = 0; index_str.is_valid(); ++i, ++index_str)
                indices[i] = index_str->to_int();
            
            if (vvv < 0 || ttt < 0 || nnn < 0)
                throw runtime_exception(L"Negative vertices not allowed in Periapsis OBJ files.");

            // add vertex
            for (int i = 0; i < 3; ++i)
            {
                if (vvv)
                    sm->triangle_vertices.get_buffer().append(vertices[(vvv-1)*3 + i]);
                if (ttt && i < 2)
                    sm->triangle_texcoords.get_buffer().append(texcoords[(ttt-1)*2 + i]);
                if (nnn)
                    sm->triangle_normals.get_buffer().append(normals[(nnn-1)*3 + i]);
            }
        } // add_triangle_vertex()


        static void add_point_vertex(submesh *sm, const string & s,
                                     const simple_array<gsgl::real_t> & vertices)
        {
            list<string> a = s.split(L"/");
            int vvv = a[0].to_int();
            if (vvv < 0)
                throw runtime_exception(L"Negative vertices not allowed in Periapsis OBJ files.");

            for (int i = 0; i < 3; ++i)
                sm->point_vertices.get_buffer().append(vertices[(vvv-1)*3 + i]);
        } // add_point_vertex()

        
        static void add_line_vertex(submesh *sm, const string & s,
                                    const simple_array<gsgl::real_t> & vertices)
        {
            list<string> a = s.split(L"/");
            int vvv = a[0].to_int();
            if (vvv < 0)
                throw runtime_exception(L"Negative vertices not allowed in Periapsis OBJ files.");

            for (int i = 0; i < 3; ++i)
                sm->line_vertices.get_buffer().append(vertices[(vvv-1)*3 + i]);
        } // add_line_vertex()


        //

        void mesh_file::load_obj_file(const string & fname)
        {
            // initialize current mesh & submesh
            string mesh_name = L"__default__mesh__";
            mesh *cur_mesh = new mesh(mesh_name);
            meshes[mesh_name] = cur_mesh;

            submesh *cur_submesh = 0;

            // initialize current material
            string cur_mtllib_name;
            string cur_material_name;

            // read through file
            simple_array<gsgl::real_t> vertices;
            simple_array<gsgl::real_t> texcoords;
            simple_array<gsgl::real_t> normals;
            
            // read through stream
            file f(fname);
            ft_stream fs(fname);
            int line_number = 0;
            string line;
            
            for (fs >> line; !fs.at_end(); fs >> line)
            {
                line_number++;
                line.trim();
                
                if (line.size() == 0 || line[0] == L'#')
                    continue;
                    
                list<string> tokens = line.split(L" \t");
                
                if (tokens[0] == L"g" || tokens[0] == L"o")
                {
                    cur_mesh = new mesh(tokens[1]);
                    meshes[tokens[1]] = cur_mesh;
                    
                    cur_submesh = 0;
                }
                else if (tokens[0] == L"mtllib")
                {
                    cur_mtllib_name = f.get_dir_name() + tokens[1];
                }
                else if (tokens[0] == L"usemtl")
                {
                    cur_material_name = tokens[1];
                    cur_submesh = 0;
                }
                else if (tokens[0] == L"v")
                {
                    for (gsgl::index_t i = 1; i < 4; ++i)
                        vertices.append(static_cast<gsgl::real_t>(tokens[i].to_double()));
                }
                else if (tokens[0] == L"vt")
                {
                    for (gsgl::index_t i = 1; i < 3; ++i)
                        texcoords.append(static_cast<gsgl::real_t>(tokens[i].to_double()));
                }
                else if (tokens[0] == L"vn")
                {
                    for (gsgl::index_t i = 1; i < 4; ++i)
                        normals.append(static_cast<gsgl::real_t>(tokens[i].to_double()));
                }
                else if (tokens[0] == L"f")
                {
                    if (!cur_submesh)
                    {
                        cur_mesh->get_submeshes().append(cur_submesh = new submesh());
                        cur_submesh->mat = new material(cur_mtllib_name, cur_material_name);
                    }

                    if (tokens.size() == 5)
                    {
                        add_triangle_vertex(cur_submesh, tokens[1], vertices, texcoords, normals);
                        add_triangle_vertex(cur_submesh, tokens[2], vertices, texcoords, normals);
                        add_triangle_vertex(cur_submesh, tokens[3], vertices, texcoords, normals);
                        
                        add_triangle_vertex(cur_submesh, tokens[3], vertices, texcoords, normals);
                        add_triangle_vertex(cur_submesh, tokens[4], vertices, texcoords, normals);
                        add_triangle_vertex(cur_submesh, tokens[1], vertices, texcoords, normals);
                    }
                    else if (tokens.size() == 4)
                    {
                        add_triangle_vertex(cur_submesh, tokens[1], vertices, texcoords, normals);
                        add_triangle_vertex(cur_submesh, tokens[2], vertices, texcoords, normals);
                        add_triangle_vertex(cur_submesh, tokens[3], vertices, texcoords, normals);
                    }
                    else if (tokens.size() > 5)
                    {
                        throw runtime_exception(L"Error in %ls at line %d: this program can only handle quads or triangles.", fname.w_string(), line_number);
                    }
                    // ignore faces with one or two vertices?!?!?!
                }
                else if (tokens[0] == L"p")
                {
                    if (!cur_submesh)
                    {
                        cur_mesh->get_submeshes().append(cur_submesh = new submesh());
                        cur_submesh->mat = new material(cur_mtllib_name, cur_material_name);
                    }

                    for (gsgl::index_t i = 1; i < tokens.size(); ++i)
                    {
                        add_point_vertex(cur_submesh, tokens[i], vertices);
                    }
                }
                else if (tokens[0] == L"l")
                {
                    if (!cur_submesh)
                    {
                        cur_mesh->get_submeshes().append(cur_submesh = new submesh());
                        cur_submesh->mat = new material(cur_mtllib_name, cur_material_name);
                    }

                    for (gsgl::index_t i = 2; i < tokens.size(); ++i)
                    {
                        add_line_vertex(cur_submesh, tokens[i-1], vertices);
                        add_line_vertex(cur_submesh, tokens[i], vertices);
                    }
                }
                else if (tokens[0] == L"s" || tokens[0] == L"sg" || tokens[0] == L"mg")
                {
                    // skip these
                }
                else
                {
                    throw runtime_exception(L"%ls (%d): syntax error; unknown command '%ls'.", fname.w_string(), line_number, tokens[0]);
                }
            }
        } // mesh_file::load_obj_file()


        //////////////////////////////////////////////////////////////

        typedef cache<mesh_file> mesh_file_cache;


        /// A submesh_node holds number of submeshes, to be drawn either in the opaque or translucent group.
        class submesh_node
            : public scenegraph::node
        {
            bool opaque;
            simple_array<submesh *> submeshes;

            mutable gsgl::real_t cached_max_extent;

        public:
            submesh_node(const string & name, node *parent, bool opaque);
            virtual ~submesh_node();

            bool get_opaque() { return opaque; }
            simple_array<submesh *> & get_submeshes() { return submeshes; }


            virtual gsgl::real_t get_priority(gsgl::scenegraph::context *); 
            virtual gsgl::real_t max_extent() const;

            virtual void init(gsgl::scenegraph::context *c);
            virtual void draw(gsgl::scenegraph::context *c);
            virtual void update(gsgl::scenegraph::context *c);
            virtual void cleanup(gsgl::scenegraph::context *c);
        }; // class submesh_node


        submesh_node::submesh_node(const string & name, node *parent, bool opaque)
            : node(name, parent), opaque(opaque), cached_max_extent(-1)
        {
        } // submesh_node::submesh_node()


        submesh_node::~submesh_node()
        {
            // submeshes are cached
        } // submesh_node::~submesh_node()


        gsgl::real_t submesh_node::get_priority(context *)
        {
            return opaque ? NODE_DRAW_SOLID : NODE_DRAW_TRANSLUCENT;
        } // submesh_node::get_priority()


        static double get_max_squared(const vertex_buffer & vertices)
        {
            double m, ms = 0;
            gsgl::real_t x, y, z;

            int i, len = vertices.size() / 3;
            for (i = 0; i < len; ++i)
            {
                x = vertices[i*3+0];
                y = vertices[i*3+1];
                z = vertices[i*3+2];

                m = x*x + y*y + z*z;
                if (m > ms)
                    ms = m;
            }

            return ms;
        } // get_max_squared()


        gsgl::real_t submesh_node::max_extent() const
        {
            if (cached_max_extent < 0)
            {
                double ms, msx = 0;
                
                for (simple_array<submesh *>::const_iterator i = submeshes.iter(); i.is_valid(); ++i)
                {
                    ms = get_max_squared( (*i)->point_vertices );
                    if (ms > msx)
                        msx = ms;

                    ms = get_max_squared( (*i)->line_vertices );
                    if (ms > msx)
                        msx = ms;

                    ms = get_max_squared( (*i)->triangle_vertices );
                    if (ms > msx)
                        msx = ms;
                }

                cached_max_extent = static_cast<gsgl::real_t>( ::sqrt(msx) );
            }

            return cached_max_extent;
        } // submesh_node::max_extent()


        void submesh_node::init(context *)
        {
            int i, len = submeshes.size();
            for (i = 0; i < len; ++i)
            {
                submeshes[i]->load();
            }
        } // submesh_node::init()


        void submesh_node::draw(context *c)
        {
            int i, len = submeshes.size();
            for (i = 0; i < len; ++i)
            {
                submeshes[i]->draw(c->render_flags);
            }
        } // submesh_node::draw()


        void submesh_node::update(context *)
        {
        } // submesh_node::update()


        void submesh_node::cleanup(context *)
        {
            int i, len = submeshes.size();
            for (i = 0; i < len; ++i)
            {
                submeshes[i]->unload();
            }
        } // submesh_node::cleanup()


        //////////////////////////////////////////////////////////////
        
        static int num_model_parts = 0;


        model_part::model_part(const config_record & obj_config)
            : node(string::format(L"model_part_%d", num_model_parts++), 0), opaque(0), translucent(0), inertial(0), collision(0)
        {
            mesh_file_cache & global_mesh_files = *mesh_file_cache::global_instance();

            opaque = new submesh_node(get_name() + L"_opaque", this, true);
            translucent = new submesh_node(get_name() + L"_translucent", this, false);

            // get model part
            if (!obj_config[L"file"].is_empty())
            {
                string mesh_fname = obj_config.get_directory().get_full_path() + obj_config[L"file"];

                mesh_file *mf = 0;
                if (global_mesh_files.contains_index(mesh_fname))
                {
                    mf = global_mesh_files[mesh_fname];
                }
                else
                {
                    mf = new mesh_file(mesh_fname);
                    global_mesh_files[mesh_fname] = mf;
                    mf->attach(); // keep in the cache
                }
                mf->attach();

                // get visual submeshes
                mesh *visual_mesh = 0;

                string visual_name = obj_config[L"visual"];
                if (!visual_name.is_empty())
                {
                    mesh *m = mf->get_meshes()[visual_name];

                    if (m)
                    {
                        visual_mesh = m;

                        for (simple_array<submesh *>::iterator i = m->get_submeshes().iter(); i.is_valid(); ++i)
                        {
                            if ((*i)->mat->is_opaque())
                                opaque->get_submeshes().append(*i);
                            else
                                translucent->get_submeshes().append(*i);
                        }
                    }
                    else
                    {
                        throw runtime_exception(L"Invalid visual object name %ls in model part %ls in %ls.", visual_name.w_string(), get_name().w_string(), obj_config.get_file().get_full_path().w_string());
                    }
                }
                else
                {
                    throw runtime_exception(L"Invalid model part %ls in %ls: missing visual object.", get_name().w_string(), obj_config.get_file().get_full_path().w_string());
                }

                // get collision mesh
                string collision_name = obj_config[L"collision"];
                if (!collision_name.is_empty())
                {
                    mesh *m = mf->get_meshes()[collision_name];

                    if (m)
                    {
                        collision = m;
                    }
                    else
                    {
                        throw runtime_exception(L"Invalid collision object name %ls in model part %ls in %ls.", visual_name.w_string(), get_name().w_string(), obj_config.get_file().get_full_path().w_string());
                    }
                }
                else
                {
                    collision = visual_mesh;
                }

                // get inertial mesh
                string inertial_name = obj_config[L"inertial"];
                if (!inertial_name.is_empty())
                {
                    mesh *m = mf->get_meshes()[inertial_name];

                    if (m)
                    {
                        inertial = m;
                    }
                    else
                    {
                        throw runtime_exception(L"Invalid inertial object name %ls in model part %ls in %ls.", visual_name.w_string(), get_name().w_string(), obj_config.get_file().get_full_path().w_string());
                    }
                }
                else
                {
                    inertial = visual_mesh;
                }
            }
            else
            {
                throw runtime_exception(L"Invalid model part %ls in %ls: missing file name.", get_name().w_string(), obj_config.get_file().get_full_path().w_string());
            }
        } // model_part::model_part()


        model_part::~model_part()
        {
            // the meshes are cached
        } // model_part::~model_part()
        

        data::list<platform::vertex_buffer *> & model_part::get_inertial_triangles()
        {
            assert(inertial);

            if (inertial_triangles.size() == 0)
            {
                for (simple_array<submesh *>::iterator i = inertial->get_submeshes().iter(); i.is_valid(); ++i)
                {
                    inertial_triangles.append( & (*i)->triangle_vertices );
                }
            }

            return inertial_triangles;
        } // model_part::get_inertial_triangles()


        data::list<platform::vertex_buffer *> & model_part::get_collision_triangles()
        {
            assert(collision);

            if (collision_triangles.size() == 0)
            {
                for (simple_array<submesh *>::iterator i = collision->get_submeshes().iter(); i.is_valid(); ++i)
                {
                    collision_triangles.append( & (*i)->triangle_vertices );
                }
            }

            return collision_triangles;
        } // model_part::get_collision_triangles()


        
        //////////////////////////////////////////////////////////////

        static int num_models = 0;


        model::model(const config_record & obj_config)
            : node(string::format(L"model_%d", num_models++), 0)
        {
            for (list<config_record>::const_iterator i = obj_config.get_children().iter(); i.is_valid(); ++i)
            {
                if (i->get_name() == L"model_part")
                {
                    model_part *mp = new model_part(*i);

                    mp->get_name() = string::format(L"%ls: %ls", get_name().w_string(), mp->get_name().w_string());
                    model_parts.append(mp);
                    add_child(mp);
                }
            }
        } // model::model()


        model::~model()
        {
        } // mode::~model()


        data::list<platform::vertex_buffer *> & model::get_inertial_triangles()
        {
            if (inertial_triangles.size() == 0)
            {
                for (simple_array<model_part *>::iterator i = model_parts.iter(); i.is_valid(); ++i)
                {
                    inertial_triangles.append( (*i)->get_inertial_triangles() );
                }
            }

            return inertial_triangles;
        } // model::get_inertial_triangles()
        
        
        data::list<platform::vertex_buffer *> & model::get_collision_triangles()
        {
            if (collision_triangles.size() == 0)
            {
                for (simple_array<model_part *>::iterator i = model_parts.iter(); i.is_valid(); ++i)
                {
                    collision_triangles.append( (*i)->get_collision_triangles() );
                }
            }

            return collision_triangles;
        } // model::get_collision_triangles()


        /// Provided only so that it can be created in the global metacache in the right order.
        gsgl::data_object *model::create_global_model_cache()
        {
            return new mesh_file_cache(L"mesh file cache");
        } // model::global_model_cache()


    } // namespace scenegraph


    // global material cache
    scenegraph::material_cache *scenegraph::material_cache::instance;

    // global mesh file cache
    scenegraph::mesh_file_cache *scenegraph::mesh_file_cache::instance;
    
} // namespace gsgl
