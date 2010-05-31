#ifndef GSGL_FRAMEWORK_BROKER_H
#define GSGL_FRAMEWORK_BROKER_H

//
// $Id: broker.hpp 15 2008-07-20 17:36:39Z kulibali $
//
// Copyright (c) 2008-2010, The Periapsis Project. All rights reserved. 
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

#include "base.hpp"
#include "singleton.hpp"
#include "dictionary.hpp"
#include "string.hpp"

namespace gsgl
{

    namespace data
    {

        class BASE_API brokered_object
        {
            mutable gsgl::string type_name;
            friend class broker;
        public:
            virtual const gsgl::string & get_type_name() const;
        }; // class brokered_object


        class config_record;

        /// This is a utility class used by the broker creator macros to register object types.
        class BASE_API broker_creator
        {
        public:
            /// Function type for creating objects.
            typedef brokered_object * (*creator_ft)(const config_record & obj_conf);
            
        private:
            const gsgl::string type_name;
            const creator_ft create_func;

        public:
            broker_creator(const gsgl::string & type_name, creator_ft create_func);
            ~broker_creator();

            const gsgl::string & get_type_name() const { return type_name; }
            creator_ft get_create_func() const { return create_func; }
        }; // class broker_creator


        /// This class serves to broker plugin object creation and config_recorduration.
        /// In order to make a class brokerable, simply use the BROKER_DECLARE_CREATOR() and BROKER_DEFINE_CREATOR() macros.
        /// These will create the global broker if necessary, and automatically register the class as creatable.
        /// The broker will delete itself when its last creator goes out of scope.

        class BASE_API broker
            : public data::singleton<broker>
        {
            data::dictionary<broker_creator::creator_ft, gsgl::string> creators;

        public:
			broker();
			virtual ~broker();

            static bool has_object(const gsgl::string & type_name);
            static brokered_object *create_object(const gsgl::string & type_name, const config_record & conf);

            friend class broker_creator;
        }; // class broker
                

    } // namespace data
    
} // namespace gsgl

//

#define BROKER_DECLARE_CREATOR(name) static gsgl::data::brokered_object *_create_(const gsgl::data::config_record & obj_conf) { return new name(obj_conf); } static gsgl::data::broker_creator _creator_

#define BROKER_DEFINE_CREATOR(name) gsgl::data::broker_creator name::_creator_(gsgl::string(#name), &name::_create_)

//

#endif
