#ifndef NODE_H
#define NODE_H

#include "types.h"

#ifdef __cpluplus
namespace lauSim {
extern "C" {
typedef unsigned char bool;
#endif

/* detonates what type the node is. may be a critical node */
typedef enum tag_type{
	uniform_node = 0,
	network_switch,
	pdu
} node_type_t;


typedef struct tag_component{
    char *name;
    unsigned severity;
    percentage_t degree;
} component;

typedef struct {
    char *name;
    int id;
    node_type_t type;
    unsigned num_components;
    
    rate_t failure_rate;
    rate_t recovery_rate;

    component *components[];
} node;

#ifdef __cpluplus
}
}
#endif

#endif
