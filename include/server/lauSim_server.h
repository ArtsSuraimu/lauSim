/*
 * lauSim_server.h
 *
 *  Created on: Apr 3, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_LAUSIM_SERVER_H_
#define INC_LAUSIM_SERVER_H_

#include "lauSim_algo_intf.h"
#include "lauSim_com_intf.h"

#define ARGNUM 2

typedef enum tag_option{
	ST_CONSOLE = 0,
	ST_DAEMON = 1,
	ST_GUI = 2,

	ST_UNDEFINED = 0xFF
}SERVER_TYPE;

typedef struct tag_lausim_config{
	SERVER_TYPE type;
	int debug;
}LAUSIM_CONFIG;


#endif /* INC_LAUSIM_SERVER_H_ */
