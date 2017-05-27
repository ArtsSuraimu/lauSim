#ifndef COM_H
#define COM_H

#include <cstddef>
#include "node.h"
#include "component.h"

namespace lauSim {

class com {
public:
    virtual bool send_fail(Node *target, Component *cmp, failure_type *type, unsigned severity) = 0;
};

}

#endif