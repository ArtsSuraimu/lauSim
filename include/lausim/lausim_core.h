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