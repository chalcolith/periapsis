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

        broker *broker::instance;


		broker::broker()
			: singleton<broker>()
		{
            gsgl::log(L"broker: creating global broker");
		} // broker::broker()


		broker::~broker()
		{
            gsgl::log(L"broker: destroying global broker");
		} // broker::~broker()
        

        bool broker::has_object(const string & type_name)
        {
            if (instance)
            {
                return instance->creators.contains_index(type_name);
            }
            else
            {
                throw internal_exception(__FILE__, __LINE__, L"You must declare an instance of the global broker object!");
            }
        } // broker::has_object()
        

        brokered_object *broker::create_object(const string & type_name, const config_record & obj_conf)
        {
            if (instance)
            {
                gsgl::log(string(L"broker: creating object of type ") + type_name);

                broker_creator::creator_ft ccc = instance->creators[type_name];

                if (ccc)
                {
                    brokered_object *result = ccc(obj_conf);
                    if (result)
                        result->type_name = type_name;
                    return result;
                }
                else
                {
                    gsgl::log(string(L"broker: UNABLE TO CREATE OBJECT OF TYPE ") + type_name);
                    throw runtime_exception(L"%ls: unknown object type!", type_name.w_string());
                }
            }
            else
            {
                throw internal_exception(__FILE__, __LINE__, L"You must declare an instance of the global broker object!");
            }
        } // broker::create_object()


        //

        broker_creator::broker_creator(const string & type_name, creator_ft create_func)
            : type_name(type_name), create_func(create_func)
        {
            if (type_name.is_empty())
                throw internal_exception(__FILE__, __LINE__, L"You cannot declare a brokered object creator with a blank type name!");

            if (!create_func)
                throw internal_exception(__FILE__, __LINE__, L"You cannot declare a brokered object creator with a null creator function!");

            if (!broker::instance)
                throw internal_exception(__FILE__, __LINE__, L"You must declare an instance of the global broker object!");

            if (broker::instance->creators.contains_index(type_name))
                throw internal_exception(__FILE__, __LINE__, L"You cannot declare more than one creator for type '%ls'.", type_name.w_string());

            broker::instance->creators[type_name] = create_func;
        } // broker_creator::broker_creator()


        broker_creator::~broker_creator()
        {
            if (broker::instance)
                broker::instance->creators.remove(type_name);
        } // broker_creator::~broker_creator()


    } // namespace data

} // namespace gsgl
