/*
 * algo_random.h
 *
 *  Created on: Apr 4, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#include "algo_random.h"
#include "pcg/pcg_basic.h"

static pcg32_random_t rng;



algo_t* init_random(
	void
){
	algo_t* instance = (algo_t*) malloc (sizeof(algo_t));

	algo_randdata_t* pData = (algo_randdata_t*) malloc (sizeof(algo_randdata_t));

	pData->rng = (pcg32_random_t*) malloc(sizeof(pcg32_random_t));

	pcg32_srandom_r(pData->rng, time(NULL) ^ (int*)&printf,
		(int*)&sprintf);

	instance->failed = random_get_failed;
	instance->reg = random_register_node;
	instance->spare = random_get_spare;
	instance->cleanup = cleanup_random;

	return instance;
}


void cleanup_random(
	algo_t* algo
){
	algo_randdata_t* pData = (algo_randdata_t*) algo->pData;
	free(pData->rng);
	free(pData->spare_node_list);
	free(pData);
	free(algo);
}
