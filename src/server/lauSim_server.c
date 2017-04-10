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
void lausim_srv_sig_handler(
	int sig
){
	switch (sig)
	{
		case SIGCHLD:
			break;
		case SIGHUP:
			syslog(LOG_NOTICE, "Reveived SIGHUP, resetting...");
			break;
		case SIGTERM:
			syslog(LOG_NOTICE, "Received SIGTERM, exiting...");
			closelog();
			exit(0);
		default:
			break;
	}
}

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

	signal(SIGCHLD, lausim_srv_sig_handler);
	signal(SIGHUP, lausim_srv_sig_handler);
	signal(SIGTERM, lausim_srv_sig_handler);

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

#if 0

int main(
	int argc,
	char** argv
){

	proc_backend();

	/*
	 * TODO:
	 * Seperate Threads for Simulating Node Failure and
	 * Monitoring Spare Nodes.
	 */

    while (1)
    {
    	syslog(LOG_NOTICE, "lauSim started.");
    	sleep (100);

    	/*
    	 * TODO
    	 * 1. Determine Running Nodes
    	 * 2. Connect to Algo, get fail list
    	 * 3. send messages to COM Backend
    	 * 4. sleep.
    	 */
    }

    syslog(LOG_NOTICE, "lauSim terminated.");
    closelog();

    return EXIT_SUCCESS;

}

#endif
