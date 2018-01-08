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

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "backend_if.h"
#include "backend.h"

int lauSim_caps = SUBSYS_NET;
unsigned long net_status = 0;
int lauSim_close_fds[2];

int switch_net(unsigned long severity) {
    if (net_status == 0 && severity != 0) {
        // TODO disable network
        net_status = 1;
        return 0;
    }

    if (net_status != 0 && severity == 0) {
        // TODO enable network
        net_status = 0;
        return 0;
    }

    // nothing to do
    return 0;
}

int is_iptables_pressent() {
    int ret = 0;
    size_t size = 0;
    char *line = NULL;
    FILE *tab = popen("iptables -V", "r");
    if (tab == NULL)
        return 0;

    getline(&line, &size, tab);
    strtok(line, "\n");
    if (line != NULL)
        puts(line);
    free(line);
    if (pclose(tab) == 0)
        ret = 1;
    fclose(tab);
    return ret;
}

int lauSim_init () {
    printf("looking for iptables");
    if (!is_iptables_pressent())
        return 1;
    return 0;
}

void cleanup_net() {
    switch_net(0);
}

void lauSim_cleanup() {
    cleanup_net();
    // TODO cleanup mem and pwr
}

int lauSim_set_state(enum en_subsys sys, unsigned long severity) {
    int res;
    switch (sys) {
    case SUBSYS_NET:
        res = switch_net(severity);
        break;
    case SUBSYS_PWR:
    case SUBSYS_MEM:
        // TODO implement
        res = 0;
        break;
    default:
        res = 1;
        break;
    }
    return res;
}

int main(int argc, char **argv) {
    if (pipe(lauSim_close_fds) < 0) {
        perror("pipe");
        return EXIT_FAILURE;
    }
    lauSim_req_close_fd = lauSim_close_fds[0];
    lauSim_main(argc, argv);
    close(lauSim_close_fds[0]);
    close(lauSim_close_fds[1]);
}