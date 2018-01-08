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

#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <sys/types.h>

typedef struct fault_s {
    char *component;
    unsigned severity;
} fault;

struct mqtt_opt{
    char *hostname;
    uint16_t port;
};

int init(const char *name, void *opt);
fault *get_faults(size_t timeout, size_t *num);
int send_back(uint8_t *buf, size_t len);
void cleanup(void);

#endif