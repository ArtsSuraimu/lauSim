/*
 * lauSim_com_intf.h
 *
 *  Created on: Mar 30, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_LAUSIM_COM_INTF_H_
#define INC_LAUSIM_COM_INTF_H_

int (*FP_SEND) (char* channel, char* buffer, int length);
int (*FP_RECV) (char* channel, char* buffer, int* length);


#endif /* INC_LAUSIM_COM_INTF_H_ */
