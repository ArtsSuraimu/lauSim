/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-07-18 10:04:40
*/

#ifndef ECHO_H
#define ECHO_H

#include <lausim/types.h>
#include <lausim/com.h>
#include <lausim/plugin.h>

com *get_com(void);
logger *get_log(void);
int init (const plugin_manager_interface*, int argc, char **argv);

plugin echo_plugin = {
    1, // version
    1, // pl_tye COM
    "echo", // name

    NULL,       // post init hook
    NULL,       // cleanup
    get_com,    // get_com_fun
    NULL,       // get_fault_fun
    NULL        // get_logger
};



#endif