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

static
char* getHostname(
	void
	){
#ifdef HOSTNAME
	return HOSTNAME;
#else
	return "localhost";
#endif
}

static
int getPort(
	void
	){
#ifdef PORT
	return PORT;
#else
	return 1883;
#endif
}

static int getKaL(
	void){
#ifdef KEEPALIVE
	return KEEPALIVE;
#else
	return 60;
#endif
}


void mqtt_cb(struct mosquitto * mosq, void * pData, const struct mosquitto_message * msg);

int mqtt_init(
	mqtt_user_backend_t* pData
){
	int ret;
	mosquitto_lib_init();
	mosq = mosquitto_new (getClid(), 1, pData);
	if(!mosq){
		return -1;
	}
	mosquitto_message_callback_set(mosq, mqtt_cb);

	ret = mosquitto_connect(mosq, getHostname(), getPort(), getKaL());
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
