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

static
int isAnyInt(
	int  num,
	int num_array,
	const int* array
){
	int i;
	for(i=0;i<num_array;i++){
		if(num == array[i]){
			return 1;
		}
	}
	return 0;
}

static
void copyStrArray(
	const char** from,
	char** to,
	int num
){
	int i;
	char* ptr;
	int substrlen;

	assert(from);
	assert(to);

	for(i=0; i<num; i++)
	{
		substrlen = strlen(from[i]) + 1;
		ptr = (char*) malloc (substrlen*sizeof(char));
		strcpy(ptr, from[i]);
		to[i] = ptr;
	}
}

static
str_list_t* str_list(
	const char* element
){
	int len;
	str_list_t* e;

	assert(element);
	len = strlen(element);

	e = (str_list_t*) malloc (sizeof(str_list_t));
	assert(e);

	e->data = (char*) malloc (len*sizeof(char));
	assert(e->data);

	strcpy(e->data, element);
	e->next = NULL;

	return e;
}


/*
 * Add a string to the minilist.
 */
static
str_list_t* insertStr (
	const char* element,
	str_list_t* tail
){
	int len;
	str_list_t* e;

	assert(element);
	assert(tail);

	e = (str_list_t*) malloc (sizeof(str_list_t));
	assert(e);
	len = strlen(element);

	e->data = (char*) malloc (len*sizeof(char));
	assert(e->data);
	memset(e->data, 0x0, len*sizeof(char));
	strcpy(e->data, element);

	e->next = NULL;
	tail->next = e;

	return e;
}


#if 0
static
int removeStr (
	const char* searchstr,
	int num_elem,
	str_list_t** head,
	str_list_t** tail
){
	int i;
	int ret = 0;
	str_list_t* p, last;
	assert(searchstr);
	assert(head);
	assert(tail);

	if(strcmp((*head)->data, searchstr)==0)
	{
		str_list_t* temp = *head;
		*head = (*head)->next;
		free(temp->data);
		free(temp);
		ret = 1;
	}else{

		p = (*head)->next;
		last = head;

		for(i=0; i<num_elem; i++){
			if(strcmp(p->data, searchstr) == 0)
			{
				//found
				free(p->data);
				last->next = p->next;
				if(p == *tail){
					*tail = last;
				}
				free(p);
				ret = 1;
				break;
			}
		}
	}
	return ret;
}spare_node_list = NULL;
*num_spare = 0;
#endif

algo_t* init_random(
	void
){
	algo_t* instance = (algo_t*) malloc (sizeof(algo_t));
	assert(instance);

	algo_randdata_t* pData = (algo_randdata_t*) malloc (sizeof(algo_randdata_t));
	assert(pData);

	pData->rng = (pcg32_random_t*) malloc(sizeof(pcg32_random_t));
	assert(pData->rng);

	pData->spare_node_head = 0;
	pData->spare_node_tail = 0;
	pData->num_spares = 0;

	pcg32_srandom_r(pData->rng, time(NULL) ^ *((int*)&printf),
		*(int*)&sprintf);

	instance->pData = pData;
	instance->failed = random_get_failed;
	instance->reg = random_register_node;
	instance->spare = random_get_spare;
	instance->cleanup = cleanup_random;

	return instance;
}

void random_get_failed(
	algo_t*	algo,
	char** 	node_list,			/* List of all Nodes */
	int		num_nodes,
	char**	failed_node_list,	/* List of Failed Nodes */
	int*	num_failed)
{
	int cnt_failed;
	int i_failed;
	int i, j=0;
	int* history;
	algo_randdata_t* pData;

	assert(algo);
	pData = (algo_randdata_t*) algo->pData;

	// Get number nodes to be fail
	cnt_failed = pcg32_boundedrand_r(pData->rng, MAX_CONCURRENT_FAIL + 1);
	if(cnt_failed == 0)
	{
		failed_node_list = NULL;
		*num_failed = 0;
		return;
	}
	// ensure plausibility
	if(cnt_failed > num_nodes){
		cnt_failed = num_nodes;
	}

	failed_node_list = (char**) malloc (cnt_failed*sizeof(char*));
	assert(failed_node_list);

	// all nodes failed
	if(cnt_failed == num_nodes){

		copyStrArray(node_list, failed_node_list, cnt_failed);
		*num_failed = cnt_failed;
		return;
	}

	// some nodes failed
	history = (int*) malloc (cnt_failed*sizeof(int));
	assert(history);
	memset(history, 0xFF, cnt_failed*sizeof(int));

	//get random node numbers
	for(i=0; i<cnt_failed; i++){

		//get index of failed node
		i_failed = pcg32_boundedrand_r(pData->rng, num_nodes);
		//ensure this occured only once in this run
		while(isAnyInt(i_failed, cnt_failed, history))
		{
			i_failed = pcg32_boundedrand_r(pData->rng, num_nodes);
		}

		// copy the name of node to failed node list
		failed_node_list[i] = (char*) malloc (
				(strlen(node_list[i_failed]) +1 ) * sizeof(char));
		strcpy(failed_node_list[i], node_list[i_failed]);

		// add nodeindex to history
		history[j] = i_failed;
		j++;
	}

	*num_failed = cnt_failed;
}

void random_register_node(
	algo_t* algo,
	char*	spare_node)
{
	algo_randdata_t* pData;

	assert(algo);
	assert(spare_node);

	pData = (algo_randdata_t*) algo->pData;

	if(pData->spare_node_head == NULL){
		pData->spare_node_head = str_list(spare_node);
		pData->spare_node_tail = pData->spare_node_head;
		pData->num_spares = 1;
	}else{
		pData->spare_node_tail =
				insertStr(spare_node, pData->spare_node_tail);
		(pData->num_spares)++;
	}
}

void random_get_spare(
	algo_t*	algo,
	char**	spare_node_list,
	int*	num_spare)
{
	algo_randdata_t* pData;
	int i;

	assert(algo);
	pData = (algo_randdata_t*) algo->pData;

	if(pData->num_spares != 0)
	{
		int cnt_nodes;

		//get random number of spare nodes
		cnt_nodes = pcg32_boundedrand_r(pData->rng, *num_spare);
		if(cnt_nodes != 0){
			str_list_t* p;

			//create a spare node list
			spare_node_list = (char**) malloc (cnt_nodes * sizeof(char*));
			assert(spare_node_list);
			*num_spare = 0;

			//take the first n spare nodes from our list
			for(i=0; i<cnt_nodes; i++){
				// detach first spare node
				p = pData->spare_node_head;
				pData->spare_node_head = p->next;
				if(p == pData->spare_node_tail){
					pData->spare_node_tail = p->next;
				}
				//copy it
				spare_node_list[i] = (char*) malloc((strlen(p->data)+1)*sizeof(char));
				strcpy(spare_node_list[i], p->data);
				//clean it up
				free(p->data);
				free(p);
				//correct count;
				(*num_spare)++;
				(pData->num_spares)--;
			}
		}
		//assume no spare node
		*spare_node_list = NULL;
		*num_spare = 0;
	}

	spare_node_list = NULL;
	*num_spare = 0;
}

void cleanup_random(
	algo_t* algo
){
	algo_randdata_t* pData = (algo_randdata_t*) algo->pData;
	str_list_t* ptr, *ptr1 = pData->spare_node_head;
	int i;
	ptr = ptr1;

	for(i=0;i<pData->num_spares; i++){
		free(ptr->data);
		ptr = ptr1->next;
		free(ptr1);
	}
	free(pData->rng);
	free(pData);
	free(algo);
}
;
