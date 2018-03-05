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

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <lausim/logger.h>
#include "transcribe.h"

logger *logg;
char *fault_name;
size_t tic_num;
FILE *file;
const plugin_manager_interface *plug_man;
fault_manager *nested;

int get_nodes(size_t *num_nodes, node ***lau_nodes) {
    size_t i;
    int ret = nested->get_nodes(num_nodes, lau_nodes);

    if (ret == 0) {
        for (i = 0; i < *num_nodes; i++)
            fprintf(file, "%s\n", (*lau_nodes)[i]->name);
    }
    return ret;
}

void tic(){
    fprintf(file, "[tic#%ld]\n", tic_num++);
    nested->tic();
}

int get_fail(fault ***failed){
    int i;
    fault *flt;
    int num_fails = nested->get_fail(failed);
    for (i = 0; i < num_fails; i++) {
        flt = (*failed)[i];
        fprintf(file, "%s,%s,%d\n"
               , flt->node
               , (flt->component == NULL) ? "" : flt->component
               , flt->severity);
    }
    return num_fails;
}

int inject_fail(fault* f) {
    return nested->inject_fail(f);
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
        logg->log_fun(LL_Error, "[TRANSCRIBE] Plugin interface version missmatch");
        return 1;
    }

    if (argc < 2) {
        logg->log_fun(LL_Error, "[TRANSCRIBE] usage: transcribe.so nested_manager filename");
        return -1;
    }

    fault_name = argv[0];
    file = fopen(argv[1], "w");
    if (file == NULL) {
        logg->log_fun(LL_Error, "[TRANSCRIBE] unable to open file");
        return -1;
    }
    plug_man = inter;

    inter->register_plugin(&trans_plugin);
    tic_num = 0;
    return 0;
}

int post(void) {
    logg = plug_man->get_logger();
    plugin *fmpl = plug_man->by_name(fault_name);
    if (fmpl == NULL) {
        logg->log_fun(LL_Error, "[TRANSCRIBE] unable aquire nested fault manager");
        return 1;
    }
    plug_man->add_role(fmpl, PL_FAULT_MANAGER);
    plug_man->set_role(&trans_plugin, PL_OTHER);
    nested = fmpl->fm();
    return 0;
}

int cleanup(void) {
    fclose(file);
    return 0;
}