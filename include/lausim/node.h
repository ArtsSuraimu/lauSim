#ifndef NODE_H
#define NODE_H

#ifdef __cpluplus
namespace lauSim {
extern "C" {
#endif

typedef struct {
    char *name;
    unsigned severity;
} component;

typedef struct {
    char *name;
    unsigned num_components;
    component **components;
} node;

#ifdef __cpluplus
}
}
#endif

#endif