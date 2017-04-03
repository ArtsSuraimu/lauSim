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
	void* uData,
	void* instance
);

int mqtt_subscribe(
	int num_topics,
	char ** topics
	);




#endif /* INC_BACKEND_MOSQUITTO_MQTTCLIENT_C_ */
