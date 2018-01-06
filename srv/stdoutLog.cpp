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
#include "stdoutLog.h"

namespace lauSim{

log_level ll_min = LL_None;

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

    std::cerr << prefix << " " << str << std::endl;
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