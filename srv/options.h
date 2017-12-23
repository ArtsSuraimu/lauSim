#ifndef OPTIONS_H
#define OPTIONS_H

#include <string>

namespace lauSim {

class Options {
public:
    Options(): cfile(""), loglevel(0) {}
    int parse(int argc, char ** argv);

    std::string cfile;
    unsigned loglevel;
};

}

#endif