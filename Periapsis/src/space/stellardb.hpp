#ifndef PERIAPSIS_SPACE_STELLAR_DB_H
#define PERIAPSIS_SPACE_STELLAR_DB_H

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

#include "space/space.hpp"
#include "data/array.hpp"
#include "data/broker.hpp"
#include "data/string.hpp"
#include "scenegraph/node.hpp"
#include "platform/vbuffer.hpp"
#include "platform/shader.hpp"

namespace periapsis
{

    namespace space
    {

        class SPACE_API stellar_db
            : public gsgl::scenegraph::node
        {
            int num_stars;

            float nearest_distance, farthest_distance;

            gsgl::platform::vertex_buffer vertices;
            gsgl::platform::shader_program star_shader;
            gsgl::platform::shader_uniform<float> *uniform_farthest_distance;

            gsgl::data::simple_array<gsgl::real_t> star_name_vertices;
            gsgl::data::simple_array<gsgl::string *> star_names;

        public:
            stellar_db(const gsgl::data::config_record & conf);
            virtual ~stellar_db();

            virtual gsgl::real_t get_priority(gsgl::scenegraph::context *);
            virtual void init(gsgl::scenegraph::context *);
            virtual void draw(gsgl::scenegraph::context *);
            virtual void cleanup(gsgl::scenegraph::context *);

            BROKER_DECLARE_CREATOR(periapsis::space::stellar_db);

        private:
            void load_db(const gsgl::string & fname);
            void add_star_name(const gsgl::math::vector & pos, const gsgl::string & name);
        }; // class stellar_db

    } // namespace space

} // namespace periapsis

#endif
