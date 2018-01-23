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
 * @Date: 2017-07-18 10:04:40
*/

#include <stdio.h>

#include "echo.h"

plugin_manager_interface *pif;
logger *llog;

int notify_fail(const char *node, const char *component, unsigned sev) {
    if (component)
        printf("Node \"%s\" Component \"%s\" failed with severity %d\n", node, component, sev);
    else
        printf("Node \"%s\" %s\n", node, sev ? "failed" : "recovered");
    return 0;
}

int notify_extern(const char *msg, unsigned len_msg) {
    printf("Node Failed: %s\n", msg);
    return 0;
}

com echo_com = {
    0,
    notify_fail,
    notify_extern,
    NULL,
    NULL,
    NULL
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

    if (i->version != PL_INTF_VERSION) {
        printf("[ECHO] Plugin interface version missmatch\n");
        return 1;
    }

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