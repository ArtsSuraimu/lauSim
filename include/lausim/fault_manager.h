/*
   Copyright 2018 Clemens Jonischkeit

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
 * @Date: 2017-06-26 14:20:59 
 * @Last Modified by:   C. Jonischkeit
 * @Last Modified time: 2017-06-30 13:03:35 
 */
#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

#include <lausim/node.h>

#ifdef __cplusplus
namespace lauSim {
extern "C" {
#endif  //__cplusplus

/**
 * This struct represents a fault. It is filled by the fault manager with all details
 * neccessary for the server to dispatch it.
 */
typedef struct tag_fault{
    /**
     * The id of the fault
     */
    int id;
    /**
     * The name of the fault
     */
    char *name;
    /**
     * The name of the component that failed
     */
    char *component;
    /**
     * The name of the node the fault happend on
     */
    char *node;
    /**
     * The severity of the fault
     */
    unsigned severity;
} fault;

typedef struct tag_fault_manager{
	/**
	  For storing the current fault manager version
	*/
	int version;

    /**
     *  Nodes are configured and handeled in the fault manager, but to
     *  notify the end points the nodes also have to be known to the simultor
     *  this function asks for all nodes and their components
     *  ownership is shared by the fault manager and the lausim core
     *  The memory is assumed to stay valid until the cleanup function of the module
     *  is called, when this memory should be freed by the fault manager
     *  (the ownership is still subject to be changed)
     *  param num_nodes output: the number of nodes
     *  param nodes output: the nodes
     *  return 0 on success
     */
    int (*get_nodes)(size_t *num_nodes, node ***nodes);

    /**
     *  Advances the time by one tic. It is caled in regular intervals
     *  however no implication is made on the simulated timespan
     */
    void (*tic) ();

    /**
     *  this functions asks the manager if and which nodes have failed
     *  param failed the node and component that failed (output)
     *  result 0 if a node failed, a positive number 
     */
    int (*get_fail)(fault ***failed);

    /**
     *  forces a fault
     */
    int (*inject_fail)(fault* f);
} fault_manager;

typedef fault_manager *(*get_fault_manager_fun) ();

#ifdef __cplusplus
}
}
#endif

#endif
