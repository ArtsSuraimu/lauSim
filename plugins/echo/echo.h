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
int post_init();
int init (const plugin_manager_interface*, int argc, char **argv);
extern logger *llog;
extern plugin_manager_interface *pif;

plugin echo_plugin = {
    1, // version
    PL_COM_ACTOR | PL_COM_EXTERN, // pl_tye COM
    "echo", // name

    post_init,       // post init hook
    NULL,       // cleanup
    get_com,    // get_com_fun
    NULL,       // get_fault_fun
    NULL        // get_logger
};



#endif