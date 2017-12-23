#include <unistd.h>
#include <iostream>

#include "options.h"

using namespace lauSim;
char optstr[] = "c:l:";

int Options::parse(int argc, char **argv) {
    int opt;

    while((opt = getopt(argc, argv, optstr)) > 0) {
        switch(opt) {
        case 'c':
            cfile = std::string(optarg);
            break;
        case 'l':
            loglevel = (unsigned) strtoul(optarg, NULL, 0);
            break;
        default:
            std::cerr << "unrecognized option '" << (char) opt << "'" << std::endl;
            return -1;
        }
    }
    
    return 0;
}