/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-07-18 10:04:40
*/

#include <stdio.h>

#include "echo.h"

int notify_fail(char *node, char *component, unsigned sev) {
    printf("Node \"%s\" Component \"%s\" failed with severity %d\n", node, component, sev);
    return 0;
}

int notify_extern(char *msg, int *len_msg) {
    printf("Node Failed: %s\n", msg);
    return 0;
}

com echo_com = {
    notify_fail,
    notify_extern
};

com *get_com(){
    return &echo_com;
}

int init(const plugin_manager_interface *i, int argc, char **argv) {
    for (int n = 0; n < argc; n++) {
        printf("[ECHO] Arg #%d: %s\n", n, argv[n]);
    }

    if (i->version != PL_INTF_VERSION)
        return -1;

    i->register_plugin(&echo_plugin);

    return 0;
}