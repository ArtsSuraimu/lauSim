#ifndef COMPONENT_H
#define COMPONENT_H

#include <vector>

namespace lauSim {

struct failure_type {
    char *name;
    double propability;
};

class Component {
public:
    char *name;
    double reliablitiy;
    std::vector<failure_type> failure_types;
};

}

#endif