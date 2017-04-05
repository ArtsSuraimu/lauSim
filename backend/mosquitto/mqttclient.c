/*
 * mqttclient.c
 *
 *  Created on: Mar 30, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <mosquitto.h>

#include "mqttclient.h"
#include "config.h"

static

struct mosquitto *mosq = NULL;

static inline
int min(
	int a,
	int b
	){
	if(a>b){
		return b;
	}else
	{
		return a;
	}
}

static
char* getClid(
	void
	){
#ifdef CLIENTID
	return CLIENTID;
#else
	return "foo";
#endif
}




void mqtt_cb(struct mosquitto * mosq, void * pData, const struct mosquitto_message * msg);

int mqtt_init(
	char* 			client_id,
	char* 			address,
	int				port,
	int				keep_alive,
	com_backend_t* 	com
){
	int ret;

	mqtt_user_backend_t* pData = (mqtt_user_backend_t*) malloc (sizeof(mqtt_user_backend_t));

	mosquitto_lib_init();
	mosq = mosquitto_new (getClid(), 1, pData);
	if(!mosq){
		return -1;
	}
	mosquitto_message_callback_set(mosq, mqtt_cb);

	if(!address){
		address = "localhost";
	}

	if(!port){
		port = 1883;
	}

	if(!keep_alive){
		keep_alive = 60;
	}

	ret = mosquitto_connect(mosq, address, port, keep_alive);
	if(ret!=MOSQ_ERR_SUCCESS)
	{
		return -1;
	}

	mosquitto_reconnect_delay_set(mosq, 1, 30, 0);
	ret = mosquitto_loop_start(mosq);
	if(ret!=MOSQ_ERR_SUCCESS)
	{
		return -1;
	}

	com->pData = pData;
	com->isConnected = 1;
	com->addr = address;
	com->port = port;
	com->recv = mqtt_recv;
	com->send = mqtt_publish;

	return 0;
}

int mqtt_subscribe(
	int num_topics,
	char ** topics
){
	if(mosq){
		int i = 0;
		int ret;
		for(; i<num_topics; i++)
		{
			ret = mosquitto_subscribe(mosq, NULL, topics[i], 2);
			if(ret != MOSQ_ERR_SUCCESS)
			{
				return -1;
			}
		}
	}
	return -2;
}



void mqtt_cb(
	struct mosquitto * mosq,
	void * pData,
	const struct mosquitto_message* msg
){
	int len;

	mqtt_user_backend_t* data = pData;
	data->key = msg->topic;

	len = min(data->bufLen, msg->payloadlen);
	memcpy(data->msg, msg->payload, len);
	*(data->msglen) = len;
	data->callback(data->key, data->msg, len);
}


int mqtt_recv(
	char* channel,
	char* buffer,
	int*  length
	){
	assert(0);
	return 0;
}

int mqtt_publish(
	char* channel,
	char* buffer,
	int   length
	){
	assert(0);
	return 0;
}
