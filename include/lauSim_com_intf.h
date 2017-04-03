/*
 * lauSim_com_intf.h
 *
 *  Created on: Mar 30, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_LAUSIM_COM_INTF_H_
#define INC_LAUSIM_COM_INTF_H_


int (*FP_SEND) (char* channel, char* buffer, int length);
int (*FP_RECV) (char* channel, char* buffer, int* length);
int (*FP_COM_INIT) (void* pData, void* instance);

typedef enum tag_com_type{
	COM_MQTT = 1,
	COM_TCP = 2,
	COM_UDP = 3,
	COM_SOCKET = 4,
	COM_PIPE = 5,
	COM_FILE = 6
};

typedef struct tag_com{
	void* instance;
	void* pData;
	FP_COM_INIT init;
	FP_SEND send;
	FP_RECV recv;
}com_backend_t;



#endif /* INC_LAUSIM_COM_INTF_H_ */
