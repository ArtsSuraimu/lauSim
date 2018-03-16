/*
   Copyright 2017 Clemens Jonischkeit

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
*/

/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-26 14:20:38 
 * @Last Modified by:   C. Jonischkeit
 * @Last Modified time: 2017-06-30 13:03:00 
 */
#ifndef NODE_H
#define NODE_H

#define NODE_STATE_UNKNOWN 0
#define NODE_STATE_READY 1
#define NODE_STATE_ERROR 2
#define NODE_STATE_FINISHED 3

#ifdef __cplusplus
namespace lauSim {
extern "C" {
#endif

/* detonates what type the node is. may be a critical node */
typedef enum tag_type{
	uniform_node = 0,
	network_switch,
	pdu
} node_type_t;

typedef struct tag_component{
    /**
     * name of the component
     */
    char *name;
    /**
     * the fault state the component is in [0,100], 0 is nominal, 100 is complete failure
     */
    unsigned severity;
} component;

typedef struct {
    /**
     * name of the node
     */
    char *name;
    /**
     * id of the node
     */
    int id;
    /**
     * type of the node
     */
    node_type_t type;
    /**
     * the state of the client (not the fault state).
     */
    unsigned state;
    /**
     * the number of components
     */
    unsigned num_components;
    /**
     * a list of components. It is of kind *(components[1]), so
     * the fault manager can easily derive a struct from component
     * and not need to copy everything for the server
     */
    component **components;
} node;

#ifdef __cplusplus
}
}
#endif

#endif
