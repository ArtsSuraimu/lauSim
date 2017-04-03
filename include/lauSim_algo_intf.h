/*
 * lauSim_algo_intf.h
 *
 *  Created on: Mar 30, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_LAUSIM_ALGO_INTF_H_
#define INC_LAUSIM_ALGO_INTF_H_

void (*FP_RANDOM_FAILURE)
		(char ** node_list, int num_nodes,
		 char ** failed_node_list, int* num_failed);

#endif /* INC_LAUSIM_ALGO_INTF_H_ */
