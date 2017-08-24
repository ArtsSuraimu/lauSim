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

typedef struct tag_logger{
    int version;
    set_log_level_fun set_ll;
    lausim_log_fun log_fun;
} logger;


typedef logger *(*get_logger_fun) ();

#ifdef __cplusplus
}
}
#endif

#endif