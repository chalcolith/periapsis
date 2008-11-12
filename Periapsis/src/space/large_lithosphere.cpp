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

#include "space/large_lithosphere.hpp"
#include "space/celestial_body.hpp"

#include "data/pointer.hpp"
#include "math/units.hpp"
#include "platform/texture.hpp"
#include "scenegraph/camera.hpp"
#include "framework/application.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::math;
using namespace gsgl::scenegraph;
using namespace gsgl::platform;

namespace periapsis
{

    namespace space
    {

        /// A quadtree used for drawing planetary lithospheres.
        class lithosphere_quadtree
            : public spherical_quadtree
        {
            friend class lithosphere_qt_node;

            /// Multiple instances of this class will share the first instance's shader.
            /// This ensures that when the last instance of the class is deleted, the shader will be also.
            gsgl::data::shared_pointer<gsgl::platform::shader_program> shader;

            gsgl::platform::shader_uniform<int>      *uniform_num_lights;
            gsgl::platform::shader_uniform<float[4]> *uniform_texture_bounds;
            gsgl::platform::shader_uniform<bool>     *uniform_use_heightmap;
            gsgl::platform::shader_uniform<int>      *uniform_height_map;
            gsgl::platform::shader_uniform<float[4]> *uniform_heightmap_bounds;
            gsgl::platform::shader_uniform<float>    *uniform_heightmap_max;

        public:
            lithosphere_quadtree(large_lithosphere *parent_sg_node, const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius);
            virtual ~lithosphere_quadtree();

            virtual void init(const gsgl::scenegraph::simulation_context *);
            virtual void draw(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);
            virtual void cleanup();

            virtual sph_qt_node *create_node(sph_qt_node *parent);

        private:
            static gsgl::data::shared_pointer<gsgl::platform::shader_program> *shader_instance;
        }; // class lithosphere_quadtree


        /// Quadtree nodes for the lithosphere quadtree.
        class lithosphere_qt_node
            : public sph_qt_node
        {
            gsgl::platform::material       *own_material;     ///< If this node has generated its own material, this is it.
            const gsgl::platform::material *current_material; ///< This is the material that the node is using; it may be a parent's.

            static const gsgl::platform::material *last_material;

            /// Stores minimum longitude and latitude, and maximum longitude and latitude, of the quad's texture.
            /// Longitude is specified as the actual longitude / 2PI, and latitude as the 0.5 + the actual latitude / PI.
            gsgl::real_t texture_bounds[4]; ///< Min longitude and latitude

            gsgl::platform::texture *own_heightmap;     ///< If this node has generated its own heightmap, this is it.
            const gsgl::platform::texture *current_heightmap; ///< This is the heightmap that the node is using; it may be a parent's.

            gsgl::real_t heightmap_bounds[4];

            friend class lithosphere_quadtree;

        public:
            lithosphere_qt_node(lithosphere_quadtree *parent_quadtree, sph_qt_node *parent);
            virtual ~lithosphere_qt_node();

            virtual void draw(const gsgl::scenegraph::simulation_context *, const gsgl::scenegraph::drawing_context *);
        }; // class lithosphere_qt_node


        const gsgl::platform::material *lithosphere_qt_node::last_material = 0;


        lithosphere_qt_node::lithosphere_qt_node(lithosphere_quadtree *parent_quadtree, sph_qt_node *parent)
            : sph_qt_node(parent_quadtree, parent),
              own_material(0), current_material(0)
        {
            lithosphere_qt_node *pqn = dynamic_cast<lithosphere_qt_node *>(parent);

            if (pqn)
            {
                // get new texture if necessary
                current_material = pqn->current_material;

                for (int i = 0; i < 4; ++i)
                    texture_bounds[i] = pqn->texture_bounds[i];
            }
        } // lithosphere_qt_node::lithosphere_qt_node()


        lithosphere_qt_node::~lithosphere_qt_node()
        {
        } // lithosphere_qt_node::~lithosphere_qt_node()


        void lithosphere_qt_node::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            if (should_draw(sim_context, draw_context))
            {
                if (is_a_leaf())
                {

                    lithosphere_quadtree *lqt = dynamic_cast<lithosphere_quadtree *>(parent_quadtree);
                    assert(lqt->shader.ptr());

                    if (current_material && (current_material != last_material))
                    {
                        if (last_material)
                            last_material->unbind();

                        current_material->bind();
                        last_material = current_material;
                    }

                    lqt->uniform_texture_bounds->set(texture_bounds);

                    lithosphere *ls = dynamic_cast<lithosphere *>(lqt->get_parent_sg_node());
                    assert(ls);

                    const celestial_body *cb = ls->get_parent_body();
                    if (current_material && current_material->get_height_map())
                    {
                        lqt->uniform_use_heightmap->set(true);
                        lqt->uniform_height_map->set(current_material->get_height_map()->get_texture_unit());
                        lqt->uniform_heightmap_bounds->set(heightmap_bounds);
                        lqt->uniform_heightmap_max->set(cb->get_simple_height_max());
                    }
                    else
                    {
                        lqt->uniform_use_heightmap->set(false);
                    }
                }

                sph_qt_node::draw(sim_context, draw_context);
            }
        } // lithosphere_qt_node::draw()


        //////////////////////////////////////////////////////////////

        shared_pointer<shader_program> *lithosphere_quadtree::shader_instance = 0;


        lithosphere_quadtree::lithosphere_quadtree(periapsis::space::large_lithosphere *parent_sg_node, const gsgl::real_t & polar_radius, const gsgl::real_t & equatorial_radius)
            : spherical_quadtree(parent_sg_node, polar_radius, equatorial_radius), shader(shader_instance ? *shader_instance : 0)
        {
            if (!shader_instance)
            {
                shader_instance = new shared_pointer<shader_program>(new shader_program());
                shader = *shader_instance;
                shader->add_vertex_shader(gsgl::framework::application::SYS_DATA_PATH + L"Shaders" + io::directory::SEPARATOR + L"lithosphere_vertex_shader.glsl");
            }
        } // lithosphere_quadtree::lithosphere_quadtree()


        lithosphere_quadtree::~lithosphere_quadtree()
        {
            if (shader.get_ref_count() == 2)
            {
                delete shader_instance;
                shader_instance = 0;
            }
        } // lithosphere_quadtree::~lithosphere_quadtree()


        void lithosphere_quadtree::init(const simulation_context *sim_context)
        {
            shader->load(); // redundant, but it won't try to load unnecessarily

            // these will not leak; they will be cleaned up by the shader
            uniform_num_lights       = shader->get_uniform<int>     (L"NumLights");
            uniform_texture_bounds   = shader->get_uniform<float[4]>(L"TextureBounds");
            uniform_use_heightmap    = shader->get_uniform<bool>    (L"UseHeightmap");
            uniform_height_map       = shader->get_uniform<int>     (L"Heightmap");
            uniform_heightmap_bounds = shader->get_uniform<float[4]>(L"HeightmapBounds");
            uniform_heightmap_max    = shader->get_uniform<float>   (L"HeightmapMax");

            spherical_quadtree::init(sim_context);

            // assign simple texture
            lithosphere *ls = dynamic_cast<lithosphere *>(parent_sg_node);
            if (ls)
            {
                const celestial_body *cb = ls->get_parent_body();
                assert(cb);

                for (int i = 0; i < 6; ++i)
                {
                    lithosphere_qt_node *q = dynamic_cast<lithosphere_qt_node *>(root_nodes[i]);
                    if (q)
                    {
                        q->current_material = cb->get_simple_material();

                        // color map
                        gsgl::real_t lon_offset = cb->get_simple_color_offset().get_x();
                        q->texture_bounds[0] = 0 + lon_offset;
                        q->texture_bounds[2] = 1 + lon_offset;

                        gsgl::real_t lat_offset = cb->get_simple_color_offset().get_y();
                        q->texture_bounds[1] = 0 + lat_offset;
                        q->texture_bounds[3] = 1 + lat_offset;

                        // height map
                        lon_offset = cb->get_simple_height_offset().get_x();
                        q->heightmap_bounds[0] = 0 + lon_offset;
                        q->heightmap_bounds[2] = 1 + lon_offset;

                        lat_offset = cb->get_simple_height_offset().get_y();
                        q->heightmap_bounds[1] = 0 + lat_offset;
                        q->heightmap_bounds[3] = 1 + lat_offset;
                    }
                }
            }
        } // lithosphere_quadtree::init()


        void lithosphere_quadtree::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            display::scoped_shader shdr(*draw_context->screen, shader.ptr());
            uniform_num_lights->set(draw_context->num_lights);
            lithosphere_qt_node::last_material = 0; // force binding the first material

            spherical_quadtree::draw(sim_context, draw_context);

            if (lithosphere_qt_node::last_material)
                lithosphere_qt_node::last_material->unbind();
        } // lithosphere_quadtree::draw()


        void lithosphere_quadtree::cleanup()
        {
            shader->unload(); // redundant, but it won't try to unload unnecessarily
        } // lithosphere_quadtree::cleanup()


        sph_qt_node *lithosphere_quadtree::create_node(sph_qt_node *parent)
        {
            return new lithosphere_qt_node(this, parent);
        } // lithosphere_quadtree::create_node()


        //////////////////////////////////////////////////////////////

        large_lithosphere::large_lithosphere(const string & name, node *parent, body_rotator *rotator)
            : lithosphere(name, parent, rotator), quadtree(0)
        {
            assert(get_parent_body());
            quadtree = new lithosphere_quadtree(this, get_parent_body()->get_polar_radius(), get_parent_body()->get_equatorial_radius());
        } // large_lithosphere::large_lithosphere()


        large_lithosphere::~large_lithosphere()
        {
            delete quadtree;
        } // large_lithosphere::~large_lithosphere()


        gsgl::real_t large_lithosphere::draw_priority(const simulation_context *, const drawing_context *)
        {
            // the lithosphere is drawn by its parent body
            return node::NODE_DRAW_IGNORE;
        } // large_lithosphere::draw_priority()


        gsgl::real_t large_lithosphere::view_radius() const
        {
            assert(get_parent());
            return get_parent()->view_radius();
        } // large_lithosphere::view_radius()


        gsgl::real_t large_lithosphere::default_view_distance() const
        {
            assert(get_parent());
            return get_parent()->default_view_distance();
        } // large_lithosphere::default_view_distance()


        gsgl::real_t large_lithosphere::minimum_view_distance() const
        {
            assert(get_parent());
            return get_parent()->minimum_view_distance();
        } // large_lithosphere::minimum_view_distance()


        void large_lithosphere::init(const simulation_context *c)
        {
            quadtree->init(c);
        } // large_lithosphere::init()


        void large_lithosphere::draw(const simulation_context *sim_context, const drawing_context *draw_context)
        {
            // all the OpenGL setup is handled by the parent celestial body
            quadtree->draw(sim_context, draw_context);
        } // large_lithosphere::draw()


        void large_lithosphere::update(const simulation_context *c)
        {
            // rotate into the right position
            lithosphere::update(c);

            // update the quad tree
            assert(get_parent_body());
            quadtree->update(c, (get_parent_body()->get_draw_results() & (node::NODE_DREW_POINT & node::NODE_OFF_SCREEN)) != 0);
        } // large_lithosphere::update()


        void large_lithosphere::cleanup(const simulation_context *c)
        {
            quadtree->cleanup();
        } // large_lithosphere::cleanup()


    } // namespace space


} // namespace periapsis
