/*
 * mqttclient.c
 *
 *  Created on: Mar 30, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_BACKEND_MOSQUITTO_MQTTCLIENT_C_
#define INC_BACKEND_MOSQUITTO_MQTTCLIENT_C_

#include "lauSim_com_intf.h"


typedef void (*FP_MQTT_CB) (char* topic, void* msg, int msglen);

typedef struct tag_mosquitto_backend{
	FP_MQTT_CB callback;
	char* key;
	void* msg;
	int* msglen;
	int bufLen;
	FP_SEND send;
	FP_RECV recv;
}mqtt_user_backend_t;

int mqtt_init(
	char* 			client_id,
	char* 			address,
	int				port,
	int				keep_alive,
	com_backend_t* 	com
);

int mqtt_subscribe(
	int num_topics,
	char ** topics
	);


int mqtt_recv(
	char* channel,
	char* buffer,
	int*  length
	);

int mqtt_publish(
	char* channel,
	char* buffer,
	int   length
	);


#endif /* INC_BACKEND_MOSQUITTO_MQTTCLIENT_C_ */
