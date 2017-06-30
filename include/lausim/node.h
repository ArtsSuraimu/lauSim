/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-26 14:20:38 
 * @Last Modified by:   C. Jonischkeit
 * @Last Modified time: 2017-06-30 13:03:00 
 */
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
} node_type_t;


typedef struct tag_component{
    char *name;
    unsigned severity;
} component;

typedef struct {
    char *name;
    int id;
    node_type_t type;
    unsigned num_components;
    component **components;
} node;

#ifdef __cpluplus
}
}
#endif

#endif
