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

#ifndef BACKEND_IF_H
#define BACKEND_IF_H

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
 * initializes the backend
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