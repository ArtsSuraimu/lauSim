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

#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <interface_backend.h>
#include "backend_if.h"

FILE *fifo = NULL;
volatile int fifo_created = 1;
volatile int lauSim_req_close_fd = 0;

FILE *lauSim_get_fifo() {
    if (fifo != NULL)
        return fifo;
    mode_t old = umask(0);
    if (0 != mkfifo(FIFO_FILE, 0666)) {
        perror("mkfifo");
        fifo_created = 0;
    }
    umask(old);

    fprintf(stderr, "fifo created\n");
    fifo = fopen(FIFO_FILE, "r");
    if (fifo == NULL) {
        perror("fopen");
        unlink(FIFO_FILE);
        return NULL;
    }
    setvbuf(fifo, NULL, _IOLBF, 0);
    return fifo;
}

void lauSim_fifo_cleanup() {
    if (fifo_created && fifo != NULL) {
        fclose(fifo);
        unlink(FIFO_FILE);
    }
}

void lauSim_backend_cleanup() {
    lauSim_fifo_cleanup();
}

int lauSim_init(int argc, char **argv) {
    fifo = lauSim_get_fifo();
    fgetc(fifo);
    /**
     * the lausim backend init function is expected to call lauSim main.
     * this gives the opportunity to create a new thread for lauSim main
     * (required by the preload library)
     */
    if (lauSim_backend_init () != 0)
        return EXIT_FAILURE;
    return 0;
}

int lauSim_main() {
    size_t size = 0;
    char *line = NULL, *cmd_str, *opt_str;
    unsigned long opt, cmd;
    int fifo_fd;
    fd_set fds;
    
    fifo_fd = fileno(fifo);
    if (fifo == NULL) {
        perror("fdopen");
        return EXIT_FAILURE;
    }

    while(1) {
        FD_ZERO(&fds);
        FD_SET(fifo_fd, &fds);
        FD_SET(lauSim_req_close_fd, &fds);
        select(lauSim_req_close_fd > fifo_fd ? (lauSim_req_close_fd + 1) : (fifo_fd + 1),
                &fds, NULL, NULL, NULL);
        if (FD_ISSET(lauSim_req_close_fd, &fds)) {
            fputs("requested to close\n", stderr);
            break;
        }
        if (!FD_ISSET(fifo_fd, &fds))
            continue;

        if (getline(&line, &size, fifo) < 0)
            continue;
        fprintf(stderr, "command: %s", line);
        cmd_str = strtok(line, " \n");
        if (cmd_str == NULL)
            continue;
        
        if (strcmp(cmd_str, "exit") == 0)
            break;

        opt_str = strtok(NULL, " \n");
        if (opt_str == NULL) {
            fprintf(stderr, "empty optstr! (%s)", cmd_str);
            continue;
        }
        opt = strtoul(opt_str, NULL, 0);
        if (strcmp(cmd_str, "net") == 0)
            cmd = SUBSYS_NET;
        if (strcmp(cmd_str, "power") == 0)
            cmd = SUBSYS_PWR;
        if (strcmp(cmd_str, "memory") == 0)
            cmd = SUBSYS_MEM;
        lauSim_set_state(cmd, opt);
    }

    lauSim_fifo_cleanup();
    lauSim_cleanup();
    return EXIT_SUCCESS;
}
