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

/*
 * @Author: C. Jonischkeit 
 * @Date: 2017-06-26 14:20:50 
 * @Last Modified by:   C. Jonischkeit
 * @Last Modified time: 2017-06-30 12:58:45 
 */
#ifndef COM_H
#define COM_H

#ifdef __cplusplus
#include <cstddef>
#endif

#ifdef __cplusplus
namespace lauSim {
extern "C" {
#endif

#define COM_NONE = 0
#define COM_SYNC = 1
#define COM_ASYNC = 2

typedef struct {
    /**
     * capabilities of the backchannel
     * 
     * mode | meaning
     * :--- | :------
     * COM_NONE | no backchannel is implemented
     * COM_SYNC | synchronus backchannel
     * COM_ASYNC | asynchdonous backchannel, messages will be passed to the registered callback as they are received
     */
    int capabilities;

    /**
     * Notifies the backend of faults that occured
     * 
     * @param node the name of the node that failed
     * @param component the name of the component that failed
     * @param severity the severity of the fail (e.g. total fail or just corruptions)
     * @return 0 on success
     */
    int (*notify_fail) (const char *node, const char *component, unsigned severity);

    /**
     * notifies lauSim of a fault
     */
    int (*notify_extern) (const char* msg, unsigned len_msg);
    
    /**
     * updates the com mode for the backchannel
     * 
     * does not has to be defined by backends that provide no backchannel (capabilities = COM_NONE)
     * @param the com mode that shall be used (COM_SYNC, COM_ASYNC, COM_NONE)
     * @return 0 on success
     */
    int (*set_mode) (int com_mode);

    /**
     * updates the callback function for the asynchronous backchannel
     * 
     * @param on_message function to be called when a message is received
     * @return the callback that was previously registered
     */
    void *(*set_callback) (size_t (*on_message)(void **msg_arr, size_t **len_arr));
} com;

typedef com *(*get_com_fun) ();

#ifdef __cplusplus
}
}
#endif

#endif
