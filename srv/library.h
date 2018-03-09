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
 * used to load plugins and keep them in memory
 */
class library {
public:
    library() = default;
    library(library&&) = default;
    library& operator=(library&&) = default;
    library(library&) = delete;
    library& operator=(library&) = delete;
    
    /**
     * opens a plugin (shared object file)
     * 
     * for each plugin a new symbol namespace is created resulting in libraries (like libc) beeing loaded multiple times
     */
    int open(const char *filename);

    /**
     * resolves a symbol defined by this plugin
     */
    void *get_sym_addr(const char *symname);
    ~library();
    /**
     * stores the number of arguments supplyied to the library init
     */
    int argc;
    /**
     * stores the argument vector supplyied to the library
     * The keeps the vector valid during the lifetime of the plugin and also
     * enables freeing these resources
     */
    char **argv;
protected:
    bool is_init = false;
    std::string name;
    void * handle;
};

}

#endif