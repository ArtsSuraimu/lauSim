/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-07-18 10:04:40
*/

#include <stdio.h>

#include "echo.h"

plugin_manager_interface *pif;
logger *llog;

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

int post_init() {
    llog = pif->get_logger();
    return 0;
}

int init(const plugin_manager_interface *i, int argc, char **argv) {
    char buf[512];
    llog = i->get_logger();
    for (int n = 0; n < argc; n++) {
        snprintf(buf, sizeof(buf), "[ECHO] Arg #%d: %s", n, argv[n]);
        llog->log_fun(LL_Debug, buf);
    }

    if (i->version != PL_INTF_VERSION) {
        llog->log_fun(LL_Error, "[ECHO] plugin manager version mismatch\n");
        return -1;
    }

    i->register_plugin(&echo_plugin);
    pif = (plugin_manager_interface*) i;
    return 0;
}