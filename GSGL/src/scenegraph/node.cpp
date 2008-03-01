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

#include "scenegraph/node.hpp"
#include "scenegraph/camera.hpp"
#include "scenegraph/light.hpp"
#include "scenegraph/utils.hpp"

#include "data/pqueue.hpp"
#include "math/units.hpp"

#include "platform/budget.hpp"
#include "platform/lowlevel.hpp"

#include <cmath>
#include <cfloat>

namespace gsgl
{

    using namespace data;
    using namespace math;
    
    namespace scenegraph
    {

        node_relative_path::node_relative_path(node *reference, node *cur)
            : relative_transform(transform::IDENTITY)
        {
            initialize(reference, cur);
        } // node_relative_path::node_relative_path()


        static void get_parent_list(node *n, simple_array<node *> & a)
        {
            if (n->get_parent())
                get_parent_list(n->get_parent(), a);
            a.append(n);
        } // get_parent_list()


        void node_relative_path::initialize(node *reference, node *cur)
        {
            simple_array<node *> list_from;
            simple_array<node *> list_to;

            get_parent_list(cur, list_from);
            get_parent_list(reference, list_to);

            int i, num = gsgl::min_val(list_from.size(), list_to.size());
            for (i = 0; i < num; ++i)
            {
                if (list_from[i] != list_to[i])
                    break;
            }

            to_cur.clear();
            for (int j = i; j < list_from.size(); ++j)
                to_cur.append(list_from[j]);

            from_ref.clear();
            for (int j = i; j < list_to.size(); ++j)
                from_ref.append(list_to[j]);
        } // node_relative_path::initialize()

        
        const math::transform & node_relative_path::calc_relative_transform(bool include_translation)
        {
            relative_transform = transform::IDENTITY;

            int i, len = from_ref.size();
            for (i = 0; i < len; ++i)
            {
                relative_transform = relative_transform * from_ref[i]->get_orientation().transpose();
                if (include_translation)
                    relative_transform = relative_transform * transform::translation_transform(-from_ref[i]->get_translation() * from_ref[i]->get_scale());
            }

            len = to_cur.size();
            for (i = 0; i < len; ++i)
            {
                if (include_translation)
                    relative_transform = relative_transform * transform::translation_transform(to_cur[i]->get_translation() * from_ref[i]->get_scale());
                relative_transform = relative_transform * to_cur[i]->get_orientation();
            }

            return relative_transform;
        } // node_relative_path::calc_relative_transform()


    
        node::node(const string & name, node *parent)
            : scenegraph_object(),
              parent(parent), name(name), scale(1),
              translation(vector::ZERO), orientation(transform::IDENTITY), 
              modelview(transform::IDENTITY),
              draw_flags(0), draw_results(0)
        {
            if (parent)
                parent->add_child(this);
        } // node::node()

        
        node::node(const data::config_record & conf)
            : scenegraph_object(),
              parent(0), scale(1),
              translation(vector::ZERO), orientation(transform::IDENTITY), 
              modelview(transform::IDENTITY),
              draw_flags(0), draw_results(0)
        {
            name = conf[L"name"];
            parent_name = conf[L"parent"];

            if (!conf[L"scale"].is_empty())
                scale = math::units::parse(conf[L"scale"]);
            if (!conf[L"position"].is_empty())
                translation = vector::parse(conf[L"position"]);
            if (!conf[L"orientation"].is_empty())
                orientation = transform::parse(conf[L"orientation"]);
        } // node::node()
        

        node::~node()
        {
            for (simple_array<node *>::iterator i = children.iter(); i.is_valid(); ++i)
                delete *i;
        } // node::~node()
        

        // accessors
        
        const node *node::get_parent() const
        {
            return parent;
        } // node::get_parent()


        node * & node::get_parent()
        {
            return parent;
        } // node::get_parent()
        

        data::simple_array<node *> & node::get_children()
        {
            return children;
        } // node::get_children()
        

        static int num_unnamed_nodes = 0;

        const string & node::get_name() const
        {
            string & name_ref = const_cast<string &>(name);

            if (name_ref.is_empty())
            {
                name_ref = string::format(L"node_%d", num_unnamed_nodes++);

                if (parent && !parent->get_name().is_empty())
                {
                    int i, len = parent->get_children().size();
                    for (i = 0; i < len; ++i)
                        if (parent->get_children()[i] == this)
                            break;

                    if (i < len)
                        name_ref += string::format(L" (%ls child %d)", parent->get_name(), i);
                }
            }

            return name;
        } // node::get_name()

        string & node::get_name()
        {
            return name;
        } // node::get_name()
        
        string & node::get_parent_name()
        {
            return parent_name;
        } // node::get_parent_name()

        gsgl::real_t & node::get_scale()
        {
            return scale;
        } // node::get_scale()
        
        math::vector & node::get_translation()
        {
            return translation;
        } // node::get_translation()
        
        math::transform & node::get_orientation()
        {
            return orientation;
        } // node::get_orientation()
                
        math::transform & node::get_modelview()
        {
            return modelview;
        } // node::get_modelview()
        
        //

        const gsgl::real_t node::NODE_DRAW_IGNORE       = 0.000f;
        const gsgl::real_t node::NODE_DRAW_SOLID        = 0.001f;
        const gsgl::real_t node::NODE_DRAW_TRANSLUCENT  = 0.002f;
        const gsgl::real_t node::NODE_DRAW_FIRST        = FLT_MAX;


        gsgl::real_t node::get_priority(context *)
        {
            return NODE_DRAW_IGNORE;
        } // node::get_priority()


        gsgl::real_t node::max_extent() const
        {
            return 0;
        } // node::max_extent()


        gsgl::real_t node::default_view_distance() const
        {
            return static_cast<gsgl::real_t>(32.0);
        } // node::default_view_distance()


        gsgl::real_t node::minimum_view_distance() const
        {
            return max_extent();
        } // node::minimum_view_distance()

        
        void node::add_child(node *child)
        {
            assert(child);

            children.append(child);
            child->parent = this;
        } // node::add_child()


        void node::detach()
        {
            if (parent)
            {
                simple_array<node *>::iterator child = parent->children.find_value(this);
                if (child.is_valid())
                    parent->children.remove(child);
            }

            parent = 0;
        } // node::detach()

        
        bool node::connect(node *branch)
        {
            if (name == branch->parent_name)
            {
                this->add_child(branch);
                return true;
            }
            
            for (simple_array<node *>::iterator i = children.iter(); i.is_valid(); ++i)
            {
                if ((*i)->connect(branch))
                    return true;
            }
            
            return false;
        } // node::connect()
        

        void node::init(context *)
        {
        } // node::init()
    
        void node::draw(context *)
        {
        } // node::draw()
        
        void node::update(context *)
        {
        } // node::update()

        void node::cleanup(context *)
        {
        } // node::cleanup()

        bool node::handle_event(context *, sg_event &)
        {
            return false;
        } // node::handle_event()
        
        data::config_record *node::save() const
        {
            return 0;
        } // node::save()
        

        //

        void node::build_draw_list(node *cur, node *prev, context *c, const transform & modelview, pre_draw_rec & rec)
        {
            // collect parent
            if (cur->parent && cur->parent != prev)
            {
                transform to_parent = cur->orientation.transpose() * transform::translation_transform(cur->translation * (-1 * cur->parent->scale));
                build_draw_list(cur->parent, cur, c, modelview * to_parent, rec);
            }
            
            // collect me
            cur->modelview = modelview;

            light *l = dynamic_cast<light *>(cur);
            if (l)
            {
                rec.light_queue.push(l, utils::pos_in_eye_space(l).mag2());
            }
            else
            {
                gsgl::real_t pri = cur->get_priority(c);

                if (pri == node::NODE_DRAW_SOLID)
                    rec.solids.append(cur);
                else if (pri == node::NODE_DRAW_TRANSLUCENT)
                    rec.translucents.append(cur);
                else if (pri > node::NODE_DRAW_TRANSLUCENT)
                    rec.paint_queue.push(cur, pri);
            }

            // collect children (& scale)
            int i, len = cur->children.size();
            for (i = 0; i < len; ++i)
            {
                node *child = cur->children[i];

                if (child != prev && !(child->draw_flags & node::NODE_DUMMY_OBJECT))
                {
                    transform from_parent = transform::translation_transform(child->translation * cur->scale) * child->orientation;
                    build_draw_list(child, cur, c, modelview * from_parent, rec);
                }
            }
        } // build_draw_list()


        void node::pre_draw_scene(context *c, pre_draw_rec & rec)
        {
            rec.light_queue.clear();
            rec.paint_queue.clear();
            rec.solids.clear();
            rec.translucents.clear();

            build_draw_list(c->cam, 0, c, transform::IDENTITY, rec);
        } // node::pre_draw_scene()


        static const string RENDER_CATEGORY = L"render: ";

        static config_variable<platform::color> AMBIENT_LIGHT(L"scenegraph/draw/ambient_light", platform::color(0.15f, 0.15f, 0.15f, 1.0f));
        static config_variable<gsgl::real_t> LOCAL_CULL_DISTANCE(L"scenegraph/draw/local_cull_distance", 100000.0f);


        void node::draw_scene(context *c, pre_draw_rec & rec)
        {
            int i, len;

            // init viewport
            glViewport(0, 0, c->screen->get_width(), c->screen->get_height());                                      CHECK_GL_ERRORS();

            glPushAttrib(GL_ALL_ATTRIB_BITS);                                                                       CHECK_GL_ERRORS();
            glPushClientAttrib(GL_CLIENT_ALL_ATTRIB_BITS);                                                          CHECK_GL_ERRORS();

            // set up lights
            glLightModelfv(GL_LIGHT_MODEL_AMBIENT, AMBIENT_LIGHT.get_value().get_val());                                        CHECK_GL_ERRORS();

            int max_lights;
            glGetIntegerv(GL_MAX_LIGHTS, &max_lights);                                                              CHECK_GL_ERRORS();

            for (i = 0; i < max_lights; ++i)
                glDisable(GL_LIGHT0 + i); // light::bind() will call glEnable on valid lights...
            c->num_lights = 0;

            if (!(c->render_flags & context::RENDER_UNLIT))
            {
                len = rec.light_queue.size();
                for (i = 0; i < len && i < max_lights; ++i)
                {
                    light *l = rec.light_queue[i];

                    glMatrixMode(GL_MODELVIEW);                                                                         CHECK_GL_ERRORS();
                    glLoadMatrixf(l->get_modelview().ptr());
                    l->bind(GL_LIGHT0 + i);                                                                             CHECK_GL_ERRORS();
                }

                c->num_lights = i;
            }

            // draw distant nodes with painter's algo; they will set up their own projections
            len = rec.paint_queue.size();
            for (i = 0; i < len; ++i)
            {
                node *n = rec.paint_queue[i];
                n->draw_results = NODE_NO_DRAW_RESULTS;
             
                platform::budget_record br(RENDER_CATEGORY + n->get_type_name());

                glMatrixMode(GL_MODELVIEW);                                                                         CHECK_GL_ERRORS();
                glLoadMatrixf(n->get_modelview().ptr());
                
                if (utils::is_on_screen(n, c->cam->get_field_of_view(), c->screen->get_aspect_ratio(), vector::ZERO, n->max_extent() * n->scale))
                    n->draw(c);
                else
                    n->draw_results |= NODE_OFF_SCREEN;
            }

            // draw local nodes (these should not set up their own projections!)
            if (rec.solids.size() || rec.translucents.size())
            {
                // get maximum extent of local nodes
                gsgl::real_t max_ext = 0;
                gsgl::real_t min_ext = FLT_MAX;

                len = rec.solids.size();
                for (i = 0; i < len; ++i)
                {
                    node *n = rec.solids[i];
                    n->draw_results = NODE_NO_DRAW_RESULTS;

                    if (!utils::is_on_screen(n, c->cam->get_field_of_view(), c->screen->get_aspect_ratio(), vector::ZERO, n->max_extent()))
                    {
                        n->draw_results |= NODE_OFF_SCREEN;
                        continue;
                    }

                    gsgl::real_t e = utils::pos_in_eye_space(n).mag();
                    if (e > LOCAL_CULL_DISTANCE)
                    {
                        n->draw_results |= NODE_DISTANCE_CULLED;
                        continue;
                    }

                    gsgl::real_t x = n->max_extent() * n->scale;

                    if ((e+x) > max_ext)
                        max_ext = e+x;

                    gsgl::real_t m = max_val(e-x, 0.1f);
                    if (m < min_ext)
                        min_ext = m;
                }

                len = rec.translucents.size();
                for (i = 0; i < len; ++i)
                {
                    node *n = rec.translucents[i];
                    n->draw_results = NODE_NO_DRAW_RESULTS;

                    if (!utils::is_on_screen(n, c->cam->get_field_of_view(), c->screen->get_aspect_ratio(), vector::ZERO, n->max_extent()))
                    {
                        n->draw_results |= NODE_OFF_SCREEN;
                        continue;
                    }

                    gsgl::real_t e = utils::pos_in_eye_space(n).mag();
                    if (e > LOCAL_CULL_DISTANCE)
                    {
                        n->draw_results |= NODE_DISTANCE_CULLED;
                        continue;
                    }

                    gsgl::real_t x = n->max_extent() * n->scale;

                    if ((e+x) > max_ext)
                        max_ext = e+x;

                    gsgl::real_t m = max_val(e-x, 0.1f);
                    if (m < min_ext)
                        min_ext = m;
                }

                glClearDepth(1);                                                                                    CHECK_GL_ERRORS();
                glClear(GL_DEPTH_BUFFER_BIT);                                                                       CHECK_GL_ERRORS();
                glEnable(GL_DEPTH_TEST);                                                                            CHECK_GL_ERRORS();

                float near_plane = min_ext * 0.9f;
                float far_plane = max_ext * 1.1f;

                glMatrixMode(GL_PROJECTION);                                                                        CHECK_GL_ERRORS();
                glLoadIdentity();                                                                                   CHECK_GL_ERRORS();
                gluPerspective(c->cam->get_field_of_view(), c->screen->get_aspect_ratio(), near_plane, far_plane);  CHECK_GL_ERRORS();

                // draw solid objects
                len = rec.solids.size();
                for (i = 0; i < len; ++i)
                {
                    node *n = rec.solids[i];

                    if (n->draw_results & (NODE_OFF_SCREEN | NODE_DISTANCE_CULLED))
                        continue;

                    platform::budget_record br(RENDER_CATEGORY + n->get_type_name());

                    glMatrixMode(GL_MODELVIEW);
                    glLoadMatrixf(n->get_modelview().ptr());

                    n->draw(c);
                }

                // draw translucent objects
                glEnable(GL_BLEND);                                                                                 CHECK_GL_ERRORS();
                glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);                                                  CHECK_GL_ERRORS();

                len = rec.translucents.size();
                for (i = 0; i < len; ++i)
                {
                    node *n = rec.translucents[i];

                    if (n->draw_results & (NODE_OFF_SCREEN | NODE_DISTANCE_CULLED))
                        continue;

                    platform::budget_record br(RENDER_CATEGORY + n->get_type_name());

                    glMatrixMode(GL_MODELVIEW);
                    glLoadMatrixf(n->get_modelview().ptr());

                    n->draw(c);
                }
            }

            // we're done
            glPopClientAttrib();                                                                                    CHECK_GL_ERRORS();
            glPopAttrib();                                                                                          CHECK_GL_ERRORS();
        } // node::draw_scene()


    } // namespace scenegraph
    
} // namespace gsgl
