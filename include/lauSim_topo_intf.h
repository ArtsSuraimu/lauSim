/*
 * lauSim_topo_intf.h
 *
 *  Created on: Apr 26, 2017
 *      Author: Dai Yang
 *	(C) 2017 Technische Universitaet Muenchen, LRR
 */

#ifndef INC_INCLUDE_LAUSIM_TOPO_INTF_H_
#define INC_INCLUDE_LAUSIM_TOPO_INTF_H_

typedef enum tag_topo_types{
	TT_CANONICAL = 1,
	TT_SINGLE = 2,
	TT_CIRCLE = 3,
	TT_MATRIX = 4,
	TT_USER = 0xFF
}TOPO_TYPES;

typedef enum tag_element_types{
	ET_SWITCH	= 1,
	ET_HUB		= 2,
	ET_NIC		= 3,
	ET_NODE		= 4,
	ET_AP	 	= 5,
	ET_GENERIC  = 0xFF
};

typedef double (*FP_GETNW) (int node_n);
typedef int (*FP_UPDATETOPO) (void*);
typedef struct tag_node node;


typedef struct tag_node{
	char* node;
	node* left;
	node* right;
	node* up;
	node* down;
};

typedef struct tag_node_list{
	node* list;
	int count;
}node_list;

typedef struct tag_topo_ctrl{
	node_list * all;
	int  		all_count;
	node_list *	failed;
	int 		failed_count;
	node_list * up;
	int 		up_list;
	TOPO_TYPES  topology;

	FP_GETNW getNW;

}topo_ctrl_t;

topo_ctrl_t* init_topo_intf(
	node_list* all,
	int 	all_count,
	FP_GETNW getNW
);

void cleanup_topo_intf(
	topo_ctrl_t* intf
);

double get_canonical_nw (int node_n);
double get_circle_nw (int node_n);
double get_matrix_nw (int node_n);



#endif /* INC_INCLUDE_LAUSIM_TOPO_INTF_H_ */
