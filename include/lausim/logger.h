/*
   Copyright 2017 C. Jonischkeit, D. Yang

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
 * @Author: D. Yang 
 * @Date: 2017-06-26 14:20:47 
 * @Last Modified by: D. Yang
 * @Last Modified time: 2017-06-26 14:21:18
 */
#ifndef LOGGER_H
#define LOGGER_H

#include <lausim/types.h>

#ifdef __cplusplus
namespace lauSim{
extern "C" {
#endif

typedef void (*set_log_level_fun) (log_level);
typedef void (*lausim_log_fun)(log_level l, const char *msg);

/**
 * This struct describes the interface to a logger
 */
typedef struct tag_logger{
    /**
     * version of the logger
     */
    int version;
    /**
     * function to set the log level. Only messages with a log level at least
     * as high as the set log level shall be logged
     */
    set_log_level_fun set_ll;
    /**
     * logging function
     * @param l the log level of the message
     * @param msg the message to be logged
     */
    lausim_log_fun log_fun;
} logger;


typedef logger *(*get_logger_fun) ();

#ifdef __cplusplus
}
}
#endif

#endif