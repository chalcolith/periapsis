#ifndef GSGL_FRAMEWORK_PACKAGE_H
#define GSGL_FRAMEWORK_PACKAGE_H

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

#include "framework/framework.hpp"
#include "data/string.hpp"
#include "data/list.hpp"
#include "data/config.hpp"

namespace gsgl
{

    namespace framework
    {

        /// Stores information about scenery modules for the UI.
        class FRAMEWORK_API pkg_scenery
            : public framework_object
        {
            gsgl::string fname;
            gsgl::string scenery_name;
            gsgl::string description;

        public:
            pkg_scenery(const gsgl::string & fname);
            virtual ~pkg_scenery();

            const gsgl::string & get_fname() const { return fname; }
            const gsgl::string & get_scenery_name() const { return scenery_name; }
            const gsgl::string & get_description() const { return description; }
        }; // class pkg_scenery


        /// Stores information about vehicle classes for the UI.
        class FRAMEWORK_API pkg_vehicle
            : public framework_object
        {
            gsgl::string fname;
            gsgl::string class_name;
            gsgl::string vehicle_name;
            gsgl::string description;

        public:
            pkg_vehicle(const gsgl::string & fname);
            virtual ~pkg_vehicle();

            const gsgl::string & get_fname() const { return fname; }
            const gsgl::string & get_vehicle_name() const { return vehicle_name; }
            const gsgl::string & get_description() const { return description; }
            const gsgl::string & get_class_name() const { return class_name; }
        }; // class pkg_vehicle


        /// Stores information about shared library files for the UI.
        class FRAMEWORK_API pkg_library
            : public framework_object
        {
            gsgl::string fname;

        public:
            pkg_library(const gsgl::string & fname);
            virtual ~pkg_library();

            const gsgl::string & get_fname() const { return fname; }
        }; // struct pkg_library


        /// Stores information about library packages for the UI.
        class FRAMEWORK_API package
            : public framework_object
        {
            gsgl::string fname;
            gsgl::string package_name;
            gsgl::string description;

            gsgl::data::list<pkg_scenery *> loaded_sceneries;
            gsgl::data::list<pkg_vehicle *> loaded_vehicles;
            gsgl::data::list<pkg_library *> loaded_libraries;

        public:
            package(const gsgl::string & fname);
            virtual ~package();

            const gsgl::string & get_fname() const { return fname; }
            const gsgl::string & get_package_name() const { return package_name; }
            const gsgl::string & get_description() const { return description; }

            const gsgl::data::list<pkg_scenery *> & get_loaded_sceneries() const { return loaded_sceneries; }
            const gsgl::data::list<pkg_vehicle *> & get_loaded_vehicles() const { return loaded_vehicles; }
            const gsgl::data::list<pkg_library *> & get_loaded_libraries() const { return loaded_libraries; }
        }; // class package

    } // namespace framework

} // namespace gsgl

#endif
