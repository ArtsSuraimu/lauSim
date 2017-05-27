#ifndef NODE_H
#define NODE_H

#include <vector>

#include "component.h"

namespace lauSim{

class Node {
public:
    char *name;
    std::vector<Component> components;
protected:
};

}

#endif