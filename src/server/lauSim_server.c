/*
 * lauSim_server.c
 *
 *  Created on: Apr 4, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <syslog.h>



#ifdef DEBUG
#include <debug.h>
#endif

#include "server/lauSim_server.h"



static
void proc_backend(
	void
){

	/* detach */
	pid_t pid;
	pid = fork();

	if (pid < 0){
		exit(1);
	}
	if (pid > 0){
		exit(0);
	}

	/* take owner */
	if (setsid() < 0)
		exit(1);

	//TODO: signal handler */
	signal(SIGCHLD, SIG_IGN);
	signal(SIGHUP, SIG_IGN);

	/* Detach */
	pid = fork();
	if (pid < 0){
		exit(1);
	}
	if (pid > 0){
		exit(0);
	}

	/* Secure */
	umask(0);
	chdir("/");

	/* Close all FD */
	int x;
	for (x = sysconf(_SC_OPEN_MAX); x>=0; x--)
	{
		close (x);
	}

	/* Open Log File */
	openlog ("lauSim", LOG_PID, LOG_DAEMON);

}


int main(
	int argc,
	char** argv
){

	proc_backend();

    while (1)
    {
    	syslog(LOG_NOTICE, "lauSim started.");
    	sleep (100);
        //TODO: Insert daemon code here.
    }

    syslog(LOG_NOTICE, "lauSim terminated.");
    closelog();

    return EXIT_SUCCESS;

}
