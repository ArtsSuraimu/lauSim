/*
 * lauSim_com_intf.c
 *
 *  Created on: Apr 5, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include "lauSim_com_intf.h"
#include "mqttclient.h"

com_backend_t* init_com(
	com_type_t	type,
	char*		addr,
	int			port
){
	int ret;
	com_backend_t* com = NULL;

	assert(addr);


	com = (com_backend_t*)
				malloc (sizeof(com_backend_t));
	switch(type)
	{
		case COM_MQTT:
		{
			ret = mqtt_init(NULL, addr, port, 60, com);
			if(!ret)
			{
				free(com);
				return 0;
			}

			break;
		}
		case COM_TCP:
			assert(0);
			break;
		case COM_UDP:
			assert(0);
			break;
		case COM_SOCKET:
			assert(0);
			break;
		case COM_PIPE:
			assert(0);
			break;
		case COM_FILE:
			assert(0);
			break;
		default:
			break;
	}

	return com;
}
