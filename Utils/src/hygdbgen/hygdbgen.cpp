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

#include "data/list.hpp"
#include "data/string.hpp"
#include "data/fstream.hpp"
#include "data/dictionary.hpp"

using namespace gsgl;
using namespace gsgl::data;
using namespace gsgl::io;

//

struct star_rec
{
    int hyg_id;

    float right_ascension;
    float declination;
    float distance;
    float abs_magnitude;
    
    unsigned char color[4];

    bool operator== (const star_rec & sr) const
    {
        return hyg_id == sr.hyg_id
            && right_ascension == sr.right_ascension
            && declination == sr.declination
            && distance == sr.distance
            && abs_magnitude == sr.abs_magnitude
            && color[0] == sr.color[0]
            && color[1] == sr.color[1]
            && color[2] == sr.color[2]
            && color[3] == sr.color[3];
    }
}; // struct star_rec

struct color_rec
{
    string name;
    unsigned char color[4];

    dictionary<struct color_rec, wchar_t> children;

    bool operator== (const color_rec & cr) const
    {
        return name == cr.name
            && color[0] == cr.color[0]
            && color[1] == cr.color[1]
            && color[2] == cr.color[2]
            && color[3] == cr.color[3]
            && children == cr.children;
    }
}; // color_rec

static int color_db_size = 0;

//

static void add_color_rec(color_rec & node, const string & name, int index, unsigned char *ccc)
{
    if (index == name.size())
    {
        ++color_db_size;
        node.name = name;
        node.color[0] = ccc[0];
        node.color[1] = ccc[1];
        node.color[2] = ccc[2];
        node.color[3] = ccc[3];
    }
    else
    {
        add_color_rec(node.children[name[index]], name, index+1, ccc);
    }
} // add_color_rec()


static const color_rec & find_color_rec(const color_rec & node, const string & name, int index)
{
    if (index == name.size())
    {
        return node;
    }
    else
    {
        return find_color_rec(node.children[name[index]], name, index+1);
    }
} // find_color_rec()


static void load_color_database(const string & fname, color_rec & db)
{
    ft_stream f(fname);
    string line;
    int line_number = 1;
    
    for (f >> line; !f.at_end(); ++line_number, f >> line)
    {
        list<string> tokens = line.split(L",");

        if (tokens.size() != 7)
            throw io_exception(L"ill-formatted line %d in %ls", line_number, fname.w_string());

        string name = tokens[0];
        name.trim();
        unsigned char ccc[4];

        ccc[0] = tokens[3].to_int();
        ccc[1] = tokens[4].to_int();
        ccc[2] = tokens[5].to_int();
        ccc[3] = 255;

        add_color_rec(db, name, 0, ccc);
    }
} // load_color_database()


//

static void load_star_database(const string & fname, data::list<star_rec> & db, const color_rec & colors, dictionary<string, int> & names)
{
    //
    ft_stream f(fname);
    string line;
    int line_number = 1;

    for (f >> line; !f.at_end(); ++line_number, f >> line)
    {
        if (line_number == 1)
            continue;

        if ((line_number % 1000) == 0)
            ft_stream::out << line_number << " (size of color db: " << color_db_size << ")\n";

        list<string> tokens = line.split(L",");

        if (tokens.size() != 14)
            throw io_exception(L"ill-formatted line %d in %ls", line_number, fname.w_string());

        star_rec rec;
        rec.hyg_id          = tokens[0].to_int();
        rec.right_ascension = static_cast<float>(tokens[7].to_double());
        rec.declination     = static_cast<float>(tokens[8].to_double());
        rec.distance        = static_cast<float>(tokens[9].to_double());
        rec.abs_magnitude   = static_cast<float>(tokens[10].to_double());

        string name = tokens[6].trim();
        //if (name.is_empty())
        //    name = tokens[4].trim();

        if (!name.is_empty())
            names[rec.hyg_id] = name;

        rec.color[0] = 255;
        rec.color[1] = 255;
        rec.color[2] = 255;
        rec.color[3] = 255;

        string temp = tokens[12];
        temp.trim();
        list<string> color_tokens = temp.split(L"/");
        if (color_tokens.size() > 1)
        {
            color_rec ccc = find_color_rec(colors, color_tokens[0], 0);
            if (!ccc.name.is_empty())
            {
                rec.color[0] = ccc.color[0];
                rec.color[1] = ccc.color[1];
                rec.color[2] = ccc.color[2];
                rec.color[3] = ccc.color[3];
            }
        }

        db.append(rec);
    }

    ft_stream::out << "read " << db.size() << " stars, " << names.size() << " names\n";
} // load_star_database()

//

static const string star_db_cookie = L"Periapsis Stellar Database 2.0";

static void write_star_database(const string & fname, const list<star_rec> & star_db, dictionary<string, int> & star_names)
{
    fd_stream f(fname, FILE_OPEN_WRITE);

    // write file cookie
    f << star_db_cookie;

    // write number of star records
    int num = star_db.size();
    f << num;

    // write star records
    for (list<star_rec>::const_iterator i = star_db.iter(); i.is_valid(); ++i)
    {
        f << i->hyg_id;
        f << i->right_ascension;
        f << i->declination;
        f << i->distance;
        f << i->abs_magnitude;

        f << i->color[0];
        f << i->color[1];
        f << i->color[2];
        f << i->color[3];
    }

    ft_stream::out << "wrote " << num << " stars, ";

    // write number of star names
    num = star_names.size();
    f << num;

    for (dictionary<string, int>::iterator i = star_names.iter(); i.is_valid(); ++i)
    {
        f << i.get_index();
        f << *i;

        //f << i->second;
        //f << i->first;
    }

    ft_stream::out << num << " names\n";
} // write_star_database()

//

int main(int argc, char **argv)
{
    string star_fname, color_fname, output_fname;

    if (argc != 4)
    {
        ft_stream::out << "Usage: hygdbgen star_data.csv color_data.csv output.dat\n";
        return 1;
    }
    else
    {
        star_fname   = string(argv[1]);
        color_fname  = string(argv[2]);
        output_fname = string(argv[3]);
    }

    //

    try
    {
        list<star_rec> star_db;
        color_rec color_db;
        dictionary<string, int> star_names;

        load_color_database(color_fname, color_db);
        load_star_database(star_fname, star_db, color_db, star_names);

        write_star_database(output_fname, star_db, star_names);
    }
    catch (exception & e)
    {
        ft_stream::out << "Error: " << e.get_message() << "\n";
    }

    return 0;
} // main()
