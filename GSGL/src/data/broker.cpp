//
// $Id: broker.cpp 314 2008-03-01 16:33:47Z Gordon $
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

#include "data/broker.hpp"
#include "data/log.hpp"
#include "data/exception.hpp"
#include "data/string.hpp"
#include "data/config.hpp"

#include <typeinfo>

#ifdef WIN32
// disable warning about regster instance
#pragma warning (disable:4661) 
#endif


namespace gsgl
{

    namespace data
    {

        const gsgl::string & brokered_object::get_type_name() const
        {
            if (type_name.is_empty())
            {
                brokered_object *ptr = const_cast<brokered_object *>(this);
                const type_info & info = typeid(*ptr);
                type_name = string(info.name());
            }

            return type_name;
        } // brokered_object::get_type_name()
        

        //

		broker::broker()
			: global_register<broker_creator, gsgl::data::dictionary<broker_creator *, gsgl::string> >()
		{
            gsgl::log(L"broker: creating global broker");
		} // broker::broker()


		broker::~broker()
		{
            gsgl::log(L"broker: destroying global broker");
		} // broker::~broker()
        

        bool broker::has_object(const string & type_name)
        {
            return registered_resources.contains_index(type_name);
        } // broker::has_object()
        

        brokered_object *broker::create_object(const string & type_name, const config_record & obj_conf)
        {
            gsgl::log(string(L"broker: creating object of type ") + type_name);

            broker_creator *bc = registered_resources[type_name];

            if (bc)
            {
                broker_creator::creator_ft ccc = bc->get_create_func(); assert(ccc);
                brokered_object *result = ccc(obj_conf);
                if (result)
                    result->type_name = type_name;
                return result;
            }
            else
            {
                gsgl::log(string(L"broker: UNABLE TO CREATE OBJECT OF TYPE ") + type_name);
                throw runtime_exception(L"%ls: unknown object type", type_name.w_string());
            }
        } // broker::create_object()


        broker *broker::global_instance()
        {
            broker *temp = dynamic_cast<broker *>(singleton<global_register<broker_creator, gsgl::data::dictionary<broker_creator *, gsgl::string> > >::global_instance());
            if (!temp)
                temp = new broker();
            return temp;
        } // broker::global_instance()


        //

        broker_creator::broker_creator(const string & type_name, creator_ft create_func)
            : type_name(type_name), create_func(create_func)
        {
            assert(!type_name.is_empty());
            assert(create_func);
            broker::global_instance()->register_resource(this);
        } // broker_creator::broker_creator()


        broker_creator::~broker_creator()
        {
            broker::global_instance()->unregister_resource(this);
        } // broker_creator::~broker_creator()


        // global broker instance
        global_register<broker_creator, gsgl::data::dictionary<broker_creator *, gsgl::string> > *data::broker::instance = 0;

    } // namespace data


} // namespace gsgl
