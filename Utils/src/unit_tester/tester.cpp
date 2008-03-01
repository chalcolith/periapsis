//
// $Id: tester.cpp 316 2008-03-01 16:34:12Z Gordon $
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


#if defined (WIN32)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#else
#include <dlfcn.h>
#endif



#include <list>
#include <string>
#include <iostream>
#include <algorithm>

using namespace std;



#include "data/exception.hpp"



#if defined (__OpenBSD__)
#define DLOPEN_FLAG DL_LAZY
#elif defined (__linux__)
#define DLOPEN_FLAG RTLD_NOW
#elif defined (WIN32)
#elif defined (__APPLE__)
#define DLOPEN_FLAG RTLD_NOW
#else
#error You must specify the correct flag to dlopen() for your OS.
#endif

extern "C" {
    typedef void (*test_func)(void);
    struct test_rec {
        char *name;
        test_func func;
    };
}

static void print_usage() {
    wcerr << "Usage: tester [-f func_name ...] library.so ..." << endl;
} // print_usage()

int main(int argc, char **argv) {
    typedef pair<string, void *> library_rec;
    typedef list<library_rec> library_list;
    library_list libraries;

    list<string> functions;

    //GC_find_leak = 1;

    // collect libraries and function names
    const string name_flag("-f");
    for (int i = 1; i < argc; ++i) {
        if (name_flag == argv[i]) {
            if (++i < argc) {
                functions.push_back(string(argv[i]));
            } else {
                print_usage();
                return 1;
            }
        } else {
            libraries.push_back(library_rec(string(argv[i]), 0));
        }
    }

    if (libraries.size() == 0) {
        print_usage();
        return 2;
    }

    // load libraries
    library_list::iterator lib;
    for (lib = libraries.begin(); lib != libraries.end(); ++lib) {
#if defined (WIN32)
		if (!(lib->second = LoadLibraryA(lib->first.c_str()))) {
			wcerr << "Unable to load " << lib->first.c_str() << endl;
			return 3;
		}
#else
        if (!(lib->second = dlopen(lib->first.c_str(), DLOPEN_FLAG))) {
            cerr << dlerror() << endl;
            return 3;
        }
#endif
    }
    
    // go through libraries
    int num_run = 0;
    int num_succeeded = 0;
    int num_failed = 0;

    for (lib = libraries.begin(); lib != libraries.end(); ++lib) {
        // load functions
        typedef pair<string, test_func> test_pair;
        typedef list<test_pair> test_list;
        test_list tests;

#if defined (WIN32)
        typedef void *(*gtf_ft)(void);
        gtf_ft get_test_functions = (gtf_ft) GetProcAddress((HMODULE) lib->second, "get_test_functions");
        test_rec *trec = static_cast<test_rec *>(get_test_functions());
#else
        test_rec *trec = static_cast<test_rec *>(dlsym(lib->second, "test_functions"));
#endif

        while (trec && trec->name) {
            string tstr(trec->name);
            
            if (functions.size() == 0 || count(functions.begin(), functions.end(), tstr) > 0) {
                tests.push_back(test_pair(string(trec->name), trec->func));
            }

            ++trec;
        }

        tests.sort();

        if (!tests.size()) {
            wcerr << "No function found.\n";
            return 5;
        }
        
        // run tests
        test_list::const_iterator test;
        for (test = tests.begin(); test != tests.end(); ++test) {
            //cout << test->first << endl;

            try {
                ++num_run;
                test_func f = test->second;
                f();
                ++num_succeeded;
            } catch (std::exception & e) {
                ++num_failed;
                wcerr << test->first.c_str() << " FAILED: " << e.what() << endl;
            } catch (gsgl::exception & e) {
                ++num_failed;
                wcerr << test->first.c_str() << " FAILED: " << e.get_message() << endl;
            } catch (...) {
                ++num_failed;
                wcerr << test->first.c_str() << " FAILED: " << "UNKNOWN EXCEPTION" << endl;
            }
        }
    }
    
    //GC_gcollect();

    return num_failed;
} // main()
