/*
   Copyright 2017 Clemens Jonischkeit

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/


/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 13:00:25
*/

#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>

namespace lauSim {

/**
 * This class is used to load and hold libraries containing plugins. It is also
 * used to resolve the mandatory init function and to keep a copy of the arguments for that
 * function. Storing the arguments over the lifetime of the library is neccessary to support a "main like"
 * semantic and enables the library to use references to the argument vector
 */
class library {
public:
    library() = default;
    library(library&&) = default;
    library& operator=(library&&) = default;
    library(library&) = delete;
    library& operator=(library&) = delete;
    
    /**
     * Plugins are stored in dynamic libraries (shared object files). This function loads such
     * file into memory using a dynamic library loader (dlmopen). For each library a new namespace
     * is created. This prevents symbols from one library shadowing symbols from a different library.
     * Each library can contain multiple plugins.
     */
    int open(const char *filename);

    /**
     * Thsi function seloves a symbol exposed by the loaded library by its name. This is used to find
     * initialization routines and other functions.
     */
    void *get_sym_addr(const char *symname);
    ~library();
    /**
     * stores the number of arguments supplyied to the library init
     */
    int argc;
    /**
     * stores the argument vector supplyied to the library
     * The keeps the vector valid during the lifetime of the library and also
     * enables freeing these resources upon unloading the library
     */
    char **argv;
protected:
    /**
     * stores the initialization state. True if the library was initialized
     */
    bool is_init = false;
    /**
     * holds the name of the library
     */
    std::string name;
    /**
     * holds the handle from dlopen
     */
    void * handle;
};

}

#endif