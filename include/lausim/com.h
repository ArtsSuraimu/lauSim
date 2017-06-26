#ifndef COM_H
#define COM_H

#include <cstddef>
#include "node.h"

#ifdef __cpluplus
namespace lauSim {
extern "C" {
#endif

typedef struct {
    /**
     *  This methode should notify the backend of node
     *  that a component has failed or degraded
     *  param node the name of the node that failed
     *  param component the name of the component that failed
     *  param severity the severity of the fail (e.g. total fail or just corruptions)
     */
    int (*notify_fail) (char *node, char *component, unsigned severity);
} com;

#ifdef __cpluplus
}
}
#endif

#endif