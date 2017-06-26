/*
 * @Author: D. Yang 
 * @Date: 2017-06-26 14:20:47 
 * @Last Modified by: D. Yang
 * @Last Modified time: 2017-06-26 14:21:18
 */
#ifndef LAUSIM_CORE_H
#define LAUSIM_CORE_H

#include "types.h"

#ifdef __cplusplus
namespace lauSim{
extern "C"{
#endif

void lausim_log(LogLevel l, char * msg, ...);

#ifdef __cplusplus
}
}
#endif //__cplusplus

#endif //LAUSIM_CORE_H