/*
 * lauSim_act_intf.h
 *
 *  Created on: Mar 31, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_INCLUDE_LAUSIM_ACT_INTF_H_
#define INC_INCLUDE_LAUSIM_ACT_INTF_H_


typedef enum tag_actor_features{
	SHUTDOWN = 0b1,
	REBOOT = 0b10,
	KILL = 0b100,
	KILLALL = 0b1000
}actor_features_t;

typedef void (*FP_FAIL) (int delay);
typedef void (*FP_REBOOT) (int delay);
typedef void (*FP_KILL) (int delay, int pid);
typedef void (*FP_KILLALL) (int delay, char* pname);


#endif /* INC_INCLUDE_LAUSIM_ACT_INTF_H_ */
