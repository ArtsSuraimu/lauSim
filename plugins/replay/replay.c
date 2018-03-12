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

#define _POSIX_C_SOURCE 200809L
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <lausim/logger.h>
#include "replay.h"

logger *logg;
size_t tic_num, nxt_tic, id;
FILE *file;
int done;
node *nodes;
node **node_ptr;
size_t nodes_num;
int num_fault;
fault **faults;
const plugin_manager_interface *plug_man;

size_t get_tic_num(char *buf) {
    size_t num;
    char *start, *end;
    strtok(buf, " #\t\n");
    start = strtok(NULL, " #\t\n");
    while (start != NULL) {
        num = strtoull(start, &end, 0);
        if (start != end)
            break;
        start = strtok(NULL, " #\t\n");
    }
    return num;
}

int get_nodes(size_t *num_nodes, node ***lau_nodes) {
    char debug[512];
    char *buf, *cur;
    ssize_t len;
    size_t i, j, size;
    component* comps;
    node_ptr = *lau_nodes;
    size = 0;
    buf = NULL;

    len = getline(&buf, &size, file);
    if (len == -1) {
        logg->log_fun(LL_Error, "[REPLAY] unable to read line");
        return 1;
    }
    snprintf(debug, sizeof(debug), "[REPLAY] line: %s", buf);
    logg->log_fun(LL_Debug, debug);
    nodes_num = strtoull(buf, &cur, 0);
    snprintf(debug, sizeof(debug), "[REPLAY] nodes_num: %zu", nodes_num);
    logg->log_fun(LL_Debug, debug);
    *num_nodes = nodes_num;
    if (cur == buf) {
        logg->log_fun(LL_Error, "[REPLAY] unable to read number of nodes");
        return 1;
    }
    
    nodes = calloc(nodes_num, sizeof(node));
    *lau_nodes = calloc(nodes_num, sizeof(node*));
    if (nodes == NULL || *lau_nodes == NULL) {
        logg->log_fun(LL_Error, "[REPLAY] not enough memory");
        return 1;
    }

    for (i = 0; i < nodes_num; i++) {
        len = getline(&buf, &size, file);
        if (len == -1) {
            logg->log_fun(LL_Error, "[REPLAY] unable to read line");
            return 1;
        }   
        snprintf(debug, sizeof(debug), "[REPLAY] line: %s", buf);
        logg->log_fun(LL_Debug, debug);
        
        (*lau_nodes)[i] = &(nodes[i]);
        nodes[i].id = i;
        nodes[i].type = uniform_node;
        nodes[i].state = NODE_STATE_UNKNOWN;
        cur = strtok(buf, ",\n");
        // do not forget null termination
        nodes[i].name = calloc(1, strlen(cur) + 1);
        if (nodes[i].name == NULL) {
            logg->log_fun(LL_Error, "[REPLAY] not enough memory to allocate node name");
            return 1;
        }
        // cant overrun because of allocation
        strcpy(nodes[i].name, cur);
        cur = strtok(NULL, ",\n");
        if (cur == NULL) {
            logg->log_fun(LL_Error, "[REPLAY] unexpected end of line");
            return 1;
        }
        nodes[i].num_components = strtoull(cur, NULL, 0);
        nodes[i].components = calloc(nodes[i].num_components, sizeof(component*));
        comps = calloc(nodes[i].num_components, sizeof(component));
        for (j = 0; j < nodes[i].num_components; j++) {
            cur = strtok(NULL, ",\n");
            if (cur == NULL) {
                logg->log_fun(LL_Error, "[REPLAY] unexpected end of line");
                return 1;
            }
            nodes[i].components[j] = (comps + j);
            comps[j].name = calloc(strlen(cur) + 1, 1);
            strcpy(comps[j].name, cur);
        }
        if (strtok(NULL, ",\n") != NULL) {
            logg->log_fun(LL_Warning, "[REPLAY] garbage at the end of the line");
        }
    }
    len = getline(&buf, &size, file);
    if (len < 0) {
        logg->log_fun(LL_Warning, "[REPLAY] no tic information present");
    } else {
        snprintf(debug, sizeof(debug), "[REPLAY] line: %s", buf);
        logg->log_fun(LL_Debug, debug);
        if (strstr(buf, "tic") != NULL) {
            nxt_tic = get_tic_num(buf);
        } else {
            cur = malloc(0x100);
            snprintf(cur, 0x100, "[REPLAY] expected [tic#<n>], found %s", buf);
            free(cur);
        }
    }
    free (buf);
    return 0;
}

void free_faults() {
    int i;
    for (i = 0; i < num_fault; i++) {
        free(faults[i]->node);
        free(faults[i]->component);
        free(faults[i]);
    }
    free(faults);
}

void tic(){
    char debug[512];
    char *buf, *name, *comp, *sev_str, *tmp;
    size_t size, severity;
    ssize_t len;
    free_faults();
    faults = NULL;
    num_fault = 0;
    buf = NULL;
    size = 0;
    if (done)
        return;

    if (tic_num++ < nxt_tic)
        return;

    while (nxt_tic < tic_num && !done) {
        len = getline(&buf, &size, file);
        if (len < 0) {
            done = 1;
            break;
        }
        snprintf(debug, sizeof(debug), "[REPLAY] line: %s", buf);
        logg->log_fun(LL_Debug, debug);
        if (len == 0) {
            logg->log_fun(LL_Warning, "[REPLAY] unexpected empty line");
            continue;
        }
        if (strstr(buf, "tic#") != NULL) {
            nxt_tic = get_tic_num(buf);
            if (nxt_tic < tic_num)
                logg->log_fun(LL_Warning, "[REPLAY] tic numbers not strict monotone");
            continue;
        }
        comp = strchr(buf, ',');
        if (comp == NULL) {
            logg->log_fun(LL_Warning, "[REPLAY] incomplete line");
            continue;
        }
        sev_str = strchr(comp + 1, ',');
        if (sev_str == NULL) {
            logg->log_fun(LL_Warning, "[REPLAY] incomplete line");
            continue;
        }

        comp[0] = 0;
        sev_str[0] = 0;
        name = buf;
        comp = comp + 1;
        sev_str = sev_str + 1;
        /*
        name = strtok(buf, " \t,\n");
        comp = strtok(comp, " \t,\n");
        sev_str = strtok(sev_str, " \t,\n");
        tmp = strtok(NULL, " \t,\n");
        */
        if (sev_str == NULL) {
            logg->log_fun(LL_Warning, "[REPLAY] incomplete line");
            continue;
        }
        severity = strtoull(sev_str, &tmp, 0);
        if (tmp == sev_str) {
            logg->log_fun(LL_Warning, "[REPLAY] expected severity to be a number");
            continue;
        }
        faults = realloc(faults, ++num_fault * (sizeof(fault*)));
        faults[num_fault - 1] = calloc(1, sizeof(fault));
        faults[num_fault - 1]->id = ++id;
        faults[num_fault - 1]->name = NULL;
        faults[num_fault - 1]->node = malloc(strlen(name) + 1);
        strcpy(faults[num_fault - 1]->node, name);
        if (comp[0] != 0) {
            faults[num_fault - 1]->component = malloc(strlen(comp) + 1);
            strcpy(faults[num_fault - 1]->component, comp);
        }
        faults[num_fault - 1]->severity = severity;
    }
    free(buf);
}

int get_fail(fault ***failed){
    *failed = faults;
    return num_fault;
}

int inject_fail(fault* f) {
    logg->log_fun(LL_Error, "[REPLAY] injecting faults in replay mode is not possible");
    return 1;
}

fault_manager fm = {
    .version = 1,          // version
    .get_nodes = get_nodes,
    .tic = tic,
    .get_fail = get_fail,
    .inject_fail = inject_fail
};

fault_manager *get_fm() {
    return &fm;
}

int init(const plugin_manager_interface *inter, int argc, char **argv) {
    logg = inter->get_logger();

    if (inter->version != PL_INTF_VERSION) {
        logg->log_fun(LL_Error, "[REPLAY] Plugin interface version missmatch");
        return 1;
    }

    if (argc < 1) {
        logg->log_fun(LL_Error, "[REPLAY] usage: libreplay.so filename");
        return -1;
    }

    file = fopen(argv[0], "r");
    if (file == NULL) {
        logg->log_fun(LL_Error, "[REPLAY] unable to open file");
        return -1;
    }
    plug_man = inter;

    inter->register_plugin(&replay_plugin);
    tic_num = 0;
    done = 0;
    return 0;
}

int post(void) {
    logg = plug_man->get_logger();
    return 0;
}

int cleanup(void) {
    size_t i;
    free_faults();
    for (i = 0; i < nodes_num; i++){
        free(nodes[i].components);
    }
    free(nodes);
    free(node_ptr);
    fclose(file);
    return 0;
}