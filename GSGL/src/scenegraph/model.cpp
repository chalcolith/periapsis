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

#include "data/log.hpp"
#include "data/list.hpp"
#include "data/dictionary.hpp"
#include "data/fstream.hpp"
#include "data/file.hpp"

#include <cmath>


namespace gsgl
{

    using namespace data;
    using namespace io;
    using namespace platform;
    
    namespace scenegraph
    {

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


        void submesh::draw(const drawing_context *draw_context)
        {
            display & fb = *draw_context->screen;

            display::scoped_state state(fb, draw_context->display_flags());
            display::scoped_material cur_mat(fb, mat.ptr());

            if (point_vertices.size())
            {
                display::scoped_buffer points(fb, display::PRIMITIVE_POINTS, point_vertices);
                points.draw(point_vertices.size()/3);
            }

            if (line_vertices.size())
            {
                display::scoped_buffer lines(fb, display::PRIMITIVE_LINES, line_vertices);
                lines.draw(line_vertices.size()/3);
            }

            if (triangle_vertices.size())
            {
                display::scoped_buffer tris(fb, display::PRIMITIVE_TRIANGLES, triangle_vertices, triangle_normals, triangle_texcoords);
                tris.draw(triangle_vertices.size()/3, 0);
            }
        } // submesh::draw()

#if 0


            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                       CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                          CHECK_GL_ERRORS();

            // lighting and material
            if (!(render_flags & drawing_context::RENDER_NO_LIGHTING))
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
                if (render_flags & drawing_context::RENDER_WIREFRAME)
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
#endif


        //////////////////////////////////////////

        mesh::mesh(const string & name)
            : name(name)
        {
        } // mesh::mesh()


        mesh::~mesh()
        {
        } // mesh::~mesh()


        //////////////////////////////////////////

        class mesh_file
        {
        public:
            dictionary<shared_pointer<mesh>, string> meshes;

            mesh_file(const string & category, const string & fname);
            ~mesh_file();

        private:
            void load_obj_file(const string & category, const string & fname);
        }; // class mesh_file


        //

        mesh_file::mesh_file(const string & category, const string & fname)
        {
            string full_path = io::file::get_full_path(fname);

            string ext = full_path.right_substring(4);
            if (ext.make_lower() == L".obj")
                load_obj_file(category, full_path);
            else
                throw runtime_exception(L"%ls: invalid mesh file format.", full_path.w_string());
        } // mesh_file::mesh_file()


        mesh_file::~mesh_file()
        {
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

        void mesh_file::load_obj_file(const string & category, const string & fname)
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
                        cur_mesh->submeshes.append(cur_submesh = new submesh());
                        cur_submesh->mat = new material(category, cur_mtllib_name, cur_material_name);
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
                        cur_mesh->submeshes.append(cur_submesh = new submesh());
                        cur_submesh->mat = new material(category, cur_mtllib_name, cur_material_name);
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
                        cur_mesh->submeshes.append(cur_submesh = new submesh());
                        cur_submesh->mat = new material(category, cur_mtllib_name, cur_material_name);
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

        submesh_node::submesh_node(const string & name, node *parent, bool opaque)
            : node(name, parent), opaque(opaque), cached_view_radius(-1)
        {
        } // submesh_node::submesh_node()


        submesh_node::~submesh_node()
        {
        } // submesh_node::~submesh_node()


        gsgl::real_t submesh_node::draw_priority(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            return opaque ? NODE_DRAW_SOLID : NODE_DRAW_TRANSLUCENT;
        } // submesh_node::draw_priority()


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


        gsgl::real_t submesh_node::view_radius() const
        {
            if (cached_view_radius < 0)
            {
                double ms, msx = 0;
                
                for (object_array<shared_pointer<submesh> >::const_iterator i = submeshes.iter(); i.is_valid(); ++i)
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

                cached_view_radius = static_cast<gsgl::real_t>( ::sqrt(msx) );
            }

            return cached_view_radius;
        } // submesh_node::view_radius()


        void submesh_node::init(const simulation_context *)
        {
            int i, len = submeshes.size();
            for (i = 0; i < len; ++i)
            {
                submeshes[i]->load();
            }
        } // submesh_node::init()


        void submesh_node::draw(const simulation_context *, const drawing_context *draw_context)
        {
            int i, len = submeshes.size();
            for (i = 0; i < len; ++i)
            {
                submeshes[i]->draw(draw_context);
            }
        } // submesh_node::draw()


        void submesh_node::update(const simulation_context *)
        {
        } // submesh_node::update()


        void submesh_node::cleanup(const simulation_context *)
        {
            int i, len = submeshes.size();
            for (i = 0; i < len; ++i)
            {
                submeshes[i]->unload();
            }
        } // submesh_node::cleanup()


        //////////////////////////////////////////////////////////////

        typedef dictionary<shared_pointer<mesh_file>, string> mesh_file_cache;
        static dictionary<mesh_file_cache, string> mesh_files;

        static int num_model_parts = 0;


        model_part::model_part(const string & category, const config_record & obj_config)
            : node(string::format(L"model_part_%d: %ls::%ls::%d", num_model_parts++, category.w_string(), obj_config.get_file().get_full_path(), obj_config.get_line_number()), 0), 
              opaque(0), translucent(0), inertial(0), collision(0)
        {
            gsgl::log(string::format(L"model_part: creating '%ls'.", get_name()));

            opaque = new submesh_node(get_name() + L" opaque", this, true);
            translucent = new submesh_node(get_name() + L" translucent", this, false);

            // get model part
            if (!obj_config[L"file"].is_empty())
            {
                string mesh_fname = obj_config.get_directory().get_full_path() + obj_config[L"file"];

                shared_pointer<mesh_file> mf(0);
                if (mesh_files[category].contains_index(mesh_fname))
                {
                    gsgl::log(string::format(L"model_part: loading mesh file '%ls::%ls'.", category.w_string(), mesh_fname.w_string()));
                    mf = mesh_files[category][mesh_fname];
                }
                else
                {
                    gsgl::log(string::format(L"model_part: creating new mesh file '%ls::%ls'.", category.w_string(), mesh_fname.w_string()));

                    mf = new mesh_file(category, mesh_fname);
                    mesh_files[category][mesh_fname] = mf;
                }

                // get visual submeshes
                string visual_name = obj_config[L"visual"];
                shared_pointer<mesh> visual_mesh;

                if (!visual_name.is_empty())
                {
                    if (mf->meshes.contains_index(visual_name))
                    {
                        visual_mesh = mf->meshes[visual_name];

                        for (object_array<shared_pointer<submesh> >::iterator i = visual_mesh->submeshes.iter(); i.is_valid(); ++i)
                        {
                            if ((*i)->mat->is_opaque())
                                const_cast<object_array<shared_pointer<submesh> > &>(opaque->get_submeshes()).append(*i);
                            else
                                const_cast<object_array<shared_pointer<submesh> > &>(translucent->get_submeshes()).append(*i);
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
                    if (mf->meshes.contains_index(collision_name))
                    {
                        collision = mf->meshes[collision_name];
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
                    if (mf->meshes.contains_index(inertial_name))
                    {
                        inertial = mf->meshes[inertial_name];
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
            assert(inertial.ptr());

            if (inertial_triangles.size() == 0)
            {
                for (object_array<shared_pointer<submesh> >::iterator i = inertial->submeshes.iter(); i.is_valid(); ++i)
                {
                    inertial_triangles.append( & (*i)->triangle_vertices );
                }
            }

            return inertial_triangles;
        } // model_part::get_inertial_triangles()


        data::list<platform::vertex_buffer *> & model_part::get_collision_triangles()
        {
            assert(collision.ptr());

            if (collision_triangles.size() == 0)
            {
                for (object_array<shared_pointer<submesh> >::iterator i = collision->submeshes.iter(); i.is_valid(); ++i)
                {
                    collision_triangles.append( & (*i)->triangle_vertices );
                }
            }

            return collision_triangles;
        } // model_part::get_collision_triangles()


        
        //////////////////////////////////////////////////////////////

        static int num_models = 0;


        model::model(const string & category, const config_record & obj_config)
            : node(string::format(L"model_%d: %ls::%ls::%d", num_models++, category.w_string(), obj_config.get_file().get_full_path().w_string(), obj_config.get_line_number()), 0)
        {
            gsgl::log(string::format(L"model: creating '%ls'.", get_name().w_string()));

            for (list<config_record>::const_iterator i = obj_config.get_children().iter(); i.is_valid(); ++i)
            {
                if (i->get_name() == L"model_part")
                {
                    model_part *mp = new model_part(category, *i);

                    mp->get_name() = string::format(L"%ls: %ls", get_name().w_string(), mp->get_name().w_string());
                    model_parts.append(mp);
                    add_child(mp);
                }
            }
        } // model::model()


        model::~model()
        {
            // model parts will get deleted by the node destructor, as they are children
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


        void model::clear_cache(const string & category_name)
        {
            for (dictionary<mesh_file_cache, string>::iterator category = mesh_files.iter(); category.is_valid(); ++category)
            {
                if (category_name == category.get_index() || category_name == L"__ALL__") {
                    for (mesh_file_cache::iterator file = category->iter(); file.is_valid(); ++file)
                    {
                        for (dictionary<shared_pointer<mesh>, string>::iterator mi = (*file)->meshes.iter(); mi.is_valid(); ++mi)
                        {
                            if (mi->get_ref_count() > 1)
                                throw runtime_exception(L"Dangling mesh '%s'!", (*mi)->name);
                        }
                    }
                }

                category->clear();
            }
        } // model::clear_cache()


    } // namespace scenegraph
    
} // namespace gsgl
