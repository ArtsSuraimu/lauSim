/*
 * linux_actor.c
 *
 *  Created on: Mar 31, 2017
 *      Author: Dai Yang, Clemens Jonischkeit
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

#include "linux_actor.h"

static
void linux_wait(
	int delay
){
	struct timespec sleep_time;
	struct timespec sleep_time_left;
	int res;

	// do not sleep if you don't have to
	if (delay <= 0)
		return;

	// prepare the struct
	sleep_time.tv_sec = delay;
	sleep_time.tv_nsec = 0;

	do {
		res = nanosleep(&sleep_time, &sleep_time_left);
		if (res != 0) {
			switch (errno) {
			case EFAULT:
				// there was a problem copying data from user space
				return;
			case EINVAL:
				// nsec was not in [0..999999999]
				sleep_time.tv_nsec = 0;
				break;
			case EINTR:
				// try to recover from interrupts and possible errors
				sleep_time.tv_sec = sleep_time_left.tv_sec;
				sleep_time.tv_nsec = sleep_time_left.tv_nsec;
				break;
			default:
				return;
			}
		}
	} while (res != 0);
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
	snprintf(buffer, sizeof(buffer), "kill %d", pid);
	system(buffer);
}

void linux_actor_killall(
	int delay,
	char* pname
){
	char buffer[256];
	linux_wait(delay);
	snprintf(buffer, sizeof(buffer), "killall %s", pname);
	system(buffer);
}

