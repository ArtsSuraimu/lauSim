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

/** \file */

#ifndef BACKEND_IF_H
#define BACKEND_IF_H

/**
 * \def FIFO_FILE
 * \brief A macro holding the file name of the fifo
 * 
 * This macro holds the file name of the fifo queue used to convey
 * commands from the client, listening for commands from the server,
 * to the backend interacting with the application.
 * 
 * \warning having a fixed path means that only one application may run at a time per host
 */

#define FIFO_FILE "/tmp/lauSim/backend"

/**
 * \enum en_subsys
 * 
 * This enum eneumerates the different types of subsystems that can be affected by a backend
 * Name  | Effects
 * ------|--------
 * SUBSYS_NET | The Network subsystems. A fail can result in dropped messages, connections and closed sockets
 * SUBSYS_MEM | The Memory subsystem. Fails can result in bitflips in memory.
 * SUBSYS_PWR | Power subsystem. A fail usually forces the entire application to quit
 */
enum en_subsys {
    SUBSYS_NET = 1,
    SUBSYS_MEM = 2,
    SUBSYS_PWR = 4
};

/**
 * contains the capabilities of the actor
 */
extern int lauSim_caps;

/**
 * \fn lauSim_backend_init()
 * This is called to initialize the backend
 * 
 * @return 0 on success
 */
int lauSim_backend_init();

/**
 * updates the state of a component
 * 
 * @param sys the subsystem that is targeted by the fault
 * @param severity the severity of the fault (0 = running without error, 100 = complete fault)
 * @return 0 on success
 */
int lauSim_set_state(enum en_subsys sys, unsigned long severity);

/**
 * cleans the backend
 */
void lauSim_cleanup();

#endif