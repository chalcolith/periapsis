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

#include "framework/package.hpp"
#include "framework/application.hpp"

#include "platform/lowlevel.hpp"

namespace gsgl
{

    using namespace data;
    using namespace io;

    namespace framework
    {

        pkg_scenery::pkg_scenery(const string & fname)
            : framework_object(), fname(fname)
        {
            config_record scenery_config(fname);

            if (scenery_config.get_name() != L"scenery")
                throw runtime_exception(L"Invalid scenery specification %ls", fname.w_string());

            scenery_name = scenery_config[L"name"];
            description = scenery_config.get_child(L"description").get_text();
        } // pkg_scenery::pkg_scenery()


        pkg_scenery::~pkg_scenery()
        {
        } // pkg_scenery::~pkg_scenery()


        //////////////////////////////////////////////////////////////

        pkg_vehicle::pkg_vehicle(const string & fname)
            : framework_object(), fname(fname)
        {
            config_record vehicle_config(fname);

            if (vehicle_config.get_name() != L"vehicle")
                throw runtime_exception(L"Invalid vehicle specification %ls", fname.w_string());

            vehicle_name = vehicle_config[L"name"];
            class_name = vehicle_config[L"class"];
            description = vehicle_config.get_child(L"description").get_text();
        } // pkg_vehicle::pkg_vehicle()


        pkg_vehicle::~pkg_vehicle()
        {
        } // pkg_vehicle::~pkg_vehicle()


        //////////////////////////////////////////////////////////////


#ifdef WIN32
        static const string LIB_SFX = L".dll";
#else
        static const string LIB_SFX = L".so";
#endif

        pkg_library::pkg_library(const string & fname)
            : framework_object(), fname(fname)
        {
            void *lib = 0;
#ifdef WIN32
            if (!(lib = LoadLibraryA(fname.c_string())))
                throw runtime_exception(L"Unable to load library %ls.", fname.w_string());
#else
            if (!(lib = dlopen(fname.c_string(), DLOPEN_FLAG)))
                throw runtime_exception(L"Unable to load library %ls: %hs", fname.w_string(), dlerror());
#endif
        }; // pkg_library()


        pkg_library::~pkg_library()
        {
        } // pkg_library::~pkg_library()

        //////////////////////////////////////////////////////////////

        package::package(const string & fname)
            : framework_object(), fname(fname)
        {
            // load package configuration
            config_record app_config(fname);

            if (app_config.get_name() != L"package")
                throw runtime_exception(L"Invalid package specification %ls", fname.w_string());

            package_name = app_config[L"name"];
            description = app_config.get_child(L"description").get_text();

            // load scenery configuration
            config_record & scenery_config = app_config.get_child(L"sceneries");
            for (list<config_record>::iterator i = scenery_config.get_children().iter(); i.is_valid(); ++i)
            {
                if (i->get_name() == L"scenery")
                {
                    const string & fname = i->get_attribute(L"filename");

                    if (!fname.is_empty())
                        loaded_sceneries.append(new pkg_scenery(app_config.get_directory().get_full_path() + fname));
                }
            }

            config_record & vehicle_config = app_config.get_child(L"vehicles");
            for (list<config_record>::iterator i = vehicle_config.get_children().iter(); i.is_valid(); ++i)
            {
                if (i->get_name() == L"vehicle")
                {
                    const string & fname = i->get_attribute(L"filename");

                    if (!fname.is_empty())
                        loaded_vehicles.append(new pkg_vehicle(app_config.get_directory().get_full_path() + fname));
                }
            }

            config_record & library_config = app_config.get_child(L"libraries");
            for (list<config_record>::iterator i = library_config.get_children().iter(); i.is_valid(); ++i)
            {
                if (i->get_name() == L"library")
                {
                    string fname = i->get_attribute(L"basename");

                    if (!fname.is_empty())
                    {
                        fname = app_config.get_directory().get_full_path() + fname + LIB_SFX;
                        loaded_libraries.append(new pkg_library(fname));
                    }
                }
                else if (i->get_name() == L"default_library")
                {
                    string fname = i->get_attribute(L"basename");

                    if (!fname.is_empty())
                    {
                        fname = application::PROGRAM_PATH + fname + LIB_SFX;
                        const wchar_t *str = fname.w_string();
                        loaded_libraries.append(new pkg_library(fname));
                    }
                }
            }
        } // package::package()

        
        package::~package()
        {
            for (list<pkg_library *>::iterator i = loaded_libraries.iter(); i.is_valid(); ++i)
            {
                delete *i;
            }
        } // package::~package()


    } // namespace framework

} // namespace gsgl
