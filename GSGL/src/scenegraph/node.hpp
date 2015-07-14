#ifndef GSGL_SG_NODE_H
#define GSGL_SG_NODE_H

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

#include "scenegraph/scenegraph.hpp"
#include "scenegraph/context.hpp"
#include "scenegraph/event.hpp"

#include "data/array.hpp"
#include "data/list.hpp"
#include "data/string.hpp"
#include "data/pqueue.hpp"
#include "data/config.hpp"
#include "data/broker.hpp"

#include "math/vector.hpp"
#include "math/transform.hpp"

namespace gsgl
{

    namespace scenegraph
    {

        class light;
        class node;

        /// Stores the nodes in the scene graph needed to calculate the transform from src's frame to dest's.
        class SCENEGRAPH_API node_relative_path
        {
            data::simple_array<node *> to_cur;
            data::simple_array<node *> from_ref;

            math::transform relative_transform;

        public:
            node_relative_path(node *reference, node *cur);

            const math::transform & get_relative_transform() const { return relative_transform; }

            void initialize(node *reference, node *cur);
            const math::transform & calc_relative_transform(bool include_translation);
        }; // class node_relative_path

    
        /// Base class for nodes in the scene graph.
        class SCENEGRAPH_API node
            : public scenegraph_object, public data::brokered_object
        {
            node *parent;
            data::simple_array<node *> children;
            
            string name, parent_name;

            gsgl::real_t    scale;       ///< The scale of the node in meters per unit.

            math::vector    translation; ///< The translation of the node's frame in its parent's frame and scale.
            math::transform orientation; ///< Multiply this by a position in the node's frame to yield the position in the parent's frame.  Should never contain a translation component!
            
            math::transform modelview;   ///< The modelview matrix the node should be drawn with.

            gsgl::flags_t draw_flags;   ///< Flags that control how the node should be drawn (these stay fairly constant).
            gsgl::flags_t draw_results; ///< Flags that indicate what happened to the node when it was drawn (these may change frame to frame).

        public:

            /// Creates a node with a given name and parent.
            node(const gsgl::string & name, node *parent);

            /// Creates a node from a config_record record.
            node(const data::config_record & conf);

            virtual ~node();
            
            /// \name Accessors.
            /// @{

            const node *get_parent() const;
            node * & get_parent();
            data::simple_array<node *> & get_children();

            const string & get_name() const;
            string & get_name();
            string & get_parent_name();
            
            inline gsgl::real_t    & get_scale()       { return scale; }

            inline math::vector    & get_translation() { return translation; }
            inline math::transform & get_orientation() { return orientation; }

            inline math::transform & get_modelview()   { return modelview; }
            
            /// @}

            /// \name Scene Graph Functionality.
            /// @{

            /// Add a child node.  Also sets the child's parent to \c this.
            void add_child(node *child);

            /// Removes the node from the scene graph (does not delete it!).
            void detach();
            
            /// If this is called on the root of a scene graph, it will insert the branch node in the appropriate place in the graph, or return false.
            bool connect(node *branch);


            /// Flags that control node drawing.
            enum node_draw_flags
            {
                NODE_NO_DRAW_FLAGS    = 0,
                NODE_NO_FRUSTUM_CHECK = 1 << 0, ///< Don't perform a frustum check when drawing this node.
                NODE_DRAW_UNLIT       = 1 << 1  ///< This node should not be lit.
            };

            /// \return The node's draw flags (a bitset using node_draw_flags).
            const gsgl::flags_t & get_draw_flags() const { return draw_flags; }

            /// \return The node's draw flags (a bitset using node_draw_flags).
            gsgl::flags_t & get_draw_flags() { return draw_flags; }


            /// Flags that indicate what happened when the node was drawn.
            enum node_draw_results
            {
                NODE_NO_DRAW_RESULTS = 0,
                NODE_OFF_SCREEN      = 1 << 0, ///< The node was not drawn because it was out-of-frame.
                NODE_DREW_POINT      = 1 << 1, ///< The node was drawn as a point.
                NODE_DISTANCE_CULLED = 1 << 2  ///< The node was not drawn due to distance.
            };

            /// \return The node's draw results (a bitset using node_draw_results).
            const gsgl::flags_t & get_draw_results() const { return draw_results; }

            /// \return The node's draw results (a bitset using node_draw_results).
            gsgl::flags_t & get_draw_results() { return draw_results; }


            /// Contains information about the current drawing pass.
            struct pre_draw_rec
            {
                data::pqueue<light *, gsgl::real_t> light_queue;
                data::pqueue<node *, gsgl::real_t> paint_queue;
                data::simple_array<node *> solids;
                data::simple_array<node *> translucents;
            }; // struct pre_draw_rec

            /// Collect information about the scene to draw.  Unsafe to call while update is being called in the tree.
            static void pre_draw_scene(gsgl::scenegraph::simulation_context *sim_context, gsgl::scenegraph::drawing_context *draw_context, pre_draw_rec & rec);

            /// Draws a scene.  Safe to call while update is also being called in the tree.
            static void draw_scene(gsgl::scenegraph::simulation_context *sim_context, gsgl::scenegraph::drawing_context *draw_context, pre_draw_rec & rec);

            /// @}

            /// \name Node Life Cycle.
            /// @{

            /// Called when the simulation is created.  The node's modelview matrix is invalid at this point.
            virtual void init(const gsgl::scenegraph::simulation_context *sim_context);
            
            /// Called to draw the node.  The node's modelview matrix is in the correct state for drawing, and already loaded into the OpenGL modelview matrix.
            virtual void draw(const gsgl::scenegraph::simulation_context *sim_context, const gsgl::scenegraph::drawing_context *draw_context);
            
            /// Called from the root of the world-tree up to update the node's state.  The node's modelview matrix is that of the previously-drawn frame.
            virtual void update(const gsgl::scenegraph::simulation_context *sim_context);

            /// Called when the simulation is done.
            virtual void cleanup(const gsgl::scenegraph::simulation_context *sim_context);

            /// Called with events.
            virtual bool handle_event(const gsgl::scenegraph::simulation_context *sim_context, sg_event & e);
            
            /// Called to save the node to a config_record structure.
            virtual void save(gsgl::data::config_record & rec_to_save) const;
            
            /// @}

            /// \name Drawing Information Functions.
            /// @{

            static const gsgl::real_t NODE_DRAW_IGNORE;
            static const gsgl::real_t NODE_DRAW_SOLID;
            static const gsgl::real_t NODE_DRAW_TRANSLUCENT;
            static const gsgl::real_t NODE_DRAW_FIRST;

            /// Called to determine the draw priority of the node.
            /// A value of NODE_DRAW_IGNORE means don't draw (invisible nodes or nodes that are drawn by their parents -- the modelview matrix is still built).
            /// A value of NODE_DRAW_SOLID means the object is solid and should be drawn after the painter's algorithm.
            /// A value of NODE_DRAW_TRANSLUCENT means the object is translucent and should be drawn after solid objects.
            /// A value > NODE_DRAW_TRANSLUCENT is interpreted as the distance to the object.  Objects that return > 2.0 are drawn with a painter's algorithm.
            virtual gsgl::real_t draw_priority(const gsgl::scenegraph::simulation_context *sim_context, const gsgl::scenegraph::drawing_context *draw_context);

            /// Should return the maximum extent of the object (in the node's coordinates, i.e. not scaled to meters).
            virtual gsgl::real_t view_radius() const;
            
            virtual gsgl::real_t default_view_distance() const;
            virtual gsgl::real_t minimum_view_distance() const;
            /// @}

        private:
            static void build_draw_list(node *cur, node *prev, simulation_context *sim_context, drawing_context *draw_context, const math::transform & modelview, pre_draw_rec &);

            static void draw_scene_lighting(simulation_context *sim_context, drawing_context *draw_context, pre_draw_rec & rec);
            static void draw_distant_nodes(simulation_context *sim_context, drawing_context *draw_context, pre_draw_rec & rec);
            static void get_local_info(simulation_context *sim_context, drawing_context *draw_context, data::simple_array<node *> &, float & near, float & far);
            static void draw_local_objects(simulation_context *sim_context, drawing_context *draw_context, data::simple_array<node *> &);
        }; // class node
        
    } // namespace scenegraph
    
} // namespace gsgl

#endif
