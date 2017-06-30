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
    handle = dlopen(filename, RTLD_LAZY);
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