/*
 * MQTTConsole.c
 *
 *  Created on: Mar 30, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */


#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
#include "lauSim_com_intf.h"
#include "MQTTConsole.h"
#include "mqttclient.h"

void printOut(
	char* topic,
	void* payload,
	int msglen){

	char* ptr = (char*) payload;
	int i;

	struct timeval t;
	gettimeofday(&t, NULL);


	printf("[MQTT] %ld: Topic-> %s, Len-> %d, Message-> ", t.tv_sec, topic, msglen);

	for(i=0; i<msglen; i++){
		printf("%c", ptr[i]);
	}

	printf("\n");
}


#ifdef MOSQ_TEST
int main(
	int argc,
	char** argv
	){

	mqtt_user_backend_t myData;
	int msgLen;
	char buf[1024];
	char* topic[] = {"envelope/laik/repart", ""};

	myData.bufLen = 1024;
	myData.callback = &printOut;
	myData.msglen = &msgLen;
	myData.msg = buf;

	mqtt_init(&myData, NULL);
	mqtt_subscribe(1, topic);

	while(1)
	{
		sleep(100);
	}

	return 0;
}
#endif
