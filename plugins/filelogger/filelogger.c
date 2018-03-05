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
#include <pthread.h>

#include <lausim/logger.h>
#include "filelogger.h"

logger *logg;
const plugin_manager_interface *plug_man;
void set_log_level (log_level level);
void logging_fun (log_level l, const char *msg);
log_level lvl;
pthread_mutex_t log_mutex = PTHREAD_MUTEX_INITIALIZER;
FILE *log_file;

const logger file_log = {
    .version = 0,
    .set_ll = set_log_level,
    .log_fun = logging_fun
};

/**
 * init funciton - called by the plugin manager upon loading of the plugin
 * @param inter Interface to the plugin manager
 * @param argc length of the argv argument vector
 * @return 0 on success
 */
int init(const plugin_manager_interface *inter, int argc, char **argv) {
    // safe plugin manager for later
    plug_man = inter;
    logg = inter->get_logger();

    // parse the arguments
    if (argc < 0)
        return 1;
    log_file = fopen(argv[0], "w");
    if (log_file == NULL)
        return 1;

    inter->register_plugin(&filelogger_plugin);
    return 0;
}

/**
 * post init function - called after all plugins have been loaded
 * important: update your logger, as it may differ from the one during "init"
 * (you can resolve dependencies at this point)
 * @return 0 on success
 */
int post(void) {
    logg = plug_man->get_logger();
    return 0;
}

/**
 * called prior to server teardown. Release all resources here
 * @return 0 on success
 */
int cleanup(void) {
    logg = plug_man->get_logger();
    fclose(log_file);
    return 0;
}

logger *get_logger(void) {
    return (logger *) &file_log;
}

void set_log_level(log_level level) {
    pthread_mutex_lock(&log_mutex);
    lvl = level;
    pthread_mutex_unlock(&log_mutex);
}

void logging_fun(log_level level, const char *msg) {
    char *prefix;
    switch(level) {
    default:
    case LL_None:
        prefix = "[NONE] ";
        break;
    case LL_Debug:
        prefix = "[DEBUG]";
        break;
    case LL_Info:
        prefix = "[INFO] ";
        break;
    case LL_Warning:
        prefix = "[WARN] ";
        break;
    case LL_Error:
        prefix = "[ERROR]";
        break;
    case LL_Fatal:
        prefix = "[FATAL]";
        break;
    }
    pthread_mutex_lock(&log_mutex);
    if (level >= lvl)
        fprintf(log_file, "%s %s\n", prefix, msg);
    pthread_mutex_unlock(&log_mutex);
}