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

#include <iostream>

#include <lausim/types.h>
#include <lausim/plugin.h>
#include <mutex>
#include "stdoutLog.h"

namespace lauSim{

log_level ll_min = LL_None;
std::mutex log_mutex;

log_level log_level_from_int(unsigned int level) {
    switch (level) {
    case 0:
        return LL_None;
    case 1:
        return LL_Debug;
    case 2:
        return LL_Info;   
    case 3:
        return LL_Warning;
    case 4:
        return LL_Error;
    case 5:
        return LL_Fatal;
    default:
        return LL_Warning;
    }
}

void log_stdout(log_level l, const char *str) {
    const char * prefix;

    if (l < ll_min)
        return;

    switch (l) {
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

    std::unique_lock<std::mutex> log_lock(log_mutex);
    std::cerr << prefix << " " << str << std::endl;
    log_lock.unlock();
}

void set_min_log_level(log_level l) {
    ll_min = l;
}

logger standard_log = {
    2,
    set_min_log_level,
    log_stdout
};

}