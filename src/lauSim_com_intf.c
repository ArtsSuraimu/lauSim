/*
 * lauSim_com_intf.c
 *
 *  Created on: Apr 5, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#include <stdlib.h>
#include <assert.h>
#include "lauSim_com_intf.h"

com_backend_t* init_com(
	com_type_t	type,
	char*		addr,
	int			port
){
	com_backend_t* com = NULL;

	assert(addr);

	switch(type)
	{
		case COM_MQTT:
		case COM_TCP:
			assert(0);
		case COM_UDP:
			assert(0);
		case COM_SOCKET:
			assert(0);
		case COM_PIPE:
			assert(0);
		case COM_FILE:
			assert(0);
		default:
			break;
	}

	return com;
}
