#ifndef PLUGIN_H
#define PLUGIN_H

#include "com.h"
#include "failure_manager.h"

#ifdef __cpluplus
namespace lauSim {
extern "C" {
#endif

typedef struct {
    int (*init) (int argc, char *argv[]);
    int (*cleanup) ();
    com *(*get_com) ();
    failure_manager *(*get_failure_manager) ();
} plugin;

#ifdef __cpluplus
}
}
#endif

#endif