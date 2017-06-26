#ifndef NODE_H
#define NODE_H

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
} node_t;


typedef struct tag_component{
    char *name;
    unsigned severity;
    unsigned char degree;
} component;

typedef struct {
    char *name;
    int id;
    unsigned type;
    unsigned num_components;
    component *components[];
} node;

typedef struct tag_distance_graph {
    int dimension;
    bool edges;
} graph;

#ifdef __cpluplus
}
}
#endif

#endif
