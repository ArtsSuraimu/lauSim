/*
 * algo_random.h
 *
 *  Created on: Apr 4, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_BACKEND_PRIMITIVE_ALGO_ALGO_RANDOM_H_
#define INC_BACKEND_PRIMITIVE_ALGO_ALGO_RANDOM_H_

#include "pcg/pcg_basic.h"
#include "lauSim_algo_intf.h"

#define MAX_NODES_SIZE 100
#define MAX_CONCURRENT_FAIL 2

typedef struct tag_str_list str_list_t;

struct tag_str_list{
	char* data;
	str_list_t* next;
};

typedef struct tag_data_algo_rand{
	pcg32_random_t* rng;
	str_list_t* spare_node_head;
	str_list_t* spare_node_tail;
	int num_spares;
}algo_randdata_t;

algo_t* init_random(
	void
);

void random_get_failed(
	algo_t*	algo,
	char** 	node_list,
	int		num_nodes,
	char***	failed_node_list,
	int*	num_failed);

void random_get_spare(
	algo_t*	algo,
	char***	spare_node_list,
	int*	num_spare);

void random_register_node(
	algo_t* algo,
	char*	spare_node);

void cleanup_random(
	algo_t* algo
);


#endif /* INC_BACKEND_PRIMITIVE_ALGO_ALGO_RANDOM_H_ */
