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
#include <argp.h>
#include <assert.h>

#ifdef DEBUG
#include <debug.h>
#endif

#include "server/lauSim_server.h"
#include "mqttclient.h"
#include "../backend/primitive_algo/algo_random.h"
#include "proto/laik_ext.pb-c.h"


#define NODE_STATUS_TOPIC "envelope/status"




static
int parse_opt(
	int key,
	char* arg,
	struct argp_state *state
);


static
char doc[] = "This is LauSim.";


static
struct argp_option cmdlineOpts[] = {
		{"verbose", 'v', 0, 0, "Produce verbose output"},
		{"console", 'c', 0, 0, "Run in console mode"},
		{"daemon",  'd',0, 0, "Run in daemon mode"},
		{0}
};

static
char args_doc[] = "-c/-d (-v)";

static
int parse_opt(
	int key,
	char* arg,
	struct argp_state *state
){
	LAUSIM_CONFIG *local_config = state->input;

	switch (key){
		case 'v':
			local_config->debug = 1;
			break;
		case 'c':
			if(local_config->type == ST_UNDEFINED){
				local_config->type = ST_CONSOLE;
			}
			break;
		case 'd':
			if(local_config->type == ST_UNDEFINED){
				local_config->type = ST_DAEMON;
			}
			break;
		case ARGP_KEY_ARG:
			if(state->arg_num >= ARGNUM)
			{
				argp_usage(state);
			}
			break;
		case ARGP_KEY_END:
			if(state->arg_num <= 1){
				argp_usage(state);
			}
			break;

		default:
			return ARGP_ERR_UNKNOWN;
	}

	return 0;
}


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



int main(
	int argc,
	char** argv
){

	struct argp argp = {cmdlineOpts, parse_opt, args_doc, doc};
	char * nodelist[4] = {
			"node00",
			"node01",
			"node02",
			"node03"
	};
	char** failed_node_list;
	int num_failed;

	LAUSIM_CONFIG* local_config;

	com_backend_t com;
		LaikExtMsg msg = LAIK_EXT_MSG__INIT;
	void* buf;
	int len;


	local_config = (LAUSIM_CONFIG*) malloc( sizeof(LAUSIM_CONFIG));
	memset(local_config, 0x0, sizeof(LAUSIM_CONFIG));

	local_config->type = ST_UNDEFINED;

	if(argc != 1){
		argp_parse(&argp, argc, argv, 0, 0, local_config);
	}else{
		local_config->type = ST_CONSOLE;
	}

	mqtt_init("Foo", "localhost", 1883, 60, &com);


	if(local_config->type == ST_CONSOLE)
	{
		local_config->algo = init_random();

		while (1){
			local_config->algo->failed(local_config->algo, nodelist, 4, &failed_node_list, &num_failed);

			msg.n_failing_nodes = num_failed;
			msg.failing_nodes = failed_node_list;
			msg.n_spare_nodes = 0;

			len = laik_ext_msg__get_packed_size(&msg);
			buf = malloc(len);
			laik_ext_msg__pack(&msg, buf);
			com.send(NODE_STATUS_TOPIC, buf, len, &com);
			free(buf);

			//debug;
			for(int i=0; i<num_failed; i++){
				printf("Failed Node: %s\n", failed_node_list[i]);

				free(failed_node_list[i]);
			}
			//TODO: Failed Nodes aus dem Liste streichen!!!
			//TODO: Publish Node List!!

			num_failed = 0;
			free(failed_node_list);

			sleep(1);
		}
	}
	else
	{
		assert (0);
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
	}

	mqtt_cleanup(&com);
    free(local_config);

    return EXIT_SUCCESS;

}


