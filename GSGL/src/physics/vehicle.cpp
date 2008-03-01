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

#include "physics/vehicle.hpp"
#include "physics/vehicle_module.hpp"

#include "data/broker.hpp"
#include "math/vector.hpp"
#include "math/transform.hpp"
#include "math/quaternion.hpp"
#include "scenegraph/utils.hpp"
#include "platform/vbuffer.hpp"

namespace gsgl
{
    using namespace data;
    using namespace math;
    using namespace scenegraph;
    using namespace platform;

    namespace physics
    {

        static int num_vehicles = 0;


        vehicle::vehicle(const config_record & obj_config)
            : rigid_body(obj_config)
        {
            get_name() = string::format(L"vehicle_%d (%ls)", num_vehicles++, get_name().w_string());

            for (list<config_record>::const_iterator i = obj_config.get_children().iter(); i.is_valid(); ++i)
            {
                if (i->get_name() == L"module")
                {
                    string fname = (*i)[L"file"];
                    if (fname.is_empty())
                        throw runtime_exception(L"%ls: vehicle module withfile name.", i->get_file().get_full_path().w_string());

                    config_record vm_rec(obj_config.get_directory().get_full_path() + fname);
                    string classname = vm_rec.get_name();
                    if (classname.is_empty())
                        throw runtime_exception(L"%ls: vehicle module withclass name.", fname.w_string());

                    vehicle_module *vm = dynamic_cast<vehicle_module *>(broker::global_instance()->create_object(classname, vm_rec));
                    assert(vm);

                    vm->get_name() = string::format(L"%ls: %ls", get_name().w_string(), vm->get_name().w_string());
                    modules.append(vm);
                    add_child(vm);

                    string pos_str = (*i)[L"position"];
                    if (!pos_str.is_empty())
                        vm->get_translation() = vector(pos_str);

                    string qrot_str = (*i)[L"qrotation"];
                    if (!qrot_str.is_empty())
                        vm->get_orientation() = transform(quaternion(qrot_str));
                }
            }
        } // vehicle::vehicle()


        vehicle::~vehicle()
        {
        } // vehicle::~vehicle()


        //

        gsgl::real_t vehicle::get_priority(gsgl::scenegraph::context *)
        {
            return node::NODE_DRAW_IGNORE;
        } // vehicle::get_priority()

            
        gsgl::real_t vehicle::default_view_distance() const
        {
            return minimum_view_distance() * 2.0f;
        } // vehicle::default_view_distance()


        gsgl::real_t vehicle::minimum_view_distance() const
        {
            return utils::greatest_extent(this);
        } // vehicle::minimum_view_distance()


        void vehicle::init(gsgl::scenegraph::context *c)
        {
            rigid_body::init(c);
        } // vehicle::init()


        void vehicle::draw(gsgl::scenegraph::context *c)
        {
            rigid_body::draw(c);
        } // vehicle::draw()


        void vehicle::update(gsgl::scenegraph::context *c)
        {
            rigid_body::update(c);
        } // vehicle::update()


        void vehicle::cleanup(gsgl::scenegraph::context *c)
        {
            rigid_body::cleanup(c);
        } // vehicle::cleanup()


        static void calculate_inertia_subexpressions(gsgl::real_t w0, gsgl::real_t w1, gsgl::real_t w2,
                                                     gsgl::real_t & f1, gsgl::real_t & f2, gsgl::real_t & f3,
                                                     gsgl::real_t & g0, gsgl::real_t & g1, gsgl::real_t & g2)
        {
            gsgl::real_t temp0 = w0 + w1;
            f1 = temp0 + w2;
            gsgl::real_t temp1 = w0 * w0;
            gsgl::real_t temp2 = temp1 + w1 * temp0;
            f2 = temp2 + w2 * f1;
            f3 = w0 * temp1 + w1 * temp2 + w2 * f2;
            g0 = f2 + w0 * (f1 + w0);
            g1 = f2 + w1 * (f1 + w1);
            g2 = f2 + w2 * (f1 + w2);
        } // calculate_inertia_subexpressions()


        transform vehicle::calculate_inertia_tensor(gsgl::math::vector & center_of_mass)
        {
            transform ibody;

            // copy triangles from all modules & get mass
            simple_array<gsgl::real_t> triangles;
            gsgl::real_t total_mass = 0;

            for (simple_array<vehicle_module *>::iterator i = modules.iter(); i.is_valid(); ++i)
            {
                total_mass += (*i)->get_module_mass();

                for (simple_array<model *>::const_iterator j = (*i)->get_models().iter(); j.is_valid(); ++j)
                {
                    for (list<vertex_buffer *>::iterator k = (*j)->get_inertial_triangles().iter(); k.is_valid(); ++k)
                    {
                        triangles.append((*k)->get_buffer());
                    }
                }
            }

            // sanity checks
            if ((triangles.size() % 9) != 0)
                throw internal_exception(__FILE__, __LINE__, L"vertices for inertia tensor calculation are not divisible by 9");

            if (total_mass <= 0)
                throw runtime_exception(L"Vehicle %ls has a zero or negative total mass!", get_name().w_string());

            mass = total_mass;

            // Eberly S2.5
            int num = triangles.size() / 9;
            gsgl::real_t integral[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
            gsgl::real_t polyhedron_mass = 0;

            for (int i = 0; i < num; ++i)
            {
                // get vertices of triangle
                gsgl::real_t x0 = triangles[i*9 + 0];
                gsgl::real_t y0 = triangles[i*9 + 1];
                gsgl::real_t z0 = triangles[i*9 + 2];

                gsgl::real_t x1 = triangles[i*9 + 3];
                gsgl::real_t y1 = triangles[i*9 + 4];
                gsgl::real_t z1 = triangles[i*9 + 5];

                gsgl::real_t x2 = triangles[i*9 + 6];
                gsgl::real_t y2 = triangles[i*9 + 7];
                gsgl::real_t z2 = triangles[i*9 + 8];

                // get edges and cross products of edges
                gsgl::real_t a1 = x1 - x0;
                gsgl::real_t b1 = y1 - y0;
                gsgl::real_t c1 = z1 - z0;

                gsgl::real_t a2 = x2 - x0;
                gsgl::real_t b2 = y2 - y0;
                gsgl::real_t c2 = z2 - z0;

                gsgl::real_t d0 = b1*c2 - b2*c1;
                gsgl::real_t d1 = a2*c1 - a1*c2;
                gsgl::real_t d2 = a1*b2 - a2*b1;

                gsgl::real_t f1x, f1y, f1z;
                gsgl::real_t f2x, f2y, f2z;
                gsgl::real_t f3x, f3y, f3z;

                gsgl::real_t g0x, g0y, g0z;
                gsgl::real_t g1x, g1y, g1z;
                gsgl::real_t g2x, g2y, g2z;

                calculate_inertia_subexpressions(x0, x1, x2, f1x, f2x, f3x, g0x, g1x, g2x);
                calculate_inertia_subexpressions(y0, y1, y2, f1y, f2y, f3y, g0y, g1y, g2y);
                calculate_inertia_subexpressions(z0, z1, z2, f1z, f2z, f3z, g0z, g1z, g2z);

                // update integrals
                integral[0] += d0 * f1x;
                integral[1] += d0 * f2x;
                integral[2] += d1 * f2y;
                integral[3] += d2 * f2z;
                integral[4] += d0 * f3x;
                integral[5] += d1 * f3y;
                integral[6] += d2 * f3z;
                integral[7] += d0 * (y0*g0x + y1*g1x + y2*g2x);
                integral[8] += d1 * (z0*g0y + z1*g1y + z2*g2y);
                integral[9] += d2 * (x0*g0z + x1*g1z + x2*g2z);
            }

            integral[0] /= 6.0;
            integral[1] /= 24.0;
            integral[2] /= 24.0;
            integral[3] /= 24.0;
            integral[4] /= 60.0;
            integral[5] /= 60.0;
            integral[6] /= 60.0;
            integral[7] /= 120.0;
            integral[8] /= 120.0;
            integral[9] /= 120.0;

            polyhedron_mass = integral[0];

            center_of_mass.get_x() = integral[1] / polyhedron_mass;
            center_of_mass.get_y() = integral[2] / polyhedron_mass;
            center_of_mass.get_z() = integral[3] / polyhedron_mass;

            // inertia relative to world
            ibody = transform::IDENTITY;

            ibody[0]  = integral[5] + integral[6];
            ibody[5]  = integral[4] + integral[6];
            ibody[10] = integral[4] + integral[5];

            ibody[1]  = -integral[7];
            ibody[6]  = -integral[8];
            ibody[2]  = -integral[9];

            // inertia relative to centre of mass
            ibody[0]  -= polyhedron_mass * (center_of_mass.get_y() * center_of_mass.get_y() + center_of_mass.get_z() * center_of_mass.get_z());
            ibody[5]  -= polyhedron_mass * (center_of_mass.get_z() * center_of_mass.get_z() + center_of_mass.get_x() * center_of_mass.get_x());
            ibody[10] -= polyhedron_mass * (center_of_mass.get_x() * center_of_mass.get_x() + center_of_mass.get_y() * center_of_mass.get_y());

            ibody[1]  += polyhedron_mass * center_of_mass.get_x() * center_of_mass.get_y();
            ibody[6]  += polyhedron_mass * center_of_mass.get_y() * center_of_mass.get_z();
            ibody[2]  += polyhedron_mass * center_of_mass.get_z() * center_of_mass.get_x();

            // normalize mass
            gsgl::real_t mass_ratio = total_mass / polyhedron_mass;
            ibody[0]  *= mass_ratio;
            ibody[5]  *= mass_ratio;
            ibody[10] *= mass_ratio;
            ibody[1]  *= mass_ratio;
            ibody[6]  *= mass_ratio;
            ibody[2]  *= mass_ratio;

            //
            return ibody;
        } // vehicle::calculate_inertia_tensor()


    } // namespace physics

} // namespace physics
