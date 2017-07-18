/*
 * @Author: D. Yang
 * @Date: 2017-06-26 14:20:29 
 * @Last Modified by:   C. Jonischkeit
 * @Last Modified time: 2017-06-30 13:02:20 
 */
#ifndef LAUSIM_TYPES_H
#define LAUSIM_TYPES_H

#ifdef __cplusplus
#include <cstdint>
namespace lauSim{
extern "C"{
#else
#include <stdint.h>
#endif  //__cplusplus

typedef enum tag_log_level{
    LL_None = 0,
    LL_Debug, 
    LL_Info,
    LL_Warning,
    LL_Error,
    LL_Fatal
} log_level;

typedef uint64_t plugin_type_t;
typedef int (*post_init_fun) ();
typedef int (*cleanup_fun) ();

#ifndef __cplusplus
typedef unsigned char bool;
#endif //__cplusplus

#ifdef __cplusplus
}
}
#endif  //__cplusplus

#endif  //TYPES_H