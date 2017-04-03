/*
 * linux_actor.c
 *
 *  Created on: Mar 31, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>

#include "linux_actor.h"

static
void linux_wait(
	int delay
){
	struct timeval t;
	struct timeval dt;
	if(delay == 0)
	{
		return;
	}
	gettimeofday(&t, NULL);
	do{
		sleep(100);
		gettimeofday(&dt, NULL);
	}while(dt.tv_sec - t.tv_sec < delay);
}

void linux_actor_fail(
	int delay
){
	linux_wait(delay);
	system("sudo init 0");
}

void linux_actor_reboot(
	int delay
){
	linux_wait(delay);
	system("sudo reboot now");
}

void linux_actor_kill(
	int delay,
	int pid
){
	char buffer [256];
	linux_wait(delay);
	sprintf(buffer, "kill %d", pid);
	system(buffer);
}

void linux_actor_killall(
	int delay,
	char* pname
){
	char buffer[256];
	linux_wait(delay);
	sprintf(buffer, "killall %s", pname);
	system(buffer);
}
