#ifndef FAILURE_MANAGER_H
#define FAILURE_MANAGER_H

#include "node.h"
#include "component.h"

namespace lauSim {

struct failur_config{
    node *node;
    component *component;
    failure_type *type;
}

class failure_manager {

};

}

#endif
