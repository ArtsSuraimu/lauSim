/*
 * debug.h
 *
 *  Created on: Apr 4, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_INCLUDE_DEBUG_H_
#define INC_INCLUDE_DEBUG_H_

#ifndef _STDIO_H
#include <stdio.h>
#endif

#define DBGOUT(format, ...) printf("[DEBUG]: " #format "\n", ##__VA_ARGS__)



#endif /* INC_INCLUDE_DEBUG_H_ */
