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

/** \file */

#ifndef INTERFACE_H
#define INTERFACE_H

#include <stdint.h>
#include <sys/types.h>

/**
 * struct describing a fault
 */
typedef struct fault_s {
    /**
     * name of the component
     */
    char *component;
    /**
     * the severity of the fault 0 <= severity <= 100
     */
    unsigned severity;
} fault;

/**
 * options to be passed to the mqtt communication backend
 */
struct mqtt_opt{
    char *hostname;
    uint16_t port;
};

/**
 * initializes the communication backend
 * 
 * @param name the name of the node
 * @param opt a struct depending on the actual backend used
 * @return 0 if successfull, 1 if not
 */
int init(const char *name, void *opt);

/**
 * returns a list of faults, waiting for at most timeout ms
 * 
 * @param timeout maximum amount of time to wait
 * @param num returns the number of faults received
 * @return pointer to an array containing the faults. Gets freed on the next call of this function
 */
fault *get_faults(size_t timeout, size_t *num);

/**
 * sends a message over the back channel to the manager
 * 
 * @param buf byte data to be send
 * @param len number of bytes to be send
 * @return 0 on success
 */
int send_back(uint8_t *buf, size_t len);

/**
 * cleanup of the communication backend
 */
void cleanup(void);

#endif