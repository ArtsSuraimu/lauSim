/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-30 13:00:25
*/

#ifndef LIBRARY_H
#define LIBRARY_H

#include <string>

namespace lauSim {

class library {
public:
    library() = default;
    library(library&&) = default;
    library& operator=(library&&) = default;
    library(library&) = delete;
    library& operator=(library&) = delete;
    int open(const char *filename);
    void *get_sym_addr(const char *symname);
    ~library();
    int argc;
    char **argv;
protected:
    bool is_init = false;
    std::string name;
    void * handle;
};

}

#endif