#ifndef GSGL_DATA_config_record_H
#define GSGL_DATA_config_record_H

//
// $Id: config.hpp 17 2008-07-28 04:17:44Z kulibali $
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

#include "data.hpp"
#include "pointer.hpp"
#include "string.hpp"
#include "list.hpp"
#include "dictionary.hpp"
#include "file.hpp"
#include "directory.hpp"

namespace gsgl
{

    namespace data
    {

        class BASE_API global_config;


        /// Stores seralizable configuration information.
        /// This class represents a node in a simple XML data file format.
        /// It contains a list of its children and a string of data.
        /// It also stores the file and directory that the data was stored in, purely for error reporting purposes.
        ///
        /// \todo Replace this with a real XML library.
        
        class BASE_API config_record
            : public data_object
        {
            config_record *parent;
            gsgl::data::list<config_record> children;
            
            gsgl::string name;
            gsgl::string text;
            dictionary<gsgl::string, gsgl::string> attributes;

            friend class global_config;
            global_config *associated_var; ///< Many records may point to the same variable, but the variable only knows about one (the global config).
            
            data::shared_pointer<io::file> f; ///< Shared between all the children.
            int line_number;
            
        public:
            config_record();
            config_record(const config_record &);
            
            /// Creates a config_record object tree from a file.
            explicit config_record(const gsgl::string & fname);

            config_record & operator= (const config_record &);
            
            virtual ~config_record();
            

            /// Comparison function for collections of config records.
            bool operator== (const config_record &) const;


            /// \name Accessors
            /// \{

            /// \return The file that the config_record object resides in.
            const io::file & get_file() const;

            const int get_line_number() const;
            
            /// \return The directory that the config_record object resides in.
            const io::directory & get_directory() const;
            
            /// \return The name of the node.
            const gsgl::string & get_name() const;
            gsgl::string & get_name();
            
            /// \return The text of the node.
            const gsgl::string & get_text() const;
            gsgl::string & get_text();
            
            /// \return The value of the first attribute with the given name.
            const gsgl::string & get_attribute(const gsgl::string &) const;
            gsgl::string & get_attribute(const gsgl::string &);
            
            /// \return The value of the first attribute with the given name.
            inline const string & operator[] (const gsgl::string & name) const
            {
                return get_attribute(name);
            }

            inline string & operator[] (const gsgl::string & name)
            {
                return get_attribute(name);
            }
            
            /// \return The collection of the node's attributes.
            const dictionary<gsgl::string, gsgl::string> & get_attributes() const;
            dictionary<gsgl::string, gsgl::string> & get_attributes();
            
            /// \return true if the config node contains a child with the given name.
            bool contains_child(const gsgl::string & path) const;

            /// \return The child with the given path, separated by '/'.
            const config_record & get_child(const gsgl::string & path) const;
            config_record & get_child(const gsgl::string & path);
            
            /// \return A collection of the node's children.
            const list<config_record> & get_children() const;
            list<config_record> & get_children();
            /// \}

            /// Overrides the contents of this config record (and any associated config variables) with the values in the given config record.
            void override_with(const config_record & cr);

            /// Saves the config record back to its original file.
            void save();

        private:
            config_record *find_child(gsgl::data::list<gsgl::string>::iterator & pos_in_path, bool create);

            void to_stream(io::text_stream &) const;            
            void from_stream(io::text_stream &);

            void to_stream(io::text_stream &, int indent) const;
            void from_stream(io::text_stream &, int & line);
        }; // class config_record
        

        //////////////////////////////////////////////////////////////

        /// Base class for global configuration variables.
        /// The name is slightly unusual, but it was chosen so the static functions sound natural, e.g. \c global_config::override().
        class BASE_API global_config
        {
            friend class config_record;

        protected:
            static config_record global_config_vars;
            config_record & associated_record;

            global_config(const gsgl::string & path);
            virtual ~global_config();

            virtual void assign_from_string(const gsgl::string &) = 0; ///< Change the value of the config variable based on a given string.
            virtual gsgl::string get_string() const = 0;               ///< Get a string representation of the value of the variable.

        public:
            /// Returns a reference to the global config record that contains entries for all config variables.
            /// Note that not all entries may be present if package DLLs have not yet been loaded.
            static const config_record & get_config() { return global_config_vars; }

            /// Override the global config vars with the data in the given config record.
            static void override_with(const config_record &);

            /// Save the global config to a file.
            static void save(const gsgl::string & fname);
        }; // class global_config


        //////////////////////////////////////////

        template <typename T>
        class config_variable
            : public global_config
        {
            T value;
        public:
            config_variable(const gsgl::string & path, const T & value = T()) 
                : global_config(path), value(value) 
            {
                // if the record already has text in it, override our value
                if (!associated_record.get_text().is_empty())
                    assign_from_string(associated_record.get_text());
                else // otherwise, store our value
                    associated_record.get_text() = get_string();
            }

        private:
            /// You cannot copy config variables.
            config_variable(const config_variable & cv) : global_config(), value(cv.value) {}

            /// You cannot copy config variables.
            config_variable & operator= (const config_variable & cv) { value = cv.value; }

        public:
            const T & get_value() const { return value; }
            operator const T & () const { return value; }

        protected:
            virtual void assign_from_string(const gsgl::string &);
            virtual gsgl::string get_string() const;
        }; // class config_variable


        //////////////////////////////////////////

        template <>
        inline void config_variable<gsgl::string>::assign_from_string(const gsgl::string & s)
        {
            value = s;
        } // config_variable<gsgl::string>::assign_from_string()


        template <>
        inline gsgl::string config_variable<gsgl::string>::get_string() const
        {
            return value;
        } // config_variable<gsgl::string>::get_string()


        inline gsgl::string operator+ (const config_variable<gsgl::string> & cv, const wchar_t *str)
        {
            return cv.get_value() + str;
        } // operator+ ()


        inline gsgl::string operator+ (const config_variable<gsgl::string> & cv, const gsgl::string & str)
        {
            return cv.get_value() + str;
        } // operator+ ()


        template <>
        inline void config_variable<float>::assign_from_string(const gsgl::string & s)
        {
            value = static_cast<float>(s.to_double());
        } // config_variable<float>::assign_from_string()


        template <>
        inline gsgl::string config_variable<float>::get_string() const
        {
            return gsgl::string::format(L"%f", value);
        } // config_variable<float>::get_string()


        template <>
        inline void config_variable<int>::assign_from_string(const gsgl::string & s)
        {
            value = s.to_int();
        } // config_variable<int>::assign_from_string()


        template <>
        inline gsgl::string config_variable<int>::get_string() const
        {
            return gsgl::string::format(L"%d", value);
        } // config_variable<int>::get_string()

    } // namespace data
    
} // namespace gsgl

#endif
