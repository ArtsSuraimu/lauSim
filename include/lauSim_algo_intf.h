/*
 * lauSim_algo_intf.h
 *
 *  Created on: Mar 30, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_LAUSIM_ALGO_INTF_H_
#define INC_LAUSIM_ALGO_INTF_H_

#define ALGO_INTF_VER 1

typedef struct tag_algo_intf algo_t;

typedef void (*FP_GET_FAILED_NODE)(
	algo_t* algo,
	char ** node_list,
	int num_nodes,
	char ** failed_node_list,
	int* num_failed);

typedef void (*FP_GET_SPARE_NODE)(
	algo_t* algo,
	char** spare_node_list,
	int* num_spare);

typedef void (*FP_REGISTER_SPARE_NODE)(
	algo_t* algo,
	char* spare_node,
	int num_spare);

struct tag_algo_intf{

};

#endif /* INC_LAUSIM_ALGO_INTF_H_ */
