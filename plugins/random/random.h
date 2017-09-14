/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-07-18
*/

#ifndef ECHO_H
#define ECHO_H

#include <stdlib.h>

#include <lausim/node.h>
#include <lausim/fault_manager.h>
#include <lausim/plugin.h>

// extends the lausim node type to attach additional information
typedef struct{
    node p;

    int healthy;
} mynode;

fault_manager *get_fm(void);
int init (const plugin_manager_interface*, int argc, char **argv);
int cleanup(void);
int prob = RAND_MAX / 10;

plugin random_plugin = {
    1,          // version
    PL_FAULT_MANAGER,          // pl_type fault_manager
    "random",   // name

    NULL,       // post init hook
    cleanup,    // cleanup
    NULL,       // get_com_fun
    get_fm,     // get_fault_fun
    NULL        // logger
};

#endif