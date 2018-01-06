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

#define COM_SYNC = 1
#define COM_ASYNC = 2

typedef struct {
    int capabilities;
    /**
     *  This methode should notify the backend of node
     *  that a component has failed or degraded
     *  param node the name of the node that failed
     *  param component the name of the component that failed
     *  param severity the severity of the fail (e.g. total fail or just corruptions)
     */
    int (*notify_fail) (const char *node, const char *component, unsigned severity);
    int (*notify_extern) (const char* msg, unsigned len_msg);
    int (*set_mode) (int com_mode);
    void *(*set_callback) (size_t (*on_message)(void **msg_arr, size_t **len_arr));
} com;

typedef com *(*get_com_fun) ();

#ifdef __cplusplus
}
}
#endif

#endif
