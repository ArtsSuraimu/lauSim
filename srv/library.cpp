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
 * @Date: 2017-06-30 13:00:40
*/

#include <dlfcn.h>

#include "library.h"
using namespace lauSim;

int library::open(const char *filename){
    if (is_init)
        return -1;
    handle = dlmopen(LM_ID_NEWLM, filename, RTLD_LAZY);
    if (handle == nullptr)
        return -2;
    name = std::string(filename);
    is_init = true;
    return 0;
}

void *library::get_sym_addr(const char *symname) {
    if (!is_init)
        return nullptr;
    return dlsym(handle, symname);
}

library::~library() {
    if (is_init)
        dlclose(handle);
}