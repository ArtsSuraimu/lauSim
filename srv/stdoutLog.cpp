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