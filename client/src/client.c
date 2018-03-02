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

#define _POSIX_C_SOURCE 199309L

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <interface.h>
#include <interface_backend.h>
#include <backchannel.pb-c.h>

volatile int req_stop = 0;

void handler(int signum) {
    req_stop = 1;
}

struct sigaction sigact;

int main(int argc, char **argv) {
    fault *flt;
    FILE *fifo;
    size_t num, i, msg_len;
    Backchannel msg;
    uint8_t *msg_buf;
#if CON == MQTT
    struct mqtt_opt opt;
#endif

    fifo = fopen(FIFO_FILE, "w");
    if (fifo == NULL) {
        perror("fopen");
        return EXIT_FAILURE;
    }
    setvbuf(fifo, NULL, _IOLBF, 0);

    sigact.sa_sigaction = NULL;
    sigact.sa_handler = handler;
    sigact.sa_flags = 0;
    sigact.sa_restorer = NULL;
    sigemptyset(&sigact.sa_mask);
    sigaction(SIGTERM, &sigact, NULL);
    sigaction(SIGINT, &sigact, NULL);

#if CON == MQTT
    if (argc < 4) {
        printf("usage: %s name host port\n", argc ? argv[0] : "com_mqtt");
        return -1;
    }
    opt.hostname = argv[2];
    opt.port = strtoul(argv[3], NULL, 0);
    if (init(argv[1], &opt) != 0){
        printf("mqtt init failed\n");
        return -2;
    }
#endif

    backchannel__init(&msg);
    msg.type = BACKCHANNEL__MSG_TYPE__STATUS;
    msg.nodename = argv[1];
    msg.newstatus = BACKCHANNEL__NODE_STATUS__STREADY;
    msg.has_newstatus = 1;
    msg_len = backchannel__get_packed_size(&msg);
    msg_buf = calloc(1, msg_len);
    backchannel__pack(&msg, msg_buf);
    send_back(msg_buf, msg_len);
    free(msg_buf);

    while(!req_stop) {
        flt = get_faults(100, &num);
        for (i = 0; i < num; i++) {
            printf("%s failed (%3d)\n", flt[i].component, flt[i].severity);
            if (flt[i].component == NULL) {
                fprintf(fifo, "power %d\n", flt[i].severity);
                continue;
            } else {
                fprintf(fifo, "%s %d\n", flt[i].component, flt[i].severity);
            }
        }
    }
    
    // tell the backend to exit
    fputs("exit\n", fifo);
    fclose(fifo);
    cleanup();

    return EXIT_SUCCESS;
}