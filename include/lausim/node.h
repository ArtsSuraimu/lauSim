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

#ifdef __cplusplus
}
}
#endif

#endif
